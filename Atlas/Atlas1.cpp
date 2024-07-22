#include "stdafx.h"
#include <io.h>
#include "resource.h"

#include "Atlas1.h"
#include "CatalogTree.h"

#include "../Information/Global.h"
#include "../Information/POUList.h"
#include "../Information/POUListCtrl.h"
#include "../Information/POUPane.h"
#include "../Navigate/RoadTopo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtlas

CAtlas::CAtlas(CPOUPane* pPOUPane)
	: m_POUListCtrl(m_poulist)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_strName.LoadString(IDS_EMAP);
	m_strTitle.LoadString(IDS_EMAP);
	m_strHomePage = _T("Index.gis");
	m_strIndexHtml = _T("");

	m_bFree = TRUE;
	m_bEdit = TRUE;
	m_bRegionQuery = TRUE;
	m_bNavigate = FALSE;
	m_bHtml = FALSE;

	m_databaseLAB.m_dbms = DBMSTYPE::Access;
	m_pTopo = nullptr;

	if(pPOUPane!=nullptr&&m_POUListCtrl.Create(pPOUPane, true)==TRUE)
	{
		pPOUPane->SetAtlCtrl(&m_POUListCtrl);
	}
}

CAtlas::~CAtlas()
{
	delete m_pTopo;
	m_pTopo = nullptr;

	if(m_POUListCtrl.m_hWnd!=0)
	{
		m_POUListCtrl.PostMessage(WM_DESTROY, 0, 0);
		m_POUListCtrl.DestroyWindow();
	}
	m_databaseLAB.Close();
	m_databasePOI.Close();

	m_Catalog.Release();
	m_Pyramid.Release();
}

void CAtlas::Release()
{
	m_strName.LoadString(IDS_EMAP);
	m_strTitle.LoadString(IDS_EMAP);
	m_strHomePage = _T("Main.gis");
	m_strIndexHtml = _T("");

	m_bFree = TRUE;
	m_bNavigate = TRUE;
	m_bHtml = FALSE;

	m_strFilePath.Empty();

	m_Catalog.Release();
	m_Pyramid.Release();
}

bool CAtlas::Open(CString strFile, DWORD dwDBOptions)
{
	CFile file;
	if(file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite))
	{
		CArchive ar(&file, CArchive::load);

		DWORD dwVersion;
		ar >> dwVersion;
		Serialize(ar, dwVersion);

		ar.Close();
		file.Close();

		m_strFilePath = strFile;
	}
	else
	{
		AfxMessageBox(IDS_CANNOTOPENATLAS);
		return false;
	}

	return true;
}

