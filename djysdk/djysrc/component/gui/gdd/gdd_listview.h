/*
 * gdd_listview.h
 *
 *  Created on: 2016年3月2日
 *      Author: liuwei
 */

#ifndef SRC_OS_CODE_COMPONENT_GUI_INCLUDE_GDD_LISTVIEW_H_
#define SRC_OS_CODE_COMPONENT_GUI_INCLUDE_GDD_LISTVIEW_H_

/*=========================================================================================*/

//LVITEM Mask
#define LVCF_FORMAT         (1<<0)
#define LVCF_TEXT           (1<<1)
#define LVCF_TEXTCOLOR      (1<<2)
#define LVCF_BACKCOLOR      (1<<3)
#define LVCF_BORDERCOLOR    (1<<4)
#define LVCF_DATA           (1<<5)
#define LVCF_ALL            (LVCF_FORMAT|LVCF_TEXT|LVCF_TEXTCOLOR|LVCF_BACKCOLOR|LVCF_BORDERCOLOR|LVCF_DATA)

//LVITEM Format
#define LVF_TEXT            (1<<0)  //显示文字
#define LVF_IMAGE           (1<<1)  //显示图像

#define LVF_ALIGN_V_MASK    (3<<2)
#define LVF_BOTTOM          (0<<2)  //垂直底部对齐
#define LVF_VCENTER         (1<<2)  //垂直居中对齐
#define LVF_TOP             (2<<2)  //垂直顶部对齐

#define LVF_ALIGN_H_MASK    (3<<4)
#define LVF_LEFT            (0<<4)  //水平居左对齐
#define LVF_CENTER          (1<<4)  //水平居中对齐
#define LVF_RIGHT           (2<<4)  //水平居右对齐

#define LVF_DEFAULT         (LVF_TEXT|LVF_VCENTER|LVF_CENTER)


typedef struct  __LVCELL
{
    u16 Mask;
    u16 Format;

    u32 cbTextMax;
    const char *pszText;
    u32 TextColor,BackColor,BorderColor;
    const void *pData;
}LVCELL;

#define MAKE_LVCELL_IDX(row,col) ((row<<16)|col)

/*=========================================================================================*/

#define LVM_FIRST   0x7000

#define LVM_INSERTCOLUMN    (LVM_FIRST+0)   //加入列
#define LVM_INSERTROW       (LVM_FIRST+1)   //加入行。
#define LVM_SETCELL         (LVM_FIRST+2)   //设置一个单元
#define LVM_GETCELL         (LVM_FIRST+3)   //获得一个单元
#define LVM_DELETEROW       (LVM_FIRST+4)   //删除一行
#define LVM_DELETEALLROWS   (LVM_FIRST+5)   //删除所有行
#define LVM_SETFIRSTCOLUMN  (LVM_FIRST+6)
#define LVM_SETFIRSTROW     (LVM_FIRST+7)

/*=========================================================================================*/

u32 listview_proc(struct WindowMsg *pMsg);

#define LISTVIEW    listview_proc

/*=========================================================================================*/

#endif /* SRC_OS_CODE_COMPONENT_GUI_INCLUDE_GDD_LISTVIEW_H_ */
