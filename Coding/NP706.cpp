// SunLightCoder.cpp: implementation of the CNP706 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NP706.h"
#include "Instruction.h"

#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CNP706 NP706;

CNP706::CNP706()
{
	m_bUseUnicodeCharSet = FALSE;

	m_strDescription = "SL708B";
}

bool CNP706::DeCoding(CStringA& strMsg, CMessageReceived& msg)
{
	CCoder::DeCoding(strMsg, msg);

	msg.m_wCommand = GPSSERVER_VEHICLE_MOVE;
	if(DeCodingCTMsg(strMsg, msg)==true)// P Foramt . 个人用户定位
		return true;

	LPCSTR lpszGPRMC = strstr(strMsg, "PRMC");
	if(lpszGPRMC!=nullptr)// Process the gprmc data.
	{
		msg.m_wCommand = GPSSERVER_VEHICLE_MOVE;

		try// (001,001,S800002,TIME=0,M2,.GPRMC,090901.589,A,3953.9352,N,11616.4795,E,0.39,165.28,241104,,*09)
		{
			CStringA str = strMsg;
			int dd[10];
			int l = 0;
			for(int i = 0; i<10; i++)
			{
				int r = str.Find(",", l);
				if(r<0)
					break;
				l = r+1;
				dd[i] = r;
			}
			CStringA dd2 = str.Mid(dd[3] + 1, dd[4] - dd[3] - 1);
			GetStatus(dd2, msg);

			using std::string;// using namespace boost;	
			const string test_string = LPCSTR(lpszGPRMC);
			typedef boost::tokenizer<boost::char_separator<char>> Tok;
			boost::char_separator<char> sep(",", nullptr, boost::keep_empty_tokens);
			Tok t(test_string, sep);// GPRMC,112322.998,A,3953.4095,N,11621.0202,E,0.06,12.76,080203,,*36 CR LF
			Tok::iterator it = t.begin(); // Get "GPRMC"
			it++;

			char szTmp[6];
			memset(szTmp, 0, sizeof(szTmp));
			SYSTEMTIME sysTime;
			memset((void*)&sysTime, 0, sizeof(SYSTEMTIME));
			LPCSTR szField = it->c_str();// Get "112322.998(time)"
			memcpy(szTmp, szField, 2);//get hour
			szTmp[2] = 0;
			sysTime.wHour = atoi(szTmp);
			memcpy(szTmp, szField+2, 2);// get minute
			szTmp[2] = 0;
			sysTime.wMinute = atoi(szTmp);
			memcpy(szTmp, szField+4, 2);// get second
			szTmp[2] = 0;
			sysTime.wSecond = atoi(szTmp);

			it++;
			szField = it->c_str(); // Get "V or A"			
			if(!((*szField)=='A'||((*szField)=='V')||((*szField)=='P')))
				return 1;

			msg.m_bAvailable = ((*szField)=='A');
			if(!msg.m_bAvailable)
				msg.m_dwStatus |= VHC_STATE_GPS_V;

			// Data are valid		
			it++;
			szField = it->c_str(); // Get "3953.4095 Lat"
			msg.m_Lat = atof(szField);
			//			if(msg.m_Lat > 0.1)

			it++;
			szField = it->c_str(); // Get "N or S"
			msg.m_Lat *= (szField[0]=='S' ? -1 : 1);

			it++;
			szField = it->c_str(); // Get "11621.0202 Lon"
			msg.m_Lon = atof(szField);

			it++;
			szField = it->c_str(); // Get "E or W"
			msg.m_Lon *= (szField[0]=='W' ? -1 : 1);
			it++;

			szField = it->c_str(); // Get "0.06 Speed"
			msg.m_Veo = (float)atof(szField);

			it++;
			szField = it->c_str(); // Get "12.76"  Direction"
			msg.m_Direct = (float)atof(szField);

			it++;
			szField = it->c_str(); // Get 080203  height"
			memcpy(szTmp, szField, 2);//get day
			szTmp[2] = 0;
			sysTime.wDay = atoi(szTmp);
			memcpy(szTmp, szField+2, 2);// get month
			szTmp[2] = 0;
			sysTime.wMonth = atoi(szTmp);
			memcpy(szTmp, szField+4, 2);// get year
			szTmp[2] = 0;
			sysTime.wYear = atoi(szTmp);
			::SystemTimeToVariantTime(&sysTime, &msg.m_varGPSTime);

			//msg.m_Direct = atof(szField);

			double degree = int(msg.m_Lon/100);
			double minints = msg.m_Lon-degree*100;
			msg.m_Lon = degree+minints/60;

			degree = int(msg.m_Lat/100);
			minints = msg.m_Lat-degree*100;
			msg.m_Lat = degree+minints/60;

			return true;
		}
		catch(CException* ex)
		{
			ex->Delete();
			_ASSERTE(FALSE);
			return false;
		}
	}
	else if(strMsg.Find("+CSQ")>=0)
	{
		msg.m_wCommand = GPSCLIENT_VEHICLE_COMMAND;

		CStringA str = strMsg;
		CStringA str2;
		int l = 0;
		int r = 0;

		l = str.Find("+CSQ");
		r = str.Find(";");
		str2 = str.Mid(l+4, r-l-5);
		msg.m_strMessage.Format(_T("天线强度%s"), str2);

		r = str.ReverseFind(',');
		str2.Format(",版本%s", str.Mid(r+1));
		msg.m_strMessage += str2;

		if(str.Find("G=0")>=0)
			msg.m_strMessage += ",GSM有误";
		else
			msg.m_strMessage += ",GSM正常";

		if(str.Find("GP=0")>=0)
			msg.m_strMessage += ",GPS有误";
		else
			msg.m_strMessage += ",GPS正常";

		if(str.Find("E=0")>=0)
			msg.m_strMessage += ",EEP有误";
		else
			msg.m_strMessage += ",EEP正常";

		if(str.Find("SO=1")>=0)
			msg.m_strMessage += ",求救状态";
		else
			msg.m_strMessage += ",非求救状态";

		if(str.Find("A=1")>=0)
			msg.m_strMessage += ",电源正常";
		else
			msg.m_strMessage += ",电源有误";

		if(str.Find("TR=1")>=0)
			msg.m_strMessage += ",TRI开启";
		else
			msg.m_strMessage += ",TRI关闭";

		if(str.Find("SL=1")>=0)
			msg.m_strMessage += ",内部震动";

		if(str.Find("SH=1")>=0)
			msg.m_strMessage += ",外部震动";

		if(str.Find("L=1")>=0)
			msg.m_strMessage += ",电压异常(<11V)";
		else
			msg.m_strMessage += ",电压正常(>12V)";

		if(str.Find("W=0")>=0)
			msg.m_strMessage += ",电话薄读写异常";
		else
			msg.m_strMessage += ",电话薄读写正常";

		return true;
	}
	else if((strMsg.GetAt(0)=='I'&&strMsg.GetAt(7)=='S')||(strMsg.GetAt(0)=='I'&&strMsg.GetAt(7)=='N'))
	{
		msg.m_wCommand = GPSCLIENT_VEHICLE_COMMAND;

		msg.m_strMessage = strMsg;
		msg.m_strMessage.Replace(_T("I"), _T("密码"));
		msg.m_strMessage.Replace(_T("N"), _T("新密码"));
		msg.m_strMessage.Replace(_T("S"), _T("编号"));
		msg.m_strMessage.Replace(_T("A"), _T("一通"));
		msg.m_strMessage.Replace(_T("B"), _T("二通"));
		msg.m_strMessage.Replace(_T("C"), _T("三通"));
		msg.m_strMessage.Replace(_T("D"), _T("四通"));
		msg.m_strMessage.Replace(_T("E"), _T("中心"));
		msg.m_strMessage.Replace(_T("T"), _T("定时"));

		return true;
	}

	return false;
}

