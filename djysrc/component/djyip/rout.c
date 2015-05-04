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
// 模块描述: 通信的线路的选择和维护
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 11:35:21 AM/Jul 4, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "os.h"

#include "arp.h"
#include "rout.h"
#include "tcpip_cdef.h"

//RouteTabTree
//下述资源，当作为服务器时请配置大点，通信网络多的情况下也配置大
//RoutItem资源池
#define CN_NETROUTITEM_MAX         0x20
static tagNetRoutItem              sgRoutItemSpace[CN_NETROUTITEM_MAX]; //资源空间
static tagNetRoutItem              *pgItemFreeLst;                       //空闲队列

//关于Rout的几个全局变量
static tagRoutTab                  *pgRoutTabLst = NULL;   //ROUT链表
static tagRoutTab                  *pgLoopRout = NULL;     //LOOP回环链路
static tagRoutTab                  *pgRoutDefault= NULL;   //默认链路
#define CN_ROUTITEMSCAN_CYCLE      (5*60*1000*mS)          //每5分钟扫描
#define CN_ROUTITEM_LIFE           20                      //默认的20个生命周期
#define CN_ROUTITEM_WAITIME        (5*mS)                  //局域网应该非常快
static u8  sgBroadRoutMac[6] = {0xFF, 0XFF,0XFF,0XFF,0XFF,0XFF};
// =============================================================================
// 函数功能：分配一个RoutItem
// 输入参数：无
// 输出参数：
// 返回值  ：返回分配的空闲Item,NULL表示分配失败
// 说明    ：
// =============================================================================
tagNetRoutItem *__Rout_MallocItem(void)
{
	atom_low_t atom;
	tagNetRoutItem  *result = NULL;
	
	atom = Int_LowAtomStart();
	if(NULL != pgItemFreeLst)
	{
		result = pgItemFreeLst;
		pgItemFreeLst = result->next;		
		result->pre = NULL;
		result->next = NULL;	
	}
	Int_LowAtomEnd(atom);
	
	return result;
}
// =============================================================================
// 函数功能：释放一个RoutItem
// 输入参数：待释放的RoutItem
// 输出参数：
// 返回值  ：
// 说明    ：
// =============================================================================
void __Rout_FreeItem(tagNetRoutItem *item)
{
	atom_low_t atom;
	
	atom = Int_LowAtomStart();
	memset((char *)item,0, sizeof(tagNetRoutItem));
	item->next = pgItemFreeLst;
	if(NULL != pgItemFreeLst)
	{
		pgItemFreeLst->pre = item;
	}
	pgItemFreeLst = item;
	Int_LowAtomEnd(atom);
	
	return ;
}
// =============================================================================
// 函数功能：空闲RoutItem链表初始化
// 输入参数：无
// 输出参数：无
// 返回值  ：
// 说明    ：
// =============================================================================
void __Rout_FreeItemInit(void)
{
	u32 i = 0;
	
	memset((char *)&sgRoutItemSpace[0],0,CN_NETROUTITEM_MAX*sizeof(tagNetRoutItem));
	
	pgItemFreeLst = &sgRoutItemSpace[0];
	pgItemFreeLst->stat = EN_ROUTITEM_FREE;
	for(i = 1; i<CN_NETROUTITEM_MAX;i++ )
	{
		sgRoutItemSpace[i].next = pgItemFreeLst;
		pgItemFreeLst->pre = &sgRoutItemSpace[i];
		pgItemFreeLst = pgItemFreeLst->pre;
		pgItemFreeLst->stat = EN_ROUTITEM_FREE;
	}
	
	return ;
}
// =============================================================================
// 函数功能：__Rout_MatchRoutTab
//          用目的IP匹配一个Rout
// 输入参数：ipdst
// 输出参数：
// 返回值  ：tagRoutTab表，NULL表示没有符合条件的
// 说明    ：查找符合条件的Rout表,查找在同一个网络段的Rout,后续考虑负载均衡
// =============================================================================
tagRoutTab *__Rout_MatchRoutTab(u32 ipdst)
{
	tagRoutTab *result = NULL;
	tagRoutTab *tmp = NULL;
	
	tmp = pgRoutTabLst;
	
	while(NULL != tmp)
	{

		if((ipdst &tmp->netaddr.ipmsk )== (tmp->netaddr.ip &tmp->netaddr.ipmsk))
		{
			result = tmp;
			break;
		}
		else
		{
			tmp = tmp->nxt;
		}
	}
	return result;
}
// =============================================================================
// 函数功能：__Rout_MatchRoutItem
//          在Rout中查找匹配IP的Item
// 输入参数：rout,待搜索的链路表
//           ip，待匹配的IP
// 输出参数：
// 返回值  ：匹配IP的routitem,NULL表示没有匹配的
// 说明    ：
// =============================================================================
tagNetRoutItem *__Rout_MatchRoutItem(tagRoutTab *routtab, u32 ip)
{
	tagNetRoutItem *result = NULL;
	tagNetRoutItem *tmp = NULL;
	tmp = routtab->itemlst;
	while(tmp != NULL)
	{
		if(ip == tmp->ip) 
		{
			result = tmp;
			break;
		}
		else
		{
			tmp = tmp->next;
		}
	}
	
	return result;
}

