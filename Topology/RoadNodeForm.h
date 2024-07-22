#pragma once

#include "Resource.h"

#include "matrixctrl.h"
#include "NodeForm.h"

// CRoadNodeForm 对话框

class __declspec(dllexport) CRoadNodeForm : public CNodeForm
{
	DECLARE_DYNAMIC(CRoadNodeForm)

public:
	CRoadNodeForm(CWnd* pParent = nullptr); // 标准构造函数
	~CRoadNodeForm() override;

	// 对话框数据
	enum
	{
		IDD = IDD_ROADNODE
	};

public:
	void SetNode(const CTopology* pTopology, CNode* pNode) override;
	void DrawView() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog() override;

	CMatrixCtrl m_matrixCtrl;
	afx_msg LONG OnMatrixChanged(UINT WPARAM, LONG LPARAM);
};
