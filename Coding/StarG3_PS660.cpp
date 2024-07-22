// HYGPRS_Ver1_Coder.cpp: implementation of the CStarG3_PS660 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "StarG3_PS660.h"
#include <atlbase.h>
#include "Instruction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStarG3_PS660 StarG3_PS660;

CStarG3_PS660::CStarG3_PS660()
{
	m_bUseUnicodeCharSet = FALSE;

	m_strDescription = _T("STARG3_PS660");
}

bool CStarG3_PS660::Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput)
{
	switch(wCmd)
	{
	case 7101: //设定通信模式  
	{
		if(parameter.dwParam[0]==0)
			strOutput.Format("*SMD,%s,0,%s", parameter.m_password, parameter.m_SMSCard);
		else if(parameter.dwParam[0]==1)
			strOutput.Format("*SMD,%s,3,%s", parameter.m_password, parameter.m_SMSCard);
		else
			return false;
	}
	break;
	case 7102: //设定服务器的IP地址和端口      
		if(parameter.strParam[0].IsEmpty()==TRUE||parameter.strParam[1].IsEmpty()==TRUE)
			return false;
		else
			strOutput.Format("*SIP,%s,%s:%s,%s", parameter.m_password, parameter.strParam[0], parameter.strParam[1], parameter.m_SMSCard);
		break;
	case 7103: //设定GPRS的APN              
	{
		if(parameter.strParam[0].IsEmpty()==TRUE)
			return false;
		else
			strOutput.Format("*SAP,%s,%s,%s", parameter.m_password, parameter.strParam[0], parameter.m_SMSCard);
	}
	break;
	case 7104: //设定车机密码
	{
		if(parameter.strParam[0].IsEmpty()==TRUE||parameter.strParam[0]!=parameter.strParam[1])
			return false;
		else
			strOutput.Format("*SPW,%s,%s,%s,%s", parameter.m_password, parameter.strParam[0], parameter.strParam[0], parameter.m_SMSCard);
	}
	break;
	case 7105: //设定车机回报距离和时间间隔    
	{
		strOutput.Format("*SDT,%s,%d,%d,%d,%s", parameter.m_password, parameter.dwParam[0], parameter.dwParam[1], parameter.dwParam[2], parameter.m_SMSCard);
	}
	break;
	case VHC_CMD_RESPOND_SINGLE:
	{
		strOutput.Format("*QST,%s,%s", parameter.m_password, parameter.m_SMSCard);
	}
	break;
	case 7106: //设置报警短信回报号码          
	{
		if(parameter.strParam[0].IsEmpty()==TRUE)
			return false;
		else
			strOutput.Format("*SAL,%s,%s,%s", parameter.m_password, parameter.strParam[0], parameter.m_SMSCard);
	}
	break;
	case 7107: //设置报警语音回报号码          
	{
		if(parameter.strParam[0].IsEmpty()==TRUE)
			return false;
		else
			strOutput.Format("*SCL,%s,%s,%s", parameter.m_password, parameter.strParam[0], parameter.m_SMSCard);
	}
	break;
	case 7108: //返回车机当前状态              
	{
		strOutput.Format("*QSE,%s,%s", parameter.m_password, parameter.m_SMSCard);
	}
	break;
	default:
		return CCoder::Encoding(wCmd, parameter, strOutput);
	}

	return true;
}