// =============================================================================
// 函数功能：添加一个网络链路
// 输入参数：netdev,指定的网络设备
//          netaddr,netaddress
// 输出参数：
// 返回值  ：添加的链路
// 说明    ：
// =============================================================================
tagRoutTab * Rout_AddRout(tagNetDev *netdev,tagHostIpv4Addr  *netaddr)
{
	tagRoutTab           *result;
	tagRoutTab           *tmp;

	result = NULL;
	if((NULL == netdev)||(NULL == netaddr))
	{
		result = NULL;
	}
	else
	{
		//check if any rout has the same ip and netdev
		tmp = (tagRoutTab *)pgRoutTabLst;
		while(NULL != tmp)
		{
			if((netaddr->ip == tmp->netaddr.ip)&&
				(netdev == tmp->netdev))
			{
				result = tmp;
				break;
			}
			else
			{
				tmp = tmp->nxt;
			}
		}
		if(NULL != result)
		{
			//there has been the same rout
			printk("Rout:same rout has existed!\n\r");
			result = NULL;
		}
		else
		{
			//add the rout
			result = M_Malloc(sizeof(tagRoutTab),CN_TIMEOUT_FOREVER);
			if(NULL !=  result)
			{
				//初始化好，并且添加到RoutTab资源下
				result->itemlst = NULL;
				result->netdev = netdev;
				result->netaddr = *netaddr;
				result->itemsync = Lock_MutexCreate(NULL);
				
				//add it to the tablist
				result->nxt = pgRoutTabLst;
				pgRoutTabLst = result;
				//loop网卡还是不一样的
				if(EN_LINK_INTERFACE_LOOP == netdev->iftype)
				{
					pgLoopRout = result;
				}
				else
				{
					//add the gateway item
					result->itemlst = __Rout_MallocItem();
					result->itemlst->ip = result->netaddr.gateway;
					result->itemlst->type = EN_ROUTITEM_STATIC;
					result->itemlst->stat = EN_ROUTITEM_PENDING;
					result->itemlst->life = CN_ROUTITEM_LIFE;
					if(NULL == pgRoutDefault)
					{
						//if not defined yet, we use the first rout as the  default
						pgRoutDefault = result;
					}
				}
			}
		}	
	}
	return result;
}

// =============================================================================
// 函数功能：设置某个链路为默认链路
// 输入参数：待设置的链路
// 输出参数：
// 返回值  ：
// 说明    ：
// =============================================================================
bool_t Rout_SetDefault(tagRoutTab *rout)
{
	bool_t result = false;
	
	if(NULL != rout)
	{
		pgRoutDefault = rout;
		result = true;
	}
	return result;
}

// =============================================================================
// 函数功能：Rout_IsLocalIp
//          判定一个IP是不是本地IP
// 输入参数：ip,待判定的IP
// 输出参数：
// 返回值  ：true，是本地连接，否则不是本地连接
// 说明    ：该函数主要是用来判断上层的指定，到底是不是本地IP
//        INADDR_ANY和127.0.0.1一定是本地IP
//        否则遍历ROUT，看是否有对应的ROUT
// =============================================================================
bool_t  Rout_IsLocalIp(u32 ip)
{
	bool_t result = false;
	tagRoutTab *tmp;

	if(INADDR_ANY == ip)
	{
		result = true;
	}
	else
	{
		tmp = pgRoutTabLst;
		while(NULL != tmp)   //遍历所有的网卡
		{
			if(ip == tmp->netaddr.ip)
			{
				//the rout has the same ip, so bingo
				result = true;
				break;
			}
			else
			{
				tmp = tmp->nxt;
			}
		}
	}
	return result;
}

