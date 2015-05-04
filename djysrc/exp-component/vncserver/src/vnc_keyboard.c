// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名称：vncdisplay.c
// 模块描述: vncserver
// 模块版本: V1.00
// 创建时间: 2013-6-24
// 原始作者: Administrator
// 功能说明：该头文件用以实现操作键盘显示的接口
// =============================================================================
#include "stdint.h"
#include "vnc_keyboard.h"
#include "cpu_peri.h"
#include "keysymdef.h"
#include "rfbproto.h"
#include "debug.h"
#include "vnc_bmp.h"//从文件中初始化keymap

#if CN_CFG_KEYBOARD == 1

static u32 vnc_key_value = 0;
//按键值，允许组合键的接入，但是仅仅支持三个按键同时按下，对于VNC而言，需要先按下组合
//键，然后按下需要组合的按键 ，然后按下需要组合的键，然后按下组合键表示组合结束
//用key_value的高8位的最低位来表示是不是组合键
static u8  vnc_key_flag = 0;


#define cn_vnc_keyboard_name "vnc keyboard"
#define cn_vnc_touchlcd_name  "vnc_touchboard"
// =============================================================================
// 函数功能:vnc按键读取函数
// 输入参数:server
// 输出参数：
// 返回值:
// 说明:资源不需要用临界资源锁保护，因为不涉及到销毁问题，对实时性要求不高
// =============================================================================
u32 vnc_keyboard_read(void)
{
    if(vnc_key_flag)//该标志会由vnc的消息服务函数进行设置，当按键完成以后才会激活
    {
//      debug_printf("read an key =%d \n",vnc_key_value);
        vnc_key_flag = 0x00;

        return vnc_key_value;
    }
    else
    {
    //  debug_printf("no key---vnc soft keyboard!\n");
        return 0;
    }
}

//cyg装置专用键盘的映射关系
//const u8 cyg_keyboard[cn_vnc_key_num] = {37,38,39,40,13,27,112,113,cn_vnc_func_key};
const u8 cyg_keyboard[cn_vnc_key_num] = {37,38,39,40,13,27,112,113};
// =============================================================================
// 函数功能:实现键盘值的转换
// 输入参数:key，需要转化的键盘值
// 输出参数：
// 返回值:转换后的键盘值（0XFF是找不到的情况）
// 说明:我们希望输入的键盘值都是0-32最好，实际上不是这样的，各种编码让你搓手不及，
//      此函数的用意就是将键盘值转换为我们需要的0-32，好用于标记。因此不同的键盘的值
//      自己进行转换，于应用相关
// =============================================================================
u8  translate_key(u8 key)
{
    u8 i = 0;
    for(i=0; i < cn_vnc_key_num; i++)
    {
        if(cyg_keyboard[i] == key)
        {
            return i;
        }
    }
    return cn_invalid_key;//找不到对应的key
}
// =============================================================================
// 函数功能:vnc键盘驱动安装
// 输入参数:server,服务器
// 输出参数：
// 返回值:
// 说明:之所以放在server中，是因为server退出时必然要将此device删除
// =============================================================================
bool_t install_vncserver_keyboard(rfbScreenInfoPtr server)
{
    s32 ret = 0;
    server->vnc_keyboard.key_bak = 0;
    server->vnc_keyboard.key_now = 0;
    server->vnc_keyboard.vtime_limit = 0;
    server->vnc_keyboard.read_keyboard = vnc_keyboard_read;
    ret = Keyboard_InstallDevice(cn_vnc_keyboard_name,&(server->vnc_keyboard));
    if(-1==ret)
    {
        debug_printf("##failed to install the vnc keyboard!\n");
    }
    else
    {
        debug_printf("--succeed to install the vnc keyboard!\n");
    }
    return 1;
}

