#pragma once

class CTopology;
class CNode;

class __declspec(dllexport) CNodeForm : public CDialog
{
	DECLARE_DYNAMIC(CNodeForm)

public:
	CNodeForm(CWnd* pParent = nullptr); // ��׼���캯��
	CNodeForm(UINT nIDD, CWnd* pParent = nullptr);

	~CNodeForm() override;

public:
	const CTopology* m_pTopo;
	const CNode* m_pNode;

public:
	virtual void SetNode(const CTopology* pTopology, CNode* pNode);
	virtual void DrawView();

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog() override;

	afx_msg void OnPaint();
	afx_msg void OnOK() override;
	afx_msg void OnCancel() override;
	afx_msg void OnNext();
};
