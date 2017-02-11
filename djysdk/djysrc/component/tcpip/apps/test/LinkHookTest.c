/*
 * LinkHookTest.c
 *
 *  Created on: 2016Äê11ÔÂ9ÈÕ
 *      Author: zhangqf
 */


#include <sys/socket.h>

bool_t  LinkHook_0807(ptu32_t handle,u16 proto,tagNetPkg *pkg)
{
	printf("dev:%08x protocol:0x%04x len:%d\n\r",handle,proto,pkg->datalen);
	return true;
}


bool_t LinkHook_ResgisterTest(u16 proto,const char *name)
{
	return LinkRegisterRcvHook(LinkHook_0807,NULL,proto,name);
}
bool_t LinkHook_UnResgisterTest(u16 proto,const char *name)
{
	return LinkUnRegisterRcvHook(LinkHook_0807,NULL,proto,name);
}
