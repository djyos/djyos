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
#include <stdlib.h>
#include <stdio.h>
#include "./testbench_lib/testbench_config.h"

extern const char *TestChinese;
extern const char *TestEnglish;
extern const unsigned char gImage_rgb565_240x180[86400];// 来自"/test/testbmp/"
extern const unsigned char acTulips_128_96[12288];
extern const unsigned char big_file[4320000];
extern const unsigned char middle_file[302400];
extern const unsigned char small_file[259200];
extern struct FileSysType YAF2;
extern struct FileSysType FAT;


//
// 测试配置项
//
unsigned char *ConfigFileSource = (unsigned char *)small_file;//middle_file;//acTulips_128_96;//big_file;//
unsigned char *ConfigFileName = "small_file_0000";//"middle_file_0000";//"acTulips_128_96_0000";//"big_file_0000";//
u32 ConfigFileSize = sizeof(small_file);//sizeof(middle_file);//sizeof(acTulips_128_96);//sizeof(big_file);//
//
// 测试状态机
//
enum TestState{
	IDLE,
	DIR_CREATE,
	WRITE,
	READ,
	DELETE,
	EXTRA,
	DONE,
};

//-----------------------------------------------------------------------------
//功能: 
//参数: FileNums -- 测试最大文件数，目前最大只能支持到10000个
//返回:
//备注: 
//-----------------------------------------------------------------------------
void __DeepFileTest(const char *Root, const u16 FileNums, const enum TestState State)
{

	u32 j,i;
	s32 Ret;
	char *Path;
	FILE *Fp;
	u8 *Buffer;
	u32 i_1 = 1, i_2 = 1, i_3 = 1, i_4 = 1;
	char *TestFileName = ConfigFileName;
	u32 DataLen = ConfigFileSize;
	unsigned char *TestFile = ConfigFileSource;
    u8 RootLen = strlen(Root);
	u8 FileLen = strlen(TestFileName);
	u8 PathLen = RootLen + FileLen + 2;// 2个'/'
	u32 FileCounts = 0;
	u8 Temp = 1;

	Buffer = malloc(DataLen);
	if(NULL == Buffer)
	{
		TB_PRINT("error: memory run out.\r\n");
		return;
	}

	Path = malloc(PathLen+1);// 1个'\0'
	if(NULL == Path)
	{
		TB_PRINT("error: memory run out.\r\n");
		return;
	}
	
	Path[0] = '/';
	memcpy(Path+1, Root, RootLen);
	Path[RootLen+1] = '/';
	strcpy(Path+RootLen+2, TestFileName);
	TB_PRINT("New Test ->");
	TB_PRINT("File PATH example: \"%s\"\r\n", Path);

	while(Temp);
    do
    {
#if 0
		if((++FileCounts > FileNums)||(TestStateChange))
		{
			i_4 = i_3 = i_2 = i_1 = 1;
			FileCounts = 0;
			TestStateChange = 0;
			switch(State)
			{
			case IDLE: State = WRITE;break;
			case WRITE: State = READ;break;
			case READ: State = READ;break;
			case DELETE: State = DONE;break;
			default :
				TB_PRINT("unknown test state \"0x%x\"\r\n", State);
				State = DONE;
			}
		}
#else
		if(++FileCounts > FileNums)
		{
			free(Path);
			free(Buffer);
			return;
		}
#endif
		if(IDLE == State)
		{
			return;
		}

		if(WRITE == State)
		{
			Fp = fopen(Path, "w+");
			if(NULL == Fp)
			{
				TB_PRINT("cannot open \"%s\"\r\n", Path);
				while(Temp);
				continue;
			}
			for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
			{
				Ret = fwrite(TestFile+j, 512, 1, Fp);
				if(512 != Ret)
					TB_PRINT("\"%s\" write error : \"%d\" should be \"512\"\r\n", Path, Ret);
			}

			Ret = fseek(Fp, 3600, V_SEEK_SET);
			if(Ret)
			{
				TB_PRINT("cannot seek \"%s\"\r\n", Path);
			}

			if(j < DataLen)
			{
				Ret = fwrite(TestFile+j, (DataLen - j), 1, Fp);
				if((DataLen - j) != Ret)
					TB_PRINT("\"%s\" write error : \"%d\" should be \"%d\"\r\n", Path, Ret, (DataLen - j));
			}
			Ret = fflush(Fp);
			if(Ret)
			{
				TB_PRINT("can not flush \"%s\"\r\n", Path);
			}
			Ret = fclose(Fp);
			if(Ret)
			{
				TB_PRINT("can not close \"%s\"\r\n", Path);
			}
		}

		if(READ == State)
		{
			Fp = fopen(Path, "r+");
			if(NULL == Fp)
			{
				TB_PRINT("cannot open \"%s\"\r\n", Path);
				continue;
			}

			memset(Buffer, 0, DataLen);

			for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
			{
				Ret = fread((Buffer + j), 512, 1, Fp);
				if(512 != Ret)
					TB_PRINT("\"%s\" read error : \"%d\" should be \"512\"\r\n", Path, Ret);
			}

			Ret = fseek(Fp, 3600, V_SEEK_SET);
			if(Ret)
			{
				TB_PRINT("cannot seek \"%s\"\r\n", Path);
			}

			if(j < DataLen)
			{
				Ret = fread((Buffer + j), (DataLen - j), 1, Fp);
				if((DataLen - j) != Ret)
					TB_PRINT("\"%s\" read error : \"%d\" should be \"%d\"\r\n", Path, Ret, (DataLen - j));
			}

			for(i = 0; i < DataLen; i++)
			{
				if(Buffer[i] != TestFile[i])
				{
					TB_PRINT("read back compare the byte \"%d\" in \"%d\"unmatch\r\n", i, DataLen);
					break;
				}
			}

			Ret = fclose(Fp);
			if(Ret)
			{
				TB_PRINT("can not close \"%s\"\r\n", Path);
			}
		}

		if(DELETE == State)
		{
			Ret = remove(Path);
			if(Ret)
			{
				TB_PRINT("cannot remove \"%s\"\r\n", Path);
				while(Temp);
			}
		}

		if(DONE == State)
		{
			free(Buffer);
			free(Path);
			break;
		}
        /* 文件名由XXXX_0000~XXXX_9999的循环 */
        if (Path[PathLen - 1] != '9')
        {
            Path[PathLen - 1] = ('0' + i_1++);
            if (10 == i_1)
                i_1 = 1;
        }
        else if (Path[PathLen - 2] != '9')
        {
            Path[PathLen - 2] = ('0' + i_2++);
            Path[PathLen - 1] = '0';
            if (10 == i_2)
                i_2 = 1;
        }
        else if (Path[PathLen - 3] != '9')
        {
            Path[PathLen - 3] = ('0' + i_3++);
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_3)
                i_3 = 1;
        }
        else if (Path[PathLen - 4] != '9')
        {
            Path[PathLen - 4] = ('0' + i_4++);
            Path[PathLen - 3] = '0';
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_4)
                i_4 = 1;
        }

    }while(1);
}
//-----------------------------------------------------------------------------
//功能: 长期深度测试
//参数: MaxFile -- 测试最大文件数，目前最大只能支持到10000个
//		MaxDir -- 测试最大目录树，目前最大只能支持10000个
//返回:
//备注:
//-----------------------------------------------------------------------------
void DeepTest(const char *Root, const u16 MaxDirs, const u16 MaxFiles)
{

	s32 DirFd, Ret;
	char *Path, *PathBackup;
	u32 i_1 = 1, i_2 = 1, i_3 = 1, i_4 = 1;
	char DirName[] = "test_0000";
    u8 RootLen = strlen(Root);
	u8 DirLen = strlen(DirName);
	u8 PathLen = RootLen + DirLen + 2;// 2个'/'
	enum TestState State = IDLE;
	u8 TestStateChange = 0;
	u32 DirCounts = 0;

	TB_PRINT("\r\n");
	TB_PRINT("-------------------------------------------\r\n");
	TB_PRINT("<<<<  - FILE - SYSTEM - DEEP - TEST -  >>>>\r\n");
	TB_PRINT("<<<< root: %-27s >>>>\r\n", Root);
	TB_PRINT("<<<< nums of Dirs: %-19d >>>>\r\n", MaxDirs);
	TB_PRINT("<<<< nums of Files per Dir: %-10d >>>>\r\n", MaxFiles);
	TB_PRINT("-------------------------------------------\r\n");

	Path = malloc(PathLen+1);// 1个'\0'
	PathBackup = malloc(PathLen+1);
	if((NULL == Path) || (NULL == PathBackup))
	{
		TB_PRINT("error: memory run out.\r\n");
		return;
	}

	Path[0] = '/';
	memcpy(Path+1, Root, RootLen);
	Path[RootLen+1] = '/';
	strcpy(Path+RootLen+2, DirName);
	strcpy(PathBackup, Path);
	TB_PRINT("New Test ->");
	TB_PRINT("Dir PATH example: \"%s\"\r\n", Path);

    do
    {
		if((TestStateChange) || (++DirCounts >= MaxDirs))
		{
			i_4 = i_3 = i_2 = i_1 = 1;
			strcpy(Path, PathBackup);
			TestStateChange = 0;
			DirCounts = 0;
			switch(State)
			{
			case IDLE: State = DIR_CREATE;break;
			case DIR_CREATE: State = WRITE;break;
			case WRITE: State = READ;break;
			case READ: State = DELETE;break;
			case DELETE: State = IDLE;break;
			default :
				TB_PRINT("unknown test state \"0x%x\"\r\n", State);
				return;
			}
		}
		if(IDLE == State)
		{
			TestStateChange = 1;
			DirCounts = 0;
			continue;
		}

		if(DIR_CREATE == State)
		{
			DirFd = open(Path, O_DIRECTORY | O_CREAT, 0);
			if(-1 == DirFd)
			{
				TB_PRINT("cannot open dir \"%s\"\r\n", Path);
			}
			else
			{
				Ret = close(DirFd);
				if(Ret)
				{
					TB_PRINT("cannot close dir \"%s\"\r\n", Path);
				}
			}
		}

		if(WRITE == State)
		{
			__DeepFileTest(Path, MaxFiles, State);
		}

		if(READ == State)
		{
			__DeepFileTest(Path, MaxFiles, State);
		}

		if(DELETE == State)
		{

			__DeepFileTest(Path, MaxFiles, State);
			Ret = remove(Path);
			if(Ret)
			{
				TB_PRINT("cannot remove \"%s\"\r\n", Path);
			}
		}

        /* 文件名由XXXX_0000~XXXX_9999的循环 */
        if (Path[PathLen - 1] != '9')
        {
            Path[PathLen - 1] = ('0' + i_1++);
            if (10 == i_1)
                i_1 = 1;
        }
        else if (Path[PathLen - 2] != '9')
        {
            Path[PathLen - 2] = ('0' + i_2++);
            Path[PathLen - 1] = '0';
            if (10 == i_2)
                i_2 = 1;
        }
        else if (Path[PathLen - 3] != '9')
        {
            Path[PathLen - 3] = ('0' + i_3++);
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_3)
                i_3 = 1;
        }
        else if (Path[PathLen - 4] != '9')
        {
            Path[PathLen - 4] = ('0' + i_4++);
            Path[PathLen - 3] = '0';
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_4)
                i_4 = 1;
        }

    }while(1);
}
//-----------------------------------------------------------------------------
//功能: 功能测试
//参数: 
//返回:
//备注:
//-----------------------------------------------------------------------------
void FunctionCheck(const char *Root)
{
	FILE *Fp;
	s32 FileFd, Ret;
	char *Path;
	u8 *Buffer;
	char *TestFileName = ConfigFileName;
	u32 DataLen = ConfigFileSize;
	unsigned char *TestFile = ConfigFileSource;
    u8 RootLen = strlen(Root);
	u8 FileLen = strlen(TestFileName);
	u8 PathLen = RootLen + FileLen + 2;// 2个'/'

	TB_PRINT("\r\n");
	TB_PRINT("---------------------------------\r\n");
	TB_PRINT("<< file system functional test >>\r\n");
	TB_PRINT("---------------------------------\r\n");

	Buffer = (u8*)malloc(DataLen);
	if(NULL == Buffer)
	{
		TB_PRINT("error: memory run out.\r\n");
		while(1);
	}

	// 建立路径
	Path = malloc(PathLen+1);// 1个'\0'
	if(NULL == Path)
	{
		TB_PRINT("error: memory run out.\r\n");
		while(1);
	}
	Path[0] = '/';
	memcpy(Path+1, Root, RootLen);
	Path[RootLen+1] = '/';
	strcpy(Path+RootLen+2, TestFileName);
	
	// 创建一个文件
	Fp = fopen(Path, "w+");
	if(NULL == Fp)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(1);
	}

	// 关闭这个文件
	Ret = fclose(Fp);
	if(Ret)
	{
		TB_PRINT("cannot close \"%s\"\r\n", Path);
		while(1);
	}
	
	// 不能创建同名文件和目录
	FileFd = open(Path, O_CREAT | O_EXCL, 0);
	if(-1 != FileFd)
	{
		TB_PRINT("can not create a dir while there already exists a file with the same name\r\n");
		while(1);
	}
	FileFd = open(Path, O_DIRECTORY | O_CREAT, 0);
	if(-1 != FileFd)
	{
		TB_PRINT("can not create a file while there already exists a file with the same name\r\n");
		while(1);
	}

	// 能重复打开文件
	FileFd = open(Path, 0, 0);
	if(-1 == FileFd)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(1);
	}
	Fp = fopen(Path, "r+");
	if(-1 == FileFd)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(1);
	}
	Ret = close(FileFd);
	if(Ret)
	{
		TB_PRINT("cannot close \"%s\"\r\n", Path);
		while(1);
	}
	Ret = fclose(Fp);
	if(Ret)
	{
		TB_PRINT("cannot close \"%s\"\r\n", Path);
		while(1);
	}
	Fp = fopen(Path, "r+");
	if(-1 == FileFd)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(1);
	}
	FileFd = open(Path, 0, 0);
	if(-1 == FileFd)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(1);
	}
	Ret = close(FileFd);
	if(Ret)
	{
		TB_PRINT("cannot close \"%s\"\r\n", Path);
		while(1);
	}
	Ret = fclose(Fp);
	if(Ret)
	{
		TB_PRINT("cannot close \"%s\"\r\n", Path);
		while(1);
	}

	// stat功能测试
	{
		struct stat FileInifo = {0};
		Ret = stat(Path, &FileInifo);
		if(Ret)
		{
			TB_PRINT("cannot stat \"%s\"\r\n", Path);
			while(1);
		}
	}

	// fstat功能测试
	{
		struct stat FileInifo = {0};

		FileFd = open(Path, 0, 0);
		if(-1 == FileFd)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fstat(FileFd, &FileInifo);
		if(Ret)
		{
			TB_PRINT("cannot fstat \"%s\"\r\n", Path);
			while(1);
		}
		Ret = close(FileFd);
		if(Ret)
		{
			TB_PRINT("cannot close \"%s\"\r\n", Path);
			while(1);
		}

	}

	// freopen功能测试
	{
		char *NewPath;
		FILE *NewFp;

		Fp = fopen(Path, "w+");
		if(NULL == Fp)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fclose(Fp);
		if(Ret)
		{
			TB_PRINT("cannot close \"%s\"\r\n", Path);
			while(1);
		}

		Fp = fopen(Path, "r");
		if(NULL == Fp)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			while(1);
		}
		NewPath = malloc(strlen(Path)+1);
		if(NULL == NewPath)
		{
			TB_PRINT("memory run out! \"%s\"\r\n", Path);
			while(1);
		}
		strcpy(NewPath, Path);
		NewPath[strlen(Path)-1] = '2';
		remove(NewPath);
		NewFp = freopen(NULL, "w+", Fp);
		if(NULL == NewFp)
		{
			TB_PRINT("cannot reopen \"%s\"\r\n", Path);
			while(1);
		}
		NewFp = freopen(NewPath, "w+", Fp);
		if(NULL == NewFp)
		{
			TB_PRINT("cannot open \"%s\"\r\n", NewPath);
			while(1);
		}
		Ret = fclose(NewFp);
		if(Ret)
		{
			TB_PRINT("cannot close \"%s\"\r\n", NewPath);
			while(1);
		}

	}

	// 测试当前工作路径
	{
		char *TempPath;
		char *TempCurrentWorkPath = "/temp", *Root = "/";

		PathLen = (u8)CurWorkPathLen();
		TempPath = (char*)malloc(PathLen);
		if(CurWorkPath(TempPath, PathLen))
		{
			TB_PRINT("current working path is not set!\r\n");
			while(1);
		}
		if(strcmp(TempPath, Root))
		{
			TB_PRINT("current working path\"%s\" is not \"/\"!\r\n", TempPath);
			while(1);
		}
		free(TempPath);

		remove(TempCurrentWorkPath);
		FileFd = open(TempCurrentWorkPath, O_DIRECTORY | O_CREAT, 0);
		if(-1 == FileFd)
		{
			TB_PRINT("cannot open dir\"%s\"\r\n", TempCurrentWorkPath);
			while(1);
		}
		if(SetPWD(TempCurrentWorkPath))
		{
			TB_PRINT("cannot set current working path:\"%s\"\r\n", TempCurrentWorkPath);
			while(1);
		}
		PathLen = (u8)CurWorkPathLen();
		TempPath = (char*)malloc(PathLen);
		if(NULL == TempPath)
		{
			TB_PRINT("memory run out! \"%s\"\r\n", Path);
			while(1);
		}
		if(CurWorkPath(TempPath, PathLen))
		{
			TB_PRINT("current working path is not set!\r\n");
			while(1);
		}
		if(strcmp(TempPath, TempCurrentWorkPath))
		{
			TB_PRINT("current working path\"%s\" is not \"%s\"!\r\n", TempPath, TempCurrentWorkPath);
			while(1);
		}
		free(TempPath);
	}
