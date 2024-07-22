#include "stdafx.h"

#include "History.h"

#include "../Viewer/Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CHistory::CHistory()
{
	m_fScale = 1;
	m_Anchor = CPoint(0, 0);
}

void CHistory::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();

	AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocX"), m_Anchor.x);
	AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocY"), m_Anchor.y);

	if(pWnd!=nullptr)
	{
		pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strMap, m_fScale);
	}
}

const CHistory& CHistory::operator =(const CHistory& Src)
{
	if(&Src!=this)
	{
		m_strMap = Src.m_strMap;
		m_Anchor = Src.m_Anchor;
		m_fScale = Src.m_fScale;
	}

	return *this;
}
