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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: 传输层资源树建立
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 7:46:28 PM/Jul 14, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "tpl.h"
// NET资源根节点
struct tagRscNode              *pgNetRscNodeTree = NULL;  //树根节点
static struct tagMutexLCB      *pgNetRscNodeSync = NULL;

typedef struct _tagNetRscNode
{
	struct tagRscNode node;
	int number;   //family 节点则为AF_INET等值
	            //type   节点则为STREAM等值
}tagNetRscNode;

// =============================================================================
// 函数功能：TransProto_GetTypeNode
//          根据指定的family和type找到指定的type资源节点
// 输入参数：family（域,AF_INET等）
//          type,(类型，SOCK_DGRAM等)
//          create,true则找不到对应的节点就建立，否则不建立
// 输出参数：无
// 返回值  ：返回找到的或者建立的type节点，如果不存在
// 说明    ：如果没有对应的FAMILY和TYPE则会创建对应的FAMILY或者TYPE
// =============================================================================
tagNetRscNode *__TPL_GetTypeNode(int family, int type, bool_t needcreate)
{
	bool_t  findit;
	tagNetRscNode *result;
	tagNetRscNode *temp;
	tagNetRscNode *headson;
	
	if(NULL != pgNetRscNodeTree)
	{
		findit = false;
		//先找family
		headson = (tagNetRscNode *)Rsc_GetSon(pgNetRscNodeTree);
		if(NULL == headson) //没有需要建立
		{
			findit = false;
		}
		else
		{
			temp = headson;
			do{
				if(temp->number == family)
				{
					findit = true;
					result = temp;
					temp = headson;
				}
				else
				{
					temp = (tagNetRscNode *)Rsc_GetNext(&temp->node);
				}
			}while(headson != temp);
		}
		if(true == findit)  
		{
			//找到了family节点，则在该family节点上查找type节点
			findit = false;
			headson = (tagNetRscNode *)Rsc_GetSon(&result->node);
			if(NULL == headson)
			{
				findit = false;
			}
			else
			{
				temp = headson;
				do{
					if(type == temp->number)
					{
						result = temp;
						findit = true;
						temp = headson;
					}
					else
					{
						temp = (tagNetRscNode *)Rsc_GetNext(&temp->node);
					}
					
				}while(temp != headson);
			}
			if(true != findit)//没有找到type节点
			{
				if(true == needcreate)
				{
					temp = result;
					result = M_Malloc(sizeof(tagNetRscNode), CN_TIMEOUT_FOREVER);
					if(NULL != result)
					{
						result->number = type;
						Rsc_AddSon(&temp->node, &result->node,\
								    sizeof(tagNetRscNode),RSC_RSCNODE,NULL);
					}
				}
				else
				{
					result = NULL;
				}
			}
		}
		else 
		{
			//没有找到family节点，看是否需要建立
			if(true == needcreate)
			{
				result = M_Malloc(sizeof(tagNetRscNode), CN_TIMEOUT_FOREVER);
				if(result != NULL)
				{
					result->number = family;
					Rsc_AddSon(pgNetRscNodeTree, &result->node, \
							sizeof(tagNetRscNode),RSC_RSCNODE,NULL);
					//建立对应的type节点
					temp = result;
					result = M_Malloc(sizeof(tagNetRscNode), CN_TIMEOUT_FOREVER);
					if(NULL != result)
					{
						result->number = type;
						Rsc_AddSon(&temp->node, &result->node, \
								sizeof(tagNetRscNode),RSC_RSCNODE,NULL);
					}
				}
			}
			else
			{
				result = NULL;
			}
		}	
	}
	return result;
}
// =============================================================================
// 函数功能：TPL_RegisterProto
//          添加一个具体的传输层协议
// 输入参数：family（域,AF_INET等）
//          type,(类型，SOCK_DGRAM等)
//          protocal,（协议号）
//          proto,具体的传输层协议
// 输出参数：
// 返回值  ：注册成功的资源树节点，NULL表示注册不成功（空间不足）
// 说明    ：如果没有对应的FAMILY和TYPE则会创建对应的FAMILY或者TYPE
// =============================================================================
tagTplProtoRscNode* TPL_RegisterProto(int family, int type, int protocal,\
		                           tagTlayerProto *proto)
{
	tagTplProtoRscNode *result = NULL;
	tagNetRscNode *typenode = NULL;
	tagTplProtoRscNode *headson = NULL;
	tagTplProtoRscNode *temp = NULL;

	if((NULL != proto)&&(NULL != pgNetRscNodeTree))
	{
		if(true == Lock_MutexPend(pgNetRscNodeSync, CN_TIMEOUT_FOREVER))
		{
			typenode = __TPL_GetTypeNode(family, type, true);
			if(NULL != typenode) //找到或者创建成功了
			{
				headson = (tagTplProtoRscNode *)Rsc_GetSon(&typenode->node);
				if(NULL == headson)
				{
					//现在的type下还没有节点，则完全可以建立新的
					result = M_Malloc(sizeof(tagTplProtoRscNode), CN_TIMEOUT_FOREVER);
					if(NULL != result)
					{
						result->protocal = protocal;
						result->proto = *proto;
						Rsc_AddSon(&typenode->node, &result->node,\
								sizeof(tagTplProtoRscNode),RSC_RSCNODE,NULL);
					}
				}
				else
				{
					//看看是否有重复的，有重复的不必再添加，否则要添加
					temp = headson;
				    do{
				    	if(temp->protocal == protocal)
				    	{
				    		result = temp;
				    		temp = headson;
				    	}
				    	else
				    	{
				    		temp = (tagTplProtoRscNode *)Rsc_GetNext(&temp->node);
				    	}
				    }while(temp != headson);
				    if(result != NULL) //已经有存在的
				    {
				    	result = NULL;
				    }
				    else
				    {
						//没有protocal的节点，建立
						result = M_Malloc(sizeof(tagTplProtoRscNode), CN_TIMEOUT_FOREVER);
						if(NULL != result)
						{
							result->protocal = protocal;
							result->proto = *proto;
							Rsc_AddSon(&typenode->node, &result->node,\
									sizeof(tagTplProtoRscNode),RSC_RSCNODE,NULL);
						}
				    }
				}
			}
			Lock_MutexPost(pgNetRscNodeSync);
		}	
	}
	return result;
}

