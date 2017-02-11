//-----------------------------------------------------------------------------
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

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <djyfs/file.h>
#include <djyfs/iofile.h>
#include <djyfs/mount.h>
#include <shell.h>
#include <driver.h>
#include <stdlib.h>

//
//
//
static bool_t SH_Format(const char *Param);
static bool_t SH_PrintWorkingPath(const char *Param);
static bool_t SH_MkDir(const char *Param);
static bool_t SH_List(const char *Param);
static bool_t SH_ChangeDir(const char *Param);
static bool_t SH_Remove(const char *Param);
static bool_t SH_Cat(const char *Param);
static bool_t SH_Copy(const char *Param);
//
//
//
static struct ShellCmdRsc *s_pCmdRsc;
char *g_pWokingPath;
static struct ShellCmdTab const FsCmdTable[] =
{
    {
        "format",
        SH_Format,
        "文件系统格式化",
        "格式: format 挂载点目录(路径)"
    },
    {
        "pwd",
        SH_PrintWorkingPath,
        "显示用户当前工作路径",
        "\r\n"
    },
    {
        "mkdir",
        SH_MkDir,
        "创建目录",
        "\r\n"
    },
    {
        "ls",
        SH_List,
        "显示目录项",
        "\r\n"
    },
    {
        "cd",
        SH_ChangeDir,
        "更改当前工作路径",
        "\r\n"
    },
    {
        "rm",
        SH_Remove,
        "删除文件或者目录",
        "\r\n"
    },
    {
        "cat",
        SH_Cat,
        "读文件并打印",
        "\r\n"
    },
	{
		"cp",
		SH_Copy,
		"拷贝文件",
		"example: cp /fat/abc /yaffs/abc\r\n"
	},
#if 0
    {
        "mount",
        Sh_Mount,
        "挂载文件系统",
        "todo: "
    },
    {
        "fstell",
        SH_FsTell,
        "已安装文件系统信息",
        "todo:"
    },
#endif
};

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
const char *s_cREG = "文件";
const char *s_cLINK = "链接";
const char *s_cDIR = "目录";
const char *s_cUNKNOW = "未知";
static const char *PropertyToStr(u32 Mode)
{
    if(((S_ISREG(Mode)) && (S_ISLNK(Mode))) ||
        ((S_ISREG(Mode)) && (S_ISDIR(Mode))) ||
        ((S_ISDIR(Mode)) && (S_ISLNK(Mode))))
        return (s_cUNKNOW);

    if(S_ISREG(Mode))
        return (s_cREG);

    if(S_ISLNK(Mode))
        return (s_cLINK);

    if(S_ISDIR(Mode))
        return (s_cDIR);

    return (s_cUNKNOW);
}

