#include "stdafx.h"

#include "NodeForm.h"
#include "RoadNodeForm.h"

#include "Topo.h"
#include "RoadNode.h"

#include "Global.h"

#include "Node.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CRoadNodeForm 对话框

IMPLEMENT_DYNAMIC(CRoadNodeForm, CDialog)

CRoadNodeForm::CRoadNodeForm(CWnd* pParent)
	: CNodeForm(CRoadNodeForm::IDD, pParent)
{
}

CRoadNodeForm::~CRoadNodeForm()
{
}

void CRoadNodeForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MATRIX, m_matrixCtrl);
}

BEGIN_MESSAGE_MAP(CRoadNodeForm, CDialog)

	//{{AFX_MSG_MAP(CGeoView)
	ON_MESSAGE(WM_MATRIXCHANGED, OnMatrixChanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

// CRoadNodeForm 消息处理程序

BOOL CRoadNodeForm::OnInitDialog()
{
	CNodeForm::OnInitDialog();

	if(m_pNode!=nullptr)
	{
		m_matrixCtrl.Initialize(&((CRoadNode*)m_pNode)->m_Forbid);
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRoadNodeForm::SetNode(const CTopology* pTopology, CNode* pNode)
{
	if(pNode!=nullptr)
		m_matrixCtrl.Initialize(&((CRoadNode*)m_pNode)->m_Forbid);
	else
		m_matrixCtrl.Initialize(nullptr);

	if(m_pNode!=pNode)
	{
		if(m_pNode!=nullptr)
		{
			if(((CButton*)GetDlgItem(IDC_TRAFFICLIGHT))->GetCheck()==BST_CHECKED)
				((CRoadNode*)m_pNode)->m_bAttribute1 |= NODE_TRAFFICLIGHT;
			else
				((CRoadNode*)m_pNode)->m_bAttribute1 &= ~NODE_TRAFFICLIGHT;
		}
	}

	CNodeForm::SetNode(pTopology, pNode);
}

void CRoadNodeForm::DrawView()
{
	CWnd* pWnd = nullptr;
	CWnd* pViewWnd = GetDlgItem(IDC_VIEW);
	if(pViewWnd!=nullptr)
	{
		if(m_pNode!=nullptr&&m_pTopo!=nullptr)
		{
			//		g.TransformPoints(CoordinateSpaceDevice,CoordinateSpaceWorld,&point,1);
			//		point.x -= 8;
			//		point.y -= 8;
			//		CClientDC clidc(pViewWnd);

			//		m_matrixCtrl.m_imagelist.Draw(&clidc,m_pNode->m_Forbid.GetValue(m_matrixCtrl.m_nActiveRow,index) == false ? 2 : 3, point, ILD_TRANSPARENT);
		}
	}

	CNodeForm::DrawView();
}

LONG CRoadNodeForm::OnMatrixChanged(UINT WPARAM, LONG LPARAM)
{
	if(m_pNode!=nullptr)
	{
		m_pNode->m_bModified = true;
	}

	return 0;
}
