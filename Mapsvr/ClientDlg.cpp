// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Mapsvr.h"
#include "ClientDlg.h"

#include "..\Socket\MapSinkSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog


CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientDlg)
		
	//}}AFX_DATA_INIT
}


void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientDlg)
	DDX_Control(pDX, IDC_CLIENTGRID, m_ClientGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	//{{AFX_MSG_MAP(CClientDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ClientGrid.SetTextMatrix(0,0,"用户名称");
	m_ClientGrid.SetColWidth(0,1500);
	m_ClientGrid.SetColAlignment(0,3);

	m_ClientGrid.SetTextMatrix(0,1,"用户IP");
	m_ClientGrid.SetColWidth(1,1600);
	m_ClientGrid.SetColAlignment(1,3);

	m_ClientGrid.SetTextMatrix(0,2,"登录时间");
	m_ClientGrid.SetColWidth(2,3000);
	m_ClientGrid.SetColAlignment(2,3);

/*	m_ClientGrid.SetTextMatrix(0,3,"客户数量");
	m_ClientGrid.SetColWidth(2,1000);
	m_ClientGrid.SetColAlignment(3,3);	
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClientDlg::InsertClient(CMapSinkSocket* pSocket) 
{
	int index = 0;
	if(m_ClientGrid.GetTextMatrix(1,1).IsEmpty() == TRUE)
	{
		index = 1;
	}
	else
	{
		index = m_ClientGrid.GetRows();
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = index;
		m_ClientGrid.AddItem("",Var);			
	}
					
	m_ClientGrid.SetTextMatrix(index,0, pSocket->m_strUser);
	m_ClientGrid.SetTextMatrix(index,1, pSocket->m_strIP);

	CString strGmt = CTime::GetCurrentTime().Format("%y年%m月%d日 %H:%M:%S");
	m_ClientGrid.SetTextMatrix(index,2, strGmt);
	m_ClientGrid.SetRowData(index,(DWORD)pSocket);
}


void CClientDlg::RemoveClient(CMapSinkSocket* pSocket)
{
	int nCount = m_ClientGrid.GetRows();
	for(int index=nCount-1; index>0;index--)
	{
		CMapSinkSocket* socket = (CMapSinkSocket*)m_ClientGrid.GetRowData(index);
		if(pSocket == socket)
		{
			if(m_ClientGrid.GetRows() == 2)
			{
				m_ClientGrid.SetTextMatrix(1, 0,"");
				m_ClientGrid.SetTextMatrix(1, 1,"");
				m_ClientGrid.SetTextMatrix(1, 2,"");
			//	m_ClientGrid.SetTextMatrix(1, 3,"");
			}
			else
			{	
				m_ClientGrid.RemoveItem(index);
			}	
		}
	}
}