bool CNP706::Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput)
{
	switch(wCmd)
	{
	case VHC_CMD_SND_MESSAGE: // send message to the vehicle.
	{
		if(parameter.strParam[0].IsEmpty()==FALSE)
			strOutput = parameter.strParam[0];
		else
			return false;
	}
	break;
	default:
		return CCoder::Encoding(wCmd, parameter, strOutput);
	}

	return true;
}

bool CNP706::DeCodingCTMsg(LPCSTR szMsg, CMessageReceived& msg)
{
	char szField[15];
	memset(szField, 0, sizeof(szField));

	// S002,C001,M10,3953.8890N,11616.5107E,13916583420,00,00,#
	if(strlen(szMsg)<35)
		return false;
	if(szMsg[0]=='S'&&szMsg[5]=='C'&&szMsg[10]=='M')
	{
		strncpy(szField, szMsg+13, 9);
		szField[9] = 0;

		msg.m_Lat = atof(szField);// Get "3953.4095 Lat"
		msg.m_Lat *= (szMsg[22]=='S' ? -1 : 1);

		strncpy(szField, szMsg+24, 10);
		szField[10] = 0;
		msg.m_Lon = atof(szField);
		msg.m_Lon *= (szMsg[34]=='W' ? -1 : 1);

		double degree;
		double minints;

		degree = int(msg.m_Lon/100);
		minints = msg.m_Lon-degree*100;
		msg.m_Lon = degree+minints/60;

		degree = int(msg.m_Lat/100);
		minints = msg.m_Lat-degree*100;
		msg.m_Lat = degree+minints/60;
		const CStringA str = szMsg;
		int dd[10];
		int l = 0;
		CStringA strTemp;

		// S002,C001,M10,3953.8890N,11616.5107E,13916583420,00,00,#
		l = 0;
		for(int i = 0; i<10; i++)
		{
			const int r = str.Find(",", l);
			if(r<0)
				break;
			l = r+1;
			dd[i] = r;
		}
		CStringA dd3 = str.Mid(dd[1] + 1, dd[2] - dd[1] - 1);
		GetStatus(dd3, msg);

		/*必要时要恢复
		strTemp = str.Mid(dd[4]+1, dd[5]-dd[4]-1);
		if(strTemp.Find("13") == 0)
			strTemp.Insert(0, "86");
		else if(strTemp.Find("09") == 0)
		{
			strTemp.Delete(0);
			strTemp.Insert(0, "886");
		}
		else if(strTemp.Find("86") == 0 || strTemp.Find("886") == 0)
		{
		}
		else
			strTemp.Empty();

		if(strTemp.GetLength() > 10)
			msg.m_strIVUSendTo = strTemp;

		strTemp = str.Mid(dd[5]+1, dd[6]-dd[5]-1);
		if(strTemp.Find("13") == 0)
			strTemp.Insert(0, "86");
		else if(strTemp.Find("09") == 0)
		{
			strTemp.Delete(0);
			strTemp.Insert(0, "886");
		}
		else if(strTemp.Find("86") == 0 || strTemp.Find("886") == 0)
		{
		}
		else
			strTemp.Empty();

		if(strTemp.GetLength() > 10)
			msg.m_strIVUSendTo = strTemp;

		strTemp = str.Mid(dd[6]+1, dd[7]-dd[6]-1);
		if(strTemp.Find("13") == 0)
			strTemp.Insert(0, "86");
		else if(strTemp.Find("09") == 0)
		{
			strTemp.Delete(0);
			strTemp.Insert(0, "886");
		}
		else if(strTemp.Find("86") == 0 || strTemp.Find("886") == 0)
		{
		}
		else
			strTemp.Empty();

		if(strTemp.GetLength() > 10)
			msg.m_strIVUSendTo = strTemp;
		*/
		strTemp = msg.m_strMessage;
		msg.m_strMessage.Format(_T("%6.3fE %5.3fN%s"), msg.m_Lon, msg.m_Lat, strTemp);

		//today		msg.m_wCommand = GPSSERVER_VEHICLEPERSONAL;

		return true;
	}
	else
		return false;
}

