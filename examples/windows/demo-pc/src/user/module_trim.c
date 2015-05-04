#include "stdint.h"
#include "config-prj.h"
#include "driver.h"
#include "gkernel.h"
#include "djyos.h"
extern ptu32_t ModuleInstall_DebugInfo(ptu32_t para);

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}

const u32 gc_u32CfgTimeZone = 8;     //本地时区为东8区(北京)

const char *gc_pCfgDefaultCharsetName = "gb2312";  //默认字符集
const char *gc_pCfgDefaultFontName = "gb2312_8_16_1616";  //默认字体

const u32 gc_u32CfgGuiCmdDeep = 512;   //gkernel命令缓冲区深度(字节数,非命令条数)
const u32 gc_u32CfgGuiStsDeep = 256;    //gkernel状态缓冲区深度(字节数,非命令条数)

const u32 gc_u32CfgDeviceLimit = 50;     //定义设备数量。
const u32 gc_u32CfgLockLimit = 50;      //定义锁数量。用户调用semp_create和mutex_create创建的锁，不包括内核用到的锁。
const u32 gc_u32CfgEventLimit = 50;     //事件数量
const u32 gc_u32CfgEvttLimit = 50;      //总事件类型数
const u32 gc_u32CfgWdtLimit = 50;       //允许养狗数量
const u32 gc_u32CfgPipeLimit = 50;       //管道数量
const u32 gc_u32CfgMemPoolLimit = 50;    //允许建立10个内存池
const u32 gc_u32CfgStdinDeviceLimit = 50;
const u32 gc_u32CfgMainStackLen = 2000;

const u32 gc_u32CfgOpenedfileLimit = 50;
const u32 gc_u32CfgTimerLimit = 50;

const char *gc_pCfgStddevName = "windows_cmd";  //标准输入输出设备
const char *gc_pCfgWorkPath = "sys:\\";   //工作路径设置

bool_t Exp_InfoDecoderInit(void);

//罗列GDD组件输入设备的名称
static  const char *pGddInputDev[]={

    "sim touch",
    "sim keyboard",
};

void    ModuleInstall_GDD(struct tagGkWinRsc *desktop,const char *InputDevName[],s32 InputDevNum);

//----操作系统运行参数配置-----------------------------------------------------
//功能: 配置操作系统运行参数,例如锁的数量/事件数量等.使用文本文件配置时使用。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Sys_ConfigRunParam(char *CfgfileName)
{
}

