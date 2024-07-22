// Coder.h: interface for the CCoder class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include "Message.h"

static const double LOCAL_SYS_TIME_SPAN = 0.33333333333333;

class __declspec(dllexport) CCoder
{
public:
	struct SStatus
	{
	public:
		DWORD dwMasker;
		unsigned char nAbnomal;
		CString strMeaning;

		SStatus()
		{
			dwMasker = 0X00;
			nAbnomal = 0;
		}
	};

	struct SParameter
	{
	public:
		CString m_password;

		static CString m_SMSCard;

		CString strParam[4];
		DWORD dwParam[4];
		double fParam[4];
		double dDate[2];
		int iSelect;
	};

public:
	// 公用车机命令字定义
	enum VEHICLE_CMDS_PUBLIC
	{
		VHC_CMD_SET_MODE = 7011, // 7019 设置车机通讯模式 0 MSN 1 GPRS
		VHC_CMD_SET_SERVER, // 7020 设置服务器IP地址和端口
		VHC_CMD_SET_APN, // 7020 GPRS APN
		VHC_CMD_MODIFY_PSW, // 7002 修改密码
		VHC_CMD_SYS_RESET, // 7009 车机重启

		VHC_CMD_RESPOND_SINGLE, // 7004 单次回报
		VHC_CMD_RESPOND_CONTINUE, // 7000 连续回报(定次)
		VHC_CMD_RESPOND_CONTINUE_STOP, // 7001 停止连续回报
		VHC_CMD_RESPOND_CONTINUE_TIME, // 7018 连续回报(定时)		

		VHC_CMD_RESPOND_STATUS, // 7003 回报车机状态

		VHC_CMD_SET_SMSSIM_CENTER, // 7003 设置回报监控中心号码
		VHC_CMD_SET_SMSTEL_ALERT, // 7003 设置紧急情况下短信通知号码
		VHC_CMD_SET_VOXTEL_ALERT, // 7003 设置紧急情况下车机自动拨通的语音电话号码

		VHC_CMD_POWER_ON,
		VHC_CMD_POWER_OFF,
		VHC_CMD_OIL_OFF, // 7005 切断油路
		VHC_CMD_OIL_ON, // 7006 恢复油路
		VHC_CMD_SND_MESSAGE, // 7010 发送短信
		VHC_CMD_CLEAR_WARNING, // 7013 解除报警
		VHC_CMD_SET_OVERSPEED, // 7014 设定超速值          
		VHC_CMD_SET_REGION, // 7015 区域设定      
		VHC_CMD_SET_REGION_ALARM, // 7016 开启/关闭越区报警
		VHC_CMD_SET_SMSCENTER_CODE, // 7017 设置短信中心号码
	};

	// 公用车机状态字定义
	enum VEHICLE_STATES_PUBLIC
	{
		VHC_STATE_VEHICLE_REPLAY = 0X80000000, // 轨迹回放
		VHC_STATE_VEHICLE_REPLAY_END = 0X00008000, // 回放结束
		VHC_STATE_NORMAL = 0X00000000, // 正常状态
		VHC_STATE_WARNING_SOS = 0X00000001, // 紧急求救报警
		VHC_STATE_WARNING_OVERSPEED = 0X00000002, // 超速报警
		VHC_STATE_WARNING_OFFPOWER = 0X00000004, // 断电报警
		VHC_STATE_WARNING_OUTREGION = 0X00000008, // 越区报警
		VHC_STATE_WARNING_MOVE = 0X00000010, // 拖掉报警
		VHC_STATE_WARNING_THIEF = 0X00000020, // 防盗报警
		VHC_STATE_WARNING_SHOCK = 0X00000040, // 震动报警
		VHC_STATE_WARNING_GPSLINE = 0X00000080, // GPS天线断线报警
		VHC_STATE_WARNING_LOWPOWER = 0X00000100, // 低电压报警
		VHC_STATE_GPS_V = 0X00000200, // 无效定位
	};

public:
	CCoder();
	virtual ~CCoder();

public:
	bool m_bSupportHTTP;// 车机是否支持HTTP发送协议
	bool m_bSupportUDP;// 车机是否支持GPRS通讯方式下的UDP发送协议
	bool m_bSupportTCP;// 车机是否支持GPRS通讯方式下的UDP发送协议
	bool m_bSupportSMS;// 车机是否支持gsm通讯方式下的SMS发送协议

	BOOL m_bUseUnicodeCharSet;
	CString m_strDescription;//通过数据库编码解码描述唯一识别该车机

public:
	SStatus* m_pStatus;
	int m_nStatus;

public:
	virtual void FetchStatus(CDatabase* pDatabase);

public:
	static void SetSMSCard(CString strSMSCard);
	static CCoder* GetCoder(CStringA strIVU);

public:
	virtual BOOL IsAbnomal(DWORD dwStatus);
	virtual BOOL IsAlarm(DWORD dwStatus);
	virtual BOOL IsPersonal(DWORD dwStatus);

	virtual bool IsSupportSMS();
	virtual bool IsSupportUDP();
	virtual bool IsSupportTCP();
	virtual bool IsSupportHTTP();

	virtual BOOL IsUseUnicodeCharSet() const;
	virtual CString GetDescription() const;

	virtual CString GetStatusDescrip(DWORD dwStatus);

public:
	virtual bool DeCoding(CStringA& strMsg, CMessageReceived& msg);
	virtual bool Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput);
};