#if 0
	// 测试ftruncate
	{
		struct stat FileInifo = {0};

		remove(Path);
		FileFd = open(Path, O_DIRECTORY | O_CREAT | O_RDWR, 0);
		if(-1 == FileFd)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			return;
		}
		Ret = ftruncate(FileFd, 3600);
		if(Ret)
		{
			TB_PRINT("cannot ftruncate \"%s\"\r\n", Path);
			while(1); // 错误
		}
		Ret = ftruncate(FileFd, 200);
		if(Ret)
		{
			TB_PRINT("cannot ftruncate \"%s\"\r\n", Path);
			while(1); // 错误
		}
		Ret = fstat(FileFd, &FileInifo);
		if(Ret)
		{
			TB_PRINT("cannot fstat \"%s\"\r\n", Path);
			while(1); // 错误
		}
		if(FileInifo.Size != 200)
		{
			TB_PRINT("truncate \"%s\" is error!\r\n", Path);
			while(1);
		}
	}
#endif
	// 同时操作同一个文件
	{
		FILE *Fp0, *Fp1;
		u32 RetLen, i;

		remove(Path);
		Fp0 = fopen(Path, "w+");
		if(NULL == Fp0)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			while(1);
		}
		Fp1 = fopen(Path, "w+");
		if(NULL == Fp1)
		{
			TB_PRINT("cannot open \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fseek(Fp1, DataLen, V_SEEK_SET);
		if(Ret)
		{
			TB_PRINT("cannot seek \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fflush(Fp1);
		if(Ret)
		{
			TB_PRINT("cannot flush \"%s\"\r\n", Path);
			while(1);
		}
		RetLen = fwrite(TestFile, 1, DataLen, Fp0);
		if(DataLen != RetLen)
		{
			TB_PRINT("cannot write \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fflush(Fp0);
		if(Ret)
		{
			TB_PRINT("cannot flush \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fseek(Fp1, 0, V_SEEK_SET);
		if(Ret)
		{
			TB_PRINT("cannot seek \"%s\"\r\n", Path);
			while(1);
		}
		RetLen = fread(Buffer, 1, DataLen, Fp1);
		if(DataLen != RetLen)
		{
			TB_PRINT("cannot read \"%s\"\r\n", Path);
			while(1);
		}
		for(i = 0; i < DataLen; i++)
		{
			if(TestFile[i] != Buffer[i])
			{
				TB_PRINT("cannot working the same file\r\n");
				while(1);
			}
		}
		Ret = fclose(Fp0);
		if(Ret)
		{
			TB_PRINT("cannot close \"%s\"\r\n", Path);
			while(1);
		}
		Ret = fclose(Fp1);
		if(Ret)
		{
			TB_PRINT("cannot close \"%s\"\r\n", Path);
			while(1);
		}

	}

	{
		free(Buffer);
		free(Path);
	}
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void ExtraTest(u8 NoDelete)
#if 0
{
	FILE *Fp;
	u8 Stop = 1;
	const char *Path = "/efs/middle_file";
	u32 DataLen = sizeof(middle_file);
	char *TestFile = (char*)middle_file;
	u32 i, j, LeftLen, ThisLen;
	s32 Ret;
	char *Buffer = malloc(2048);

	if(!Buffer)
	{
		TB_PRINT("out of memory \r\n");
		return;
	}
	//写
	Fp = fopen(Path, "w+");
	if(NULL == Fp)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(Stop);
	}

	for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
	{
		Ret = fwrite(TestFile+j, 512, 1, Fp);
		if(512 != Ret)
			TB_PRINT("\"%s\" write error : \"%d\" should be \"512\"\r\n", Path, Ret);
	}

	Ret = fseek(Fp, 3600, V_SEEK_SET);
	if(Ret)
	{
		TB_PRINT("cannot seek \"%s\"\r\n", Path);
	}

	if(j < DataLen)
	{
		Ret = fwrite(TestFile+j, (DataLen - j), 1, Fp);
		if((DataLen - j) != Ret)
			TB_PRINT("\"%s\" write error : \"%d\" should be \"%d\"\r\n", Path, Ret, (DataLen - j));
	}

	Ret = fflush(Fp);
	if(Ret)
	{
		TB_PRINT("can not flush \"%s\"\r\n", Path);
	}

	Ret = fclose(Fp);
	if(Ret)
	{
		TB_PRINT("can not close \"%s\"\r\n", Path);
	}

	//
	Fp = fopen(Path, "r+");
	if(NULL == Fp)
	{
		TB_PRINT("cannot open \"%s\"\r\n", Path);
		while(Stop);
	}

	for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
	{
		Ret = fread((Buffer + j), 512, 1, Fp);
		if(512 != Ret)
			TB_PRINT("\"%s\" read error : \"%d\" should be \"512\"\r\n", Path, Ret);
	}

	for(i = 0; i < 2048; i++)
	{
		if(Buffer[i] != TestFile[i])
		{
			TB_PRINT("read back compare the byte \"%d\" in \"%d\"unmatch\r\n", i, DataLen);
			break;
		}
	}

	Ret = fseek(Fp, 3600, V_SEEK_SET);
	if(Ret)
	{
		TB_PRINT("cannot seek \"%s\"\r\n", Path);
	}

	LeftLen = DataLen - 2048;
	for(;;)
	{
		if(LeftLen >= 2048)
			ThisLen = 2048;
		else
			ThisLen = LeftLen;

		Ret = fread((Buffer), 1, ThisLen, Fp);
		if(ThisLen != Ret)
			TB_PRINT("\"%s\" read error : \"%d\" should be \"%d\"\r\n", Path, Ret, ThisLen);

		for(i = 0; i < ThisLen; i++)
		{
			if(Buffer[i] != TestFile[DataLen - LeftLen + i])
			{
				TB_PRINT("read back compare the byte \"%d\" in \"%d\"unmatch\r\n", i, DataLen);
				break;
			}
		}

		LeftLen -= ThisLen;
		if(!LeftLen)
			break;
	}

	Ret = fclose(Fp);
	if(Ret)
	{
		TB_PRINT("can not close \"%s\"\r\n", Path);
		while(Stop);
	}

	//
	if(0 == NoDelete)
	{
		Ret = remove(Path);
		if(Ret)
		{
			TB_PRINT("cannot remove \"%s\"\r\n", Path);
			while(Stop);
		}
	}

}
#else
{
	return;
}
#endif
//-----------------------------------------------------------------------------
//功能:
//参数: FileNums -- 测试最大文件数，目前最大只能支持到10000个
//返回:
//备注:
//-----------------------------------------------------------------------------
void DeepFileTest(const char *Root, const u16 FileNums)
{

	u32 j,i;
	s32 Ret;
	char *Path, *PathBackup;
	FILE *Fp;
	u8 *Buffer;
	u32 i_1 = 1, i_2 = 1, i_3 = 1, i_4 = 1;
	char *TestFileName = ConfigFileName;
	u32 DataLen = ConfigFileSize;
	unsigned char *TestFile = ConfigFileSource;
    u8 RootLen = strlen(Root);
	u8 FileLen = strlen(TestFileName);
	u8 PathLen = RootLen + FileLen + 2;// 2个'/'
	u32 FileCounts = 0;
	u8 TestStateChange = 0;
	enum TestState State = IDLE;
	u8 Stop = 0;
	u8 NoDelete = 0;


	Buffer = malloc(DataLen);
	if(NULL == Buffer)
	{
		TB_PRINT("error: memory run out.\r\n");
		return;
	}

	Path = malloc(PathLen+1);// 1个'\0'
	PathBackup = malloc(PathLen+1);
	if((NULL == Path) || (NULL == PathBackup))
	{
		TB_PRINT("error: memory run out.\r\n");
		return;
	}

	Path[0] = '/';
	memcpy(Path+1, Root, RootLen);
	Path[RootLen+1] = '/';
	strcpy(Path+RootLen+2, TestFileName);
	strcpy(PathBackup, Path);
	TB_PRINT("New Test ->");
	TB_PRINT("File PATH example: \"%s\"\r\n", Path);

	while(Stop);
    do
    {
		if((++FileCounts > FileNums)||(TestStateChange))
		{
			i_4 = i_3 = i_2 = i_1 = 1;
			FileCounts = 0;
			TestStateChange = 0;
			strcpy(Path, PathBackup);
			switch(State)
			{
			case IDLE: State = WRITE;break;
			case WRITE: State = READ;break;
			case READ: State = EXTRA;break;
			case EXTRA: if(NoDelete)
						{
							State = DONE;break;
						}
						else
						{
							State = DELETE;break;
						}

			case DELETE: State = DONE;break;
			default :
				TB_PRINT("unknown test state \"0x%x\"\r\n", State);
				State = DONE;
			}
		}

		if(IDLE == State)
		{
			TestStateChange = 1;
			continue;
		}

		if(WRITE == State)
		{
			Fp = fopen(Path, "w+");
			if(NULL == Fp)
			{
				TB_PRINT("write test:cannot open \"%s\"\r\n", Path);
				while(Stop);
				goto NextFile;
			}
			for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
			{
				Ret = fwrite(TestFile+j, 1, 512, Fp);
				if(512 != Ret)
					TB_PRINT("\"%s\" write error : \"%d\" should be \"512\"\r\n", Path, Ret);
			}

			Ret = fseek(Fp, 3600, V_SEEK_SET);
			if(Ret)
			{
				TB_PRINT("cannot seek \"%s\"\r\n", Path);
			}

			if(j < DataLen)
			{
				Ret = fwrite(TestFile+j, 1, (DataLen - j), Fp);
				if((DataLen - j) != Ret)
					TB_PRINT("\"%s\" write error : \"%d\" should be \"%d\"\r\n", Path, Ret, (DataLen - j));
			}
			Ret = fflush(Fp);
			if(Ret)
			{
				TB_PRINT("write test:can not flush \"%s\"\r\n", Path);
			}
			Ret = fclose(Fp);
			if(Ret)
			{
				TB_PRINT("write test:can not close \"%s\"\r\n", Path);
			}
		}

		if(READ == State)
		{
			Fp = fopen(Path, "r+");
			if(NULL == Fp)
			{
				TB_PRINT("read test:cannot open \"%s\"\r\n", Path);
				goto NextFile;
			}

			memset(Buffer, 0, DataLen);

			for(j = i = 0; ((j <= DataLen) && (i < 4)); j += 512, i++)
			{
				Ret = fread((Buffer + j), 1, 512, Fp);
				if(512 != Ret)
					TB_PRINT("\"%s\" read error : \"%d\" should be \"512\"\r\n", Path, Ret);
			}

			Ret = fseek(Fp, 3600, V_SEEK_SET);
			if(Ret)
			{
				TB_PRINT("read test:cannot seek \"%s\"\r\n", Path);
			}

			if(j < DataLen)
			{
				Ret = fread((Buffer + j), 1, (DataLen - j), Fp);
				if((DataLen - j) != Ret)
				{
					TB_PRINT("\"%s\" read error : \"%d\" should be \"%d\"\r\n", Path, Ret, (DataLen - j));
				}
			}

			for(i = 0; i < DataLen; i++)
			{
				if(Buffer[i] != TestFile[i])
				{
					TB_PRINT("read back compare the byte \"%d\" in \"%d\"unmatch\r\n", i, DataLen);
					break;
				}
			}

			Ret = fclose(Fp);
			if(Ret)
			{
				TB_PRINT("read test:can not close \"%s\"\r\n", Path);
				while(Stop);
			}
		}

		if(EXTRA == State)
		{
			ExtraTest(NoDelete);
			TestStateChange = 1;
		}

		if(DELETE == State)
		{
			Ret = remove(Path);
			if(Ret)
			{
				TB_PRINT("cannot remove \"%s\"\r\n", Path);
				while(Stop);
			}
		}

		if(DONE == State)
		{
			State = IDLE;
			TestStateChange = 1;
			continue;
		}

NextFile:
        /* 文件名由XXXX_0000~XXXX_9999的循环 */
        if (Path[PathLen - 1] != '9')
        {
            Path[PathLen - 1] = ('0' + i_1++);
            if (10 == i_1)
                i_1 = 1;
        }
        else if (Path[PathLen - 2] != '9')
        {
            Path[PathLen - 2] = ('0' + i_2++);
            Path[PathLen - 1] = '0';
            if (10 == i_2)
                i_2 = 1;
        }
        else if (Path[PathLen - 3] != '9')
        {
            Path[PathLen - 3] = ('0' + i_3++);
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_3)
                i_3 = 1;
        }
        else if (Path[PathLen - 4] != '9')
        {
            Path[PathLen - 4] = ('0' + i_4++);
            Path[PathLen - 3] = '0';
            Path[PathLen - 2] = '0';
            Path[PathLen - 1] = '0';
            if (10 == i_4)
                i_4 = 1;
        }

    }while(1);
}
//-----------------------------------------------------------------------------
//功能: BOOT文件测试
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void BootFunctionTest(void)
{
	u32 i, j, LefToRW;
	FILE *Fp;
	u8 *FakeBinOrig, *ChkBufOrig;
	char *FakePath = "/iboot/apptest.bin";
	char *FakePath2 = "/iboot/app2.bin";
	u8 *FakeBin = (u8 *)small_file;
	u32 SizeofFakeBin = sizeof(small_file);
	u8 *ChkBuf = malloc(SizeofFakeBin);
	u32 FragmentSize = 200;

	if(!ChkBuf)
		while(1);

	FakeBinOrig = FakeBin;
	memset(ChkBuf, 0x00, SizeofFakeBin);
	ChkBufOrig = ChkBuf;
	Fp = fopen(FakePath, "w+");
	if(!Fp)
		while(1);

	for(i = 0, j = 0; i < SizeofFakeBin ; i += FragmentSize)
	{
		if(200 != fwrite(FakeBin, 1, FragmentSize, Fp))
			while(1);

		FakeBin += FragmentSize;
		j++;
		if(j >= 5)
			break; // 写5次
	}

	LefToRW = SizeofFakeBin - FragmentSize * j;
	if(LefToRW != (j = fwrite(FakeBin, 1, LefToRW, Fp)))
		while(1);

	if(fclose(Fp))
		while(1);

	Fp = fopen(FakePath, "r");
	if(!Fp)
		while(1);

	for(i = 0, j = 0; i < SizeofFakeBin ; i += FragmentSize)
	{
		if(200 != fread(ChkBuf, 1, FragmentSize, Fp))
			while(1);

		ChkBuf += FragmentSize;
		j++;
		if(j >= 5)
			break; // 读5次
	}

	LefToRW = SizeofFakeBin - FragmentSize * j;
	if(LefToRW != (j = fread(ChkBuf, 1, LefToRW, Fp)))
		while(1);

	for(i = 0; i < SizeofFakeBin; i++)
	{
		if(ChkBufOrig[i] != FakeBinOrig[i])
			while(1);
	}
	
	if(fclose(Fp))
		while(1);

	Fp = fopen(FakePath2, "r");
	if(!Fp)
		while(1);
}
