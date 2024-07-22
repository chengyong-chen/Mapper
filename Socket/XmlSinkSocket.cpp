// XmlSinkSocket.cpp: implementation of the CXmlSinkSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlSinkSocket.h"
#include "Exstdcfunction.h"
#include "atlconv.h"
#include "../Coding/Instruction.h"

#include "../Smcom/Global.h"
//

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlSinkSocket::CXmlSinkSocket() :
	CGpsSinkSocket()
{
	m_headLength = 4;

	try
	{
		m_strXmlCmd = GetCurrentDir()+_T("\\cmddef.xml");
		m_pXmlParser = new CMarkup();
		m_pXmlParser->Load(m_strXmlCmd);
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

CXmlSinkSocket::~CXmlSinkSocket()
{
	try
	{
		delete m_pXmlParser;
		m_pXmlParser = nullptr;
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

CString CXmlSinkSocket::GetXmlCmd(CString name) const
{
	CString strCmd;
	try
	{
		m_pXmlParser->ResetPos();
		if(m_pXmlParser->FindChildElem(_T("system")))
		{
			m_pXmlParser->IntoElem();
			while(m_pXmlParser->FindChildElem(_T("cmd")))
			{
				strCmd = m_pXmlParser->GetChildSubDoc();
				m_pXmlParser->IntoElem();
				if(m_pXmlParser->GetAttrib(_T("name"))==name)
				{
					m_pXmlParser->OutOfElem();
					break;
				}
				m_pXmlParser->OutOfElem();
				strCmd.Empty();
			}
		}
		m_pXmlParser->OutOfElem();
	}
	catch(CException*ex)
	{
		ex->Delete();
		strCmd.Empty();
	}
	return strCmd;
}

BOOL CXmlSinkSocket::SendMsg(char* lp, DWORD length)
{
	if(lp==nullptr)
		return FALSE;

	BOOL bRet = FALSE;
	try
	{
		STRUCT_MSG_HEADER hdr;
		memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));
		CString strCmd;

		switch(hdr.wCmdId)
		{
		case SERVER_LOGIN_FAILED:
		{
			STRUCT_SERVER_REPLY_LOGIN msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_SERVER_REPLY_LOGIN));
			strCmd = GetXmlCmd(_T("login"));
			if(!strCmd.IsEmpty())
			{
				CMarkup parser(strCmd);
				strCmd.Empty();
				if(parser.FindChildElem(_T("result")))
				{
					parser.IntoElem();
					parser.SetAttrib(_T("code"), msg.dwErr);
					switch(msg.dwErr)
					{
					case 0:
						parser.SetData(_T("��¼�ɹ�!"));
						break;
					case 1:
						parser.SetData(_T("��¼ʧ��,�û���������!"));
						break;
					case 2:
						parser.SetData(_T("��¼ʧ��,���벻��ȷ!"));
						break;
					case 3:
						parser.SetData(_T("��¼ʧ��,�ͻ��˰汾̫��,���������ͻ���!"));
						break;
					case 4:
						parser.SetData(_T("��¼ʧ��,�û��Ѿ���¼!"));
						break;
					}
					parser.OutOfElem();
				}
				strCmd = parser.GetDoc();
			}
		}
		break;
		case SERVER_LOGIN_SUCCEED:
		{
			STRUCT_SERVER_REPLY_LOGIN msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_SERVER_REPLY_LOGIN));

			strCmd = GetXmlCmd(_T("login"));
			if(!strCmd.IsEmpty())
			{
				CMarkup parser(strCmd);
				strCmd.Empty();
				if(parser.FindChildElem(_T("name")))
				{
					parser.IntoElem();
					parser.SetAttrib(_T("id"), this->m_dwUser);
					parser.SetData(this->m_strUser);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("result")))
				{
					parser.IntoElem();
					parser.SetAttrib(_T("code"), 0);
					parser.SetData(_T("��¼�ɹ�!"));
					parser.OutOfElem();
				}
				strCmd = parser.GetDoc();
			}
		}
		break;
		case GPSSERVER_VEHICLE_NEW:
		{
			STRUCT_GPSSERVER_VEHICLE_NEW msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_GPSSERVER_VEHICLE_NEW));
			strCmd = GetXmlCmd(_T("vehicle"));
			if(!strCmd.IsEmpty())
			{
				CMarkup parser(strCmd);
				strCmd.Empty();
				if(parser.FindChildElem(_T("id")))
				{
					CString str;
					str.Format(_T("%d"), msg.dwVehicle);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("code")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.vhcCode));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("color")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.vhcColor));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("tel")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.vhcIVU));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("use")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.vhcType));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("pass")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.vhcPass));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("type")))
				{
					parser.IntoElem();
					parser.SetData(CA2W(msg.strIVU));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("driver")))
				{
					parser.IntoElem();
					if(parser.FindChildElem(_T("name")))
					{
						parser.IntoElem();
						parser.SetData(CA2W(msg.ownName));
						parser.OutOfElem();
					}
					if(parser.FindChildElem(_T("IVU")))
					{
						parser.IntoElem();
						parser.SetData(CA2W(msg.ownIVU));
						parser.OutOfElem();
					}
					parser.OutOfElem();
				}
				strCmd = parser.GetDoc();
			}
		}
		break;
		case GPSSERVER_VEHICLE_MOVE:
		{
			STRUCT_GPSSERVER_VEHICLE_MOVE msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_GPSSERVER_VEHICLE_MOVE));
			strCmd = GetXmlCmd(_T("move"));
			if(!strCmd.IsEmpty())
			{
				CString str;
				CMarkup parser(strCmd);
				strCmd.Empty();
				if(parser.FindChildElem(_T("id")))
				{
					str.Format(_T("%d"), msg.dwVehicle);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("av")))
				{
					parser.IntoElem();
					parser.SetData(msg.bAV ? _T("true") : _T("false"));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("lat")))
				{
					str.Format(_T("%.4f"), msg.dbLat);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("log")))
				{
					str.Format(_T("%.4f"), msg.dbLng);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("veo")))
				{
					str.Format(_T("%.1f"), msg.flVeo);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("direct")))
				{
					str.Format(_T("%d"), msg.wDirect);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("stacode")))
				{
					str.Format(_T("%u"), msg.dwStatus);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("state")))
				{
					parser.IntoElem();
					parser.SetData(_T(""));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("gpstime")))
				{
					COleDateTime t(msg.gpsTime);
					parser.IntoElem();
					parser.SetData(t.Format(_T("%Y-%m-%d %H:%M:%S")));
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("loctime")))
				{
					COleDateTime t(msg.locTime);
					parser.IntoElem();
					parser.SetData(t.Format(_T("%Y-%m-%d %H:%M:%S")));
					parser.OutOfElem();
				}
				strCmd = parser.GetDoc();
			}
		}
		break;
		case GPSSERVER_VEHICLE_MESSAGE:
		{
			STRUCT_GPSSERVER_VEHICLE_MESSAGE msg;
			memcpy((char*)&msg, lp, sizeof(STRUCT_GPSSERVER_VEHICLE_MESSAGE));
			strCmd = GetXmlCmd(_T("message"));
			if(!strCmd.IsEmpty())
			{
				CString str;
				CMarkup parser(strCmd);
				strCmd.Empty();
				if(parser.FindChildElem(_T("id")))
				{
					str.Format(_T("%d"), msg.dwVehicle);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("msg")))
				{
					parser.IntoElem();
					parser.SetData(msg.msg);
					parser.OutOfElem();
				}
				if(parser.FindChildElem(_T("IVU")))
				{
					str.Format(_T("%s"), msg.cIVUSendto);
					parser.IntoElem();
					parser.SetData(str);
					parser.OutOfElem();
				}
				strCmd = parser.GetDoc();
			}
		}
		break;
		}

		if(!strCmd.IsEmpty())
		{
			CStringA stra(strCmd);
			bRet = SendMessage(stra.GetBuffer(), strCmd.GetLength());
			stra.ReleaseBuffer();
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
		return FALSE;
	};
	return bRet;
}

