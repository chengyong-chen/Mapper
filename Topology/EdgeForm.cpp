#include "stdafx.h"
#include "resource.h"
#include "EdgeForm.h"
#include "Topo.h"
#include "Edge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CEdgeForm 对话框

IMPLEMENT_DYNAMIC(CEdgeForm, CDialog)

CEdgeForm::CEdgeForm(CWnd* pParent /*=nullptr*/)
{
	m_pTopo = nullptr;
	m_pEdge = nullptr;
}

CEdgeForm::CEdgeForm(UINT nId, CWnd* pParent)
	: CDialog(nId, pParent)
{
	m_pTopo = nullptr;
	m_pEdge = nullptr;
}

CEdgeForm::~CEdgeForm()
{
	m_pTopo = nullptr;
	m_pEdge = nullptr;
}

void CEdgeForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEdgeForm, CDialog)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
END_MESSAGE_MAP()

BOOL CEdgeForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CEdgeForm 消息处理程序
void CEdgeForm::OnOK()
{
}

void CEdgeForm::OnCancel()
{
}

void CEdgeForm::SetEdge(const CTopology* pTopology, CEdge* pEdge)
{
	if(m_pTopo!=pTopology)
	{
		if(pTopology!=nullptr)
		{
			CString strMaxID;
			strMaxID.Format(_T("%d"), pTopology->m_dwMaxEdgeId);
			GetDlgItem(IDC_MAXID)->SetWindowText(strMaxID);
			GetDlgItem(IDC_MINID)->SetWindowText(_T("1"));
		}
		else
		{
			GetDlgItem(IDC_MAXID)->SetWindowText(nullptr);
			GetDlgItem(IDC_MINID)->SetWindowText(nullptr);
		}
	}

	if(pEdge!=nullptr)
	{
	}
	else
	{
		GetDlgItem(IDC_EDITID)->SetWindowText(nullptr);
	}

	m_pTopo = pTopology;
	m_pEdge = pEdge;

	DrawView();
}

void CEdgeForm::OnPaint()
{
	CDialog::OnPaint();

	DrawView();
}

void CEdgeForm::DrawView()
{
}

void CEdgeForm::OnNext()
{
	// TODO: 在此添加控件通知处理程序代码
}
