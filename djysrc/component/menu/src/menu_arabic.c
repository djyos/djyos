
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
//所属模块:menu manager  block
//作者:  .
//版本：V1.0.0
//文件描述:操作菜单窗口的工具定义
//其他说明:具体实现阿拉伯文字的安装和解码
//修订历史:
//
//1. 日期: 2012-10-26
//   作者:
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------


#include "Menu_arabic.h"

//安装阿拉伯字体

#define cfg_charset_arabic  1
#define cfg_arabic  1
#define cn_nls_charset_arabic   "arabic"
#define cn_font_arabic  "arabic_fontx16"

//阿拉伯字体的编码和点阵文件，用fontmaker生成的
//unsigned char *ArialFile="E:\\字体生成工具\\fontmaker 2.01\\output\\arialuni_U16.bin";
unsigned char *ArialFile="arialuni_U16.bin";
//将文件内容读取到这个数组中
unsigned char *arabic_file_code=NULL;
//unicode编码分扇区
static   unsigned  nSections=0;
//指向第一个扇区头
static struct  tagFlSectionInfo   *pSectionInfo=NULL;

// 注释参照 encoding.h-> tagCharset -> mbs_to_uc4s
s32 __arabic_mbs_to_ucs4s(u32* pwcs, const char* mbs, s32 n)
{
    return 0;
}

s32 __arabic_ucs4s_to_mbs(u32* pwcs, const char* mbs, s32 n)
{
    return 0;
}
s32 __arabic_ucs4_to_mb(u32* pwcs, const char* mbs, s32 n)
{
    return 0;
}
//原码unicode到点阵索引码
s32 __arabic_mb_to_ucs4(u32* pwc, const u16* mbs, s32 n)
{
    u16 code=0;
    s32 result=-1;
    int i=0;
    u32 wc=0;


 //   printf("In arabic mb to ucs4!\n");

    if((NULL==pwc)||(NULL==mbs))
        {
          return result;
        }
    code=*(u16 *)mbs;
    //code=0x0632;
    //由于转换的是unicode编码，所以
     for(i=0;i<nSections;i++)
        {

          if((code>=pSectionInfo[i].First)&&(code<=pSectionInfo[i].Last))
            {

              wc=*((u32 *)(arabic_file_code+pSectionInfo[i].OffAddr+(code-pSectionInfo[i].First)*4));
              //printf("Orignal code=%04x:in section %d,wc=%08x\n",*mbs,i,wc);
              break;
          }
     }
     if(i<nSections)//找到了相关的码字
        {
          if(wc<=(sizeof(struct tagFontLibHeader)+nSections*sizeof(struct tagFlSectionInfo)))//小于字符头文件
          {
              result = -1;
              printf("In the file header!\n");

          }
          else
            {

             result=2;
          }
     }
     else //没有对应的码字
     {
         result=-1;
     }
    *pwc=wc;
    return result;
}


unsigned char * GetLanguagecodeFromFile(const char *FileName)
{

   FILE  *fp;
   struct tagFontLibHeader fileHeader;
   u32    FileLength;
   unsigned char *LanguageCode=NULL;

   fp=fopen(FileName,"rb");
   if(fp==NULL)
    {
      printf("open the file failed!\n");
      goto exit;
   }
   fseek(fp,0L,SEEK_SET);

   fread(&fileHeader,sizeof(struct tagFontLibHeader),1,fp);

    if((fileHeader.magic[0] != 'U' && fileHeader.magic[0] != 'M'))
    {
        printf("Cann't support file format!\n");
        return 0;
    }

    if('M' == fileHeader.magic[0])     //mbcs 编码
    {
        printf("This is the Mbcs code!\n");
    }
    else
    {
        printf("This is the Unicode code!\n");
    }
    //对fileHeader的数据的顺序进行调整
  //  AdjustProcedure( &fileHeader.Size, &fileHeader.Size, sizeof(fileHeader.Size));
//  AdjustProcedure( &fileHeader.wCpFlag, &fileHeader.wCpFlag, sizeof(fileHeader.wCpFlag));

    printf("filesize=%08x\n",fileHeader.Size);
    printf("LanguageF=%04x\n",fileHeader.wCpFlag);

    LanguageCode=M_MallocLc(fileHeader.Size,0);
    if(NULL==LanguageCode)
        {

      fclose(fp);
      goto exit;
    }
    fseek(fp,0L,SEEK_SET);
    FileLength=fread(LanguageCode,fileHeader.Size,sizeof(unsigned char),fp);
    printf("---File--Length=%08x\n",FileLength);
//  memcpy(&fileHeader,LanguageCode,0x10);
    fclose(fp);

exit:
    return LanguageCode;
}


