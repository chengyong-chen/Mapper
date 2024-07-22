#include "stdafx.h"

#include "Laydb.h"
#include "Global.h"
#include "LayerTree.h"

#include "../Geometry/Geom.h"

#include "../Public/ODBCDatabase.h"
#include "../Database/ODBCRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLaydb::CLaydb(CTree<CLayer>& tree)
	: CLayer(tree)
{
	m_bDynamic = 0X00;
}

CLaydb::CLaydb(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLayer(tree, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
	m_bDynamic = 0X00;
}

CLaydb::~CLaydb()
{
}

void CLaydb::Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetPrev(pos);
		delete pGeom;
	}
	m_geomlist.RemoveAll();
	m_geoDatasource.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
	CLayer::Serialize(ar, dwVersion, bIgnoreGeoms);
}

void CLaydb::Invalidate(CDocument* pDocument) const
{
	m_lastRect.SetRectEmpty();
	pDocument->UpdateAllViews(nullptr);
}

bool CLaydb::CanActivateGeom(const CGeom* pGeom) const
{
	if(pGeom == nullptr)
		return false;
	if(pGeom->m_bActive == true)
		return false;

	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase == nullptr)
		return false;
	
	
	CString strSQL;
	strSQL.Format(_T("Select Active From %s Where GID=%d"), m_geoDatasource.GetTableA(), pGeom->m_geoId);
	bool bOK = false;
	pDatabase->ReadValue(strSQL, bOK);
	return bOK;
}

void CLaydb::Dispose(CWnd* pWnd, const CDatainfo& datainfo, const CRectF& geoRect)
{
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase == nullptr)
		return;

	if(geoRect != m_lastRect || m_bVisible == false)
	{
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			this->PreInactivateGeom(datainfo, pGeom);
			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom);
			delete pGeom;
			pGeom = nullptr;
		}
		m_geomlist.RemoveAll();
	}
}

void CLaydb::Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect)
{
	this->Dispose(pWnd, datainfo, mapRect);
}

void CLaydb::GeomActivated(const CGeom* pGeom) const
{
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase == nullptr)
		return;
	if(pGeom == nullptr)
		return;

	CString strSQL;
	//	strSQL.Format(_T("Update %s Set Active=1 Where GID=%d"),m_geoDatasource.GetHeaderProfile().m_tableA,pGeom->m_geoId);
	//	pDatabase->ExecuteSQL(strSQL);
}

void CLaydb::NewGeomDrew(CGeom* pGeom)
{
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase != nullptr)
	{
		CString strSQL;
		strSQL.Format(_T("Insert Into %s default values"), m_geoDatasource.GetHeaderProfile().m_tableA);
		pDatabase->ExecuteSQL(strSQL);
		const DWORD dwId = GetMaxId(pDatabase, m_geoDatasource.GetHeaderProfile().m_tableA);

		CString strFields;
		CString strValues;
		if(m_geoDatasource.GetHeaderProfile().m_whereA.IsEmpty() == FALSE)
		{
			CString strWhere = m_geoDatasource.GetHeaderProfile().m_whereA;
			while(strWhere.Find(_T("=")) != -1)
			{
				int pos = strWhere.Find(_T('='));
				if(pos > 0 && (strWhere[pos - 1] == '>' || strWhere[pos - 1] == '<'))
				{
					strWhere = strWhere.Mid(pos + 1);
					continue;
				}

				CString strField = strWhere.Left(pos);
				CString strValue = strWhere.Mid(pos + 1);

				strField.TrimLeft();
				strField.TrimRight();
				strValue.TrimLeft();
				strValue.TrimRight();

				if(strField.ReverseFind(_T(' ')) != -1)
				{
					const int pos = strField.ReverseFind(_T(' '));
					strField = strField.Mid(pos + 1);
				}
				strFields += (strField + _T(","));

				strWhere = strValue;
				if(strValue[0] == '\'')
				{
					pos = strValue.Find(_T('\''), 1);
					strWhere = strValue.Mid(pos + 1);
					strValue = strValue.Left(pos + 1);

					strValues += (strValue + _T(','));
				}
				else if(strValue.Find(' ') != -1)
				{
					pos = strValue.Find(_T(' '), 1);
					strWhere = strValue.Mid(pos + 1);
					strValue = strValue.Left(pos);

					strValues += (strValue + _T(','));
				}
				else
				{
					strValues += (strValue + _T(','));
				}
			}
		}
		const COleDateTime datetime = COleDateTime::GetCurrentTime();
		const CString strDate = datetime.Format(_T("%Y-%m-%d"));

		strFields += _T("Date_Create,Date_Modify");
		strValues += (strDate + strDate);

		CString strSQL2;
		strSQL2.Format(_T("Insert Into %s (GID,%s) Values(%d,'%s')"), m_geoDatasource.GetHeaderProfile().m_tableA, dwId, strFields, strValues);
		pDatabase->ExecuteSQL(strSQL2);
		pGeom->m_geoId = dwId;
	}
}