// =============================================================================
// 函数功能：判定目的IP是不是本地IP
// 输入参数：ip,待判定的IP
// 输出参数：
// 返回值  ：true目的地址是发给本地的，否则不是
// 说明    ：该函数和Rout_IsLocalIp的区别是该函数还要判断是不是广播IP
// =============================================================================
bool_t  Rout_IsLocalDst(u32 ip)
{
	bool_t result = false;
	tagRoutTab *tmp;

	if(INADDR_ANY == ip)
	{
		result = true;
	}
	else
	{
		//遍历所有的网卡
		tmp = pgRoutTabLst;
		while(NULL != tmp)
		{
			if((ip == tmp->netaddr.ip)||\
			   ((ip&tmp->netaddr.ipmsk)==(tmp->netaddr.ip&tmp->netaddr.ipmsk)))
			{
				//the rout has the same ip, so bingo
				result = true;
				break;
			}
			else
			{
				tmp = tmp->nxt;
			}
		}
	}
	return result;
}
// =============================================================================
// 函数功能：查找一个通信链路
// 输入参数：ipdst,所要到达的目的IP
// 输出参数：outpara,查找到的对应链路
// 返回值  ：true，查找成功，false，没有查找到，只有true时outpara的内容才有意义
// 说明    ：1,本地网络直接使用LOOP网卡
//        2,外地网络直接使用默认的网络的网关的MAC地址
//        3,同一个子网的则找对应的Item,否则发送ARP请求
// =============================================================================
bool_t Rout_Resolve(u32 ipdst, tagRoutLink *outpara)
{
	bool_t               result;
	tagNetRoutItem       *routitem;
	tagRoutTab           *rout;
	
	result = false;
	rout = __Rout_MatchRoutTab(ipdst);
	if(NULL == rout)
	{
		//证明是外网,OK，只要发送给默认网卡的网关即可
		if(NULL != pgRoutDefault)
		{
			outpara->dev = pgRoutDefault->netdev;
			outpara->ip = pgRoutDefault->netaddr.ip;
			routitem = __Rout_MatchRoutItem(rout,pgRoutDefault->netaddr.ip);
			if(routitem->stat == EN_ROUTITEM_PENDING)
			{
				Arp_Request(rout->netdev,ipdst,rout->netaddr.ip);
				Lock_MutexPost(rout->itemsync);
				Djy_EventDelay(CN_ROUTITEM_WAITIME);
				Lock_MutexPend(rout->itemsync, CN_TIMEOUT_FOREVER);
				if(routitem->stat == EN_ROUTITEM_STABLE)
				{
					outpara->ip = rout->netaddr.ip;
					outpara->dev = rout->netdev;
					memcpy(outpara->mac,routitem->mac,6);
					result = true;
				}
			}
			else
			{
				outpara->ip = rout->netaddr.ip;
				outpara->dev = rout->netdev;
				memcpy(outpara->mac,routitem->mac,6);
				result = true;
			}			
			result = true;
		}
	}
	else
	{
		if(Lock_MutexPend(rout->itemsync, CN_TIMEOUT_FOREVER))
		{
			//本地网络，那么要看看是使用LOOP还是其他
			if(ipdst == rout->netaddr.ip)
			{
				//loop
				if(NULL != pgLoopRout)
				{
					result = true;
					outpara->dev = pgLoopRout->netdev;
					outpara->ip =pgLoopRout->netaddr.ip;
					memcpy(outpara->mac,pgLoopRout->netdev->mac,6);
					result = true;
				}
			}
			else if(ipdst == ((~rout->netaddr.ipmsk)|((rout->netaddr.ip&rout->netaddr.ipmsk))))
			{
				//广播网络
				outpara->ip = rout->netaddr.ip;
				outpara->dev = rout->netdev;
				memcpy(outpara->mac,sgBroadRoutMac,6);
				result = true;
			}
			else
			{
				//本地网络，查找Item
				routitem = __Rout_MatchRoutItem(rout,ipdst);
				if(NULL == routitem)
				{
					//需要建立
					routitem = __Rout_MallocItem();
					if(NULL != routitem)
					{
						routitem->ip = ipdst;
						routitem->life = CN_ROUTITEM_LIFE;
						routitem->type = EN_ROUTITEM_DYNAMIC;
						routitem->stat = EN_ROUTITEM_PENDING;
						//add the routitem to the rout lsrt
						routitem->next = rout->itemlst;
						if(NULL != rout->itemlst)
						{
							rout->itemlst->pre = routitem;
						}
						rout->itemlst = routitem;
					}
				}
				if(routitem->stat == EN_ROUTITEM_PENDING)
				{
					Arp_Request(rout->netdev,ipdst,rout->netaddr.ip);
					Lock_MutexPost(rout->itemsync);
					Djy_EventDelay(CN_ROUTITEM_WAITIME);
					Lock_MutexPend(rout->itemsync, CN_TIMEOUT_FOREVER);
					if(routitem->stat == EN_ROUTITEM_STABLE)
					{
						outpara->ip = rout->netaddr.ip;
						outpara->dev = rout->netdev;
						memcpy(outpara->mac,routitem->mac,6);
						result = true;
					}
				}
				else
				{
					outpara->ip = rout->netaddr.ip;
					outpara->dev = rout->netdev;
					memcpy(outpara->mac,routitem->mac,6);
					result = true;
				}
			}
			Lock_MutexPost(rout->itemsync);
		}
	}
	
	return result;
}
// =============================================================================
// 函数功能：收到通信方信息时，主动更新我们的Route条目
// 输入参数：ip,更新的IP
//          mac,对应于IP的MAC
// 输出参数：无
// 返回值  ：无
// 说明    ：更新IP对应的RoutItem
// =============================================================================
void Rout_ItemUpdate(u32 ip, u8 *mac)
{
    tagRoutTab        *rout;
    tagNetRoutItem    *routitem;    
    if(NULL != mac)
    {
        rout = __Rout_MatchRoutTab(ip);
        if(NULL != rout)
        {
        	if(Lock_MutexPend(rout->itemsync, CN_TIMEOUT_FOREVER))
        	{
        		routitem = __Rout_MatchRoutItem(rout,ip);
        		if(NULL!= routitem)
        		{
        			routitem->life = CN_ROUTITEM_LIFE;
        			routitem->stat = EN_ROUTITEM_STABLE;
        			memcpy(routitem->mac, mac, 6);   //会引起ARP攻击
        		}
        		else
        		{
        			//既然没有匹配的，如果非广播之类的，应该可以建立起对应的Item
        			//毕竟可能很快就会通信了
        			if(ip != ((~(rout->netaddr.ip&rout->netaddr.ipmsk))|rout->netaddr.ip))
        			{
            			routitem = __Rout_MallocItem();
            			if(NULL != routitem)
            			{
    						routitem->ip = ip;
    	        			memcpy(routitem->mac, mac, 6);  
    	        			routitem->life = CN_ROUTITEM_LIFE;
    	        			routitem->stat = EN_ROUTITEM_STABLE;
    						routitem->type = EN_ROUTITEM_DYNAMIC;
    						//add the routitem to the rout lsrt
    						routitem->next = rout->itemlst;
    						if(NULL != rout->itemlst)
    						{
    							rout->itemlst->pre = routitem;
    						}
    						rout->itemlst = routitem;
            			}
        			}		
        		}
				Lock_MutexPost(rout->itemsync);
        	}
        }
    }
  
	return;
}