/*
函数名:安装阿拉伯文字的解码部分
函数功能:完成字体编码的安装

*/


void module_init_encode_arabic(ptu32_t para)
{
    static struct tagCharset encoding;

    int i=0;
   //加载字库
     printf("Init the code\n");
    arabic_file_code=GetLanguagecodeFromFile(ArialFile);
    if(arabic_file_code==NULL)
        {
         printf("Get file code failed!\n");
         return;
    }
    nSections=((struct tagFontLibHeader*)arabic_file_code)->nSection;
    pSectionInfo=(struct tagFlSectionInfo *)(arabic_file_code+0x10);

    //打印编码信息
    for(i=0;i<nSections;i++)
        {
     printf("Section=%d ,First=%04x,Last=%04x,Addr=%08x\n",i,pSectionInfo[i].First,\
        pSectionInfo[i].Last,pSectionInfo[i].OffAddr);

    }

    encoding.max_len = 1;
    encoding.mb_to_ucs4 = __arabic_mb_to_ucs4;
    encoding.ucs4_to_mb = __arabic_ucs4_to_mb;
    encoding.mbs_to_ucs4s = __arabic_mbs_to_ucs4s;
    encoding.ucs4s_to_mbs = __arabic_ucs4s_to_mbs;
    if( Charset_NlsInstallCharset(&encoding, cn_nls_charset_arabic))
    {
        printf("arabic encoding install sucess\n\r");
        return 1;
    }else
    {
        Djy_SaveLastError(EN_GK_CHARSET_INSTALL_ERROR);
        printf("arabic encoding install fail\n\r");
        return 0;
    }


}

//----点阵提取-----------------------------------------------------------------
//功能: 安装阿拉伯点阵
//参数: charcode，点阵索引码

//      bitmap，保存所提取的点阵的位图，缓冲区由调用者提供
//返回: true=正常，false=charcode不是该字体所支持的字符集范围，但此时仍然返回
//      默认字符的点阵
//-----------------------------------------------------------------------------
bool_t __arabic_get_char_bitmap(u32 charcode, u32 size,u32 resv,
                                    struct tagRectBitmap *bitmap)
{
    u8 i;
    u32 offset;
    bool_t result = true;

    struct tagFontLibHeader fileHeader;

    u32  AddrOffset;
    u8   width;
    u8   lineBytes;
    u8   height;

 //   printf("In arabic mb to get char bitmap!\n");
    if(NULL==arabic_file_code)
        {

          printf("No init the arabic_file_code pointer!\n");
          return false;
    }


    fileHeader=*(struct tagFontLibHeader *)arabic_file_code;


//  printf("charcode=%08x\n",charcode);
    AddrOffset=(charcode&0x03FFFFFF);//低26位表示偏移地址
    width=(charcode>>24);//高6位表示宽度
    width=width>>2;
 //   printf("width=%02x\n",width);
    height=fileHeader.YSize;

    lineBytes=(width+7)/8;
//    printf("linbte=%02x\n",lineBytes);
//   printf("width=%02x\n",width);
/*
   if(AddrOffset<=(sizeof(struct tagFontLibHeader)+nSections*sizeof(struct tagFlSectionInfo)))//小于字符头文件
    {
          result = false;
          printf("In the file header!\n");
          goto exit;

   }
   */
    if((AddrOffset+lineBytes*height)>fileHeader.Size)//超出字符的集的范围
        {

          result = false;
          printf("ecceed!\n");
          goto exit;
    }

    // 得到字符字模在数组中的偏移量
    bitmap->pf_type = CN_SYS_PF_GRAY1;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->linebytes = lineBytes;

    if(bitmap->bm_bits != NULL)
    {
        for(i=0; i<lineBytes*height; i++)
        {
            bitmap->bm_bits[i] = arabic_file_code[AddrOffset+i];
        }
    }

exit:
    return result;
}
bool_t __arabic_load_font(u8* zk_addr)
{


   return true;//do nothing


}
bool_t __arabic_unload_font(u8* zk_addr)
{

   return true;//do nothing


}
/*
函数名:安装阿拉伯文字的点阵部分
函数功能:完成字体点阵的映射
*/

