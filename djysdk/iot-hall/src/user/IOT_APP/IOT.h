#ifndef	__IOT_H__
#define	__IOT_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "stdio.h"
#include "string.h"

#include "os.h"
#include "int.h"
#include "lowpower.h"
#include "uart.h"

/*=====================================================================*/
#define	TRANSFER_MODULE_LOG	"Transfer-20160413-1645\r\n"
#define	MONITOR_MODULE_LOG	"Monitor-20160413-1645\r\n"
#define TRANSFER_MODULE_UPDATE_LOG  "Transfer UPDATE\r\n"
#define MONITOR_MODULE_UPDATE_LOG   "Monitor Update\r\n"

/*=====================================================================*/

#define SINGLE_UPDATE_PKG_SIZE_MAX     180
#define UPDATE_PKG_NUM_MAX             2048    //这意味着升级的APP最大程序为360k.
#define MONITOR_NUM_MAX                256     //一个中继器对应的监测端节点最大数.
#define CN_REPEAT_MAX_NUM              4
//#define	SX1278_TX_TIME		800		//(2.6K)SX1278 RF数据包发送所需要的时间(ms)
#define	SX1278_TX_TIME		200		//(18.5K)SX1278 RF数据包发送所需要的时间(ms)
#define	SX1278_RX_TIMEOUT	2000	//SX1278 RF数据包接收超时时间(ms)

/*=====================================================================*/
#define	BATT_VOL_STEP	50	//电池电压步进值(50mV)
#define	VOICE_VOL_STEP	10	//音量电压步进值(10mV)

//升级时中继器向主机或者监测端节点向中继器回复ACK命令，dat区内容
#define ACK_CODE_UPDATE_CMD       0xA1   //UPDATE_CMD 回复的ACK
#define ACK_CODE_UPDATE_MINITOR   0xA2   //UPDATE_MONITOR_NUM回复的ACK

//升级时中继器向主机或者监测端向主机回复UPDATE_ACK命令时，dat区内容
#define ACK_CODE_UPDATE_SUCCESS  0xA1
#define ACK_CODE_UPDATE_FAILED   0xA2
#define ACK_CODE_UPDATE_END      0xA3

//升级失败原因
#define CN_COMMUNICATION_ERROR   0xA1
#define CN_FLASH_WRITE_ERROE     0xA2
#define CN_MONITOR_NO_WORK       0xA3


//升级结果
#define CN_UPDATE_FAILED       0xA1
#define CN_UPDATE_SUCCESS      0xA2

//节点角色，分中继器和监测端节点
#define CN_TRANSFER_ROLE      0xA1
#define CN_MONITOR_ROLE       0xA2

/*=====================================================================*/

//主机/中继端发给监测端的命令
#define	GET_STATUS				0x01	//获得状态
#define	SET_VOICE_ONOFF			0x02	//设置音频部件开/关
#define	GET_VOICE_VOLUME		0x03	//获得音频音量(电压值)
#define	RESET_VOICE_VOLUME		0x04	//重新开始记录音量值(之前记录的值将清零)
#define	GET_VOICE_RECODE		0x05	//获得录音数据
#define	SET_ENTER_SLEEP_TIME	0x06	//设置空闲状态下,进入睡眠状态时间.
#define	VOICE_RECODE_RESTART	0x07	//重新开始录音
#define	VOICE_PLAY_HOST			0x08	//播放数据(由Host传过去的声音数据)
#define	VOICE_PLAY_SOUND_1		0x09	//播放本地预置音频1(询问是否有人)
#define	WAKEUP_DEVICE			0xE0	//用于Host端唤醒设备
#define	ACK						0xF0	//应答
#define UPDATE_CMD              0xA0    //升级命令
#define UPDATE_DATA             0xB0    //升级有效数据
#define UPDATE_ACK              0xC0    //升级回复(包括中继器回复主机和监测端回复中继器)
#define UPDATE_DATA_REPEAT      0xD0    //前面错误，再次传输
#define UPDATE_MONITOR_NUM       0xD1    //传输网络中监测端总数





#pragma	pack(1)

#define	HOST_PKT_TAG	0xF0A0

