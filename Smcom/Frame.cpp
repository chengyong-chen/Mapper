// Frame.cpp: implementation of the CFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Frame.h"

#include "stdio.h"

#include "global.h"

#include "../Public/Function.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseFrame::CBaseFrame()
{
	m_bSendUseUnicode = FALSE;
}

CBaseFrame::~CBaseFrame()
{
}

//////////////////////////////////////////////////////////////////////
// MSCommFrame MSComm控件参数设置类
MSCommFrame::MSCommFrame()
{
	nErrCode = 0;
}

MSCommFrame::~MSCommFrame()
{
}

// 解析发送数据格式
bool MSCommFrame::ParseSendData()
{
	switch(m_ATID)
	{
	case AT_CMG_HASMSG:
	case AT_CMG_NOMSG: // 发送AT指令
	{
		//	int size = strlen((char*)m_Vt.bstrVal);//返回值有误
		char* lpszText = _com_util::ConvertBSTRToString(m_Vt.bstrVal);
		const int size = strlen(lpszText);

		SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, size);
		if(psa!=nullptr)
		{
			LPVOID lpArrayData;
			::SafeArrayAccessData(psa, &lpArrayData);
			::memcpy((char*)lpArrayData, lpszText, size);
			::SafeArrayUnaccessData(psa);

			m_Vt.vt = VT_ARRAY|VT_UI1;
			m_Vt.parray = psa;

			::SafeArrayDestroy(psa);
		}
		if(lpszText!=nullptr)
		{
			delete lpszText;
			lpszText = nullptr;
		}
	}
	break;
	case WAVCOM_SEND_AT: // wavcom PDU发送格式
	{
		// 0011000D91683119809221F00008AA0E4F60597D00480045004C004C004F
		// AT指令PDU格式发送编码注释
		// 00 - Length of SMSC information. Here the length is 0, which means that the SMSC stored in the phone should be used
		// 11 - First octet of the SMS-SUBMIT message
		// 00 - TP-Message-Reference. The "00" value here lets the phone set the message reference number itself
		// 0D - Address-Length. Length of phone number 
		// 91 - Type-of-Address. 
		// 683119809221F0 - The phone number in semi octets 
		// 00 - TP-PID. Protocol identifier
		// 08 - Data coding scheme (0: ASCII; 8:UNICODE)
		// AA - TP-Validity-Period. "AA" means 4 days. 
		// 0E - User data length
		// 4F60597D00480045004C004C004F - TP-UD 8-bit octets representing 7-bit data

		// 转换为PDU格式的电话号码
		char phone[15];
		{
			memset(phone, 0, 15);
			const char* pPhoneNo = _com_util::ConvertBSTRToString(m_vtPhoneNo.bstrVal);
			memcpy(phone, pPhoneNo, strlen(pPhoneNo));
			const int iii = ((strlen(phone)+1)/2)*2;
			while(strlen(phone)<iii)
				sprintf(phone, "%sF", phone);

			ConvertPDUPhoneNo(phone, iii);
			delete pPhoneNo;
			pPhoneNo = nullptr;
		}

		BOOL bUseUNICODE = m_bSendUseUnicode;
		char* lpVt = _com_util::ConvertBSTRToString(m_Vt.bstrVal);
		if(HasWideChar(lpVt)==TRUE)// 判断是否含有汉字
			bUseUNICODE = TRUE;

		int length = 0;
		char* str = new char[nOutBufferSize];
		memset(str, 0, nOutBufferSize);

		if(bUseUNICODE==TRUE) // 以UNICODE编码消息内容发送
		{
			if(strstr(phone, "68")==phone)
				sprintf(str, "0011000D91%s0008AA", phone);
			if(strstr(phone, "88")==phone)
				sprintf(str, "0011000CA1%s0008AA", phone);
			else
				sprintf(str, "0011000D91%s0008AA", phone);

			length = strlen(str);

			const OLECHAR* wcVt = MultiByteToUnicode(CP_ACP, lpVt);
			const int cnt = wcslen(wcVt);

			int sizeContent = min(cnt*4, nOutBufferSize-length-2);
			sizeContent = sizeContent/4;

			// 填写消息长度
			char cLen[3];
			memset(cLen, 0, 3);
			sprintf(cLen, "%X", sizeContent*2);
			if(strlen(cLen)<2)
			{
				cLen[1] = cLen[0];
				cLen[0] = '0';
			}
			str[length++] = cLen[0];
			str[length++] = cLen[1];

			// 填写消息内容
			for(int i = 0; i<sizeContent; i++)
			{
				if(length+4>nOutBufferSize)// 超出发送缓冲区时，丢弃超出部分
					break;

				char ch[5];
				memset(ch, 0, 5);
				sprintf(ch, "%X", wcVt[i]);
				if(strlen(ch)<4)
				{
					ch[3] = ch[1];
					ch[2] = ch[0];
					ch[1] = '0';
					ch[0] = '0';
				}

				str[length++] = ch[0];
				str[length++] = ch[1];
				str[length++] = ch[2];
				str[length++] = ch[3];
			}

			delete[] wcVt; // 释放在ConvertToUnicode()中new的空间
		}
		else// 以ASCII编码消息内容发送
		{
			if(strstr(phone, "68")==phone)
				sprintf(str, "0011000D91%s0000AA", phone);
			else if(strstr(phone, "0068")==phone)
				sprintf(str, "0011000F91%s0000AA", phone);
			else if(strstr(phone, "88")==phone)
				sprintf(str, "0011000C91%s0000AA", phone);
			else if(strstr(phone, "0088")==phone)
				sprintf(str, "0011000E91%s0000AA", phone);
			else
				sprintf(str, "0011000D91%s0000AA", phone);

			length = strlen(str);
			const int cnt = strlen(lpVt);
			const int sizeContent = min(cnt*2, nOutBufferSize-length-2)/2;// 填写消息内容长度

			// 填写消息长度
			char cLen[3];
			memset(cLen, 0, 3);
			sprintf(cLen, "%X", sizeContent);
			if(strlen(cLen)<2)
			{
				cLen[1] = cLen[0];
				cLen[0] = '0';
			}
			str[length++] = cLen[0];
			str[length++] = cLen[1];

			// 转换消息内容
			char* buff = new char[sizeContent*2+1];
			memset(buff, 0, sizeContent*2+1);
			for(int i = 0; i<sizeContent; i++)
			{
				char ch[3];
				memset(ch, 0, 3);
				sprintf(ch, "%X", lpVt[i]);
				if(strlen(ch)<2)
				{
					ch[1] = ch[0];
					ch[0] = '0';
				}

				buff[i*2] = ch[0];
				buff[i*2+1] = ch[1];
			}

			// 发送前转换 Octects To Septects
			const char* septects = OctToSeptects(buff, sizeContent*2);
			memcpy(str+length, septects, strlen(septects));
			length += strlen(septects);

			delete[] buff;
			delete[] septects; // 在OctToSeptects()中new的内存
		}

		delete lpVt;
		lpVt = nullptr;

		// 填写发送指令
		char* buff = new char[nOutBufferSize+20];
		memset(buff, 0, nOutBufferSize+20);
		sprintf(buff, "AT+CMGS=%d\r%s%c", (length-2)/2, str, 0x1A);

		length = strlen(buff);
		SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, length);
		if(psa!=nullptr)
		{
			LPVOID lpArrayData;
			::SafeArrayAccessData(psa, &lpArrayData);
			::memcpy((char*)lpArrayData, buff, length);
			::SafeArrayUnaccessData(psa);

			m_Vt.vt = VT_ARRAY|VT_UI1;
			m_Vt.parray = psa;

			::SafeArrayDestroy(psa);
		}
		delete[] buff;

		delete[] str;
	}
	break;
	default: // 不支持的指令
		break;
	}

	return true;
}