ptu32_t module_init_font_arabic(ptu32_t para)
{
    static struct tagFontRsc font_arabic;
  //  __Charset_Gb2312_816_1616LoadFont((u8*)para);  //加载字库
    font_arabic.fi.type = CN_FONT_TYPE_DOT;
    font_arabic.LoadFont = __arabic_load_font;
    font_arabic.UnloadFont = __arabic_unload_font;
    font_arabic.GetBitmap = __arabic_get_char_bitmap;

    if(Gk_FontInstallFont(&font_arabic, cn_font_arabic))
    {
        printf("arabic  font install sucess\n\r");
        return 1;
    }else
    {
        Djy_SaveLastError(EN_GK_FONT_INSTALL_ERROR);
        printf("arabic font install fail\n\r");
        return 0;
    }
}


//测试阿拉伯文字的安装

void test_arabic( void)
{
//初始化字体编码


  struct tagFontRsc  *pfont=NULL,font;
  struct tagCharset  *pencode=NULL,encode;

  #if (cfg_charset_arabic== 1)       //初始化阿拉伯字符集
    module_init_encode_arabic(0);
  #endif


//以下初始化字体点阵
  #if (cfg_arabic== 1)     //初始化阿拉伯点阵字库
    module_init_font_arabic(0);
  #endif

  //设置arabic为默认字体

  encode.node.name="arabic";
  pencode=Charset_NlsSetCurCharset(&encode);
  font.node.name="arabic_fontx16";
  pfont=Font_SetCurFont(&font);

  printf("cur_encode=%s\n",pencode->node.name);
  printf("cur_font=%s\n",pfont->node.name);

}

//0621到064a的变形码，超过此部分的都是自己本身码字

const u16 Arbic_Position[][4]=  // first, last, middle, alone
{
{ 0xfe80, 0xfe80, 0xfe80, 0xfe80},    // 0x621
{ 0xfe82, 0xfe81, 0xfe82, 0xfe81},
{ 0xfe84, 0xfe83, 0xfe84, 0xfe83},
{ 0xfe86, 0xfe85, 0xfe86, 0xfe85},
{ 0xfe88, 0xfe87, 0xfe88, 0xfe87},
{ 0xfe8a, 0xfe8b, 0xfe8c, 0xfe89},
{ 0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d},
{ 0xfe90, 0xfe91, 0xfe92, 0xfe8f},   // 0x628
{ 0xfe94, 0xfe93, 0xfe93, 0xfe93},
{ 0xfe96, 0xfe97, 0xfe98, 0xfe95},   // 0x62A
{ 0xfe9a, 0xfe9b, 0xfe9c, 0xfe99},
{ 0xfe9e, 0xfe9f, 0xfea0, 0xfe9d},
{ 0xfea2, 0xfea3, 0xfea4, 0xfea1},
{ 0xfea6, 0xfea7, 0xfea8, 0xfea5},
{ 0xfeaa, 0xfea9, 0xfeaa, 0xfea9},
{ 0xfeac, 0xfeab, 0xfeac, 0xfeab},   // 0x630
{ 0xfeae, 0xfead, 0xfeae, 0xfead},
{ 0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf},
{ 0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1},
{ 0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5},
{ 0xfeba, 0xfebb, 0xfebc, 0xfeb9},
{ 0xfebe, 0xfebf, 0xfec0, 0xfebd},
{ 0xfec2, 0xfec3, 0xfec4, 0xfec1},
{ 0xfec6, 0xfec7, 0xfec8, 0xfec5},  // 0x638
{ 0xfeca, 0xfecb, 0xfecc, 0xfec9},
{ 0xfece, 0xfecf, 0xfed0, 0xfecd},  //0x63A
{ 0x63b, 0x63b, 0x63b, 0x63b},
{ 0x63c, 0x63c, 0x63c, 0x63c},
{ 0x63d, 0x63d, 0x63d, 0x63d},
{ 0x63e, 0x63e, 0x63e, 0x63e},
{ 0x63f, 0x63f, 0x63f, 0x63f},
{ 0x640, 0x640, 0x640, 0x640},   // 0x640
{ 0xfed2, 0xfed3, 0xfed4, 0xfed1},
{ 0xfed6, 0xfed7, 0xfed8, 0xfed5},
{ 0xfeda, 0xfedb, 0xfedc, 0xfed9},
{ 0xfede, 0xfedf, 0xfee0, 0xfedd},
{ 0xfee2, 0xfee3, 0xfee4, 0xfee1},
{ 0xfee6, 0xfee7, 0xfee8, 0xfee5},
{ 0xfeea, 0xfeeb, 0xfeec, 0xfee9},
{ 0xfeee, 0xfeed, 0xfeee, 0xfeed},   // 0x648
{ 0xfef0, 0xfeef, 0xfef0, 0xfeef},
{0xfef2, 0xfef3, 0xfef4, 0xfef1},   // 0x64A
};
//字符集一，用于判读前连
const u16 theSet1[23]={
        0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
        0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
        0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626};

