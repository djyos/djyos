//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// MenuInitGeneral.c
// 模块描述: 菜单显示模块之初始化菜单显示参数
// 模块版本:
// 创建时间: 2012-12-31
// 作者    : Administrator
//           菜单显示参数的初始化，指的是对其显示模式、外设响应方法的初始化，
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#include "MenuInitGeneral.h"

#include "MenuSpaceGeneral.h"
#include "MenuInit_9cell.h"
#include "MenuInit_pull.h"

// =============================================================================
// 函数功能:菜单初始化函数，这个是一个待扩展函数，目前只有九宫格模式和下拉模式
// 输入参数:menuitem，关联的菜单树的第一个菜单项，win，应用程序提供的window
// 输出参数：
// 返回值  :初始化后的显示参数，并进行了主菜单的显示
// 说明：目前仅仅实现了九宫格和下拉模式，后续要有新的模式，请参照下拉和九宫格的形式
//       自己实现
// =============================================================================
struct menu_displaypara* menuappinit_general(struct menu_item *menuitem,\
        struct GkWinRsc *Appwin, u8 mydismode)
{
    struct menu_displaypara *operatingDispara;

    switch (mydismode)
    {
        case DIS_9CELL:
            operatingDispara=module_menu_init_9cell(menuitem,Appwin);
            break;
        case DIS_PULL:
            operatingDispara=module_menu_init_pull(menuitem,Appwin);
            break;
        default:
            break;
    }

    return operatingDispara;
}
// =============================================================================
// 函数功能:菜单退出显示
// 输入参数:menuitem，关联的菜单树的第一个菜单项，win，应用程序提供的window
// 输出参数：
// 返回值    :
// =============================================================================
bool_t menu_exit(struct menu_displaypara *dispara)
{
    dispara = getmainmenudispara(dispara);
    deldisplaypara(dispara);
    return true;
}
// =============================================================================
// 函数功能:执行菜单显示参数的显示函数，将本菜单栏显示出来
// 输入参数:dispara，待显示的显示参数
// 输出参数：
// 返回值  :
// 说明    : 调用该显示参数的显示函数进行显示，当然你的函数指针为空的话不会有任何反应，
//          或者你本身希望跳到0000处，但是这个不是我们的事。
// =============================================================================
void menu_display(struct menu_displaypara* dispara)
{
    if((NULL==dispara)||(NULL==dispara->MyMenuDispalyFunction))
    {
        printf("Invalid parameter--- MenuDisplay!\n");
        return ;
    }
    else
    {
        dispara->MyMenuDispalyFunction(dispara);
        return ;
    }
}
/*
// =============================================================================
// 函数功能:菜单外设响应函数（挂载的是具体的外设）
// 输入参数:operatingDispara ：当前正在操作的显示参数，para，该项操作需要的参数
// 输出参数：
// 返回值    :更新过的显示参数，比方说新建一个菜单显示参数，或者返回到父菜单。
// 说明         : 调用该显示参数的显示函数进行显示，当然你的函数指针为空的话不会有任何反应，
//          或者你本身希望跳到0000处，但是这个不是我们的事。
//          返回的显示参数应该很好好保存，不要随意修改，因为下次调用时用的就是这样的参数
//          该函数针对的是具体的外设。比方说键盘或者其他媒介，看你在初始化的时候安装的对应的外设。
// =============================================================================
struct menu_displaypara*  menuperipheral_respond(\
        struct menu_displaypara *operatingDispara, void* para )
{
    if((NULL == operatingDispara)||\
            (NULL == operatingDispara->MyMenuDispalyFunction))
    {
        printf("Invalid parameter----menuperipheral_respond!\n");
        return operatingDispara;
    }
    else
    {
        operatingDispara = operatingDispara->MyDisplayPeripheralRespond(\
                operatingDispara,para);
        return operatingDispara;
    }
}
*/
// =============================================================================
// 函数功能:执行菜单显示参数中键盘响应函数
// 输入参数:operatingDispara ：当前正在操作的显示参数，para，该项操作需要的参数
// 输出参数：
// 返回值 :更新过的显示参数，比方说新建一个菜单显示参数，或者返回到父菜单。
// 说明: 调用该显示参数的显示函数进行显示，当然你的函数指针为空的话不会有任何反应，
//          或者你本身希望跳到0000处，但是这个不是我们的事。
//          返回的显示参数应该很好好保存，不要随意修改，因为下次调用时用的就是这样的参数
//          该函数针对的是具体的外设。比方说键盘或者其他媒介，看你在初始化的时候安装的对应的外设。
// =============================================================================
struct menu_displaypara*  menukeyboard_respond(\
        struct menu_displaypara *operatingDispara, void* para )
{
    if((NULL == operatingDispara)||\
            (NULL == operatingDispara->menukeyboard_action))
    {
        printf("Invalid parameter-menuperipheral_respond!\n");
        return operatingDispara;
    }
    else
    {
        operatingDispara = operatingDispara->menukeyboard_action(\
                operatingDispara,para);
        return operatingDispara;
    }
}

// =============================================================================
// 函数功能:执行菜单显示参数中鼠标触摸板响应函数
// 输入参数:operatingDispara ：当前正在操作的显示参数，para，该项操作需要的参数
// 输出参数：
// 返回值 :更新过的显示参数，比方说新建一个菜单显示参数，或者返回到父菜单。
// 说明: 调用该显示参数的显示函数进行显示，当然你的函数指针为空的话不会有任何反应，
//          或者你本身希望跳到0000处，但是这个不是我们的事。
//          返回的显示参数应该很好好保存，不要随意修改，因为下次调用时用的就是这样的参数
//          该函数针对的是具体的外设。比方说键盘或者其他媒介，看你在初始化的时候安装的对应的外设。
// =============================================================================
struct menu_displaypara*  menumousetouch_respond(\
        struct menu_displaypara *operatingDispara, void* para )
{
    if((NULL == operatingDispara)||\
            (NULL == operatingDispara->menumousetouch_ction))
    {
        printf("Invalid parameter----menuperipheral_respond!\n");
        return operatingDispara;
    }
    else
    {
        operatingDispara = operatingDispara->menumousetouch_ction(\
                operatingDispara,para);
        return operatingDispara;
    }
}


// =============================================================================
// 函数功能:菜单外设动作函数(通用的版本)
// 输入参数:operatingDispara ：当前正在操作的显示参数，para，该项操作需要的参数
// 输出参数：
// 返回值 :更新过的显示参数，比方说新建一个菜单显示参数，或者返回到父菜单。
// 说明   : 调用该显示参数的显示函数进行显示，当然你的函数指针为空的话不会有任何反应，
//          或者你本身希望跳到0000处，但是这个不是我们的事。
//          返回的显示参数应该很好好保存，不要随意修改，因为下次调用时用的就是这样的参数
//          一般而言该函数针对的是一般的通用的外设，由调用者已经针对外设做了初步解码
// =============================================================================
struct menu_displaypara*  menupeiripheralrespondgeneral(\
        struct menu_displaypara *operatingDispara, void* para )
{
    if((NULL==operatingDispara)||\
            (NULL==operatingDispara->MyPeripheralGeneralAction))
    {
        printf("Invalid para------menupeiripheralrespondgeneral\n");

        return operatingDispara;
    }
    else
    {
        operatingDispara=operatingDispara->MyPeripheralGeneralAction(\
                operatingDispara,para);

        return operatingDispara;
    }
}

