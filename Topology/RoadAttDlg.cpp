// RoadAttDlg.cpp : implementation file
//

#include "stdafx.h"
#include "topology.h"
#include "RoadAttDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoadAttDlg dialog


CRoadAttDlg::CRoadAttDlg(CWnd* pParent,CTypedPtrMap<CMapWordToPtr,WORD,CTopology::ROADATT*>* mapRoadAtt)
	: CDialog(CRoadAttDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoadAttDlg)
		
	m_mapRoadAtt = nullptr;
	//}}AFX_DATA_INIT
	m_mapRoadAtt = mapRoadAtt;
}


void CRoadAttDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoadAttDlg)
	DDX_Control(pDX, IDC_ATTGRID, m_AttGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoadAttDlg, CDialog)
	//{{AFX_MSG_MAP(CRoadAttDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoadAttDlg message handlers

BOOL CRoadAttDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_AttGrid.SetTextMatrix(0,0,_T("��·����"));
	m_AttGrid.SetColWidth(0,1000);
	m_AttGrid.SetColAlignment(0,3);

	m_AttGrid.SetTextMatrix(0,1,_T("�����ٶ�"));
	m_AttGrid.SetColWidth(1,1000);
	m_AttGrid.SetColAlignment(1,3);

	m_AttGrid.SetTextMatrix(0,2,_T("��������"));
	m_AttGrid.SetColWidth(2,1000);
	m_AttGrid.SetColAlignment(2,3);

	m_AttGrid.SetTextMatrix(0,3,_T("��·���"));
	m_AttGrid.SetColWidth(3,1000);
	m_AttGrid.SetColAlignment(3,3);

	m_AttGrid.SetTextMatrix(0,4,_T("�Ƿ��շ�"));
	m_AttGrid.SetColWidth(4,1000);
	m_AttGrid.SetColAlignment(4,3);
	
	if(m_mapRoadAtt != nullptr)
	{
		int index = 0;
		POSITION pos = m_mapRoadAtt->GetStartPosition();
		while(pos!=nullptr)
		{
			WORD wId;
			CTopology::ROADATT* pRoadAtt=nullptr;

			m_mapRoadAtt->GetNextAssoc(pos,wId,pRoadAtt);
			if(pRoadAtt != nullptr)
			{
				if(index == 0)
				{
				}
				else
				{	
					VARIANT Var;
					Var.vt = VT_I4;
					Var.lVal = index+1;
					m_AttGrid.AddItem(pRoadAtt->strName,Var);	
				}
				CString strSpeed;
				strSpeed.Format(_T("%d"),pRoadAtt->nSpeed);
				m_AttGrid.SetTextMatrix(index+1, 0, pRoadAtt->strName);
				m_AttGrid.SetTextMatrix(index+1, 1, strSpeed);
			}

			index++;
		}
	}
	return TRUE; 
}

BEGIN_EVENTSINK_MAP(CRoadAttDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CRoadAttDlg)
	ON_EVENT(CRoadAttDlg, IDC_ATTGRID, -600 /* Click */, OnClickAttGrid, VTS_NONE)
	ON_EVENT(CRoadAttDlg, IDC_ATTGRID, -601 /* DblClick */, OnDblClickAttGrid, VTS_NONE)
	ON_EVENT(CRoadAttDlg, IDC_ATTGRID, -603 /* KeyPress */, OnKeyPressAttGrid, VTS_PI2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CRoadAttDlg::OnClickAttGrid() 
{
	m_AttGrid.OnClickGrid();
}

void CRoadAttDlg::OnDblClickAttGrid() 
{
	m_AttGrid.OnClickGrid();
}

void CRoadAttDlg::OnKeyPressAttGrid(short FAR* KeyAscii) 
{
	m_AttGrid.OnKeyPressGrid(KeyAscii);
}

BOOL CRoadAttDlg::PreTranslateMessage(MSG* pMsg) 
{	
	CWnd*hwnd = GetDlgItem (IDC_ATTGRID);
	HWND hGrid = hwnd->GetSafeHwnd ();

	if(pMsg->message == WM_KEYDOWN  && (pMsg->hwnd == hGrid ))
	{
		switch (pMsg->wParam)
		{
        case VK_UP : 
			m_AttGrid.GoUp ();
			return TRUE;
		case VK_DOWN:
			m_AttGrid.GoDown();
			return TRUE;
		case VK_LEFT : 
			m_AttGrid.GoLeft();
			return TRUE;
		case VK_RIGHT: 
			m_AttGrid.GoRight();
			return TRUE;	
		}	
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CRoadAttDlg::OnOK() 
{
	if(m_mapRoadAtt != nullptr)
	{
		int nCount = m_AttGrid.GetRows();
		for(int index=1; index<nCount;index++)
		{
			int index1 = 0;
			POSITION pos = m_mapRoadAtt->GetStartPosition();
			while(pos!=nullptr)
			{
				WORD wId;
				CTopology::ROADATT* pRoadAtt=nullptr;

				m_mapRoadAtt->GetNextAssoc(pos,wId,pRoadAtt);
				if(index1 == index-1)
				{
					if(pRoadAtt != nullptr)
					{
						CString strSpeed = m_AttGrid.GetTextMatrix(index,1);
						DWORD nSpeed = _ttoi(strSpeed);
						pRoadAtt->nSpeed = nSpeed;
					}
					break;
				}
				index1++;
			}
		}	
	}
	
	CDialog::OnOK();
}