// =============================================================================
// 函数功能:vnc触摸点读取函数
// 输入参数:
// 输出参数：touch_data,用于存储读取的坐标值
// 返回值:
// 说明:
// =============================================================================
static s32  vnc_touch_x = 0;
static s32  vnc_touch_y = 0;
static s32  vnc_touch_z = 0;
static bool_t   vnc_touched_flag = false;
bool_t read_touch_vnclcd(struct tagSingleTouchMsg *touch_data)
{
    if(vnc_touched_flag == true)
    {
        touch_data->x = vnc_touch_x;
        touch_data->y = vnc_touch_y;
        touch_data->z = vnc_touch_z;
        vnc_touched_flag = false;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:vnc触摸驱动安装
// 输入参数:server,服务器，msg 鼠标信息
// 输出参数：
// 返回值:
// 说明:之所以放在server中，是因为server退出时必然要将此device删除
// =============================================================================
bool_t install_vncserver_touchlcd(rfbScreenInfoPtr server)
{
    s32 ret = 0;

    server->vnc_lcdtouch.read_touch = read_touch_vnclcd;
    ret = Touch_InstallDevice(cn_vnc_touchlcd_name,&(server->vnc_lcdtouch));
    if(ret)
    {
        debug_printf("##failed to install the vnc touchlcd!\n");
    }
    else
    {
        debug_printf("--succeed to install the vnc touchlcd!\n");
    }
    return 1;
}
// =============================================================================
// 函数功能:当从一个客户端接受到一个按键时的举动
// 输入参数:
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
void vnc_input_key_message(rfbScreenInfoPtr server,u8 key_number, bool_t down)
{
    static u8 key_received = 0;//用于组合键
    static u8 multi_key = 0;  //标记是否有组合键按下
    static u32  key_bak = 0;//存储多次按键用
    u8  keyvalue = 0;//临时变量
    u8  i = 0;//临时变量
    u8 keyvalue_bak = 0;//临时变量
    u8 keyvalue_numberbak;//临时变量
    u8 keydel_flag = 0;//取消组合键的按键标记

    if(key_number >= cn_vnc_key_num)
    {
        return;//意外的按键,没有啥子好做的
    }
    keyvalue = cyg_keyboard[key_number];
//开始处理对按键的视觉处理
    if(multi_key) //是组合键，已经有组合键按下了
    {
        if(cn_vnc_func_key == keyvalue)
        {
            if(cn_key_down == down)//功能键再次被按下，表示组合结束
            {
                //将前述的按键弹起吧少年
                debug_printf("has received =%d key,keybak=%08x\n",key_received,key_bak);
                for(i=0; i<key_received; i++ )
                {
                    keyvalue_bak = (u8)((key_bak>>(i<<0x03))&0xFF);
                    keyvalue_numberbak = translate_key(keyvalue_bak);
                    //标记视觉更新
                    server->key_update_list = \
                            server->key_update_list ^(1<<keyvalue_numberbak);
                    //标记为弹起状态
                    server->key_state = \
                     server->key_state &(~(cn_key_down << keyvalue_numberbak));
                    debug_printf("multi up key= %d keynumver = %d\n",keyvalue_bak,keyvalue_numberbak);
                }
                //功能键自己也弹起吧,少年
                server->key_update_list = \
                        server->key_update_list ^(1<<key_number);
                //标记为弹起状态
                server->key_state = \
                 server->key_state &(~(cn_key_down << key_number));

                multi_key = 0;//组合键结束
                key_received = 0;//清零
                //拷贝到vnc 的key board驱动当中
                vnc_key_value = key_bak;
                key_bak = 0;
                vnc_key_flag = 1;
            }
            else//组合键只有再次按下视觉才会恢复正常，弹起是没有用的
            {

            }
        }
        else//继续增加或者取消已经选择的按键。
        {
            if(down)
            {
                //看看是不是已经存在于组合键中，那样的话就表示取消
                keydel_flag = 0x00;
                for(i = 0; i < key_received; i++)
                {
                    keyvalue_bak = (u8)((key_bak>>(i<<0x03))&0xFF);
                    if(keyvalue_bak == keyvalue)
                    {
                        keydel_flag = 0x01;//是要取消此按键的
                    }
                }
                if(keydel_flag)//取消按键
                {
                    //标记视觉更新
                    server->key_update_list = \
                            server->key_update_list ^(1<<key_number);
                    //标记为弹起状态
                    server->key_state = \
                     server->key_state &(~(cn_key_down << key_number));
                    key_received--;
                }
                else//新增组合键
                {
                    //看看是否超超标，超标的话会取消组合的最后一个键
                    if(key_received >= cn_max_multi_key_num)//实际上最多等于
                    {
                        key_received--;
                        //获取最后一次按键
                        keyvalue_bak = \
                                (u8)((key_bak >>(key_received<<0x03)) &0xFF);
                        keyvalue_numberbak = translate_key(keyvalue_bak);
                        //需要更新视觉效果一次
                        server->key_update_list = \
                                server->key_update_list ^(1<<keyvalue_numberbak);
                        //标记为弹起状态
                        server->key_state = \
                         server->key_state &(~(cn_key_down << keyvalue_numberbak));

                        key_bak=key_bak &(~((u32)(0xFF<<(key_received<<0x03))));
                        key_bak |=(u32)(keyvalue)<<(key_received<<0x03);
                        //需要更新视觉效果一次
                        server->key_update_list = \
                                server->key_update_list ^(1<<key_number);
                        //标记为按下状态
                        server->key_state = \
                                server->key_state |(cn_key_down << key_number);
                        key_received++;

                    }
                    else//没有超标
                    {
                        key_bak=key_bak &(~((u32)(0xFF<<(key_received<<0x03))));
                        key_bak =key_bak |(u32)((keyvalue)<<(key_received<<0x03));
                        //需要更新视觉效果一次
                        server->key_update_list = \
                                server->key_update_list ^(1<<key_number);
                        //标记为按下状态
                        server->key_state = \
                                server->key_state |(cn_key_down << key_number);
                        key_received++;

                    }
                }
            }
            else//在组合键的威力下，弹起本身是没有用的，抱歉啊
            {

            }

        }

    }
    else
    {
        if(cn_vnc_func_key == keyvalue)//读取的是功能键
        {
            if(down)//功能键第一次按下
            {
                //需要更新视觉效果一次
                server->key_update_list = \
                        server->key_update_list ^(1<<key_number);
                //标记为按下状态
                server->key_state = \
                        server->key_state |(cn_key_down << key_number);
                multi_key = 1;
                key_received = 0;
                vnc_key_flag = 0;
                key_bak = 0;
            }
            else//功能键两次按下方可弹起,因此功能键的弹起是没有意义的
            {

            }
        }
        else//普通按键
        {
            //按键的状态相同，不用处理
            if( (((u8)(server->key_state >> key_number)&0x01)) == down)//比较标志位
            {

            }
            else//进行状态的标记更新
            {
                server->key_update_list = \
                        server->key_update_list ^(1<<key_number);
                //根据状态来进行相应的标记
                if(down)
                {
                    server->key_state = \
                            server->key_state |(cn_key_down << key_number);
                    vnc_key_value = (u32)keyvalue;
                    multi_key = 0;
                    vnc_key_flag = 1;
                }
                else
                {
                    server->key_state = \
                     server->key_state &(~(cn_key_down << key_number));
                    debug_printf("key up = %d\n",key_number);
                }

            }
        }
    }

    return;
}
// =============================================================================
// 函数功能:向server写一个触摸点阵
// 输入参数:
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
void vnc_input_touch_message(int x, int y, int z)
{
    vnc_touch_x = x*X_SCALE;
    vnc_touch_y = y*Y_SCALE;
    vnc_touch_z = z;
    vnc_touched_flag = true;
}

// =============================================================================
// 函数功能:实现消息响应中对于客户端键盘消息的响应
// 输入参数:server,服务器，key，key值，state，key的状态
// 输出参数：
// 返回值:
// 说明:键盘的值目前都是单键值，后续考虑多键值的加入--TODO
// =============================================================================
void  vncclient_keyevent(rfbScreenInfoPtr server, rfbClientToServerMsg msg)
{
    u8 key_translate = 0xFF;
    u8 key = 0;
    u8 state = 0;

//test
    static u8 times = 0;
//  从keysymdef转换到DJY自己的八键键盘
    switch (msg.ke.key)
    {
        case XK_Return://enter
            key = 13;
            break;
        case XK_Left:
            key=37;
            break;
        case XK_Right:
            key=39;
            break;
        case XK_Up:
            key=38;
            break;
        case XK_Down:
            key=40;
            break;
        case XK_Escape:
            key=27;
            break;
        default:
            key=(u8)(msg.ke.key);
            break;
    }
    state = msg.ke.down;
//    debug_printf("--client key =%d\n",key);
    key_translate = translate_key(key);//转换成0开始的按键


    if(key_translate == cn_invalid_key)
    {
        return;//找不到，那就什么都不用做了
    }
    //和0还是本身，和1异或取反
    //向拥有键盘的事件发送消息。
    vnc_input_key_message(server,key_translate, state);
    return;//结束
}
// =============================================================================
// 函数功能:实现服务器中键盘对keylist的影响
// 输入参数:server,服务器，key，key值，state，key的状态
// 输出参数：
// 返回值:
// 说明:键盘的值目前都是单键值，后续考虑多键值的加入--TODO
// =============================================================================
void  vncserver_keyevent(rfbScreenInfoPtr server, u8 key, u8 state)
{
    u8 key_translate = 0xFF;

    key_translate = translate_key(key);
    if(key_translate == cn_invalid_key)
    {
        return;//找不到，那就什么都不用做了
    }
    //判断是否是同一个按键
    if( (server->key_state) & (state<<key_translate))
    {
        return ;
    }//多次up或者down一般的而言是不可能的，除非是触摸样式，即便那样，也是没有意义的

    //新的按键
    if(cn_key_down == state)
    {
        server->key_state = server->key_state &(~(cn_key_up << key_translate));
    }
    else
    {
        server->key_state = server->key_state &(~(cn_key_up << key_translate));
    }
    server->key_update_list = server->key_update_list ^(1<<key_translate);
    //和0异火还是本身，和1异或取反
    return;//结束
}

// =============================================================================
// 函数功能:将单bit数据流映射成565颜色的格式的数据
// 输入参数:dest，数据存储目的地址，src，数据转换源，data1,bit1所对应的数据，
//         data0 bit0所对应的数据，cout源数据的字节数,times,映射的次数
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
static void changeB2W(u8* dest, u8* src, u32 count, u16 data1, u16 data0,u16 times)
{
   u8 i=0,j=0;
   u8  data;
   u16 mapTimes = 0;
   for(i=0;i<count;i++)//字节数
   {
      data=*((u8 *)(src+i));
      for(j=0;j<8;j++)//自己位操作
      {
         if(data&(1<<(7-j)))
         {
           *((u16 *)(dest+i*16+j*2))=data1;
         }
         else
         {
             *((u16 *)(dest+i*16+j*2))=data0;
         }

         mapTimes++;
         if(mapTimes == times)//映射次数到了
         {
             return;
         }
      }
    }
}
// =============================================================================
// 函数功能:将24位888格式的数据转换为16（565格式的数据）
// 输入参数:cout表示要转换的次数
// 输出参数：
// 返回值:
// 说明:RGB,R在高位
// =============================================================================
static void change24to16(u8* dest, u8* src, u32 count)
{
   u16 i=0;
   u8 *src_buf;
   u16 *dst_buf;
   src_buf = src;
   dst_buf =(u16 *)dest;

   for(i=0; i < count; i++)//字节数
   {
       *dst_buf = ((*(src_buf))&0x1F)|(((*(src_buf+1))&0x3F)<<0x05)|\
       (((*(src_buf+1))&0x1F)<<0x0B);//(bgr顺序)

       src_buf += 3;
       dst_buf++;
      //  debug_printf("src buf =%08x  dst_buf=%08x\n",src_buf,dst_buf);
   }
}

// =============================================================================
// 函数功能:将bitmap拷贝进内存
// 输入参数:cout表示要转换的次数
// 输出参数：
// 返回值:
// 说明:需要限制，不要拷贝出frambuf的范围，不然的话不可想象。
// =============================================================================
void vnc_cp_bitmap2serverbuf(rfbScreenInfoPtr server,\
        struct tagRectBitmap *bitmap,u16 x, u16 y)
{
    u32 i = 0;//指示高度
    u32 bm_height = 0;
    u32 bm_width = 0;
    u32 bm_linebytes = 0;
    u32 fm_linebytes = 0;
    u8 *frambuf;
    u8 *bitbuf;
    u32 offset = 0;

    if(NULL == bitmap->bm_bits)
    {
        return;
    }
    //拷贝的起始点应该在framebuf的范围内
    //防止任意的X 和 Y
    if((x >= server->width)||(y >= server->height))
    {
        return;//既然这样，是么有什么好做的
    }
    //防止拷贝出界，不然死的很惨
    if((x+bitmap->width) > server->width)
    {
        bm_width = server->width - x;
    }
    else
    {
        bm_width = bitmap->width;
    }
    if((y + bitmap->height) > server->height)
    {
        bm_height = server->height - y;
    }
    else
    {
        bm_height = bitmap->height;
    }

    bm_linebytes = bitmap->linebytes;
    fm_linebytes = ((server->rfbServerFormat.bitsPerPixel+7)/8)*server->width;
    bitbuf = bitmap->bm_bits ;
    offset = fm_linebytes*y + x*((server->rfbServerFormat.bitsPerPixel+7)/8);
    frambuf = (u8 *)(server->frameBuffer + offset);
    for(i=0; i < bm_height; i++)
    {
        if(CN_SYS_PF_GRAY1 == bitmap->PixelFormat)//单bit的map
        {
            changeB2W(frambuf, bitbuf, bm_linebytes, 0x4444,0x9999, bm_width);
        }
        else if(CN_SYS_PF_RGB888 == bitmap->PixelFormat)//24位的map
        {
            change24to16(frambuf, bitbuf,bm_width);
        }
        frambuf += fm_linebytes;
        bitbuf += bm_linebytes;
    }
}
static struct tagRectBitmap  key_map[cn_vnc_key_num];
static struct tagRectBitmap  keyboardmap;

u8 keydown_map[32]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        0x00,0x00,0x00,0x00,0x00,0x70,0x0E,0x38,0x1C,0x1C,0x38,0x0E,0x70,0x07,\
        0xE0,0x03,0xC0,0x01,0x80,0x00,0x00,0x00,0x00};

u8 keyup_map[32]={0x00,0x00,0x00,0x00,0x01,0x80,0x03,0xC0,0x07,0xE0,0x0E,0x70,\
        0x1C,0x38,0x38,0x1C,0x70,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        0x00,0x00,0x00,0x00,0x00,0x00};

u8 keyleft_map[32]={0x00,0x00,0x00,0x80,0x01,0x80,0x03,0x80,0x07,0x00,0x0E,\
        0x00,0x1C,0x00,0x38,0x00,0x38,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,\
        0x80,0x01,0x80,0x00,0x80,0x00,0x00};

u8 keyright_map[32]={0x00,0x00,0x01,0x00,0x01,0x80,0x01,0xC0,0x00,0xE0,0x00,\
        0x70,0x00,0x38,0x00,0x1C,0x00,0x1C,0x00,0x38,0x00,0x70,0x00,0xE0,0x01,\
        0xC0,0x01,0x80,0x01,0x00,0x00,0x00};

u8 keyenter_map[32]={0x00,0x00,0x00,0x00,0x7F,0x42,0x7F,0x62,0x60,0x72,0x60,\
        0x5A,0x7F,0x5A,0x5F,0x5A,0x7F,0x5A,0x60,0x4E,0x60,0x46,0x60,0x46,0x7F,\
        0x42,0x7F,0x42,0x00,0x00,0x00,0x00};

u8 keyesc_map[32]={0x00,0x00,0x00,0x00,0x7F,0x0E,0x7F,0x11,0x60,0x20,0x60,0x20,\
        0x7F,0x20,0x5F,0x20,0x7F,0x20,0x60,0x20,0x60,0x20,0x60,0x20,0x7F,0x11,\
        0x7F,0x0F,0x00,0x00,0x00,0x00};

u8 keyf1_map[32]={0x00,0x00,0x3F,0xBC,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,\
        0x1F,0x98,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x18,\
        0x00,0x00,0x00,0x00,0x00,0x00};

u8 keyf2_map[32]={0x00,0x00,0x3F,0xBF,0x1F,0x01,0x18,0x01,0x18,0x01,0x18,0x01,\
        0x1F,0x9F,0x1F,0x3E,0x18,0x20,0x18,0x20,0x18,0x20,0x18,0x20,0x18,0x21,\
        0x18,0x3F,0x10,0x00,0x00,0x00};
u8 keyfn_map[32]={0x00,0x00,0x3F,0xA1,0x3F,0x31,0x30,0x29,0x30,0x2D,0x30,0x2F,\
        0x3F,0xAF,0x3F,0x2F,0x30,0x2F,0x30,0x27,0x30,0x23,0x30,0x21,0x30,0x21,\
        0x20,0x21,0x00,0x00,0x00,0x00};//组合键按钮 0x80
// =============================================================================
// 函数功能:为bitmap填充具体的图形，图形来自内存
// 输入参数:server,服务器
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
static void vnc_init_key_bitmap(struct tagRectBitmap *bitmap)
{
    u8 i = 0;

//初始化为8副单bit位图
    bitmap[0].bm_bits = keyleft_map;
    bitmap[1].bm_bits = keyup_map;
    bitmap[2].bm_bits = keyright_map;
    bitmap[3].bm_bits = keydown_map;
    bitmap[4].bm_bits = keyenter_map;
    bitmap[5].bm_bits = keyesc_map;
    bitmap[6].bm_bits = keyf1_map;
    bitmap[7].bm_bits = keyf2_map;
//增加功能键
    bitmap[8].bm_bits = keyfn_map;
    for(i = 0; i < cn_vnc_key_num; i++)
    {
        bitmap[i].PixelFormat = CN_SYS_PF_GRAY1;//单bit灰色
        bitmap[i].width = cn_vnc_key_width;
        bitmap[i].height = cn_vnc_key_height;
        bitmap[i].linebytes = 2;//
    }
}
// =============================================================================
// 函数功能:为bitmap填充具体的图形，图形来自文件
// 输入参数:server,服务器
// 输出参数：
// 返回值:
// 说明:bitmapde bits的内存在此动态分配
// =============================================================================
static void vnc_init_key_bitmap_from_file(struct tagRectBitmap *bitmap)
{
    u8 i = 0;
    char file_name[30] = "d:\\keymap\\keymap0.bmp";
    u8 file_namelen = strlen(file_name);
    for(i =0; i < cn_vnc_key_num; i++)
    {

        vnc_get_image_from_bmp(file_name,&(bitmap[i]));
        file_name[file_namelen-5]++;//从0开始数的文件，对应着key键列表,.bmp后缀
    }
}

static void vnc_init_keyboardbackground_from_file(struct tagRectBitmap *bitmap)
{

    char file_name[30] = "d:\\keymap\\keyboard.bmp";
    vnc_get_image_from_bmp(file_name,&keyboardmap);
}

//之所以做成全局变量，是为了后续的找键和按键找位置更方便
static u8 vnc_line_keynum = 0;//每行所放的key数目
static u8 vnc_keyline_num = 0;//一共放的key的行数
static u8 vnc_key_num = 0;// 要显示的key的数目
static u16 vnc_space_h_left = 0;//放置按键后剩余的土地——水平方向
static u16 vnc_space_v_left = 0;//放置key后剩余的土地-垂直方向
// =============================================================================
// 函数功能:实现vncserver对键盘的可视化的初始化
// 输入参数:server,服务器
// 输出参数：
// 返回值:
// 说明:键盘的值目前都是单键值，后续考虑多键值的加入--TODO
// =============================================================================
bool_t vnc_keyboard_visual_init(rfbScreenInfoPtr server)
{
    u8 i=0;
    u8 keyline_num_visual = 0;//一共放的key的行数
    u16 x = 0;
    u16 y = 0;
    //初始化bitmap
    server->key_state = 0;
    server->key_update_list = 0;

    //初始化软键盘的背景
    //键盘背景总是要设置的吧
    vnc_init_keyboardbackground_from_file(&keyboardmap);
    vnc_cp_bitmap2serverbuf(server,&keyboardmap,0, server->height- cn_vnc_keyboard_height);
    free(&(keyboardmap.bm_bits));
    debug_printf("set soft keyboard back ground success!\n");



    //vnc_init_key_bitmap(key_map);//从内存中读取
    vnc_init_key_bitmap_from_file(key_map);//从bmp文件中获取

    vnc_line_keynum = (server->width)/(cn_vnc_key_width + cn_vnc_keybotton_h_margin);
    keyline_num_visual = (cn_vnc_keyboard_height)/\
            (cn_vnc_key_height + cn_vnc_keybotton_v_margin);
    if((vnc_line_keynum ==0)||(keyline_num_visual == 0))
    {
        debug_printf("#init softkey board failed:no space left for show key!\n");
        return false;
    }
//根据实际的按键数目进行调整,保证按键放在最中央的位置
    vnc_keyline_num = (cn_vnc_key_num +vnc_line_keynum -1)/ vnc_line_keynum;
    if(vnc_keyline_num > keyline_num_visual )
    {
        vnc_keyline_num = keyline_num_visual;
        vnc_key_num = vnc_keyline_num * vnc_line_keynum;
    }
    else
    {
        vnc_key_num = cn_vnc_key_num;
    }
    vnc_space_h_left =(u16)(server->width - \
            vnc_line_keynum*(cn_vnc_key_width + cn_vnc_keybotton_h_margin));
    vnc_space_v_left =(u16)(cn_vnc_keyboard_height - \
            vnc_keyline_num*(cn_vnc_keybotton_v_margin + cn_vnc_key_height));

    x = vnc_space_h_left/2;
    y = server->height -cn_vnc_keyboard_height + (vnc_space_v_left+1)/2;

    for(i = 0; i < vnc_key_num; i++)
    {
        if((x+ cn_vnc_key_width + cn_vnc_keybotton_h_margin) > server->width)
        {
            y += cn_vnc_key_height + cn_vnc_keybotton_v_margin;
            x = vnc_space_h_left/2;
        }//换行处理
        //保证键在所处的botton的中央
        x = x + (cn_vnc_keybotton_h_margin+1)/2;
        vnc_cp_bitmap2serverbuf(server,&key_map[i],x, y);
        x += cn_vnc_key_width + (cn_vnc_keybotton_h_margin+1)/2;
    }
    return true;
}

#define cn_invalid_keynumber 0xFF//目前应该还没有255的键盘吧
// =============================================================================
// 函数功能:根据坐标点找软键盘的key
// 输入参数:鼠标的x y 位置
// 输出参数：
// 返回值:key的number号，具体的key值请对照key tab进行映射
// 说明:
// =============================================================================
u8 findsoftkey_by_point(rfbScreenInfoPtr server, u16 x, u16 y)
{
    u8 key_number = cn_invalid_keynumber;
    u8 v_number = 0;//垂直号
    u8 h_number = 0;//水平号

    v_number = (y + cn_vnc_keyboard_height-server->height - (vnc_space_v_left+1)/2)/\
            (cn_vnc_key_height + cn_vnc_keybotton_v_margin);
    h_number = (x - (vnc_space_h_left+1)/2)/\
            (cn_vnc_keybotton_h_margin + cn_vnc_key_width);

    key_number = v_number*vnc_line_keynum + h_number;

    return key_number;
}
// =============================================================================
// 函数功能:实现客户端对VNCdisplay的软键盘的操作
// 输入参数:server,服务器，msg 鼠标信息
// 输出参数：
// 返回值:
// 说明:当发现鼠标所停留的地方在软键盘的区域时，请采用此方法进行处理
// =============================================================================
void vnc_softkeyboard(rfbScreenInfoPtr server, rfbClientToServerMsg msg)
{

    u8 key_number = 0;
    u8 state = 0;
    u8  key = 0;

    if(msg.pe.buttonMask & (0x01))//按下
    {
        state = cn_key_down;
    }
    else
    {
        state = cn_key_up;
    }
    key_number = findsoftkey_by_point(server, msg.pe.x, msg.pe.y);

    if(key_number >= vnc_key_num)
    {
        return;//超出范围，肯定不需要处理
    }
//  //判断是否是同一个按键多次停留
//  if( (((u8)(server->key_state >> key_number)&0x01)) == state)//比较标志位
//  {
//      if(state == cn_key_up)//我相信这种情况只是你的鼠标路过而已
//      {
//          return ;//同一个按键多次连续up是不可能的事情，除非这个key就没有动作
//      }
//      else
//      {
//      //多次按下，不应该修改changed状态，因为此按键一直在按下状态，但是应该发送key
//      }
//  }
//  else//此按键状态发生了变化
//  {
//      if(cn_key_down == state)
//      {
//          server->key_state = server->key_state |(cn_key_down << key_number);
//            debug_printf("key_%d down \n",key_number);
//      }
//      else
//      {
//          server->key_state = server->key_state &(~(cn_key_down << key_number));
//            debug_printf("key_%d  up \n",key_number);
//      }
//      server->key_update_list = server->key_update_list ^(1<<key_number);
//  }
//  //将此鼠标消息封装成键盘按键消息进行处理
//  key = cyg_keyboard[key_number];

    vnc_input_key_message(server,key_number, state);
}
// =============================================================================
// 函数功能:实现客户端的鼠标响应
// 输入参数:server,服务器，msg 鼠标信息
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
void vncclient_pointevent(rfbScreenInfoPtr server, rfbClientToServerMsg msg)
{
    if(msg.pe.y > (server->height - cn_vnc_keyboard_height))//软键盘区
    {
        vnc_softkeyboard(server,msg);
    }
    else//屏幕显示区
    {
        if(msg.pe.buttonMask & (0x01))//点击下才算是触摸一次
        {
            vnc_input_touch_message(msg.pe.x,msg.pe.y,0);
        }

    }
}
// =============================================================================
// 函数功能:检测server端的某个key是不是要进行视觉更新
// 输入参数:server,服务器, check_bit，待检测的位（即key的在该键盘下的号码）
// 输出参数：
// 返回值:true，需要更新，false，不需要更新
// 说明:
// =============================================================================
bool_t vnc_check_key_changed(rfbScreenInfoPtr server, u8 check_bit)
{
    if(server->key_update_list & (0x01<<check_bit))
    {
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:检测sever端到底有多少个key需要更新
// 输入参数:server,服务器
// 输出参数：
// 返回值:需要更新的key的个数
// 说明:
// =============================================================================
u8 vnc_get_keychanged_numbers(rfbScreenInfoPtr server)
{
    u8 ret = 0;
    u8 i = 0;

    if(server->key_update_list == 0)//没有更新
    {
        return ret;
    }
    for(i = 0; i < cn_vnc_key_num; i++)
    {
      if( vnc_check_key_changed(server, i))
      {
        ret++;
      }
    }
    return ret;
}
// =============================================================================
// 函数功能:得到特定key的位置信息
// 输入参数:server,服务器, key_number,得到key_number的位置信息,x,y 存储位置
// 输出参数：
// 返回值:
// 说明:
// =============================================================================
void vnc_get_keyposition_by_number(rfbScreenInfoPtr server, u8 key_number,\
        u16 *x, u16 *y)
{
    u8  i = 0;

    i = key_number % vnc_line_keynum;
    *x = (cn_vnc_key_width + cn_vnc_keybotton_h_margin )*i+ \
            (vnc_space_h_left+1)/2 + (cn_vnc_keybotton_h_margin +1)/2;

    i = key_number / vnc_line_keynum;
    *y = server->height - cn_vnc_keyboard_height +\
    (cn_vnc_key_height + cn_vnc_keybotton_v_margin)*i + (vnc_space_v_left+1)/2;

    return;
}
// =============================================================================
// 函数功能:将server的某个key指定成相应的状态的视觉模式（填充到相应key的显存）
// 输入参数:server,服务器, key_number,该键盘的第几个key，state（按下和弹起状态）
// 输出参数：
// 返回值:无
// 说明:目前仅仅对该key的显存做基本加减处理，后面改进成拷贝生动的图片--TODO
// =============================================================================
void vnc_set_keymap2server(rfbScreenInfoPtr server, u8 key_number, u8 state)
{
//我们相信初始化的时候其实已经保证了不会写出sever的buf范围
//临时变量
    u8  pixel_bytes = 0;
    u8  i = 0;
    u16 key_height= 0;
    u16 key_width = 0;
    u16 startpoint_x;
    u16 startpoint_y;
    u32 server_linebytes = 0;
    u8  *buf;
    //获取位置
    vnc_get_keyposition_by_number(server,key_number,&startpoint_x,&startpoint_y);
    //找到buf位置
    pixel_bytes = server->rfbServerFormat.bitsPerPixel / 8;
    server_linebytes = (server->width)*pixel_bytes;
    buf = (u8 *)server->frameBuffer + startpoint_x*pixel_bytes +\
            startpoint_y*server_linebytes;

   //简单处理方案
    for(key_height = 0; key_height < cn_vnc_key_height; key_height ++)
    {
        for(key_width = 0; key_width <cn_vnc_key_width*pixel_bytes; key_width ++)
        {
            *(buf + key_width) = ~(*(buf + key_width));//仅仅是取反，高级特效待做--TODO
        }
        buf+=server_linebytes;
    }
}
// =============================================================================
// 函数功能:将server的发生改变的key全部刷新进缓存
// 输入参数:server,服务器
// 输出参数：
// 返回值:无
// 说明:根据server的key的状态记录和changgelist来进行，该函数应该在需要刷新cl的时候
//      调用，一次性刷新所有的key的状态
// =============================================================================
void vnc_changed_keyboard_visual(rfbScreenInfoPtr server)
{
    u8 i = 0;
    u8 state = 0;
    if(server->key_update_list)//没有需要刷新的应该是最开心的事情
    {
        for(i = 0; i < cn_vnc_key_num; i++)
        {
            if(vnc_check_key_changed(server, i))//该key需要刷新
            {
               state = (server->key_state >> i) & 0x01;
               vnc_set_keymap2server(server, i, state);
            }
        }
    }
}
// =============================================================================
// 函数功能:复位server的key statlist
// 输入参数:server,服务器
// 输出参数：
// 返回值:无
// 说明:
// =============================================================================
void vnc_reset_keyboard_statelist(rfbScreenInfoPtr server)
{
    server->key_update_list = 0;
}

// =============================================================================
// 函数功能:卸载驱动
// 输入参数:server,服务器
// 输出参数：
// 返回值:无
// 说明:
// =============================================================================
bool_t uninstall_vnc_keyboard(void)
{
    return Stddev_UnInstallDevice(cn_vnc_keyboard_name);
}

bool_t uninstall_vnc_touchboard(void)
{
    return Stddev_UnInstallDevice(cn_vnc_touchlcd_name);
}

#endif




