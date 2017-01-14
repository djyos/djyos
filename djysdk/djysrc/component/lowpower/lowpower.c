#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "lowpower.h"
#include "int.h"
#include "int_hard.h"

struct LowPowerCtrl g_tLowPower;

//----进入低功耗状态----------------------------------------------------------
//功能: 调用本函数后,进入设定的低功耗状态,进入前调用相关回调函数.
//参数: 无
//返回: CN_SLEEP_NORMAL,CN_SLEEP_L0-L3之一, 不可能返回CN_SLEEP_L4
//说明：使用L3要特别注意，许多设备，进入如此深的休眠后，其配置会丢失，唤醒时往往
//      不能正常工作，或者中作状态不正确。因此，必须充分利用EntrySleepReCall和
//      ExitSleepReCall两个函数。
//----------------------------------------------------------------------------
u32 LP_EntryLowPower(struct ThreadVm *vm)
{
    atom_low_t  atom_bak;

    if(g_tLowPower.DisableCounter != 0)
    {
        g_tLowPower.EntrySleepReCall(CN_SLEEP_L0);
        __LP_BSP_EntrySleepL0( );
        g_tLowPower.ExitSleepReCall(CN_SLEEP_L0);
        return CN_SLEEP_L0;
    }
    switch(g_tLowPower.SleepLevel)
    {
       case CN_SLEEP_L0:
           g_tLowPower.EntrySleepReCall(CN_SLEEP_L0);
           __LP_BSP_EntrySleepL0( );
           g_tLowPower.ExitSleepReCall(CN_SLEEP_L0);
           return CN_SLEEP_L0;
           break;
       case CN_SLEEP_L1:
           g_tLowPower.EntrySleepReCall(CN_SLEEP_L1);
           __LP_BSP_EntrySleepL1( );
           g_tLowPower.ExitSleepReCall(CN_SLEEP_L1);
           return CN_SLEEP_L1;
           break;
       case CN_SLEEP_L2:
           g_tLowPower.EntrySleepReCall(CN_SLEEP_L2);
           __LP_BSP_EntrySleepL2( );
           g_tLowPower.ExitSleepReCall(CN_SLEEP_L2);
           return CN_SLEEP_L2;
           break;
       case CN_SLEEP_L3:
           g_tLowPower.EntrySleepReCall(CN_SLEEP_L3);
           __LP_BSP_SaveSleepLevel(CN_SLEEP_L3);
           atom_bak = Int_LowAtomStart();
           __LP_BSP_AsmSaveReg(vm,__LP_BSP_SaveRamL3,__LP_BSP_EntrySleepL3);
           Int_LowAtomEnd(atom_bak);
           g_tLowPower.ExitSleepReCall(CN_SLEEP_L3);
           return CN_SLEEP_L3;
           break;
       case CN_SLEEP_L4:
           g_tLowPower.EntrySleepReCall(CN_SLEEP_L4);
           __LP_BSP_SaveSleepLevel(CN_SLEEP_L4);
           __LP_BSP_EntrySleepL4( );    //进入低功耗,并且不会返回这里
           break;

       default:
           break;
    }
    return 0;
}

//----禁止进入低功耗状态------------------------------------------------------
//功能: 调用本函数后,系统不会进入L1及以上级别的低功耗状态.并且使禁止次数计数器
//      +1,须调用同等次数的LP_EnableSleep使计数器减至0后,才允许进入L1及以上级别
//      低功耗状态.
//参数: 无
//返回: 禁止计数器当前值
//----------------------------------------------------------------------------
u32 LP_DisableSleep(void)
{
    if(g_tLowPower.DisableCounter != CN_LIMIT_UINT32)
        g_tLowPower.DisableCounter++;
    return g_tLowPower.DisableCounter;
}

//----使能进入低功耗状态------------------------------------------------------
//功能: 调用本函数后,禁止次数计数器减1,减至0后,允许进入L1及以上级别低功耗状态.
//参数: 无
//返回: 禁止计数器当前值
//----------------------------------------------------------------------------
u32 LP_EnableSleep(void)
{
    if(g_tLowPower.DisableCounter != 0)
        g_tLowPower.DisableCounter--;
    return g_tLowPower.DisableCounter;
}

//----设置低功耗级别----------------------------------------------------------
//功能: 设置当前低功耗级别,CN_SLEEP_L0---CN_SLEEP_L4
//参数: Level, CN_SLEEP_L0---CN_SLEEP_L4中的一个
//返回: 当前低功耗级别
//----------------------------------------------------------------------------
u32 LP_SetSleepLevel(u32 Level)
{
    if(Level <= CN_SLEEP_L4)
        g_tLowPower.SleepLevel = Level;
    return g_tLowPower.SleepLevel;
}

u32 EmptyReCall(u32 SleepLevel)
{
    return 0;
}
//----初始化-------------------------------------------------------------------
//功能: 初始化低功耗组件
//参数: EntrySleepReCall,进入低功耗状态前调用的回调函数
//      ExitSleepReCall, 离开低功耗状态时调用的回调函数
//返回: 无意义
//-----------------------------------------------------------------------------
extern u32 (*g_fnEntryLowPower)(struct ThreadVm *vm);  //进入低功耗状态的函数指针。
ptu32_t ModuleInstall_LowPower (u32 (*EntrySleepReCall)(u32 SleepLevel),
                                u32 (*ExitSleepReCall)(u32 SleepLevel))
{
    if(EntrySleepReCall != NULL)
        g_tLowPower.EntrySleepReCall = EntrySleepReCall;
    else
        g_tLowPower.EntrySleepReCall = EmptyReCall;
    if(ExitSleepReCall != NULL)
        g_tLowPower.ExitSleepReCall = ExitSleepReCall;
    else
        g_tLowPower.ExitSleepReCall = EmptyReCall;
    g_tLowPower.SleepLevel = 0;
    g_tLowPower.DisableCounter = 0;
    g_fnEntryLowPower = LP_EntryLowPower;
    return 0;
}

