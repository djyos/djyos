//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: GDD
//作者:  LiuWei.
//版本：V1.0.0
//文件描述: 公共代码集
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------


#include    "gdd.h"
#include    "./include/gdd_private.h"

/*============================================================================*/

void list_init(list_t *l)
{
        l->next = l->prev = l;
}

void list_insert_after(list_t *l, list_t *n)
{
        l->next->prev = n;
        n->next = l->next;

        l->next = n;
        n->prev = l;
}

void list_insert_before(list_t *l, list_t *n)
{
        l->prev->next = n;
        n->prev = l->prev;

        l->prev = n;
        n->next = l;
}

void list_remove(list_t *n)
{
        n->next->prev = n->prev;
        n->prev->next = n->next;

        n->next = n->prev = n;
}

int list_isempty(const list_t *l)
{
        return l->next == l;
}


/*============================================================================*/
u32 GUI_GetTickMS(void)
{
    return (u32)(DjyGetTime( )/1000);
}


static  struct tagMutexLCB *gdd_mutex_lock=NULL;

BOOL    GDD_Lock(void)
{
        Lock_MutexPend(gdd_mutex_lock,5000*mS);
        return TRUE;
}

void    GDD_Unlock(void)
{
        Lock_MutexPost(gdd_mutex_lock);
}

void    GDD_WindowInit(void);
void    GDD_TimerInit(void);

/*============================================================================*/
void    GDD_Init(void)
{
    gdd_mutex_lock =Lock_MutexCreate(NULL);
    if(gdd_mutex_lock!=NULL)
    {
        GDD_WindowInit();
        GDD_TimerInit();
    }
}

/*============================================================================*/

static	u16 evtt_gui_server;


static struct tagGkWinRsc *pGkDesktop;

static	const char **GDD_InputDevName;

static void gdd_input_handler(void)
{
	tpInputMsgQ pMsgQ;
	struct tagInputDeviceMsg msg;
	int i;

	pMsgQ=Stddev_CreatInputMsgQ(20,"input_msg");

	if(pMsgQ!=NULL)
	{
		if(GDD_InputDevName!=NULL)
		{
			i=0;
			while(1)
			{
				if(GDD_InputDevName[i] == NULL)
				{
					break;
				}

				Stddev_SetFocus(GDD_InputDevName[i],pMsgQ);
				i++;
			}
		}

		while(1)
		{
			if(Stddev_ReadMsg(pMsgQ,&msg,0xFFFFFFFF)!=false)
			{
				switch(msg.input_type)
				{
					case EN_STDIN_SINGLE_TOUCH:
					{
						static int x=0,y=0,z=0;

						x =msg.input_data.tagSingleTouchMsg.x;
						y =msg.input_data.tagSingleTouchMsg.y;
						z =msg.input_data.tagSingleTouchMsg.z;
						if(z>0)
						{
							MouseInput(x,y,MK_LBUTTON);
						}
						else
						{
							MouseInput(x,y,0);
						}
						//printf("ts: x=%d,y=%d,z=%d\r\n",x,y,z);
					}
					break;
					////
					case EN_STDIN_KEYBOARD:
					{
						u8* key;
						u32 time;
						u8 val,event;

						key  =msg.input_data.key_board.key_value;
						time =msg.input_data.key_board.time;

						val  =key[0];
						event =key[1];

						if(event==0x00)
						{
							KeyboardInput(val,KEY_EVENT_DOWN,time);
						}

						if(event==0xF0)
						{
							KeyboardInput(val,KEY_EVENT_UP,time);
						}

						//printf("KBD_Event: val:%02XH,%02XH; time:%d\r\n",val,event,time);
					}
					break;
					////
				}


			}
		}

		Stddev_DeleteInputMsgQ(pMsgQ);
	}
}




static	ptu32_t gdd_input_thread(void)
{
	//Djy_EventDelay(100*mS);
	printf("GDD_Input_Thread.\r\n");
	Djy_EventPop(evtt_gui_server, NULL, 0, NULL, 0, 0);
	while(1)
	{
		gdd_input_handler();
	}
	return 0;
}

static  ptu32_t gdd_server_thread(void)
{
	Djy_EventDelay(100*mS);
    printf("GDD_Server_Thread.\r\n");

    while(1)
    {
        GDD_Execu(pGkDesktop);
    }
    return 0;
}

void    ModuleInstall_GDD(struct tagGkWinRsc *desktop,const char *InputDevName[])
{
	u16 evtt;

	GDD_Init();

    pGkDesktop =desktop;

    GDD_InputDevName =InputDevName;

    ////gdd_server
    evtt_gui_server = Djy_EvttRegist(  EN_CORRELATIVE, CN_PRIO_RRS, 0, 0,
                           gdd_server_thread, NULL,2048,"gdd server");
  /*
    if (evtt != CN_EVTT_ID_INVALID)
    {
    	Djy_EventPop(evtt, NULL, 0, NULL, 0, 0);
    }
*/

    ////gdd_input
    evtt = Djy_EvttRegist(  EN_CORRELATIVE, CN_PRIO_RRS, 0, 0,
                          gdd_input_thread, NULL,2048,"gdd input");
    if (evtt != CN_EVTT_ID_INVALID)
    {
    	Djy_EventPop(evtt, NULL, 0, NULL, 0, 0);
    }


}
/*============================================================================*/
