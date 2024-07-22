#pragma once

#include "RecordsetEx.h"

/////////////////////////////////////////////////////////////////////////////
// CFieldset - results from ::SQLColumns()

class __declspec(dllexport) CFieldset : public CRecordsetEx
{
public:
	CFieldset(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CFieldset)

	// Field Data
	CString m_strTableQualifier;
	CString m_strTableOwner;
	CString m_strTableName;
	CString m_strColumnName;
	int m_nDataType;
	CString m_strTypeName;
	long m_nPrecision;
	long m_nLength;
	int m_nScale;
	int m_nRadix;
	int m_fNullable;
	CString m_strRemarks;

	// Operations
	BOOL Open(LPCTSTR pszTableQualifier = nullptr, LPCTSTR pszTableOwner = nullptr, LPCTSTR pszTableName = nullptr, LPCTSTR pszColumnName = nullptr, UINT nOpenType = CRecordset::forwardOnly);
	// Overrides
	CString GetDefaultConnect() override;
	CString GetDefaultSQL() override;
	void DoFieldExchange(CFieldExchange*) override;
	// Implementation
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	LONG GetLinkType();
	LONG GetFieldDateType(CString strField);
};
