// Tables.h: interface for the CTableset class.

#pragma once

#include "RecordsetEx.h"
class CDatabase;

/////////////////////////////////////////////////////////////////////////////
// CTableset - results from ::SQLTables()

class __declspec(dllexport) CTableset : public CRecordsetEx
{
public:
	CTableset(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CTableset)

	// Field Data
	CString m_strTableQualifier;
	CString m_strTableOwner;
	CString m_strTableName;
	CString m_strTableType;
	CString m_strRemarks;

	// Operations
	BOOL Open(LPCSTR pszTableQualifier = nullptr, LPCSTR pszTableOwner = nullptr, LPCSTR pszTableName = nullptr, LPCSTR pszColumnName = nullptr, UINT nOpenType = forwardOnly);
	// Overrides
	CString GetDefaultConnect() override;
	CString GetDefaultSQL() override;
	void DoFieldExchange(CFieldExchange*) override;

	// Implementation
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif
};
