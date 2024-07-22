// Coder_NP850.cpp: implementation of the CNP850 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NP860.h"
#include "Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNP860 NP860;

CNP860::CNP860()
{
	m_strDescription = _T("NP860");
}

CNP860::~CNP860()
{
}

// return: 
//        2 参数不完整
bool CNP860::Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput)
{
	CString strFormator;
	switch(wCmd)
	{
	case VHC_CMD_CLEAR_WARNING:
		break;
	case VHC_CMD_SET_MODE: // 7506 UDP传送模式
		strOutput.Format("I%sZAPN,INTERNET,2", parameter.m_password);
		break;
	case VHC_CMD_SET_SERVER: // 7019 设置服务器ip地址和端口
		if(parameter.strParam[0].IsEmpty()||parameter.strParam[1].IsEmpty())
			return false;
		else
			strOutput.Format("I%sZIP,%s,%s", parameter.m_password, parameter.strParam[0], parameter.strParam[1]);
		break;
	case VHC_CMD_RESPOND_CONTINUE_GPRS: // 7508 连续回报gprs
		if(parameter.dwParam[0]==-1||parameter.dwParam[1]==-1)
			return false;
		else
			strOutput.Format("I%sZTRACK,%d,%d,GPRS", parameter.m_password, parameter.dwParam[0], parameter.dwParam[1]);
		break;
	case VHC_CMD_RESPOND_CONTINUE:
		if(parameter.dwParam[0]==-1)
			return false;
		else
			strOutput.Format("I%sZTIME%d", parameter.m_password, parameter.dwParam[0]);
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
		break;;
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

bool CNP860::DeCoding(CStringA& strMsg, CMessageReceived& msg)
{
	const char* szField = (LPCSTR)strMsg;

	// >C800000,MFFFFFF,OFFFF,IFFFF,D235959,A,N2505.5344,E12133.9181,T1851.8,H359.9,Y010305,G24*2B
	// >C900001,M000080,A,N2503.4109,E12128.2699,T0.00,H,G3*2A
	const int len = strlen(szField);
	if(len<30)
		return false;

	int i = 0;
	for(; i<len; i++)
	{
		if(szField[i]==_T('>'))
			break;
	}
	if(i>=len)
		return false;

	szField += i;
	if(szField[0]!=_T('>'))
		return false;

	msg.m_wCommand = GPSSERVER_VEHICLE_MOVE;
	i = 1;
	if(szField[i]!=_T('C'))
		return false;
	char temp[10];
	memset(temp, 0, 10*sizeof(char));
	msg.m_strIVUSN = strncpy(temp, szField+i+1, 6);
	i += 8;
	if(szField[i]!='M')
		return false;
	CStringA ddd = CStringA(strncpy(temp, szField + i + 1, 6));
	GetStatus(ddd, msg);

	i += 8;
	int j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='O')
		{
			i = j;
			break;
		}
	}
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='I')
		{
			i = j;
			break;
		}
	}
	int h, m, s;
	h = m = s = 0;
	j = i;
	char* stopstr;
	for(; j<len; j++)
	{
		if(szField[j]=='D')
		{
			i = j;
			char ch[3];
			memset(ch, 0, sizeof(char));
			memcpy(ch, szField+i+1, 2);
			h = strtol(ch, &stopstr, 10);
			memcpy(ch, szField+i+3, 2);
			m = strtol(ch, &stopstr, 10);
			memcpy(ch, szField+i+5, 2);
			s = strtol(ch, &stopstr, 10);
			break;
		}
	}
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]==',')
		{
			i = j;
			if(szField[i+1]=='A')
				msg.m_bAvailable = true;
			else
				msg.m_bAvailable = false;
			break;
		}
	}

	double degree, minints;
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='N')
		{
			i = j;
			msg.m_Lat = strtod(szField+i+1, &stopstr);
			msg.m_Lat /= 100.0;
			degree = int(msg.m_Lat);
			minints = (msg.m_Lat-degree)*100;
			msg.m_Lat = degree+minints/60;
			break;
		}
	}
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='E')
		{
			i = j;
			msg.m_Lon = strtod(szField+i+1, &stopstr);
			msg.m_Lon /= 100.0;
			degree = int(msg.m_Lon);
			minints = (msg.m_Lon-degree)*100;
			msg.m_Lon = degree+minints/60;
			break;
		}
	}
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='T')
		{
			i = j;
			msg.m_Veo = strtod(szField+i+1, &stopstr);
			msg.m_Veo *= 1.8532f;
			break;
		}
	}
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]==_T('H'))
		{
			i = j;
			msg.m_Direct = strtod(szField+i+1, &stopstr);
			break;
		}
	}
	const COleDateTime time = COleDateTime::GetCurrentTime();
	int D = time.GetDay();
	int M = time.GetMonth();
	int Y = time.GetYear();
	j = i;
	for(; j<len; j++)
	{
		if(szField[j]=='Y')
		{
			i = j;
			char ch[3];
			memset(ch, 0, sizeof(char));
			memcpy(ch, szField+i+1, 2);
			D = strtol(ch, &stopstr, 10);
			memcpy(ch, szField+i+3, 2);
			M = strtol(ch, &stopstr, 10);
			memcpy(ch, szField+i+5, 2);
			Y = strtol(ch, &stopstr, 10)+2000;
			break;
		}
	}
	msg.m_varGPSTime = COleDateTime(Y, M, D, h, m, s);
	// i += 8;
	// if(szField[i] != _T('G')) return 1;

	if(!msg.m_bAvailable)
		msg.m_dwStatus |= VHC_STATE_GPS_V;

	return true;

	// return CNP706::DeCoding(strMsg, msg);
}

void CNP860::GetStatus(CStringA& strM, CMessageReceived& msg)
{
	CStringA strState = "";
	int i = atoi(strM.Mid(5, 1));

	msg.m_dwStatus = 0;

	if((i&0x01)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_OPEN_DOOR;
	}
	if((i&0x02)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_SHOCK;
	}
	if((i&0x04)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_WARNING_SOS;
	}
	if((i&0x08)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_ENGION_START;
	}

	i = atoi(strM.Mid(4, 1));
	if((i&0x01)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_WARNING_LOWPOWER;
	}
	if((i&0x02)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_OIL_SHUTOFF;
	}
	if((i&0x04)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_WARNING_MOVE;
	}
	if((i&0x08)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_REPORT_ONTIME;
	}

	i = atoi(strM.Mid(3, 1));
	if((i&0x01)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_NOT_USE;
	}
	if((i&0x02)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_SHOCK;
	}
	if((i&0x04)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_WARNING_SHOCK;
	}
	if((i&0x08)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_WARNING_THIEF;
	}

	i = atoi(strM.Mid(2, 1));
	/*if((i&0x01) != 0)
	{
		msg.m_wCommand = GPSSERVER_VEHICLE_WARM;
		msg.m_dwStatus |= VHC_STATE_WARNING_GPSLINE;
		strState += _T(" 被拆报警");
	}*/
	if((i&0x02)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_SECURITY_SET;
	}
	if((i&0x04)!=0)
	{
		msg.m_dwStatus |= VHC_STATE_SECURITY_RESET;
	}
}

BOOL CNP860::IsPersonal(DWORD dwStates)
{
	if(IsAbnomal(dwStates))
		return TRUE;
	else
		return FALSE;
}
