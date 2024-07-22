#include "stdafx.h"
#include "Tableset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

IMPLEMENT_DYNAMIC(CTableset, CRecordsetEx)

CTableset::CTableset(CDatabase* pDatabase)
	: CRecordsetEx(pDatabase)
{
	m_strTableQualifier = L"";
	m_strTableOwner = L"";
	m_strTableName = L"";
	m_strTableType = L"";
	m_strRemarks = L"";
	m_nFields = 5;
}

BOOL CTableset::Open(LPCSTR pszTableQualifier, LPCSTR pszTableOwner, LPCSTR pszTableName, LPCSTR pszTableType, UINT nOpenType)
{
	RETCODE nRetCode;
	UWORD bFunctionExists;

	// Make sure SQLTables exists
	AFX_SQL_SYNC(::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLTABLES, &bFunctionExists));
	if(!Check(nRetCode))
		AfxThrowDBException(nRetCode, m_pDatabase, m_hstmt);
	if(!bFunctionExists)
		throw _T("<::SQLTables> not supported.");

	// Cache state info and allocate hstmt
	SetState(nOpenType, nullptr, readOnly);
	if(!AllocHstmt())
		return FALSE;

	try
	{
		OnSetOptions(m_hstmt);
		AllocStatusArrays();

		// Call the ODBC function
		AFX_ODBC_CALL(::SQLTablesA(m_hstmt, (UCHAR FAR*)pszTableQualifier, SQL_NTS, (UCHAR FAR*)pszTableOwner, SQL_NTS, (UCHAR FAR*)pszTableName, SQL_NTS, (UCHAR FAR*)pszTableType, SQL_NTS));
		if(!Check(nRetCode))
			ThrowDBException(nRetCode, m_hstmt);

		// Allocate memory and cache info
		AllocAndCacheFieldInfo();
		AllocRowset();

		// Fetch the first row of data
		MoveNext();

		// If EOF, result set is empty, set BOF as well
		m_bBOF = m_bEOF;
	}
	catch(CException*ex)
	{
		//		OutputDebugString(ex->m_strError);
		Close();
		THROW_LAST();
	}
	return TRUE;
}

void CTableset::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("TABLE_QUALIFIER"), m_strTableQualifier);
	RFX_Text(pFX, _T("TABLE_OWNER"), m_strTableOwner);
	RFX_Text(pFX, _T("TABLE_NAME"), m_strTableName);
	RFX_Text(pFX, _T("TABLE_TYPE"), m_strTableType);
	RFX_Text(pFX, _T("REMARKS"), m_strRemarks);
}

CString CTableset::GetDefaultConnect()
{
	return _T("ODBC;");
}

CString CTableset::GetDefaultSQL()
{
	return _T("!"); // Direct ODBC call
}

/////////////////////////////////////////////////////////////////////////////
// CTableset diagnostics

#ifdef _DEBUG
void CTableset::AssertValid() const
{
	CRecordsetEx::AssertValid();
}

void CTableset::Dump(CDumpContext& dc) const
{
	CRecordsetEx::Dump(dc);
}
#endif //_DEBUG