bool CStarG3_PS660::DeCoding(CStringA& strMsg, CMessageReceived& msg)
{
	strMsg.TrimLeft();

	if(strMsg.Left(3)=="S/N")
	{
		msg.m_wCommand = GPSSERVER_VEHICLE_MOVE;

		int dd, mo, yy, hh, mm, ss;

		CStringA str = strMsg;
		for(int i = 0; i<8; i++)
		{
			const int pos = str.Find("\r");
			if(pos<0)
				break;

			CStringA strValid = str.Left(pos);
			str = str.Mid(pos+2);

			switch(i)
			{
			case 1:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				const char c = strValid[0];

				strValid = strValid.Mid(+1);
				strValid = strValid.Left(strValid.Find('d'));
				msg.m_Lat = atof(strValid);

				if(c=='N')
				{
					msg.m_Lat *= 1;
				}
				else if(c=='S')
				{
					msg.m_Lat *= -1;
				}
			}
			break;
			case 2:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				const char c = strValid[0];

				strValid = strValid.Mid(1);
				strValid = strValid.Left(strValid.Find(_T('d')));
				msg.m_Lon = atof(strValid);

				if(c==_T('E'))
				{
					msg.m_Lon *= 1;
				}
				else if(c==_T('W'))
				{
					msg.m_Lon *= -1;
				}
			}
			break;
			case 4:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				strValid = strValid.Left(strValid.Find("kph"));
				msg.m_Veo = atof(strValid);
			}
			break;
			case 5:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				if(strValid=="none")
					msg.m_Direct = 0.0f;
				else
					msg.m_Direct = atof(strValid);
			}
			break;
			case 6:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				sscanf(strValid, "%d/%d/%d", &dd, &mo, &yy);
			}
			break;
			case 7:
			{
				strValid = strValid.Mid(strValid.Find('=')+1);
				sscanf(strValid, "%d:%d:%d", &hh, &mm, &ss);
				msg.m_varGPSTime = COleDateTime(yy, mo, dd, hh, mm, ss);
			}
			break;
			}
		}
	}
	else if(strMsg.Left(6)=="$GPSGD")
	{
		//"$GPSGD,66000172,06-09-06,13:01:26,0031.3396,0120.6272,000,000,30A1,007,001"

		const char* szField = (LPCSTR)strMsg;
		const int len = strlen(szField);
		if(len<68)
			return false;

		msg.m_wCommand = GPSSERVER_VEHICLE_MOVE;

		for(int index = 0; index<8; index++)
		{
			int next = 0;
			while(szField[next]!=','&&szField[next]!=0)
			{
				next++;
			}

			char temp[20];
			memset(temp, 0, 20*sizeof(char));
			strncpy(temp, szField, next);

			switch(index)
			{
			case 0:
				break;
			case 1:
			{
				msg.m_strIVUSN = temp;
			}
			break;
			case 2:
				int dd, mo, yy;
				int hh, mm, ss;
				{
					sscanf(temp, "%d-%d-%d %d:%d:%d", &yy, &mo, &dd, &hh, &mm, &ss);
					msg.m_varGPSTime = COleDateTime(yy, mo, dd, hh, mm, ss);
				}
				break;
			case 3:
			{
				msg.m_Lat = atof(temp);
			}
			break;
			case 4:
			{
				msg.m_Lon = atof(temp);
				msg.m_bAvailable = true;
			}
			break;
			case 5:
			{
				msg.m_Veo = atof(temp);
			}
			break;
			case 6:
			{
				msg.m_Direct = atof(temp);
			}
			break;
			case 7:
			{
				msg.m_dwStatus = atoi(temp);
			}
			break;
			case 8:
				break;
			case 9:
			{
				msg.m_bAvailable = true;
			}
			break;
			}

			szField += (next+1);
		}
	}
	else if(strMsg.Left(7)=="!SMD=OK")
	{
		msg.m_wCommand = GPSSERVER_DEALWITH_REPORT;
		msg.m_strMessage.Format(_T("设置车机模式成功!"));
	}
	else if(strMsg.Left(5)=="!SPW=")
	{
		msg.m_wCommand = VEHICLE_PWD_CHANGED;
		msg.m_strMessage = strMsg.Mid(5);
	}
	else if(strMsg.Left(5)=="!SIP=")
	{
		msg.m_wCommand = GPSSERVER_DEALWITH_REPORT;
		msg.m_strMessage.Format(_T("设置服务器IP地址和端口成功!"));
	}
	else if(strMsg.Left(5)=="!SAL=")
	{
		msg.m_wCommand = GPSSERVER_DEALWITH_REPORT;
		msg.m_strMessage.Format(_T("设置紧急状态下的短信通知号码成功!"));
	}
	else if(strMsg.Left(5)=="!SCL=")
	{
		msg.m_wCommand = GPSSERVER_DEALWITH_REPORT;
		msg.m_strMessage.Format(_T("设置紧急状态下的语音电话通知号码成功!"));
	}

	return true;
}