//主机端数据包头结构
struct	tagHOST_PKT_HDR
{
	u16 tag;		//HOST_PKT_TAG
	u8  hdr_len;	//host头长度(6字节)
	u8  net_id;		//网络ID
	u8  usr_id_h;	//用户ID高位
	u8  usr_id_l;	//用户ID低位
	u8  dat[0];		//数据区开始
};


#define	MON_PKT_TAG		0xF0A1	//监测端数据包标识

//监测端数据包头结构
struct	tagMON_PKT_HDR
{
	u16 tag;		//MON_PKT_TAG
	u8  cmd;		//命令
	u16 pkgnum;     //包总数，当cmd不为UPDATE_CMD、UPDATE_DATA、UPDATE_ACK时，
                    //此域设为0x00.当cmd为UPDATE_DATA及UPDATE_ACK分别表示文件
                    //需要发送的数据包数和需要回复ACK的包数.
	u16 index;      //升级包序号，当cmd不为UPDATE_DATA及 UPDATE_ACK时，此域设为0x00.
	u8  dat_len;	//数据长度

	u32 ecc_code;   //ECC校验码,当cmd不为UPDATE_DATA时，此域设为0x00000000.
	u8	dat[0];		//数据区开始
};


struct tagUpdatePara
{
	u16 pkgnum;
	u16 index;
	u32 ecc_code;
};

#define	SIZEOF_MONPKT_HDR		(sizeof(struct tagMON_PKT_HDR))
#define	SIZEOF_MONPKT_END		(2) //CS8+0x00


//GET_STATUS命令的数据结构
struct	tagMON_STATUS_DATA
{
	u32 smog_status;	//烟雾状态
	u8  batt_voltage;	//电池电压(x50mV)
	s8	temperature;	//温度值
};

#define	SIZEOF_STATUS_PKT	(sizeof(struct tagMON_PKT_HDR) + sizeof(struct	tagMON_STATUS_DATA) + 2)

//SET_VOICE_ONOFF数据结构
struct	tagMON_SET_VOICE_ONOFF_DATA
{
	u8 onoff;		//0:关闭; 1:打开
};
#define	SIZEOF_SET_VOICE_ONOFF_PKT	(sizeof(struct tagMON_PKT_HDR) + sizeof(struct	tagMON_SET_VOICE_ONOFF_DATA) + 2)


//GET_VOICE_VOLUME数据结构
#define	VOICE_CHECK_COUNT	180		//音量检测深度

struct	tagMON_VOICE_VOLUME_DATA
{
	u8 vol[VOICE_CHECK_COUNT];
};

#define	SIZEOF_VOICE_VOLUME_PKT	(sizeof(struct tagMON_PKT_HDR) + sizeof(struct	tagMON_VOICE_VOLUME_DATA) + 2)

extern u8	VoiceVolBuf[VOICE_CHECK_COUNT];

//SET_ENTER_SLEEP_TIME数据结构
struct	tagMON_SET_ENTER_SLEEP_TIME_DATA
{
	u32 time;	//空闲状态下,进入睡眠状态时间,以1秒为步进
};

#define	SIZEOF_SET_ENTER_SLEEP_TIME_PKT	(sizeof(struct tagMON_PKT_HDR) + sizeof(struct	tagMON_SET_ENTER_SLEEP_TIME_DATA) + 2)

/*=====================================================================*/
//GET_VOICE数据结构

#define	VOICE_DATA_PKT_NUM		(20)
#define	SIZEOF_VOICE_DATA_SLOT	(160)

struct	tagVOICE_DATA
{
	u8 pkt_max;		//最大数据包编号
	u8 pkt_cur;		//当前数据包编号
	u8 dat[SIZEOF_VOICE_DATA_SLOT];	//数据区
};
#define	SIZEOF_VOICE_DATA_PKT	(sizeof(struct tagMON_PKT_HDR) + sizeof(struct	tagVOICE_DATA) + 2)

/*=====================================================================*/

#pragma	pack()

/*=====================================================================*/
#define	SLEEP_PKT_TAG		0xF0A2	//让无线模块进入休眠.
//A2,F0,FE,00,92,00

/*=====================================================================*/

