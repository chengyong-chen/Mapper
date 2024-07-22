// HYGPRS_Ver1_Coder.h: interface for the StarG3_PS660 class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include "Coder.h"

class __declspec(dllexport) CStarG3_PS660 : public CCoder
{
public:
	// 专用车机命令字定义
	enum VEHICLE_CMDS_GPRS_HY
	{
	};

	// 专用车机状态字定义
	enum VEHICLE_STATES_GPRS_HY
	{
	};

public:

public:
	CStarG3_PS660();

public:
	bool Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput) override;
	bool DeCoding(CStringA& strMsg, CMessageReceived& msg) override;
};
