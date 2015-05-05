#include "stdint.h"
#include "ELBC.h"
#include "SysSpace.h"

#define  CN_ELBC_ADDRBASE  (CN_CCSR_NEWADDR_V + 0x5000)
#define  CN_ELBC_FMR       (CN_ELBC_ADDRBASE + 0xe0)
#define  CN_ELBC_LBCRA     (CN_ELBC_ADDRBASE + 0xd0)
#define  CN_ELBC_LCRR      (CN_ELBC_ADDRBASE + 0xd4)

//define for the elbc
#define LBCR_BMT            0x0000FF00
#define LBCR_BMTPS          0x0000000F

extern const tagElbcCfg  tgP1020RDB_ElbcCfg[];
extern const int gElbcCfgItem;
// =============================================================================
// 函数功能:P1020ElbcInit
//          elbc的初始化
// 输入参数:
// 输出参数:无
// 返回值  :
// 说明    :
// =============================================================================
void P1020ElbcInit()
{
    int i;
    //elbc总线空间分配
    tagElbcCfg *temp = (tagElbcCfg *)CN_ELBC_ADDRBASE;
    *(u32 *)CN_ELBC_FMR = 0x0000F020;
    *(u32 *)CN_ELBC_LBCRA =0x4000000f;
    *(u32 *)CN_ELBC_LCRR = 0x80000008;
    for(i = 0; i <gElbcCfgItem; i++)
    {
        temp->br = tgP1020RDB_ElbcCfg[i].br;
        temp->org_g_f_u = tgP1020RDB_ElbcCfg[i].org_g_f_u;
        temp++;
    }
    return ;
}