//字符集二，用于判断后连
const u16 theSet2[35]={
        0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
        0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
        0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626,
        0x627, 0x623, 0x625, 0x622, 0x62f, 0x630, 0x631, 0x632,
        0x648, 0x624, 0x629, 0x649};
//特殊码字，用于连写
static u16 arabic_specs[][2]=
{
{0xFEF5,0xFEF6},
{0xFEF7,0xFEF8},
{0xFEF9,0xFEFA},
{0xFEFB,0xFEFC},
};

/*
函数名字:JudgeFirst()
函数参数:arabic_code，当前字符的前一个arabic码字
返回值:true，是前连
       false:不是前连
函数功能:判断是不是前连字符
*/
bool_t JudgeFirst(u16 arabic_code)
{
   int i=0;

   for(i=0;i<23;i++)
    {
      if(arabic_code==theSet1[i])
        {
          break;
        }
   }
   if(i<23)
    {
     return true;
   }
   else
    {
     return false;
   }
}


/*
函数名字:JudgeLast()
函数参数:arabic_code，当前字符的后一个arabic码字
返回值:true，是后连
       false:不是后连
函数功能:判断是不是后连字符
*/
bool_t JudgeLast(u16 arabic_code)
{
   int i=0;

   for(i=0;i<35;i++)
    {
      if(arabic_code==theSet2[i])
        {
          break;
        }
   }
   if(i<35)
    {
     return true;
   }
   else
    {
     return false;
   }
}

/*

函数名字:JudgeSpecCode()
参数:u16 curcode,nexcode,
返回值，判断当前符号是不是连字符

true:是连字符
false:不是连字符

*/

bool_t JudgeSpecCode(u16 curcode, u16 nexcode)
{

    bool_t  result=false;
    if(curcode!=0x0644)
        {
     return result;
    }
    if((0x0622==nexcode)||(0x0623==nexcode)||(0x0625==nexcode)||(0x0627==nexcode))
        {
      result=true;

    }
    return result;
}



/*
函数名字:GetDeformdArial()
函数参数:precode,curcode,nexcode;前一个字符,当前字符，后一个字符
返回:获取的变形后的码字
*/

u16  GetDeformArial(u16 precode,u16 curcode, u16 nexcode)
{

    u8   type=0;
    u8   offset;
    u16  result;

    bool_t bFirst;
    bool_t bLast;

    if((0x064a<curcode)||(0x0621>curcode))//变形码和本身相同
        {
      return curcode;
    }

    offset=curcode-0x0621;


    bFirst=JudgeFirst(precode);
    bLast=JudgeLast(nexcode);
    if(bFirst)
        {

      type|=1<<0;
    }
    if(bLast)
        {
      type|=1<<1;
    }
    switch(type)
        {
          case 0:
                  result=Arbic_Position[offset][3];//独立
                  break;
          case 1 :
                  result=Arbic_Position[offset][0];//前连
                  break;
          case 2:
                  result=Arbic_Position[offset][1];//后连
                  break;
          case 3:
                  result=Arbic_Position[offset][2];//中间
                  break;
           default:
                 result=Arbic_Position[offset][3];//独立
                 break;
    }

     return result;

}

/*
函数名称:GetSpecCode()
参数:u16 precode,nexcode;当前字符的前一个字符，当前字符，当前字符的后一个字符
返回值:
      连字符的unicode码字
函数功能:获取连字符的码字，即0x644和后面一个码字的连码,执行该函数前应该先判断是不是连字码

*/

u16 GetSpecCode(u16 precode,u16 nexcode)
{
   bool_t Inset1;//前面一个字符是不是在集合1中
   u16   result=0x0000;
   Inset1=JudgeFirst(precode);
   if(Inset1)
    {
       switch(nexcode)
        {
           case 0x0622:
                     result=arabic_specs[0][1];
                     break;
           case 0x0623:
                     result=arabic_specs[1][1];
                     break;
           case 0x0625:
                     result=arabic_specs[2][1];
                     break;
           case 0x0627:
                     result=arabic_specs[3][1];
                     break;
            default:
                     printf("fataerr!In getspeccode!\n");
                     break;
       }

    }
   else
    {

       switch(nexcode)
        {
           case 0x0622:
                     result=arabic_specs[0][0];
                     break;
           case 0x0623:
                     result=arabic_specs[1][0];
                     break;
           case 0x0625:
                     result=arabic_specs[2][0];
                     break;
           case 0x0627:
                     result=arabic_specs[3][0];
                     break;
            default:
                     printf("fataerr!In getspeccode!\n");
                     break;
        }

     }

    return result;
}


