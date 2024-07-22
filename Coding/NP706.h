#pragma once

#include "Coder.h"

class __declspec(dllexport) CNP706 : public CCoder
{
public:
	// 专用车机命令字定义
	enum VEHICLE_CMDS_NP850
	{
		VHC_CMD_SPECIFIED_RESPOND_GPSINFO = 7500, // 7500 指定回报车辆位置信息
		VHC_CMD_SPECIFIED_RESPOND_SIMINFO, // 7501 指定回报SIM卡信息
		VHC_CMD_RESPOND_SIMINFO, // 7502 回报SIM卡信息
		VHC_CMD_RESPOND_VHCINFO, // 7503 回报车机信息
		VHC_CMD_SINGLE_RESPON_TEL, // 7504 单呼回报号码设置
		VHC_CMD_RESPOND_CONTINUE_GPRS, // 7506 连续回报gprs
	};

	// 专用车机状态字定义
	enum VEHICLE_STATES_NP850
	{
		VHC_STATE_OPEN_DOOR = 0X00010000, // 门被开启
		VHC_STATE_SHOCK = 0X00020000, // 车子震动
		VHC_STATE_ENGION_START = 0X00040000, // 引擎启动
		VHC_STATE_OIL_SHUTOFF = 0X00080000, // 车已断油
		VHC_STATE_REPORT_ONTIME = 0X00100000, // 定时回报
		VHC_STATE_NOT_USE = 0X00200000, // 暂停试用
		VHC_STATE_SECURITY_SET = 0X00400000, // 防盗设定
		VHC_STATE_SECURITY_RESET = 0X00800000, // 防盗解除
	};

public:
	CNP706();

public:
	bool Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput) override;
	bool DeCoding(CStringA& strMsg, CMessageReceived& msg) override;
	virtual void GetStatus(CStringA& strM, CMessageReceived& msg);

private:
	bool DeCodingCTMsg(LPCSTR szMsg, CMessageReceived& msg);
};
