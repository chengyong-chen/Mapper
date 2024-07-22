#pragma once

class CTopology;
class CEdge;

// CEdgeForm 对话框

class __declspec(dllexport) CEdgeForm : public CDialog
{
	DECLARE_DYNAMIC(CEdgeForm)

public:
	CEdgeForm(CWnd* pParent = nullptr); // 标准构造函数
	CEdgeForm(UINT nIDD, CWnd* pParent = nullptr);

	~CEdgeForm() override;

public:
	const CTopology* m_pTopo;
	const CEdge* m_pEdge;

public:
	virtual void SetEdge(const CTopology* pTopology, CEdge* pEdge);
	virtual void DrawView();

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK() override;
	afx_msg void OnCancel() override;
	afx_msg void OnPaint();
	afx_msg void OnNext();

public:
	BOOL OnInitDialog() override;
};
