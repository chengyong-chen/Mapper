// SetDataBaseServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Trasvr.h"
#include "SetDataBaseServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDataBaseServerDlg dialog


CSetDataBaseServerDlg::CSetDataBaseServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSetDataBaseServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDataBaseServerDlg)
		
	//}}AFX_DATA_INIT
}


void CSetDataBaseServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDataBaseServerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDataBaseServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSetDataBaseServerDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDataBaseServerDlg message handlers