BOOL CXmlSinkSocket::SendStringToClient(DWORD vhcId, TCHAR* str)
{
	STRUCT_GPSSERVER_VEHICLE_MESSAGE* lp = new STRUCT_GPSSERVER_VEHICLE_MESSAGE(vhcId, str);
	return SendMsg((CHAR*)lp, sizeof(*lp));
	return FALSE;
}

BOOL CXmlSinkSocket::AskInspectOneVehicle(DWORD dwID)
{
	return FALSE;
}

BOOL CXmlSinkSocket::IsCorrectHeader(char* header, int size)
{
	return TRUE;
}

BOOL CXmlSinkSocket::IsCorrectMessage(char* msg, int size)
{
	return TRUE;
}

int CXmlSinkSocket::GetMessageSizeFromHeader(char* header)
{
	return (*(DWORD*)(header));
}

bool CXmlSinkSocket::SendData(void* msg, DWORD length)
{
	return cioSendBytes(msg, length);
}

void* CXmlSinkSocket::cioRecvBytes(int& length, int& Error)
{
#ifndef _DEBUG
	try{
#endif
		int l;
		int TickCount = 0;
		Error = 0;
		char* buf = new char[length];
		memset(buf, 0, length);
		char* tempbuf = buf;
		const int recl = 0;

		l = recv(GetSocket(), tempbuf, length-recl, 0);
		if(l==SOCKET_ERROR||l==0)
		{
			Error = 1;
			delete[] buf;
			return nullptr;
		};

		length = l;
		return buf;
#ifndef _DEBUG
	}
	catch(CException*ex){ ex->Delete();; return nullptr; };
#endif
}

BOOL CXmlSinkSocket::cioSendBytes(void* msg, int msg_size)
{
	if(nullptr==msg) return FALSE;
#ifndef _DEBUG
	try{
#endif
		int l;
		int TickCount = 0;
		char* tempbuf = (char*)msg;
		int sentl = 0;
		do
		{
			l = send(GetSocket(), tempbuf, msg_size-sentl, 0);
			if(l==SOCKET_ERROR) return FALSE;
			sentl += l;
			tempbuf += l;
			if(sentl<msg_size)
			{
				Sleep(50);
				TickCount += 1;
			};
		} while(sentl<msg_size&&TickCount<2000);
		if(sentl!=msg_size) return FALSE;
		return TRUE;
#ifndef _DEBUG
	}
	catch(CException*ex){ ex->Delete(); return FALSE; };
#endif
}

void* CXmlSinkSocket::ReceiveData(int& length, int& Error)
{
	try
	{
		Error = 0;
		int len = 1024;
		const void* hdrbuf = cioRecvBytes(len, Error);
		if(Error!=0)
		{
			delete[] hdrbuf;
			return nullptr;
		};

		OLECHAR* szW = new OLECHAR[len/2+1];
		memset(szW, 0, (len/2+1)*2);
		memcpy(szW, hdrbuf, len);
		const char* pStr = ConvertToAnsi(szW);
		length = strlen(pStr);
		delete[] hdrbuf;
		delete[] szW;

		char* buf = new char[length];
		memset(buf, 0, length);
		memcpy(buf, pStr, length);
		delete[] pStr;
		return buf;
	}
	catch(CException*ex)
	{
		ex->Delete();
		ASSERT(FALSE);
		return nullptr;
	};
}
