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
//author:zhangqf
//date  :下午7:42:26/2017年2月24日
//usage :
//-----------------------------------------------------------------------------
#include "ghttp.h"
#include <stdio.h>
#include <stdlib.h>
//异步请求实例
void test001()
{
    char *uri = "http://www.hao123.com";
    ghttp_request *request = NULL;
    ghttp_status status;
    char *buf;
    int bytes_read;
    request = ghttp_request_new();
    if(ghttp_set_uri(request, uri) == -1)
        exit(-1);
    if(ghttp_set_type(request, ghttp_type_get) == -1)
        exit(-1);
    /* NOTE: Set async request */
    ghttp_set_sync(request, ghttp_async);
    ghttp_prepare(request);

    while(1) {
        status = ghttp_process(request);
        if(status == ghttp_error)
            break;
        /* NOTE: buf may NULL, notice it */
        buf = ghttp_get_body(request);


        bytes_read = ghttp_get_body_len(request);
        printf("%d\n %s\n",bytes_read,buf);
        if(status == ghttp_done) {
            /* NOTE: Ok, done */
            break;
        }
    }
    ghttp_clean(request);
    ghttp_request_destroy(request);
    system("pause");
}
//同步请求实例
int test002()
{
    char *uri = "http://www.hao123.com";
    ghttp_request *request = NULL;
    ghttp_status status;
    char *buf;
    int bytes_read;
    request = ghttp_request_new();
    if(ghttp_set_uri(request, uri) == -1)
        return (-1);
    if(ghttp_set_type(request, ghttp_type_get) == -1)
        return(-1);
    ghttp_prepare(request);
    status = ghttp_process(request);
    if(status == ghttp_error)
        return(-1);
    /* OK, done */
    printf("Status code -> %d\n", ghttp_status_code(request));
    buf = ghttp_get_body(request);
    printf("%s\n",buf);
    bytes_read = ghttp_get_body_len(request);
    ghttp_clean(request);
    ghttp_request_destroy(request);
    system("pause");
    return 0;
}

static u16 gIeEvttID;

int IeTask(void)
{
	char *url;
    char *uri;
    ghttp_request *request = NULL;
    ghttp_status status;
    char *buf;
    int bytes_read;

    while(1)
    {
    	Djy_WaitEvttPop(gIeEvttID,NULL,CN_TIMEOUT_FOREVER);
    	Djy_GetEventPara(&url,NULL);
    	if(NULL == url)
    	{
    		goto ONECE_MORE;
    	}

		uri = url;
	    request = ghttp_request_new();
	    if(NULL == request)
	    {
	    	printf("%s:create request failed\n\r",__FUNCTION__);
    		goto ONECE_MORE;
	    }
	    if(ghttp_set_uri(request, uri) == -1)
	    {
	    	printf("%s:set uri failed\n\r",__FUNCTION__);
    		goto CLEAN_REQUEST;
	    }
	    if(ghttp_set_type(request, ghttp_type_get) == -1)
	    {
	    	printf("%s:set type failed\n\r",__FUNCTION__);
    		goto CLEAN_REQUEST;
	    }
	    ghttp_prepare(request);
	    status = ghttp_process(request);
	    if(status == ghttp_error)
	    {
	    	printf("%s:process failed\n\r",__FUNCTION__);
    		goto CLEAN_REQUEST;
	    }
	    /* OK, done */
	    printf("Status code -> %d\n", ghttp_status_code(request));
	    bytes_read = ghttp_get_body_len(request);
	    printf("BodyLen:%d\n\r",bytes_read);
		buf = ghttp_get_body(request);
		printf("%s\n",buf);

CLEAN_REQUEST:
		ghttp_clean(request);
		ghttp_request_destroy(request);
ONECE_MORE:
 	 	printf("%s:once more!\n\r",__FUNCTION__);

    }
    return 0;
}
//
//ptu32_t IeTask(void)
//{
//
//	int result = 0;
//	result = UrlGet();
//	printk("IeResult:%d\n\r",result);
//	return 0;
//}



bool_t IeShell(char *param)
{
	if(NULL != param)
	{
		Djy_EventPop(gIeEvttID, NULL, 0, param, 0, 0);

		Djy_EventDelay(1000*mS);
	}
	return true;
}


#include <shell.h>
#include <os.h>

struct ShellCmdTab  gServiceUrl[] =
{
    {
        "ie",
		IeShell,
        "usage:ie",
        "usage:ie + http://host:port/dir?param",
    },
};

#define CN_URLDEBUG_NUM  ((sizeof(gServiceUrl))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gServiceUrlCmdRsc[CN_URLDEBUG_NUM];

//usage:use this function to add the url debug to the kernel
bool_t gHttpInstall(ptu32_t param)
{
	bool_t result= false;

	gIeEvttID = Djy_EvttRegist(EN_CORRELATIVE, 200, 0, 1,IeTask,NULL, 0X2000,"ie");
    if(gIeEvttID == CN_EVTT_ID_INVALID)
    {
    	return result;
    }
    result = Sh_InstallCmd(gServiceUrl,gServiceUrlCmdRsc,CN_URLDEBUG_NUM);
	return result;
}





