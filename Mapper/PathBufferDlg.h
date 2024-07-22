#pragma once
#include "afxdialogex.h"
#include "Resource.h"
#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"
class CPathBufferDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPathBufferDlg)

public:
	CPathBufferDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPathBufferDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PATHBUFFER };
#endif
public:
	float distance;
	Clipper2Lib::EndType endtype;
	Clipper2Lib::JoinType jointype;
protected:
	void DoDataExchange(CDataExchange* pDX) override;  
	BOOL OnInitDialog() override;
	void OnOK() override;
	DECLARE_MESSAGE_MAP()
};
