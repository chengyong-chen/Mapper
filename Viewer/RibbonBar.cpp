#include "StdAfx.h"
#include "RibbonBar.h"

#include "POIBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRibbonBar::CRibbonBar(void)
{
	m_pPOIBar = nullptr;
}

CRibbonBar::~CRibbonBar(void)
{
	if(m_pPOIBar!=nullptr)
	{
		m_pPOIBar->PostMessage(WM_DESTROY, 0, 0);
		m_pPOIBar->DestroyWindow();
		delete m_pPOIBar;
		m_pPOIBar = nullptr;
	}
}

//{{AFX_MSG_MAP(CMFCRibbonBar)
BEGIN_MESSAGE_MAP(CRibbonBar, CMFCRibbonBar)
END_MESSAGE_MAP()
//}}AFX_MSG_MAP

void CRibbonBar::CreatePOIBar(CDatabase* pDatabase)
{
	CPOIBar* pPOIBar = new CPOIBar();
	if(pPOIBar->Create(IDD_POIBAR, this)==TRUE)
	{
		pPOIBar->SetDatabase(pDatabase);
		m_pPOIBar = pPOIBar;
	}
	else
	{
		delete pPOIBar;
		pPOIBar = nullptr;
	}
}

BOOL CRibbonBar::SetActiveCategory(CMFCRibbonCategory* pActiveCategory, BOOL bForceRestore)
{
	if(CMFCRibbonBar::SetActiveCategory(pActiveCategory, bForceRestore)==TRUE)
	{
		const CString strName = pActiveCategory->GetName();
		if(strName=="Information")
		{
			if(m_pPOIBar!=nullptr)
			{
				CMFCRibbonPanel* pMFCRibbonPanel = pActiveCategory->GetPanel(3);
				if(pMFCRibbonPanel!=nullptr)
				{
					CRect rect = pMFCRibbonPanel->GetRect();

					rect.DeflateRect(4, 4, 4, 20);
					m_pPOIBar->MoveWindow(rect);
					m_pPOIBar->ShowWindow(SW_SHOW);
				}
			}
		}
		else if(m_pPOIBar!=nullptr)
		{
			m_pPOIBar->ShowWindow(SW_HIDE);
		}

		return TRUE;
	}
	else
		return FALSE;
}