// =============================================================================
// 函数功能：TPL_GetProto
//          添加一个具体的传输层协议
// 输入参数：family（域,AF_INET等）
//          type,(类型，SOCK_DGRAM等)
//          protocal,（协议号）
// 输出参数：
// 返回值  ：family家族的type类型的第protocal个协议，NULL表示没有指定的协议
// 说明    :
// =============================================================================
tagTlayerProto *TPL_GetProto(int family, int type, int protocal)
{
	tagTlayerProto  *result;
	tagNetRscNode  *typenode;
	tagTplProtoRscNode *headson;
	tagTplProtoRscNode *temp;
	
	result = NULL;
	
	if(true == Lock_MutexPend(pgNetRscNodeSync, CN_TIMEOUT_FOREVER))
	{
		typenode = __TPL_GetTypeNode(family, type, false);
		if(NULL != typenode) //找到或者创建成功了
		{
			headson = (tagTplProtoRscNode *)Rsc_GetSon(&typenode->node);
			if(NULL != headson)
			{
				//看看是否有重复的，有重复的不必再添加，否则要添加
				temp = headson;
			    do{
			    	if(temp->protocal == protocal)
			    	{
			    		result = &temp->proto;
			    		temp = headson;
			    	}
			    	else
			    	{
			    		temp = (tagTplProtoRscNode *)Rsc_GetNext(&temp->node);
			    	}
			    }while(temp != headson);
			}
		}
		Lock_MutexPost(pgNetRscNodeSync);
	}
	return result;
}



// =============================================================================
// 函数功能：TPL_Init
//          传输层资源树根节点初始化
// 输入参数：暂时无意义
// 输出参数：
// 返回值  ：true成功  false失败
// 说明    ：注意，该初始化函数应该在所有的传输层协议初始化之前执行，否则添加的协议无效
// =============================================================================
bool_t TPL_Init(ptu32_t para)
{
	bool_t result;
	static struct tagRscNode   treenode;
	
	result =false;
	//创建资源树
	pgNetRscNodeTree = Rsc_AddTree(&treenode,sizeof(struct tagRscNode), RSC_RSCNODE,NULL);
	if (NULL != pgNetRscNodeTree)
	{
		//创建资源树互斥锁
		pgNetRscNodeSync = Lock_MutexCreate(NULL);
		if (NULL == pgNetRscNodeSync)
		{
			Rsc_DelNode(pgNetRscNodeTree);
			pgNetRscNodeTree = NULL;
		}
		else
		{
			result = true;
		}
	}
	return result;
}