#define	VOICE_STATUS_NONE			0
#define	VOICE_STATUS_POWER_ON		1
#define	VOICE_STATUS_POWER_OFF		2
#define	VOICE_STATUS_RECODE_RESTART	3	//重新开始录音
#define	VOICE_STATUS_STOP			4	//暂停
#define	VOICE_STATUS_PLAY			5
#define	VOICE_STATUS_PLAY_SOUND_1	6	//播放提示音

extern u8 voice_rec_buf[VOICE_DATA_PKT_NUM*SIZEOF_VOICE_DATA_SLOT];
extern u8 voice_play_buf[VOICE_DATA_PKT_NUM*SIZEOF_VOICE_DATA_SLOT];
extern int voice_rec_count;
extern int voice_play_count;
extern bool_t IsTransfer;

const unsigned char test_voice_data[];
const unsigned char voice_data_1[];

int VoiceGetStatus(void);
void VoiceSetStatus(int status);
int Voice_GetRecodeCount(void);

/*=====================================================================*/

void IOT_Startup(void);


void IOT_CommonInit(void);
void DebugPutHex(u32 Offset,u32 Count,const void *Buf);
bool_t	IsTransferBoard(void);
void IOT_LED_ON(void);
void IOT_LED_OFF(void);
u8  IOT_GetExternVoltage(void);

int	IOT_MakePacket(u8 *buf,u8 cmd,const void *dat,u8 len,struct tagUpdatePara *pUpdatePara);
bool_t	IOT_SendPacket(u8 cmd,const void *dat,u8 len,struct tagUpdatePara *pUpdatePara);
int		IOT_GetPacket(u8 *buf,u32 wait_ms);

void IOT_TemperatureModuleInit(void);
s8  IOT_GetTemperature(void);

void IOT_SmogModuleInit(void);
bool_t	IOT_GetSmogStatus(void);
void IOT_SmogCheck_200ms(void);

void IOT_VoiceModuleInit(void);
void IOT_VoicePowerOn(void);
void IOT_VoicePowerOff(void);
void IOT_VoiceEncodeEnable(void);
void IOT_VoiceEncodeDisable(void);
u8   IOT_GetVoiceVoltage(void);
void IOT_VoiceResetVolume(void);
void IOT_VoiceVoltageCheck_200ms(void);
//void VoicePlay(u8 *play_buf,u32 size);
//int Voice_Recode(u8* rec_buf,int size);

int MA2400_Send(const u8 *buf,u32 size);
int MA2400_Receive(u8 *buf,u32 size,u32 wait_ms);
void MA2400_SetWorkingMode(u8 mode);
void MA2400_SetBaudrate(void);
void MA2400_SetEncoderEnable(bool_t enable);
void MA2400_SetDecodeEnable(bool_t enable);
void MA2400_SetChecksumEnable(bool_t enable);
void MA2400_SetVolume(void);


void IOT_SX1278_ModuleInit(void);
void SX1278_EnterNormalMode(void);
void SX1278_EnterCenterMode(void);
void SX1278_SendWakeupCode(void);
int	SX1278_Send(const u8 *buf,u32 size);
int	SX1278_Receive(u8 *buf,u32 size,u32 wait_ms);

void SX1278_WriteCfg(const u8 *cfg_dat);
bool_t SX1278_ReadCfg(u8 *cfg_buf);

bool_t SX1278_SetNetId(u8 net_id);
bool_t SX1278_GetId(u8 *net_id,u8 *user_id_h,u8 *user_id_l);

void    SX1278_EnterSleep(void);

//bool_t SX1278_SetId(u8 net_id,u8 user_id_h,u8 user_id_l);
//bool_t SX1278_GetId(u8 *net_id,u8 *user_id_h,u8 *user_id_l);

void IOT_MonitorMain(bool_t enter_sleep);
void IOT_TransferMain(void);

void IOT_MonitorUpdate(void);
void IOT_TransferUpdate(void);
void IOT_Update(void);
void TransferToMonitorUpdate(void);
void HandleAfterUpdate(void);
int Transfer_SendToHost(const u8 *buf,int size);
void UpdateRecvData(u8 role,struct tagMON_PKT_HDR* iot_hdr);
s32 DealECCandWriteFlash(u8 *pdata,u16 pkgindex,u8 len,u32 ecc,u8 role);

/*=====================================================================*/
/*=====================================================================*/

#ifdef	__cplusplus
}
#endif

#endif	/*__IOT_H__*/