//-----------------------------------------------------------------------------
//功能: 文件系统格式化
//参数:
//返回:
//备注: todo: 逻辑待完善
//-----------------------------------------------------------------------------
static bool_t SH_Format(const char *Param)
{
    if(NULL == Param)
        return (FALSE);

    printf("文件系统格式化 ----");
    if(Format(Param))
        printf("失败!");
    else
        printf("成功!");

    printf("\r\n");
    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 显示用户当前工作路径
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t SH_PrintWorkingPath(const char *Param)
{
    s32 Len;
    bool_t Ret;
    char *PWD;

    Len = CurWorkPathLen();
    if(0 == Len)
    {
        printf("用户当前工作路径未设置\r\n");
        return (TRUE);
    }

    PWD = malloc(Len);
    if(NULL == PWD)
    {
        return (FALSE);
    }

    if(0 == CurWorkPath(PWD, Len))
    {
        printf("用户当前工作路径：\"%s\"", PWD);
        Ret = TRUE;
    }
    else
    {
        printf("未知错误\r\n");
        Ret = TRUE;
    }

    free(PWD);
    return (Ret);

}
//-----------------------------------------------------------------------------
//功能: 创建目录
//参数: -p -- 试图创建路径中所有不存在的目录项
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t  SH_MkDir(const char *Param)
{
    char *Temp, *SlashPos, *Path, *FullPath;
    int Fd;
    u32 PathLen;
    u8 Changed;
    u8 JustLast = 0;

    if(NULL == Param)
        return (FALSE);

    Temp = strstr(Param, "-p ");
    if(Temp)
        Path = (char*)Temp + 3; // 去除"-p"参数项
    else
    {
        Path = (char*)Param;
        JustLast = 1;
    }

    while(*Path == ' ')
        Path = Path + 1; // 去除多余的空格符,对于文件系统路径而言,是不可能以空格开始的

    FullPath = malloc(strlen(Path)+1);
    strcpy(FullPath, Path);
    if(JustLast)
    {
        Fd = open(FullPath, O_DIRECTORY | O_CREAT | O_EXCL |O_RDWR, 0);
        if(-1 != Fd)
            close(Fd);
    }
    else
    {
        // 获取路径的每个节点,例如/aa/bb/cc的路径，分别会获取/aa、/aa/bb、/aa//bb/cc并试图创建
        SlashPos = FullPath;
        do
        {
            while(*SlashPos == '/')
                SlashPos = SlashPos + 1; // 去除多余'/'

            while((*SlashPos != '/') && (*SlashPos != '\0'))
                SlashPos = SlashPos + 1; // 获取一个segment

            PathLen = SlashPos - FullPath;
            if('/' == FullPath[PathLen])
            {
                FullPath[PathLen] = '\0';
                Changed = 1;
            }

            Fd = open(FullPath, O_DIRECTORY | O_CREAT | O_EXCL | O_RDWR, 0);
            if(-1 != Fd)
                close(Fd);

            if(Changed)
                FullPath[PathLen] = '/';
        }while(*SlashPos != '\0');
    }

    if(-1 == Fd)
        printf("创建失败或目录已存在\r\n");

    free(FullPath);
    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 显示目录项
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t SH_List(const char *Param)
{
    DIR *Dir;
    s32 Len;
    char *CWP;
    struct dirent *DirentInfo;
    struct stat DirentStat;
    u8 Flags = 0;

    if(Param && strstr(Param, "-l"))
        Flags = 1; // 显示完整信息

    Len = CurWorkPathLen();
    if(0 == Len)
    {
        printf("用户当前工作路径未设置\r\n");
        return (TRUE);
    }

    CWP = malloc(257);
    if(NULL == CWP)
    {
        printf("内存不足\r\n");
        return (TRUE);
    }

    if(0 == CurWorkPath(CWP, Len))
    {
        Dir = opendir(CWP);
        if(Dir)
        {
            CWP[Len-1] = '/';
            while((DirentInfo = readdir(Dir)) != NULL)
            {
                if(1 == Flags)
                {
                    strcpy(CWP+Len, DirentInfo->d_name);
                    if(stat(CWP, &DirentStat))
                        printf("%s -- 该目录项无法解析\r\n", DirentInfo->d_name);
                    else
                    {
                        printf("%s", PropertyToStr(DirentStat.st_mode));
                        printf("\t");
                        printf("%d", (u32)(DirentStat.st_size));// todo: 64位逻辑还在处理中
                        printf("\t");
                        printf("%s", DirentInfo->d_name);
                        printf("\r\n");
                    }
                }
                else
                {
                    printf("%s", DirentInfo->d_name);
                    printf("    ");
                }
            }

            if(0 == Flags)
                printf("\r\n");

            closedir(Dir);
        }
    }

    free(CWP);
    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 更改当前工作路径
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t SH_ChangeDir(const char *Param)
{
    s32 Ret;
    char *Path = (char *)Param;

    if(NULL == Path)
        return (FALSE);

    while(*Path == ' ')
        Path = Path + 1; // 去除多余的空格符

    Ret = SetPWD(Path);
    if(Ret)
    {
        printf("错误：当前工作路径变更失败");
        if(-1 == Ret)
            printf("-- 目录不存在或无法进入");
        else if(-2 == Ret)
            printf("-- 目录无法进入");
        printf("\r\n");
        return (TRUE);
    }

    return (TRUE);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
bool_t Sh_PrintWorkPath(void)
{
    printf("%s", g_pWokingPath);
    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 删除文件或目录
//参数: -r 递归删除(可删除目录项)
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t SH_Remove(const char *Param)
{

    u16 Offset;
    char *PathParam, *Temp, *Path;
    s32 Ret = 0;
    u8 RemoveAll = 0;

    PathParam = (char*)Param;
    if(NULL == PathParam)
        return (FALSE);

    Temp = strstr(PathParam, "-r");
    if(Temp)
    {
        PathParam = (char*)Temp + 2; // 去除"-p"参数项
        RemoveAll = 1;
    }

    while(*PathParam == ' ')
        PathParam = PathParam + 1; // 去除多余的空格符

    Path = malloc(257);
    if(NULL == Path)
    {
        printf("内存不足\r\n");
        return (TRUE);
    }

    memset(Path, 0, 257);
    Offset = strlen(PathParam);
    if(Offset < 257)
    {
        memcpy(Path, PathParam, Offset);
        if((RemoveAll) && ('/' !=Path[Offset-1]))
            Path[Offset++] = '/';
        Path[Offset] = '\0';

        if(RemoveAll)
        {
            struct stat Info;
            DIR *Dir;
            struct dirent *Dirent;

            Ret = stat(Path, &Info);
            if(Ret)
                printf("格式错误\r\n");

            if((!Ret) && S_ISDIR(Info.st_mode))
            {
                Dir = opendir(Path);
                if(Dir)
                {
                    while((Dirent = readdir(Dir)) != NULL)
                    {
                        strcpy(Path+Offset, Dirent->d_name);
                        Ret = remove(Path);
                        if(Ret)
                        {
                            printf("文件\"%s\"无法删除(文件使用中)\r\n", Dirent->d_name);
                            break;
                        }
                    }
                    closedir(Dir);
                }
            }
        }

        if(!Ret)
        {
            Ret = remove(Path);
            if(Ret)
                printf("文件无法删除(文件使用中)\r\n");
        }
    }
    free(Path);
    return (TRUE);
}

//-----------------------------------------------------------------------------
//功能: 读文件
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static bool_t SH_Cat(const char *Param)
{
    char *Path = (char *)Param;
    int FH;
    char Buf[64];
    s32 Ret;
    u8 i;

    if(NULL == Path)
        return (FALSE);

    while(*Path == ' ')
        Path = Path + 1; // 去除多余的空格符,对于文件系统路径而言,是不可能以空格开始的


    FH = open(Param, O_RDONLY, 0);
    if(-1 == FH)
    {
        printf("\"cat\": 文件无法打开");
        return (FALSE);
    }

    printf("16进制显示模式:\r\n");
    do{
        Ret = read(FH, Buf, 64);
        for(i = 0; i < Ret; i++)
            printf("%02x", Buf[i]);
    }while(Ret == 64);

    close(FH);

    return (TRUE);
}

//-----------------------------------------------------------------------------
//功能: 拷贝文件
//参数: "cp /fat/abc /yaffs/abc"
//返回:
//备注:
//-----------------------------------------------------------------------------
#define RW_SIZE  256
static bool_t SH_Copy(const char *Param)
{

	char *PathTemp;
	char *PathSrc;
    char *PathDes;
    u16 Length;
    s32 HandleSrc;
    s32 HandleDes;
    u8 Buf[RW_SIZE];
    s32 Res;

    PathTemp = (char*)Param;
    if(NULL == PathTemp)
    {
    	printf("\"cp\": no parameters!\r\n");
    	return (FALSE);
    }

    while(*PathTemp == ' ')
    	PathTemp = PathTemp + 1; // 去除多余的空格符,对于文件系统路径而言,是不可能以空格开始的

    PathDes = PathTemp; // 临时保存头位置
    PathSrc = PathTemp;
    while(*PathTemp != ' ')
    	PathTemp = PathTemp + 1; // 找到空格

    Length = PathTemp - PathSrc;
    PathSrc = malloc(Length + 1); // 含结束符
    if(!PathSrc)
    {
    	printf("\"cp\": memory out!\r\n");
    	return (FALSE);
    }

    memcpy(PathSrc, (PathTemp - Length), Length);
    PathSrc[Length] = '\0';
    PathDes = PathTemp + 1;
    HandleSrc = open(PathSrc, O_RDONLY);
    if(-1 == HandleSrc)
    {
        printf("\"cp\": cannot open source file!\r\n");
        return (FALSE);
    }

    HandleDes = open(PathDes, O_RDWR | O_CREAT);
	if(-1 == HandleDes)
	{
		printf("\"cp\": cannot open destination file!\r\n");
		return (FALSE);
	}

	while(1)
	{
		Res = read(HandleSrc, Buf, RW_SIZE);
		if(!Res)
			break; // 全部读完

		if(Res != write(HandleDes, Buf, Res))
		{
			printf("\"cp\": write destination file error.\r\n");
			break;
		}

		if(Res != RW_SIZE)
			break; // 全部读完
	}

	Res = close(HandleDes);
	if(Res)
		printf("\"cp\": close destination file error.\r\n");

	Res = close(HandleSrc);
    if(Res)
    	printf("\"cp\": close source file error.\r\n");

    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 文件系统相关shell命令安装
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void FsShellInstall(void)
{
    u32 CmdNums;

    CmdNums = sizeof(FsCmdTable) / sizeof(struct ShellCmdTab);

    s_pCmdRsc = malloc(CmdNums * sizeof(*s_pCmdRsc));
    if(NULL == s_pCmdRsc)
        return ;

    Sh_InstallCmd(FsCmdTable, s_pCmdRsc, CmdNums);

    g_pWokingPath = malloc(257);
    if(NULL == g_pWokingPath)
        return ;

    memset(g_pWokingPath, '\0', 257);
    g_pWokingPath[0] = '/';

    Sh_PrintWorkPathSet(); // 开启当前工作路径
}
