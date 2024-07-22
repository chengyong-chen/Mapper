#ifndef _RECORDFORM_h_
#define _RECORDFORM_h_

#include "ListCtrlEx.h"

class __declspec(dllexport) CRecordCtrl : public CListCtrlEx
{
	struct SFieldInfo
	{
		CString strName;
		CString strValue;
		int nType;
	};

public:
	CRecordCtrl();

public:
	CDatabase* m_pDatabase;
	CString m_strTable;

	DWORD m_dwId;
	int m_nFieldCount;
	SFieldInfo* m_pFieldInfor;

public:
	void ActivateRec(CDatabase* pDatabase, CString strTable, CString strIdField, const DWORD& dwId);
	void DisposeRec() const;

public:
	BOOL Create(CWnd* pParent);

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CRecordCtrl() override;

protected:
	// Generated message map functions
	//{{AFX_MSG(CRecordCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
