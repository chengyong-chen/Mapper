// Coder.cpp: implementation of the CProtectSoftware class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NP706.h"
#include "NP850.h"
#include "NP860.h"

#include "StarG3_PS660.h"

// 从车机的型号引入不同的车机解码方案

extern CNP860 NP860;
extern CNP706 NP706;
extern CNP850 NP850;
extern CStarG3_PS660 StarG3_PS660;

static BOOL g_bHasLoadedCoder = FALSE;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CCoder::SParameter::m_SMSCard = _T("");

CCoder::CCoder()
{
	m_bSupportHTTP = false;
	m_bSupportUDP = false;
	m_bSupportTCP = false;
	m_bSupportSMS = true;

	m_bUseUnicodeCharSet = TRUE;

	m_strDescription = _T("Coder");

	m_pStatus = nullptr;
	m_nStatus = 0;
}

CCoder::~CCoder()
{
	delete[] m_pStatus;
	m_pStatus = nullptr;
	m_nStatus = 0;
}

void CCoder::SetSMSCard(CString strSMSCard)
{
	SParameter::m_SMSCard = strSMSCard;
}

void CCoder::FetchStatus(CDatabase* pDatabase)
{
	if(pDatabase==nullptr)
		return;
	if(pDatabase->IsOpen()==FALSE)
		return;

	delete[] m_pStatus;
	m_pStatus = nullptr;

	CRecordset rs(pDatabase);
	try
	{
		CString strSQL;
		strSQL.Format(_T("Select COUNT(*) AS 数量 From IVUStatus INNER JOIN IVU ON IVUStatus.IVU=IVU.ID Where IVU.类型='%s'"), m_strDescription);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			CString strValue;
			rs.GetFieldValue(_T("数量"), strValue);

			m_nStatus = _ttoi(strValue);
			m_pStatus = new SStatus[m_nStatus];

			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
		rs.Close();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
	try
	{
		int index = 0;

		CString strSQL;
		strSQL.Format(_T("Select IVUStatus.* From IVUStatus INNER JOIN IVU ON IVUStatus.IVU=IVU.ID Where IVU.类型='%s'"), m_strDescription);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			while(rs.IsEOF()==FALSE)
			{
				if(index>=m_nStatus)
					break;

				CString strValue;

				rs.GetFieldValue(_T("掩码"), strValue);
				m_pStatus[index].dwMasker = _ttoi(strValue);

				rs.GetFieldValue(_T("含义"), strValue);
				strValue.TrimLeft();
				strValue.TrimRight();
				m_pStatus[index].strMeaning = strValue;

				rs.GetFieldValue(_T("Abnormal"), strValue);
				m_pStatus[index].nAbnomal = _ttoi(strValue);

				index++;
				rs.MoveNext();
			}
			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
		rs.Close();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

BOOL CCoder::IsAbnomal(DWORD dwStatus)
{
	if(dwStatus==0)
		return false;
	else if(m_nStatus>0)
	{
		for(int index = 0; index<m_nStatus; index++)
		{
			if(m_pStatus[index].nAbnomal==0)
				continue;

			if((dwStatus&m_pStatus[index].dwMasker)==m_pStatus[index].dwMasker)
			{
				return true;
			}
		}
	}

	return false;
}

BOOL CCoder::IsAlarm(DWORD dwStatus)
{
	if(dwStatus==0)
		return false;
	else if(m_nStatus>0)
	{
		for(int index = 0; index<m_nStatus; index++)
		{
			if(m_pStatus[index].nAbnomal==2)
			{
				if((dwStatus&m_pStatus[index].dwMasker)==m_pStatus[index].dwMasker)
				{
					return true;
				}
			}
		}
	}

	return false;
}

CString CCoder::GetStatusDescrip(DWORD dwStatus)
{
	CString str;
	if(dwStatus==0)
		return str;
	else
	{
		for(int index = 0; index<m_nStatus; index++)
		{
			if((dwStatus&m_pStatus[index].dwMasker)==m_pStatus[index].dwMasker)
			{
				str += _T(",")+m_pStatus[index].strMeaning;
			}
		}
		return str;
	}
}

BOOL CCoder::IsPersonal(DWORD dwStates)
{
	if(IsAbnomal(dwStates))
		return TRUE;
	else
		return FALSE;
}

bool CCoder::IsSupportHTTP()
{
	return m_bSupportHTTP;
}

bool CCoder::IsSupportUDP()
{
	return m_bSupportUDP;
}

bool CCoder::IsSupportTCP()
{
	return m_bSupportTCP;
}

bool CCoder::IsSupportSMS()
{
	return m_bSupportSMS;
}

//例如飞鹰需要Ascill 这个函数应该返回假
BOOL CCoder::IsUseUnicodeCharSet() const
{
	return m_bUseUnicodeCharSet;
}

CString CCoder::GetDescription() const
{
	return m_strDescription;
}

bool CCoder::DeCoding(CStringA& strMsg, CMessageReceived& msg)
{
	msg.m_varRevcTime = COleDateTime::GetCurrentTime();
	return true;
}

bool CCoder::Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput)
{
	return true;
}

CCoder* CCoder::GetCoder(CStringA strIVU)
{
	if(strIVU=="SL708B")
		return &NP706;
	else if(strIVU=="NP850")
		return &NP850;
	else if(strIVU=="NP860")
		return &NP860;
	else if(strIVU=="STARG3_PS660")
		return &StarG3_PS660;
	else
		return nullptr;
}