void CLaydb::NewGeomBred(CGeom* pGeom)
{
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase != nullptr)
	{
		const DWORD dwId = this->DuplicateRec(pDatabase, m_geoDatasource.GetHeaderProfile().m_tableA, pGeom->m_geoId, -1);
		if(m_geoDatasource.GetHeaderProfile().m_tableA.IsEmpty() == FALSE)
		{
			const COleDateTime datetime = COleDateTime::GetCurrentTime();
			const CString strDate = datetime.Format(_T("%Y-%m-%d"));

			CString strSQL;

			strSQL.Format(_T("Update %s Set Date_Modify='%s' Where GID=%d"), m_geoDatasource.GetHeaderProfile().m_tableA, strDate, dwId);
			pDatabase->ExecuteSQL(strSQL);

			strSQL.Format(_T("Update %s Set Date_Modify='%s' Where GID=%d"), m_geoDatasource.GetHeaderProfile().m_tableA, strDate, dwId);
			pDatabase->ExecuteSQL(strSQL);
		}
		pGeom->m_geoId = dwId;
	}
}

void CLaydb::PreRemoveGeom(CGeom* pGeom) const
{
	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase != nullptr)
	{
		CString strSQL;
		strSQL.Format(_T("Delete From %s Where GID=%d"), m_geoDatasource.GetHeaderProfile().m_tableA, pGeom->m_geoId);
		pDatabase->ExecuteSQL(strSQL);
	}

	CLayer::PreRemoveGeom(pGeom);
}

void CLaydb::PreInactivateGeom(const CDatainfo& datainfo, CGeom* pGeom) const
{
	if(pGeom->m_bActive == false)
		return;
	if(pGeom->m_bModified == false)
		return;

	CODBCDatabase* pDatabase = m_tree.GetGeoDatabase(m_geoDatasource.m_strDatabase);
	if(pDatabase == nullptr)
		return;

	try
	{
		CString strSQL;
		strSQL.Format(_T("Select * From %s Where GID=%d"), m_geoDatasource.GetHeaderProfile().m_tableA, pGeom->m_geoId);

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, dbOpenDynaset);
		if(rs.IsEOF() == FALSE)
		{
			pGeom->PutValues(rs, datainfo);
			rs.Update();
		}
		rs.Close();
		const COleDateTime datetime = COleDateTime::GetCurrentTime();
		const CString strDate = datetime.Format(_T("%Y-%m-%d"));

		strSQL.Format(_T("Update %s Set Active=0,Date_Modify='%s' Where GID=%d"), m_geoDatasource.GetHeaderProfile().m_tableA, strDate, pGeom->m_attId);
		pDatabase->ExecuteSQL(strSQL);
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	pGeom->m_bModified = false;

	CLayer::PreInactivateGeom(datainfo, pGeom);
}

