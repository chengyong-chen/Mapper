#include "stdafx.h"
#include "Fieldset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CFieldset, CRecordsetEx)

CFieldset::CFieldset(CDatabase* pDatabase)
	: CRecordsetEx(pDatabase)
{
	m_strTableQualifier = L"";
	m_strTableOwner = L"";
	m_strTableName = L"";
	m_strColumnName = L"";
	m_nDataType = 0;
	m_strTypeName = L"";
	m_nPrecision = 0;
	m_nLength = 0;
	m_nScale = 0;
	m_nRadix = 0;
	m_fNullable = 0;
	m_strRemarks = L"";
	m_nFields = 12;
}

BOOL CFieldset::Open(LPCTSTR pszTableQualifier, LPCTSTR pszTableOwner, LPCTSTR pszTableName, LPCTSTR pszColumnName, UINT nOpenType)
{
	RETCODE nRetCode;
	UWORD bFunctionExists;

	// Make sure SQLColumns is supported
	AFX_SQL_SYNC(::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLCOLUMNS, &bFunctionExists));
	if(!Check(nRetCode))
		AfxThrowDBException(nRetCode, m_pDatabase, m_hstmt);
	if(!bFunctionExists)
		throw _T("<::SQLColumns> not supported.");

	// Cache state info and allocate hstmt
	SetState(nOpenType, nullptr, readOnly);
	if(!AllocHstmt())
		return FALSE;

	try
	{
		OnSetOptions(m_hstmt);
		AllocStatusArrays();

		// Call the ODBC function
		AFX_ODBC_CALL(::SQLColumns(m_hstmt, (SQLWCHAR*)pszTableQualifier, SQL_NTS, (SQLWCHAR*)pszTableOwner, SQL_NTS, (SQLWCHAR*)pszTableName, SQL_NTS, (SQLWCHAR*)pszColumnName, SQL_NTS));
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
	catch(CException* ex)
	{
		//		OutputDebugString(e->);
		Close();
		THROW_LAST();
	}
	return TRUE;
}

void CFieldset::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("TABLE_QUALIFIER"), m_strTableQualifier);
	RFX_Text(pFX, _T("TABLE_OWNER"), m_strTableOwner);
	RFX_Text(pFX, _T("TABLE_NAME"), m_strTableName);
	RFX_Text(pFX, _T("COLUMN_NAME"), m_strColumnName);
	RFX_Int(pFX, _T("DATA_TYPE"), m_nDataType);
	RFX_Text(pFX, _T("TYPE_NAME"), m_strTypeName);
	RFX_Long(pFX, _T("PRECISION"), m_nPrecision);
	RFX_Long(pFX, _T("LENGTH"), m_nLength);
	RFX_Int(pFX, _T("SCALE"), m_nScale);
	RFX_Int(pFX, _T("RADIX"), m_nRadix);
	RFX_Int(pFX, _T("NULLABLE"), m_fNullable);
	RFX_Text(pFX, _T("REMARKS"), m_strRemarks);
}

CString CFieldset::GetDefaultConnect()
{
	return _T("ODBC;");
}

CString CFieldset::GetDefaultSQL()
{
	return _T("!"); // Direct ODBC call
}

/////////////////////////////////////////////////////////////////////////////
// CFieldset diagnostics

#ifdef _DEBUG
void CFieldset::AssertValid() const
{
	CRecordsetEx::AssertValid();
}

void CFieldset::Dump(CDumpContext& dc) const
{
	CRecordsetEx::Dump(dc);
}
#endif //_DEBUG

LONG CFieldset::GetLinkType()
{
	if(IsOpen()==FALSE)
		return 0;

	int nId = 0;
	int nLng = 0;
	int nLat = 0;

	while(IsEOF()==FALSE)
	{
		CString strColumn = m_strColumnName;
		const int nPos = strColumn.Find(_T('.'));
		if(nPos==-1)
		{
			if(strColumn.CompareNoCase(_T("Id"))==0)
				nId++;
			if(strColumn.CompareNoCase(_T("X"))==0)
				nLng++;
			if(strColumn.CompareNoCase(_T("Y"))==0)
				nLat++;
		}
		else
		{
			strColumn = strColumn.Left(nPos);

			if(strColumn.CompareNoCase(_T("Id"))==0)
				nId++;
			if(strColumn.CompareNoCase(_T("Logitude"))==0)
				nLng++;
			if(strColumn.CompareNoCase(_T("Y"))==0)
				nLat++;

			strColumn = m_strColumnName;
			strColumn = strColumn.Mid(nPos+1);

			if(strColumn.CompareNoCase(_T("Id"))==0)
				nId++;
			if(strColumn.CompareNoCase(_T("Logitude"))==0)
				nLng++;
			if(strColumn.CompareNoCase(_T("Y"))==0)
				nLat++;
		}

		MoveNext();
	}

	if(nId>=1&&nLng==0&&nLat==0)
		return 1;
	if(nId>=1&&nLng>=1&&nLat>=1)
		return 2;

	return 0;
}

LONG CFieldset::GetFieldDateType(CString strField)
{
	if(IsOpen()==FALSE)
		return 0;

	while(IsEOF()==FALSE)
	{
		CString strColumn = m_strColumnName;
		const int nPos = strColumn.Find(_T('.'));
		if(nPos!=-1)
		{
			strColumn = strColumn.Left(nPos);
		}
		if(strColumn==strField)
		{
			switch(m_nDataType)
			{
			case SQL_FLOAT:
			case SQL_REAL:
			case SQL_DOUBLE:
			case SQL_DECIMAL:
			case SQL_NUMERIC:
			case SQL_TINYINT:
			case SQL_SMALLINT:
			case SQL_INTEGER:
			case SQL_BIGINT:
				return 1;
				break;
			default:
				return 0;
				break;
			}
		}

		MoveNext();
	}

	return -1;
}