// =============================================================================
// 函数功能：Rout_ItemScanTask
//          定时刷新Route表项
// 输入参数：无
// 输出参数：
// 返回值  ：无
// 说明    ：该函数是Route的内置线程
//        运行周期为1Minute，当Item的生命周期终结的时候，该Item将会被删除
// =============================================================================
void Rout_ItemScanTask(void)
{
	tagRoutTab      *rout;
	tagNetRoutItem  *item;
	tagNetRoutItem  *itemtmp;
	
	while(1)
	{
		rout = pgRoutTabLst;
		while(NULL != rout)
		{
			//每个rout的遍历
			Lock_MutexPend(rout->itemsync, CN_TIMEOUT_FOREVER);
			item = rout->itemlst;
			while(NULL != item)
			{
				itemtmp = item->next;
				if(EN_ROUTITEM_DYNAMIC ==item->type)
				{
					item->life--;
					if(0 == item->life)
					{
						//del the item for it is timeout
						if(NULL != item->pre)
						{
							item->pre->next = item->next;
						}
						if(NULL != item->next)
						{
							item->next->pre = item->pre;
						}
						__Rout_FreeItem(item);
					}
				}
				item = itemtmp;
			}//扫描指定rout的所有item
			Lock_MutexPost(rout->itemsync);			
			rout = rout->nxt;	
		}
		Djy_EventDelay(CN_ROUTITEMSCAN_CYCLE);
	}
}
// =============================================================================
// 函数功能：Rout_Init
//          链路初始化
// 输入参数：无
// 输出参数：
// 返回值  ：true成功 false失败
// 说明    ：1,建立Route内存池
//        2,建立Route表项内存池
//        3,建立Route表项对应的HOOK内存池
//        3,初始化Route树资源，该树无需锁进行同步，因为对该树的遍历或者添加删除都采用
//          原子性操作，由于该树队列不是很长，所以对系统性能影响不大
// =============================================================================
bool_t Rout_Init(void)
{
	u16    evtt_id;	
	bool_t result = true;
	//全局变量初始化
	//初始化Item链表
	__Rout_FreeItemInit();
	//添加Rout表扫描任务
    evtt_id = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
    		(ptu32_t (*)(void))Rout_ItemScanTask,NULL,0x1000, "Rout Tab Scan");
	if (evtt_id == CN_EVTT_ID_INVALID)
	{
		printk("Rout:Creat RoutScanEvent Failed\n\r");
		result = false;
	}
	else
	{
		evtt_id = Djy_EventPop(evtt_id, NULL, 0, NULL,NULL, 0);
		printk("Rout:Creat RoutScanEvent Success\n\r");
	}
	return result;
}





