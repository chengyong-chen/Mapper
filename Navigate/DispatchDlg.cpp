// Dispatch1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "DispatchDlg.h"
#include "MonitorEar.h"
#include "Port.h"
#include "../Viewer/Global.h"

#include "Vehicle.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispatch1Dlg dialog

CDispatch1Dlg::CDispatch1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CDispatch1Dlg::IDD, pParent)
	, m_pGrid(nullptr)
{
	//{{AFX_DATA_INIT(CDispatch1Dlg)

	//}}AFX_DATA_INIT
}

void CDispatch1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispatch1Dlg)
	DDX_Control(pDX, IDC_MSFLEXGRID, m_VehGrid);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDispatch1Dlg, CDialog)
	//{{AFX_MSG_MAP(CDispatch1Dlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_DIALUP, OnDialup)
	ON_BN_CLICKED(IDC_HANGUP, OnHangup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDispatch1Dlg message handlers

BOOL CDispatch1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(nullptr==m_pGrid)
		return FALSE;

	m_VehGrid.SetTextMatrix(0, 0, _T("目标编号"));
	m_VehGrid.SetColWidth(0, 900);
	m_VehGrid.SetColAlignment(0, 3);

	m_VehGrid.SetTextMatrix(0, 1, _T("车牌号"));
	m_VehGrid.SetColWidth(1, 1500);
	m_VehGrid.SetColAlignment(1, 3);

	m_VehGrid.SetTextMatrix(0, 2, _T("通讯号码"));
	m_VehGrid.SetColWidth(2, 1500);
	m_VehGrid.SetColAlignment(2, 3);

	m_VehGrid.SetTextMatrix(0, 3, _T("派遣次数"));
	m_VehGrid.SetColWidth(3, 1500);
	m_VehGrid.SetColAlignment(3, 3);

	int rowsel = m_pGrid->GetRowSel();
	int index = 1;
	CString strID;

	strID = m_pGrid->GetTextMatrix(rowsel, 0);
	m_VehGrid.SetTextMatrix(index, 0, m_pGrid->GetTextMatrix(rowsel, 0));
	m_VehGrid.SetTextMatrix(index, 1, m_pGrid->GetTextMatrix(rowsel, 2));
	m_VehGrid.SetTextMatrix(index, 2, m_pGrid->GetTextMatrix(rowsel, 5));
	m_VehGrid.SetTextMatrix(index, 3, m_pGrid->GetTextMatrix(rowsel, 16));

	for(int i = 1; i<m_pGrid->GetRows(); i++)
	{
		DWORD data = m_pGrid->GetRowData(i);
		if((data&0X00010000)!=0)
			continue;
		if(i==rowsel)
			continue;
		strID = m_pGrid->GetTextMatrix(i, 0);

		index = m_VehGrid.GetRows();
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = index;
		m_VehGrid.AddItem(strID, Var);

		m_VehGrid.SetTextMatrix(index, 0, m_pGrid->GetTextMatrix(i, 0));
		m_VehGrid.SetTextMatrix(index, 1, m_pGrid->GetTextMatrix(i, 2));
		m_VehGrid.SetTextMatrix(index, 2, m_pGrid->GetTextMatrix(i, 5));
		m_VehGrid.SetTextMatrix(index, 3, m_pGrid->GetTextMatrix(i, 16));
	}

	GetDlgItem(IDC_NUMBER)->SetWindowText(m_VehGrid.GetTextMatrix(m_VehGrid.GetRow(), 2));

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDispatch1Dlg::OnOk()
{
	CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
	if(pPort==nullptr)
		return;
	const int index = m_VehGrid.GetRowSel();
	if(index<=0||index>=m_VehGrid.GetRows())
		return;
	const DWORD dwId = _ttoi(m_VehGrid.GetTextMatrix(index, 0));
	pPort->SendControl(dwId, GPSCLIENT_VEHICLE_DISPATCH);
	const int nNum = _ttoi(m_VehGrid.GetTextMatrix(index, 3));
	CString strNum;
	strNum.Format(_T("%d"), nNum+1);
	m_VehGrid.SetTextMatrix(index, 3, strNum);
}

void CDispatch1Dlg::OnDialup()
{
}

void CDispatch1Dlg::OnHangup()
{
}

BEGIN_EVENTSINK_MAP(CDispatch1Dlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CDispatch1Dlg)
	ON_EVENT(CDispatch1Dlg, IDC_MSFLEXGRID, 70 /* RowColChange */, OnRowColChangeMsflexgrid, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDispatch1Dlg::OnRowColChangeMsflexgrid()
{
	GetDlgItem(IDC_NUMBER)->SetWindowText(m_VehGrid.GetTextMatrix(m_VehGrid.GetRow(), 2));
}