void Stddev_KnlInOutInit(u32 para)  //本函数暂未实现
{

}
//----操作系统内核组件配置-----------------------------------------------------
//功能：配置和初始化可选功能组件，在用户工程目录中必须实现本函数，在内核初始化
//      阶段调用。
//      本函数实现内核裁剪功能，例如只要注释掉
//          ModuleInstall_Multiplex(0);
//      这一行，多路复用组件就被裁剪掉了。
//      用户可从example中copy本文件，把不要的组件注释掉，强烈建议，不要删除,也
//      不要修改调用顺序。可以把用户模块的初始化代码也加入到本函数中,建议跟在
//      系统模块初始化后面.
//      有些组件有依赖关系,裁剪时,注意阅读注释.
//参数：无
//返回：无
//---------------------------------------------------------------------------
void Sys_ModuleInit(void)
{
    static struct tagGkWinRsc desktop;
    struct tagDisplayRsc *lcd_windows;
    tagDevHandle char_term_hdl;
    u32 stdinout;
    uint16_t evtt_main;

    //初始化PutChark,Putsk,GetChark,GetsK四个内核级输入输出函数,此后,printk
    //可以正常使用.printf和scanf将调用前述4个函数执行输入输出
    Stddev_KnlInOutInit( 0 );
    //shell模块,依赖:无
    ModuleInstall_Sh(0);
    //文件系统模块,依赖:shell
    ModuleInstall_Djyfs(0);
    //设备驱动模块
    ModuleInstall_Driver(0);
    //多路复用模块,提供类似Linux的epoll、select的功能
    ModuleInstall_Multiplex(0);
    //消息队列模块
    ModuleInstall_MsgQ(0);

    //提供在shell上输出内核信息的功能,依赖:shell模块
    ModuleInstall_DebugInfo(0);

    //异常监视模块,依赖:shell模块
    ModuleInstall_Exp(0);
    //安装异常信息解析器,解析异常模块记录的二进制数据
//    Exp_InfoDecoderInit( );

    //flash文件系统初始化,依赖:文件系统模块,shell模块
    ModuleInstall_DFFSD(0);
    //nand flash驱动,依赖:flash文件系统模块
    ModuleInstall_WinFs(NULL);
    //设置工作路径,依赖:文件系统,且相关路径存在.
    Djyfs_SetWorkPath(gc_pCfgWorkPath);


    //标准IO初始化,不影响printk函数,此后,printf和scanf将使用stdin/out输出和输
    //入,如果裁掉,将一直用PutChark,Putsk,GetChark,GetsK执行IO
    //依赖: 若stdin/out/err是文件,则依赖文件系统
    //      若是设备,则依赖设备驱动
    //      同时,还依赖用来输出信息的设施,例如串口,LCD等
    ModuleInstall_Stddev( 0 );
    ModuleInstall_Cmd(0);

    //打开IO设备,不同的板件,这部分可能区别比较大.
    stdinout = Driver_FindDevice(gc_pCfgStddevName);
    char_term_hdl = Driver_OpenDeviceAlias(stdinout,O_RDWR,0);
    if(char_term_hdl != NULL)
    {
        ModuleInstall_CharTerm((ptu32_t)stdinout);
        Driver_CloseDevice(char_term_hdl);
    }

    //djybus模块
    ModuleInstall_DjyBus(0);
    //IIC总线模块,依赖:djybus
    ModuleInstall_IICBus(0);
    //SPI总线模块,依赖:djybus
    ModuleInstall_SPIBus(0);

    //日历时钟模块
    ModuleInstall_TM(0);
    //使用硬件RTC,注释掉则使用tick做RTC计时,依赖:日历时钟模块
//    ModuleInstall_RTC(0);

    //定时器组件
    ModuleInstall_TimerSoft(0);

//    ModuleInstall_DjyIp(0);

    //键盘输入模块
    ModuleInstall_KeyBoard(0);
    //键盘输入驱动,依赖:键盘输入模块
    ModuleInstall_KeyBoardWindows(0);

    //字符集模块
    ModuleInstall_Charset(0);
    //gb2312字符编码,依赖:字符集模块
    ModuleInstall_CharsetGb2312(0);
    //ascii字符集,注意,gb2312包含了ascii,初始化了gb2312后,无须本模块
    //依赖:字符集模块
//    ModuleInstall_CharsetAscii(0);
    //初始化utf8字符集
//    ModuleInstall_CharsetUtf8(0);
    //国际化字符集支持,依赖所有字符集模块以及具体字符集初始化
    ModuleInstall_CharsetNls("C");


    ModuleInstall_Font(0);                 //字体模块

    //8*8点阵的ascii字体依赖:字体模块
//    ModuleInstall_FontAscii8x8Font(0);
    //6*12点阵的ascii字体依赖:字体模块
//    ModuleInstall_FontAscii6x12Font(0);
    //从数组安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块
    ModuleInstall_FontGb2312_816_1616_Array(0);
    //从文件安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块,文件系统
//    ModuleInstall_FontGb2312_816_1616_File("sys:\\gb2312_1616");
    //8*16 ascii字体初始化,包含高128字节,依赖:字体模块
    //注:如果安装了GB2312,无须再安装
    ModuleInstall_FonAscii8x16Font(0);

    ModuleInstall_GK(0);           //gkernel模块
    //lcd驱动初始化,如果用系统堆的话,第二个参数用NULL
    //堆的名字,是在lds文件中命名的,注意不要搞错.
    //依赖: gkernel模块
    lcd_windows = (struct tagDisplayRsc*)ModuleInstall_Lcd("sim_display",NULL);

    GK_ApiCreateDesktop(lcd_windows,&desktop,0,0,
                        CN_COLOR_RED+CN_COLOR_GREEN,CN_WINBUF_BUF,0,0);

    //触摸屏模块,依赖:gkernel模块和显示器驱动
    ModuleInstall_Touch(0);

    //依赖:触摸屏模块,宿主显示器驱动,以及所依赖的硬件,例如qh_1的IIC驱动.
    //     如果矫正数据存在文件中,还依赖文件系统.
    ModuleInstall_TouchWindows((ptu32_t)"sim touch");

    //看门狗模块,如果启动了加载时喂狗,看门狗软件模块硬件从此开始接管硬件狗.
//    ModuleInstall_Wdt(0);

    //GDD组件初始化
    ModuleInstall_GDD(&desktop,pGddInputDev,sizeof(pGddInputDev)/sizeof(void*));

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                __djy_main,NULL,gc_u32CfgMainStackLen,
                                "main function");
    //事件的两个参数暂设为0?如果用shell启动?可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0);

    //用户自己程序的初始化模块,建议放在这里.
    Heap_DynamicModuleInit(0);  //自此malloc函数执行块相联算法
    //至此,初始化时使用的栈,已经被系统回收,本函数就此结束,否则会死得很难看

    return ;
}


