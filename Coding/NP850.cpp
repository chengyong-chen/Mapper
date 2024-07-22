// Coder_NP850.cpp: implementation of the CNP850 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NP850.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNP850 NP850;

CNP850::CNP850()
{
	m_strDescription = _T("NP850");
}

CNP850::~CNP850()
{
}

// return: 
//        2 参数不完整
bool CNP850::Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput)
{
	CString strFormator;
	switch(wCmd)
	{
	case VHC_CMD_RESPOND_CONTINUE:
		if(parameter.dwParam[0]!=-1)
			strOutput.Format("I%sZTIME%d", parameter.m_password, parameter.dwParam[0]);
		else
			return false;
		break;
	case VHC_CMD_RESPOND_CONTINUE_STOP:
		strOutput.Format("I%sZTIME0", parameter.m_password);
		break;
	case VHC_CMD_MODIFY_PSW:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sN%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_SET_SMSSIM_CENTER:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sD%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_SINGLE_RESPON_TEL:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sC%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_SPECIFIED_RESPOND_GPSINFO:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sZMSG%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_SPECIFIED_RESPOND_SIMINFO:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sZCT%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_RESPOND_SINGLE:
		strOutput.Format("I%sZMSG", parameter.m_password);
		break;
	case VHC_CMD_RESPOND_SIMINFO:
		strOutput.Format("I%s", parameter.m_password);
		break;
	case VHC_CMD_SYS_RESET:
		strOutput.Format("I%sZRES", parameter.m_password);
		break;
	case VHC_CMD_RESPOND_VHCINFO:
		strOutput.Format("I%sZCHK", parameter.m_password);
		break;
	case VHC_CMD_OIL_OFF:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sZGPO41%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	case VHC_CMD_OIL_ON:
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput.Format("I%sZGPO40%s", parameter.m_password, parameter.strParam[0]);
		else
			return false;
		break;
	default:
		return CNP706::Encoding(wCmd, parameter, strOutput);
	}

	return true;
}

bool CNP850::DeCoding(CStringA& strMsg, CMessageReceived& msg)
{
	return CNP706::DeCoding(strMsg, msg);
}