/*
 函数名字:ChUps2Ufs()
 参数: u16 *Ups;只是以标准字母表达的字流//空间内容用户提供
       u32 count ups的个数
       u16 *Ufs;转换后的全码字流，//空间由用户提供
       u32 *number;转换后的字符的数目

返回值: true,转换成功；
        false:出错
函数功能:该函数能够将标准的码字流转换成全字流形式的阿拉伯文，应用该码字可以直接找到对应的阿拉伯文，
         包括连写的文字。
*/
bool_t ChUps2Ufs(u16 *Ups, u32 count, u16 *Ufs,u32 *number)
{
    u32 num=0;
    u32 code_num=0;
    bool_t result=true;

    if(0==count)//个数为0
        {
        *number=code_num;
        return result;
    }
    else if(1==count)//个数为1
        {
         *Ufs=*Ups;
         *number=1;
         return result;
    }

    while(num<count)
    {
      if(0==num)//第一个
      {
         *Ufs=GetDeformArial(0, *Ups, *(Ups+1));
    //   printf("This the first one!--%04x\n",*Ufs);
         num++;
         Ufs++;
         Ups++;
         code_num++;

      }
      else if(count-1==num)//最后一个
      {
         *Ufs=GetDeformArial(*(Ups-1),*Ups,0);
          num++;
          code_num++;
      }
      else//中间的字符
      {
        if(JudgeSpecCode(*Ups, *(Ups+1)))//是连字符
            {
              *Ufs=GetSpecCode(*(Ups-1),*(Ups+1));
             // printf("This the spec code!--%04x\n",*Ufs);

              Ufs++;
              Ups=Ups+2;
              num=num+2;
             }
         else
            {
               *Ufs=GetDeformArial(*(Ups-1), *(Ups), *(Ups+1));
            //   printf("This the deformed code!--%04x\n",*Ufs);

               Ufs++;
               Ups++;
               num++;
             }//end for whether spec code
          code_num++;

      }//end for num

    }//end for while

    *number=code_num;
    return result;

}//end for function

/*
函数名字:CalUpsLength
参数:u16 *Ups,  Ups码字流
     u32   count;UPs长度
返回值: u32  Ups换成Ufs的个数
*/

u32 CalUpsLength(u16 *Ups, u32 count)
{
    u32 num=0;
    u32 code_num=0;

    if(0==count)//个数为0
        {
        return code_num;
    }
    else if(1==count)//个数为1
        {
         return 1;
    }

    while(num<count)
    {
      if(0==num)//第一个
      {
         num++;
         Ups++;
         code_num++;

      }
      else if(count-1==num)//最后一个
      {
          num++;
          code_num++;
      }
      else//中间的字符
      {
        if(JudgeSpecCode(*Ups, *(Ups+1)))//是连字符
            {
              Ups=Ups+2;
              num=num+2;
             }
         else
            {
               Ups++;
               num++;
             }//end for whether spec code
          code_num++;
      }//end for num
    }//end for while
    return code_num;
}//end for function


/*

函数名字:AdjustUfsProcedure
函数参数:u16 *ufs,存储ufscode,存储区间用户负责，安全性由用户负责
         u32  count;ufs的个数
函数功能:由于阿拉伯文字是自右向左显示的，所以要将数字反序，因为数字是从左到右显示的
*/

void AdjustUfsProcedure(u16 *Ufs, u32 count)
{


   int i=0,j=0,k=0;

   u16 temp;

   printf("Begin to adjust the data procedure!\n");


   if(NULL==Ufs)
    {
      printf("You have ab NULL pointer for AdjustUfs!\n");
      return;
   }

   while(i<count)//遍历整个UFS流
    {
       if(((*(Ufs))>=0x30)&&((*(Ufs))<=0x39))//判断是不是数字，数字本身已经转换成U16格式的
        {
           j=0;
           printf("Got an digital number--%d\n",i);
           while(((*(Ufs+j))>=0x30)&&((*(Ufs+j))<=0x39))//判断有多少个连数字
            {
              j++;
             }//end while

           printf("Got an digital number--%d--%d link!\n",i,j);
           for(k=0;k<(j+1)/2;k++)
            {
              temp=*(u16 *)(Ufs+k);
              *(u16 *)(Ufs+k)=*(u16 *)(Ufs+j-1-k);
              *(u16 *)(Ufs+j-1-k)=temp;
            }//end for
            Ufs=Ufs+j;
            i=i+j;
         }//end if
        else
            {
          i++;
          Ufs++;
         }
     }//end while

}//end function

//用于测试阿拉伯文字显示

