void CAtlas::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(ar.IsStoring())
	{
		CString strIndexHtml = m_strIndexHtml;
		if(strIndexHtml.Find(_T("www."))==-1&&strIndexHtml.Find(_T("WWW."))==-1)
			AfxToRelatedPath(ar.m_strFileName, strIndexHtml);

		CString strHomePage = m_strHomePage;
		if(strHomePage.IsEmpty()==FALSE)
			AfxToRelatedPath(ar.m_strFileName, strHomePage);

		ar<<m_strName;
		ar<<m_strTitle;
		ar<<strHomePage;
		ar<<strIndexHtml;
		const BOOL bUnused1 = FALSE;
		const BOOL bUnused2 = FALSE;
		const BOOL bUnused3 = FALSE;
		ar<<bUnused1;
		ar<<bUnused2;
		ar<<m_bFree;
		ar<<m_bEdit;
		ar<<m_bRegionQuery;
		ar<<m_bNavigate;
		ar<<bUnused3;
		ar<<m_bHtml;
	}
	else
	{
		ar>>m_strName;
		ar>>m_strTitle;
		ar>>m_strHomePage;
		ar>>m_strIndexHtml;

		BOOL bUnused1 = FALSE;
		BOOL bUnused2 = FALSE;
		BOOL bUnused3 = FALSE;
		ar>>bUnused1;
		ar>>bUnused2;
		ar>>m_bFree;
		ar>>m_bEdit;
		ar>>m_bRegionQuery;
		ar>>m_bNavigate;
		ar>>bUnused3;
		ar>>m_bHtml;
		AfxToFullPath(ar.m_strFileName, m_strIndexHtml);
		AfxToFullPath(ar.m_strFileName, m_strHomePage);
	}

	m_databaseLAB.Serialize(ar, dwVersion);

	m_Catalog.Serialize(ar, dwVersion);
	m_Pyramid.Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		if(m_poulist.m_pous.size()>0)
		{
			CString strPOUFile = ar.m_strFileName;
			strPOUFile = strPOUFile.Left(strPOUFile.ReverseFind(_T('.')));
			strPOUFile += _T(".Pou");
			CFile file;
			if(file.Open(strPOUFile, CFile::modeCreate|CFile::modeWrite)==TRUE)
			{
				CArchive POUAr(&file, CArchive::store);

				m_poulist.Serialize(ar,dwVersion);

				POUAr.Close();
				file.Close();
			}
		}
	}
	else
	{
		CString strPOUFile = ar.m_strFileName;
		strPOUFile = strPOUFile.Left(strPOUFile.ReverseFind(_T('.')));
		strPOUFile += _T(".Pou");
		if(_taccess(strPOUFile, 0)!=-1)
		{
			CFile file;
			if(file.Open(strPOUFile, CFile::modeRead|CFile::shareDenyWrite)==TRUE)
			{
				CArchive POUAr(&file, CArchive::load);

				m_poulist.Serialize(ar, dwVersion);

				POUAr.Close();
				file.Close();
			}
		}

		if(m_Pyramid.m_DeckList.GetCount()>0)
		{
			if(m_Pyramid.BuildRelation()==false)
			{
				m_Pyramid.Release();
				AfxMessageBox(_T("Unable to form a paramid due to the unexpected directory structure!"));
			}
		}
	}
}

void CAtlas::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		SerializeStrCE(ar, m_strName);
		SerializeStrCE(ar, m_strTitle);
		CString strHomePage = m_strHomePage;
		strHomePage.Replace(_T(".GIS"), _T(".Cis"));
		strHomePage.Replace(_T(".Grf"), _T(".Cis"));
		SerializeStrCE(ar, strHomePage);
		SerializeStrCE(ar, m_strIndexHtml);

		ar<<m_bFree;
		ar<<m_bNavigate;
		ar<<m_bHtml;

		m_databaseLAB.ReleaseCE(ar);
	}

	m_Catalog.ReleaseCE(ar);
}

void CAtlas::SerializeWEB(CArchive& ar) const
{
}