void CNP706::GetStatus(CStringA& strM, CMessageReceived& msg)
{
	CStringA strState = "";
	const int i = atoi(strM.Mid(1));

	msg.m_dwStatus = 0;
	switch(i)
	{
	case 1:
		msg.m_dwStatus |= VHC_STATE_OPEN_DOOR;
		break;
	case 2:
		msg.m_dwStatus |= VHC_STATE_SHOCK;
		break;
	case 3:
		msg.m_dwStatus |= VHC_STATE_WARNING_SOS;
		break;
	case 4:
		msg.m_dwStatus |= VHC_STATE_ENGION_START;
		break;
	case 5:
		msg.m_dwStatus |= VHC_STATE_WARNING_LOWPOWER;
		break;
	case 6:
		msg.m_dwStatus |= VHC_STATE_OIL_SHUTOFF;
		break;
	case 7:
		msg.m_dwStatus |= VHC_STATE_WARNING_MOVE;
		break;
	case 8:
		msg.m_dwStatus |= VHC_STATE_REPORT_ONTIME;
		break;
	case 9:
		msg.m_dwStatus |= VHC_STATE_NOT_USE;
		break;
	case 10:
		msg.m_dwStatus |= VHC_STATE_SHOCK;
		break;
	case 11:
		msg.m_dwStatus |= VHC_STATE_WARNING_SHOCK;
		break;
	case 12:
		msg.m_dwStatus |= VHC_STATE_WARNING_THIEF;
		break;
	case 13:
		msg.m_dwStatus |= VHC_STATE_WARNING_GPSLINE;
		break;
	case 14:
		msg.m_dwStatus |= VHC_STATE_SECURITY_SET;
		break;
	case 15:
		msg.m_dwStatus |= VHC_STATE_SECURITY_RESET;
		break;
	default:
		msg.m_dwStatus = 0;
		break;
	}
}
