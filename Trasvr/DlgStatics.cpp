// DlgStatics.cpp : implementation file
//

#include "stdafx.h"
#include "Trasvr.h"
#include "DlgStatics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStatics dialog


CDlgStatics::CDlgStatics(CWnd* pParent /*=nullptr*/)
	: CDialog(pParent)
{
	//{{AFX_DATA_INIT(CDlgStatics)
		
	//}}AFX_DATA_INIT
}


void CDlgStatics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStatics)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStatics, CDialog)
	//{{AFX_MSG_MAP(CDlgStatics)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStatics message handlers
