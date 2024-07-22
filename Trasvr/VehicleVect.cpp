#include "stdafx.h"
#include "VehicleVect.h"
#include "..\Coding\Message.h"

#include "..\Smcom\ComThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVehicleVect::iterator CVehicleVect::find(const DWORD& dwId)
{
	for(CVehicleVect::iterator it = begin(); it != end(); it++)
	{
		if((*it)->m_dwId == dwId)
			return it;
	}

	return end();
}

void CVehicleVect::clear()
{
	for(CVehicleVect::iterator it = begin(); it != end(); it++)
	{
		delete *it;
	}
	erase(begin(), end());		
}

CVehicleVect::~CVehicleVect()
{
	for(CVehicleVect::iterator it = begin(); it != end(); it++)
	{
		delete *it;
	}		
	erase(begin(), end());		
}


SVehicle* CVehicleVect::GetVehicleInfo(const DWORD& dwId)
{
	iterator it = find(dwId);
	if(it != end())
		return (SVehicle*)(*it);
	else
		return nullptr;
}

SVehicle* CVehicleVect::GetVehicleInfo(LPCTSTR szCode)
{
	for(iterator it = begin(); it != end(); it++)
	{		
        SVehicle* pVhc = (SVehicle*)(*it);
		if(pVhc != nullptr)
		{
			if(pVhc->m_strCode.CompareNoCase(szCode) == 0)
			{
				return pVhc;
			}
		}
	}

	return nullptr;	
}

SVehicle* CVehicleVect::GetVehicleInfo(CString* strIVUSIM, BOOL bUseGPRS)
{
	for(iterator it = begin(); it != end(); it++)
	{		
		SVehicle* pVhc = (SVehicle*)(*it);
		if(bUseGPRS)
		{
			if(*strIVUSIM == pVhc->m_strIVUSN) 
				return pVhc;
		}
		else
		{
			if(*strIVUSIM == pVhc->m_strIVUSIM) 
				return pVhc;
		}
	}
	
	return nullptr;
}

// 得到指定ID车辆信息
BOOL CVehicleVect::GetVehicleInfo(const DWORD& dwId, SVehicle &vhcInfo, ADOCG::_ConnectionPtr &pDBConnect)
{
	if(pDBConnect == nullptr)	
		return FALSE;

	iterator it = find(dwId);
	if(it != end())
	{
		vhcInfo = *(*it);
		return TRUE;
	}	

	try
	{
		CString szSQLCommand ;	
		szSQLCommand.Format("SELECT * FROM View_Vehicle_Info WHERE ID = %d", dwId);
		TRACE(szSQLCommand);
		
		_variant_t varRowsAff;
		ADOCG::_RecordsetPtr rs = pDBConnect->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		if(rs->EndofFile)
			return false;

		vhcInfo.m_dwId = dwId;

		_variant_t var;
		var = (rs->Fields->Item[_T("姓名")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strDriver = var.bstrVal;
			vhcInfo.m_strDriver.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("车牌号码")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strCode = var.bstrVal;
			vhcInfo.m_strCode.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("IVUSIM")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strIVUSIM = var.bstrVal;
			vhcInfo.m_strIVUSIM.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("IVUSN")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strIVUSN = var.bstrVal;
			vhcInfo.m_strIVUSN.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("车机密码")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strIVUPwd = var.bstrVal;
			vhcInfo.m_strIVUPwd.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("类型")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strIVUType = var.bstrVal;		
			vhcInfo.m_strIVUType.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("手机号码")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strOwnerMobile = var.bstrVal;
			vhcInfo.m_strOwnerMobile.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("座机号码")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strOwnerPhone = var.bstrVal;
			vhcInfo.m_strOwnerPhone.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("颜色")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strColor = var.bstrVal;
			vhcInfo.m_strColor.TrimRight(_T(" "));
		}
		var = (rs->Fields->Item[_T("用途")]->Value);
		if(var.vt != VT_NULL)
		{
			vhcInfo.m_strType = var.bstrVal;
			vhcInfo.m_strType.TrimRight(_T(" "));
		}
		
		SVehicle *pVhcInfo = new SVehicle;
		*pVhcInfo = vhcInfo;
		push_back(pVhcInfo);
	}	
	catch(_com_error &)
	{
	/*	_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count > 0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}
		OpenCenterDataBase(pDBConnect);
*/
		return false;
	}	
	
    return true;
}

BOOL CVehicleVect::GetBroadCastVehicleInfos(ADOCG::_ConnectionPtr &pDBConnect)
{	
	try
	{
		CString szSQLCommand ;
		szSQLCommand.Format("SELECT * FROM View_Vehicle_Info");		
		
		_variant_t varRowsAff;
		ADOCG::_RecordsetPtr rs = pDBConnect->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		
		if(rs->EndofFile) 
			return false;

		rs->MoveFirst();

		clear();
		
		while(!rs->EndofFile)
		{
			SVehicle *pVhcInfo = new SVehicle;
			
			_variant_t var;
			var = (rs->Fields->Item[_T("ID")]->Value);
			pVhcInfo->m_dwId = long(var);
			
			var = (rs->Fields->Item[_T("姓名")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strDriver = var.bstrVal;
				pVhcInfo->m_strDriver.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("车牌号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strCode = var.bstrVal;
				pVhcInfo->m_strCode.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("IVUSIM")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUSIM = var.bstrVal;
				pVhcInfo->m_strIVUSIM.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("IVUSN")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUSN = var.bstrVal;
				pVhcInfo->m_strIVUSN.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("车机密码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUPwd = var.bstrVal;
				pVhcInfo->m_strIVUPwd.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("类型")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strIVUType = var.bstrVal;
				pVhcInfo->m_strIVUType.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("手机号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strOwnerMobile = var.bstrVal;
				pVhcInfo->m_strOwnerMobile.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("座机号码")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strOwnerPhone = var.bstrVal;
				pVhcInfo->m_strOwnerPhone.TrimRight(_T(" "));
			}
			var = (rs->Fields->Item[_T("颜色")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strColor = var.bstrVal;
				pVhcInfo->m_strColor.TrimRight(_T(" "));			
			}
			var = (rs->Fields->Item[_T("用途")]->Value);
			if(var.vt != VT_NULL)
			{
				pVhcInfo->m_strType = var.bstrVal;
				pVhcInfo->m_strType.TrimRight(_T(" "));
			}
			

			
			rs->MoveNext();
			push_back(pVhcInfo);
		}			
	}	
	catch(_com_error &)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count > 0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
//ccy shan			Log_or_AfxMessageBox(bstr);
		}
//ccy shan		OpenCenterDataBase(pDBConnect);
//ccy shan		Log_or_AfxMessageBox("车辆数据装载失败！");

		return false;
	}
	
    return true;
}


void CVehicleVect::DirtyFalseAllVhc()
{			
	iterator it;
	for(it = begin(); it != end(); it++)
	{		
        SVehicle* pVhc = (SVehicle*)(*it);
		if(pVhc == nullptr) 
			continue;

		pVhc->m_bDirty = FALSE;
	}
}

void CVehicleVect::ClearAllNotDirtyVhc()
{				
	for(iterator it = begin(); it != end(); it++)
	{		
        SVehicle* pVhc = (SVehicle*)(*it);
		if(pVhc->m_bDirty == FALSE)
		{
			CString str;
			str.Format(_T("车辆[%d]记录被删除"), pVhc->m_dwId);	
			CComThread::PutMsgToMessageWnd(str);

			delete pVhc;
			pVhc = nullptr;
			erase(it);	
		}
	}
}
