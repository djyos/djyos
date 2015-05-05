// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：random.c
// 创建人员: Administrator
// 创建时间: 2013-3-7
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-7>, <修改人员>: <修改功能概述>
// =======================================================================
#include <stdlib.h>


#include "authencode.h"
#include "D3des.h"

void RandomData(rfbClientPtr cl)
{
  int i=0;
  static unsigned  int j=0;
  srand(j);
  j++;
  for(i=0;i<CHALLENGESIZE;i++)
  {
	  cl->authChallenge[i]= (u8)((rand())%256);
  }
}

/*
 * Encrypt CHALLENGESIZE bytes in memory using a password.
 */

void rfbEncryptBytes(unsigned char *bytes, char *passwd)
{
    unsigned char key[8];
    unsigned int i;

    /* key is simply password padded with nulls */

    for (i = 0; i < 8; i++) {
	if (i < strlen(passwd)) {
	    key[i] = passwd[i];
	} else {
	    key[i] = 0;
	}
    }

    rfbDesKey(key, EN0);

    for (i = 0; i < CHALLENGESIZE; i += 8) {
	rfbDes(bytes+i, bytes+i);
    }
}
