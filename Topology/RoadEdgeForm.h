#pragma once
#include "resource.h"

class CRoadTopo;
class CRoadEdge;

#include "EdgeForm.h"

// CRoadEdgeForm 对话框

class __declspec(dllexport) CRoadEdgeForm : public CEdgeForm
{
	DECLARE_DYNAMIC(CRoadEdgeForm)

public:
	CRoadEdgeForm(CWnd* pParent = nullptr); // 标准构造函数
	~CRoadEdgeForm() override;

public:
	void SetEdge(const CTopology* pTopology, CEdge* pEdge) override;
	void DrawView() override;

	CBitmap m_bmpPermit;
	CBitmap m_bmpRefuse;

	// 对话框数据
	enum
	{
		IDD = IDD_ROADEDGE
	};

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonAB();
	afx_msg void OnClickedButtonBA();
	afx_msg void OnClickedChargeNo();
	afx_msg void OnClickedChargeMile();
	afx_msg void OnClickedChargeVehicle();
	afx_msg void OnClickedRestictSpeedNo();
	afx_msg void OnClickedRestictSpeedUpper();
	afx_msg void OnClickedRestictSpeedLower();
public:
	BOOL OnInitDialog() override;
};