// 解析接收数据格式 return 1 - 没有读完，返回继续读
bool MSCommFrame::ParseRecvData()
{
	switch(m_ATID)
	{
	case AT_CMG_NOMSG:
		break;
	case WAVCOM_RECV_AT: // wavcom PDU接收格式
	case AT_CMG_HASMSG: // 接收AT指令应答
	{
		// 0891683108100005F0040D91683119809221F00008306090516233230E4F60597D00480045004C004C004F
		// AT指令PDU格式接收编码注释
		// 08 - Length of the SMSC information (in this case 7 octets)
		// 91 - Type-of-address of the SMSC.(91 means international format of the phone number)
		// 683108100005F0 - Service center number(in decimal semi-octets). 
		// 04 - First octet of this SMS-DELIVER message
		// 0D - Address-Length. Length of the sender number (0B hex = 11 dec
		// 91 - Type-of-address of the sender number
		// 683119809221F0 - Sender number (decimal semi-octets), with a trailing F
		// 00 - Protocol identifier
		// 08 - Data coding scheme (0: ASCII; 8:UNICODE)
		// 30609051623323 - Time stamp (semi-octets)
		// 0E - User data length
		// 4F60597D00480045004C004C004F - TP-UD 8-bit octets representing 7-bit data
		if(m_Vt.vt==VT_EMPTY)
			break;

		_variant_t vt = m_Vt;
		m_Vt.vt = VT_EMPTY; // Clear();

		int size = GetSafeArraySize(vt.parray);
		if(size<=50)
			return false; // 未读完

		char* p = new char[size+1];
		char* p_fordelete = p;
		memset(p, 0, size+1);
		memcpy(p, (char*)(unsigned char*)vt.parray->pvData, size);
		char* start = p;

		// <test>
		// char *p = {"0891683108100005F0040D91683119809221F00000306061016265230F31D98C56B3DD70399C2D46A3CD00"};
		// int size = strlen(p);
		// </test>
		if(strstr(p, "CMGR:")!=nullptr&&(p = strstr(p, "CMGR:"))!=nullptr&&(p = strstr(p, "\r\n"))!=nullptr)
		{
			p += 2;
			size -= (p-start);
		}
		else if(strstr(p, "CMT:")!=nullptr&&(p = strstr(p, "CMT:"))!=nullptr&&(p = strstr(p, "\r\n"))!=nullptr) // 自动返回收到消息内容	
		{
			size = size-(p-start)-2;
			if(size<=0)
			{
				delete[] p_fordelete;
				return false;
			}

			p += 2;

			// 有多条信息到达时，抛弃后面的信息(暂时)
			char* cmt2 = strstr(p, "CMT:");
			if(cmt2!=nullptr)
				*cmt2 = 0;
		}
		else
		{
			delete[] p_fordelete;
			return false;
		}

		/////////////////////////////////////////////////////////////////
		// 解析消息内容
		// 0891683108100005F0040D91683119809221F00000306011610350230331D90C

		// Length of the SMSC information
		int len = atohex(p, 2);

		size -= (len+2)*2;
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		p += (len+2)*2;

		// Address-Length. Length of the sender number
		// 0D91683119809221F0
		len = atohex(p, 2);
		size -= 4;
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		p += 4;

		//得到电话号码
		{
			char* ch = new char[len+1];
			memset(ch, 0, len+1);
			memcpy(ch, p, len+1);
			ConvertPDUPhoneNo(ch, len+1);
			ch[len] = 0;

			m_vtPhoneNo.vt = VT_BSTR;
			m_vtPhoneNo.bstrVal = _com_util::ConvertStringToBSTR(ch);

			delete[] ch;
		}

		size -= ((len+1)/2*2+2);
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		p += ((len+1)/2*2+2);

		// Data coding scheme
		int coding = atohex(p, 2);

		size -= 2;
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		p += 2;

		// Time stamp (semi-octets)
		// 30601161035023

		size -= 14;
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		p += 14;

		// 得到时间
		// 字符串时间转换为double值 03/06/04,11:08:40+32
		{
			m_RecvTime.vt = VT_R8;

			char cTime[15];
			memset(cTime, 0, 15);
			memcpy(cTime, (p-14), 14);

			char ch[3];
			memset(ch, 0, 3);
			SYSTEMTIME time;

			ConvertPDUPhoneNo(cTime, 2);
			ch[0] = *cTime;
			ch[1] = *(cTime+1);
			time.wYear = atoi(ch);
			ConvertPDUPhoneNo(cTime+2, 2);
			ch[0] = *(cTime+2);
			ch[1] = *(cTime+3);
			time.wMonth = atoi(ch);
			ConvertPDUPhoneNo(cTime+4, 2);
			ch[0] = *(cTime+4);
			ch[1] = *(cTime+5);
			time.wDay = atoi(ch);
			ConvertPDUPhoneNo(cTime+6, 2);
			ch[0] = *(cTime+6);
			ch[1] = *(cTime+7);
			time.wHour = atoi(ch);
			ConvertPDUPhoneNo(cTime+8, 2);
			ch[0] = *(cTime+8);
			ch[1] = *(cTime+9);
			time.wMinute = atoi(ch);
			ConvertPDUPhoneNo(cTime+10, 2);
			ch[0] = *(cTime+10);
			ch[1] = *(cTime+11);
			time.wSecond = atoi(ch);
			time.wMilliseconds = 0;

			SystemTimeToVariantTime(&time, &m_RecvTime.dblVal);
		}

		// User data length
		size -= 2;
		if(size<0)
		{
			delete[] p_fordelete;
			return false;
		}
		len = atohex(p, 2);
		p += 2;

		if(coding==8) // UNICODE 编码
		{
			if(len*2>(size-2)) // UNICODE编码时候，使用8个bit，字节不够，认为没有读完
			{
				delete[] p_fordelete;
				return false;
			}

			// 将UNICOD码转换为ANSI码
			size = size/4;
			WCHAR* ch = new WCHAR[size+1];
			memset(ch, 0, (size+1)*sizeof(WCHAR));
			char c[5];
			for(int i = 0; i<size; i++)
			{
				c[0] = *(p+i*4);
				c[1] = *(p+i*4+1);
				c[2] = *(p+i*4+2);
				c[3] = *(p+i*4+3);
				c[4] = 0;
				ch[i] = atohex(c, 4);
			}

			char* ansi = ConvertToAnsi(ch);
			delete[] ch;

			m_MsgContent.vt = VT_BSTR;
			m_MsgContent.bstrVal = _com_util::ConvertStringToBSTR(ansi);

			delete[] ansi; // 在ConvertToAnsi()中分配的
		}
		else// if(0 == coding) // ASCII编码
		{
			if(len*7>(size*4-2)) // ASCII编码时，使用7个bit，计算bit位不够，认为没有读完
			{
				delete[] p_fordelete;
				return false;
			}

			char* ch = SepToOctets(p, size); // ASCII格式septects到octets的转换// ch返回值是new出来的
			if(ch!=nullptr)
			{
				m_MsgContent.vt = VT_BSTR;
				m_MsgContent.bstrVal = _com_util::ConvertStringToBSTR(ch);
				delete[] ch;
			}
		}

		delete[] p_fordelete;
	}
	break;
	default:
		break;
	}

	return true;
}