DWORD CLaydb::GetMaxId(CDatabase* pDatabase, CString strTable)
{
	DWORD dwMaxId = 0;
	if(pDatabase != nullptr/* && pDatabase.IsOpen() == TRUE*/)
	{
		CString strSQL;
		strSQL.Format(_T("Select Max(GID) As MaxID From %s"), strTable);

		try
		{
			CRecordset rs(pDatabase);
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			if(rs.IsEOF() == FALSE)
			{
				CStringA strValue;
				rs.GetFieldValue(_T("MaxID"), strValue);
				dwMaxId = (DWORD)atoi(strValue);
			}
			rs.Close();
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}

	return dwMaxId;
}

DWORD CLaydb::DuplicateRec(CDatabase* pDatabase, const CString& strTable, DWORD dwSourceId, DWORD dwTargetId)
{
	if(pDatabase == nullptr)
		return 0;

	DWORD dwId = dwSourceId;

	CString strSQL;
	strSQL.Format(_T("Select * From %s where GID=%d"), strTable, dwSourceId);
	try
	{
		CRecordset rs(pDatabase);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF() == FALSE)
		{
			CString strValues;
			CString strFields;
			for(short nIndex = 0; nIndex < rs.GetODBCFieldCount(); nIndex++)
			{
				CODBCFieldInfo fieldinfo;
				CString strValue;
				rs.GetODBCFieldInfo(nIndex, fieldinfo);
				rs.GetFieldValue(nIndex, strValue);
				strValue.TrimLeft();
				strValue.TrimRight();

				if(fieldinfo.m_strName == _T("GID"))
				{
					if(dwTargetId != -1)
					{
						CString strId;
						strId.Format(_T("%d"), dwTargetId);
						strFields += _T("GID,");
						strValues += strId + _T(',');
					}
					continue;
				}

				if(strValue == _T(""))
					continue;

				switch(fieldinfo.m_nSQLType)
				{
					case ADOCG::adBigInt:
					case ADOCG::adDecimal:
					case ADOCG::adDouble:
					case ADOCG::adGUID:
					case ADOCG::adSingle:
					case ADOCG::adSmallInt:
					case ADOCG::adTinyInt:
					case ADOCG::adUnsignedBigInt:
					case ADOCG::adUnsignedInt:
					case ADOCG::adUnsignedSmallInt:
					case ADOCG::adUnsignedTinyInt:
					case ADOCG::adInteger:
					case ADOCG::adNumeric:
					case ADOCG::adVarNumeric:
					case ADOCG::adBoolean:
					case ADOCG::adCurrency:
						{
							strFields += fieldinfo.m_strName + _T(',');
							if(strValue.IsEmpty() == TRUE)
							{
								strValues += _T("0,");
							}
							else
							{
								strValues += strValue + _T(',');
							}
						}
						break;
					case ADOCG::adBinary:
					case ADOCG::adVarBinary:
					case ADOCG::adLongVarBinary:
						{
						}
						break;
					case ADOCG::adBSTR:
					case ADOCG::adChapter:
					case ADOCG::adChar:
					case ADOCG::adDate:
					case ADOCG::adDBDate:
					case ADOCG::adDBTime:
					case ADOCG::adDBTimeStamp:
					case ADOCG::adEmpty:
					case ADOCG::adError:
					case ADOCG::adFileTime:
					case ADOCG::adIDispatch:
					case ADOCG::adIUnknown:
					case ADOCG::adLongVarChar:
					case ADOCG::adLongVarWChar:
					case ADOCG::adPropVariant:
					case ADOCG::adUserDefined:
					case ADOCG::adVarChar:
					case ADOCG::adVariant:
					case ADOCG::adVarWChar:
					case ADOCG::adWChar:
						{
							strFields += fieldinfo.m_strName + _T(',');
							strValues += _T('\'') + strValue + _T("\',");
						}
						break;
					default:
						break;
				}
			}

			rs.Close();

			if(strValues.IsEmpty() == FALSE)
			{
				strValues = strValues.Left(strValues.GetLength() - 1);
			}
			if(strFields.IsEmpty() == FALSE)
			{
				strFields = strFields.Left(strFields.GetLength() - 1);
			}

			CString strSQL;
			if(strValues.IsEmpty() == FALSE && strFields.IsEmpty() == FALSE)
				strSQL.Format(_T("Insert Into %s (%s) Values(%s)"), strTable, strFields, strValues);
			else
				strSQL.Format(_T("Insert Into %s Default Values"), strTable);
			pDatabase->ExecuteSQL(strSQL);

			if(dwTargetId == -1)
				dwId = GetMaxId(pDatabase, strTable);
			else
				dwId = dwTargetId;
		}
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return dwId;
}
