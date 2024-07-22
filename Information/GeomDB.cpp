#include "stdafx.h"
#include "Information.h"
#include "IO.h"


#include "GeomDB.h"

#include "InfoShowDlg.h"

#include "..\Database\Create.h"
#include "..\Database\Field.h"
#include "..\Mapper\Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SERIAL(CGeomDB, CODBCDatabase, 0)

CGeomDB::CGeomDB()
{
}

CGeomDB::~CGeomDB()
{		
}

void CGeomDB::ShowRecord(CString strTable,DWORD dwId)
{
	if(m_database.IsOpen() == false)
		return;

	CRecordset rsMark(&m_database);
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM [%s] WHERE GID=%d"),strTable,dwId);
	CODBCDatabase::ParseSQL(this,strSQL);
	
	if(rsMark.Open(CRecordset::snapshot, strSQL) == FALSE)
	{
		AfxMessageBox(_T("Cann't open table!"));
		return;
	}

	if(rsMark.IsEOF() == false)
	{
		HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst    = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		if(strTable == _T("特殊单位"))
		{
			CInfoShowDlg dlg(nullptr,&rsMark);
			dlg.DoModal();
		}
		if(strTable == _T("特殊单位"))
		{
//			CAttributeDlg dlg(nullptr,&rsMark);
//			dlg.DoModal();
		}
					
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	rsMark.Close();
}