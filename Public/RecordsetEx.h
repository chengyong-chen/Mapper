#pragma once

// RecordsetEx.h : header file

/////////////////////////////////////////////////////////////////////////////
// CRecordsetEx recordset
class CRecordsetEx : public CRecordset
{
public:
	CRecordsetEx(CDatabase* pDatabase = nullptr);
	DECLARE_DYNAMIC(CRecordsetEx)

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordsetEx)
public:
	BOOL Open(UINT nOpenType = snapshot, LPCTSTR lpszSql = nullptr, DWORD dwOptions = none) override;
	//}}AFX_VIRTUAL

	void AllocAndCacheFieldInfo();
	BOOL ExecDirect(const CString& sSQL);
	void GetFieldValue(short nIndex, CString& strValue);

	// Implementation
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif
private:
	static long GetData(CDatabase* pdb, HSTMT hstmt,short nFieldIndex, short nFieldType, LPVOID pvData, int nLen, short nSQLType);
};
