#pragma once
#include "afxcmn.h"

// CCreateTableDlg 对话框

class CCreateTableDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateTableDlg)

public:
	CCreateTableDlg(CDatabase* pDatabase,CObList* pLiayerlist,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateTableDlg();

// 对话框数据
	enum { IDD = IDD_CREATETABLE };

public :
	CDialog*  d_pFieldCtrl;
public:
	CDatabase* m_pDatabase;
	CObList* m_pLayerlist;
	
public:
	HTREEITEM  GetDownItem(HTREEITEM hitem);
	HTREEITEM  GetItemById(WORD wItemID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_LayerTree;
	virtual BOOL OnInitDialog() override;
};
