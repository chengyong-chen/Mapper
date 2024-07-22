#pragma once
#include <optional>
#include "Resource.h"

class CPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent, const DWORD& dwGeomId, const DWORD& dwAttId, CStringA& strGeoCode, std::optional<bool> m_bClockwise); // standard constructor
	~CPropertyDlg() override;

	enum { IDD = IDD_GEOMPROPERTY };
private:
	DWORD m_dwGeomId;
	DWORD m_dwAttId;	
	CString m_strClockwise;
public:
	CString m_strGeoCode;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