int CAtlas::KeyQuery(CListCtrl* listCtrl, CString strKey, DWORD dwClass, WORD wMap)
{
	if(listCtrl==nullptr||strKey.IsEmpty())
		return 0;
	else if(m_databaseLAB.m_strDatabase.IsEmpty()==TRUE)
		return 0;

	CDatabase database;
	try
	{
		CString strConnect;
		strConnect.Format(_T("Driver={Microsoft Access Driver (*.mdb)};DBQ=%s;UID=%s;PWD=%s"), m_databaseLAB.m_strDatabase, m_databaseLAB.m_strUID, m_databaseLAB.m_strPWD);
		database.OpenEx(strConnect, CDatabase::noOdbcDialog|CDatabase::openReadOnly);
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		database.Close();
		ex->Delete();
		//CString string = ::LoadString(IDS_CANNOTOPENDB);
		AfxMessageBox(IDS_CANNOTOPENDB);
		//OutputDebugString(string);
		return 0;
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	CString strClass;
	CString strMap;
	if(dwClass!=0XFFFFFFFF)
	{
		//		strClass.Format(_T("Code Like %d And"),dwClass);
	}
	if(wMap!=0XFFFF)
	{
		strMap.Format(_T("Map Like %d And"), wMap);
	}

	CString strWhere;
	if(strKey.FindOneOf(_T(" +&|"))==-1)
	{
		strWhere.Format(_T("WHERE %s %s Name Like '%c%s%c'"), strClass, strMap, '%', strKey, '%');
	}
	else
	{
		strWhere = _T("WHERE ");
		CString buff;
		int pos = strKey.FindOneOf(_T(" +&|"));
		while(pos!=-1)
		{
			CString strOperate = strKey.Mid(pos, 1);
			CString str = strKey.Left(pos);
			str.Trim();
			switch(strOperate[0])
			{
			case _T('+'):
			case _T('&'):
			case _T(' '):
			{
				buff.Format(_T("Name Like '%c%s%c' And "), '%', str, '%');
				strWhere += buff;
			}
			break;
			case _T('|'):
			{
				buff.Format(_T("Name Like '%c%s%c' Or "), '%', str, '%');
				strWhere += buff;
			}
			break;
			}
			strKey = strKey.Mid(pos+1);
			pos = strKey.FindOneOf(_T("+&|"));
		}

		strKey.Trim();
		buff.Format(_T("Name Like '%c%s%c'"), '%', strKey, '%');
		strWhere += buff;
	}

	CMapWordToPtr mapIDDeck;
	POSITION pos = m_Pyramid.m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDeck* pDeck = m_Pyramid.m_DeckList.GetNext(pos);
		mapIDDeck.SetAt(pDeck->m_wId, pDeck);
	}

	CString strSQL;
	strSQL.Format(_T("SELECT Map,Name,Layer,Geom FROM Label %s"), strWhere);

	int count = 0;
	CRecordset rs(&database);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			PLACE* place = new PLACE;

			int nItem = listCtrl->GetItemCount();
			int index = listCtrl->InsertItem(nItem, nullptr);
			listCtrl->SetItemData(index, (DWORD)place);

			CString strValue;

			rs.GetFieldValue((short)0/*_T("Map")*/, strValue);
			place->wMap = _ttoi(strValue);

			CDeck* pDeck = nullptr;
			mapIDDeck.Lookup(place->wMap, (void*&)pDeck);
			if(pDeck!=nullptr)
				listCtrl->SetItemText(nItem, 1, pDeck->m_strName);

			rs.GetFieldValue((short)1/*_T("Name")*/, strValue);
			listCtrl->SetItemText(nItem, 0, strValue);

			rs.GetFieldValue((short)2/*_T("Layer")*/, strValue);
			place->wLayer = _ttoi(strValue);

			rs.GetFieldValue((short)3/*_T("Geom")*/, strValue);
			place->dwGeom = _tcstoul(strValue, nullptr, 10);

			rs.MoveNext();
			count++;
		}

		rs.Close();
	}
	catch(CDBException*ex)
	{
		//OutputDebugString(ex->errormessage);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}

	mapIDDeck.RemoveAll();
	database.Close();
	return count;
}

