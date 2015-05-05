//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:工程配置
//作者：lst
//版本：V1.0.0
//文件描述: 本文件罗列了具体工程所启用的中断，对使用了的每个中断，都会分配一个
//      struct int_line结构。这对于内存比较充足的系统来说，不成为负担，对这类系
//      统，把系统所有中断线都加上就可以了。
//          但对于内存非常有限且中断线众多的单片机来说，不管该中断是否被使用，都
//      分配一个struct int_line结构的话，内存表示亚历山大。实际工程中所用到的中
//      断线往往不多，在这里只把使用到了的中断线罗列一下就好。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 20130907
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "int.h"
#include "cpu_peri.h"
const ufast_t tg_IntUsed[] =
{
    cn_int_line_timer_event,
    cn_int_line_switch_context,
    cn_int_line_cmd,
};

//定义中断线控制数据结构
const ufast_t tg_IntUsedNum = sizeof(tg_IntUsed)/sizeof(ufast_t);


