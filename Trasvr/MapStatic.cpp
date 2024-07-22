// MapStatic.cpp : implementation file
//

#include "stdafx.h"
#include "Trasvr.h"
#include "MapStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapStatic

CMapStatic::CMapStatic()
{
}

CMapStatic::~CMapStatic()
{
}


BEGIN_MESSAGE_MAP(CMapStatic, CStatic)
	//{{AFX_MSG_MAP(CMapStatic)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapStatic message handlers

BOOL CMapStatic::OnEraseBkgnd(CDC* pDC) 
{
	CBrush brBkgnd;
	brBkgnd.CreateSolidBrush(GetSysColor(COLOR_BACKGROUND)); 

	CRect rc;
	GetClientRect(&rc);

	pDC->FillRect(&rc, &brBkgnd);
	
	return TRUE; // CStatic::OnEraseBkgnd(pDC);
}