bool CAtlas::Save()
{
	if(m_strFilePath.IsEmpty()==TRUE)
	{
		CFileDialog dlg(FALSE,
			_T("Atl"),
			nullptr,
			OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
			_T("Atalas File (*.Atl)|*.Atl||"),
			nullptr);

		if(dlg.DoModal()==IDOK)
		{
			m_strFilePath = dlg.GetPathName();
		}
		else
			return false;
	}

	CFile file;
	if(file.Open(m_strFilePath, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		CArchive ar(&file, CArchive::store);
		DWORD dwVersion = AfxGetCurrentArVersion();
		ar << dwVersion;
		Serialize(ar,dwVersion);

		ar.Close();
		file.Close();
		return true;
	}
	else
	{
		AfxMessageBox(IDS_CANNOTCREATEORSAVEATLAS);
		return false;
	}
}

bool CAtlas::SaveAs(CString strFile)
{
	CFile file;
	if(file.Open(strFile, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		CArchive ar(&file, CArchive::store);
		DWORD dwVersion = AfxGetCurrentArVersion();
		ar << dwVersion;
		Serialize(ar, dwVersion);

		ar.Close();
		file.Close();

		m_strFilePath = strFile;
		return true;
	}
	else
		return false;
}

void CAtlas::PublishPC()
{
	CDaoDatabase* pDB = new CDaoDatabase();

	m_databaseLAB.m_dbms = DBMSTYPE::Access;
	if(m_databaseLAB.m_strDatabase.IsEmpty()==FALSE&&m_databaseLAB.m_strServer.IsEmpty()==FALSE&&_taccess(m_databaseLAB.m_strServer, 0)!=-1)
	{
		pDB->Open(m_databaseLAB.m_strServer, TRUE, FALSE, nullptr);
	}
	else
	{
		CString strPath = m_strFilePath;
		strPath = strPath.Left(strPath.ReverseFind(_T('.')));
		m_databaseLAB.m_strServer = strPath.Mid(strPath.ReverseFind(_T('\\'))+1);
		try
		{
			pDB->Create(strPath);
		}
		catch(CDaoException*ex)
		{
			//OutputDebugString(ex->errormessage);
			ex->ReportError();
			ex->Delete();
			delete pDB;
			return;
		}
		catch(CException*ex)
		{
			ex->Delete();
		}

		m_databaseLAB.m_dbms = DBMSTYPE::Access;

		if(pDB->IsOpen()==FALSE)
		{
			delete pDB;
			pDB = nullptr;
			return;
		}

		CDaoTableDef td(pDB);
		td.Create(_T("Label"), dbAttachExclusive, _T(""), _T("DATABASE = "));

		CDaoFieldInfo fieldInfo;
		fieldInfo.m_strName = _T("Name");
		fieldInfo.m_nType = dbText;
		fieldInfo.m_lSize = 100;
		fieldInfo.m_lAttributes = dbVariableField;
		fieldInfo.m_nOrdinalPosition = 0;
		fieldInfo.m_bRequired = FALSE;
		fieldInfo.m_bAllowZeroLength = TRUE;
		fieldInfo.m_lCollatingOrder = 0;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Map");
		fieldInfo.m_lSize = 2;
		fieldInfo.m_nType = dbInteger;
		fieldInfo.m_lAttributes = dbFixedField;
		fieldInfo.m_bAllowZeroLength = FALSE;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Layer");
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Code");
		fieldInfo.m_lSize = 4;
		fieldInfo.m_nType = dbLong;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Geom");
		td.CreateField(fieldInfo);

		td.Append();
		td.Close();
	}

	CDaoRecordset rs(pDB);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), "Label");
		rs.Open(dbOpenDynaset, strSQL);
	}
	catch(CDaoException*ex)
	{
		//OutputDebugString(ex->errormessage);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
	AfxGetApp()->CloseAllDocuments(TRUE);

	m_Pyramid.PublishPC(pDB, rs, _T("label"));

	rs.Close();
	pDB->Close();
	delete pDB;

	Save();
}

void CAtlas::PublishCE()
{
	AfxGetApp()->CloseAllDocuments(TRUE);

	m_Pyramid.PublishCE();

	CFile file;
	if(file.Open(m_strFilePath, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		CArchive Ar(&file, CArchive::store);

		ReleaseCE(Ar);

		Ar.Close();
		file.Close();
	}
}

void CAtlas::PublishWEB()
{
	AfxGetApp()->CloseAllDocuments(TRUE);

	m_Pyramid.PublishWEB();

	CString strFile = m_strFilePath;
	strFile = strFile.Left(strFile.GetLength()-4);
	strFile = strFile+_T(".Wlt");

	CFile file;
	if(file.Open(strFile, CFile::modeCreate|CFile::modeWrite)==TRUE)
	{
		CArchive Ar(&file, CArchive::store);

		SerializeWEB(Ar);

		Ar.Close();
		file.Close();
	}
}
