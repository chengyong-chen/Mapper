#include "stdafx.h"

#include "GisDoc.h"
#include "MainFrm.h"

#include "../Mapper/Global.h"

#include "../Atlas/Atlas1.h"

#include "../Geometry/Global.h"

#include "../Information/POUPane.h"
#include "../Information/POUListCtrl.h"
#include "../Information/TDBGridCtrl.h"
#include "../Information/POUDBGridCtrl.h"
#include "../information/TablePane.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/ATTDBGridCtrl.h"
#include "../Information/ATTTDBGridCtrl.h"

#include "../Public/AttributeCtrl.h"
#include "../Public/PropertyPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGisDoc, CGrfDoc)

BEGIN_MESSAGE_MAP(CGisDoc, CGrfDoc)
	//{{AFX_MSG_MAP(CGisDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGisDoc::CGisDoc()
	: m_POUListCtrl(m_poulist)
{
	EnableCompoundFile(FALSE);

	CPOUPane* pPOUPane = (CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
	if(pPOUPane!=nullptr)
	{
		if(m_POUListCtrl.Create(pPOUPane, true)==TRUE)
		{
			m_POUListCtrl.m_pDocument = this;
		}
	}
}

CGisDoc::~CGisDoc()
{
	for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
	{
		const CLink* link = *it;
		delete link;
	}
	m_linklist.clear();

	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane!=nullptr)
	{
		if(pTablePane->m_pATTTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->SetDatabase(nullptr);
		}
		if(pTablePane->m_pATTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pATTDBGridCtrl->Clear();
		}
		if(pTablePane->m_pPOUTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pPOUTDBGridCtrl->SetDatabase(nullptr);
		}
		if(pTablePane->m_pPOUDBGridCtrl!=nullptr)
		{
			pTablePane->m_pPOUDBGridCtrl->Clear();
		}
	}
	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
		if(pAttributeCtrl!=nullptr)
		{
			pAttributeCtrl->OnTableChanged(nullptr, _T(""));
		}
	}
	CPOUPane* pPOUPane = (CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
	if(pPOUPane!=nullptr)
	{
		pPOUPane->SetMapCtrl((CPOUListCtrl*)nullptr);
	}
	if(m_POUListCtrl.m_hWnd!=nullptr)
	{
		m_POUListCtrl.PostMessage(WM_DESTROY, 0, 0);
		m_POUListCtrl.DestroyWindow();
	}
}

void CGisDoc::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	CGrfDoc::Draw(pView, viewinfo, g, invalidRect);
	const CPoint docPoint1 = invalidRect.TopLeft();
	const CPointF geoPoint1 = m_Datainfo.DocToWGS84(docPoint1);
	const CPoint docPoint2 = invalidRect.BottomRight();
	const CPointF geoPoint2 = m_Datainfo.DocToWGS84(docPoint2);

	CRectF geoRect;
	geoRect.left = geoPoint1.x;
	geoRect.top = geoPoint1.y;
	geoRect.right = geoPoint2.x;
	geoRect.bottom = geoPoint2.y;

	for(std::list<CPOU*>::reverse_iterator it = m_poulist.m_pous.rbegin(); it!=m_poulist.m_pous.rend(); ++it)
	{
		CPOU* pPOU = *it;
		bool drawTag = true;
		if(viewinfo.m_levelCurrent<pPOU->m_minLevelTag)
			drawTag = false;
		if(viewinfo.m_levelCurrent>=pPOU->m_maxLevelTag)
			drawTag = false;

		pPOU->Draw(g, viewinfo, geoRect);
	}

	CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame->m_pAtlas!=nullptr)
	{
		for(std::list<CPOU*>::reverse_iterator it = pAppFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it!=pAppFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			pPOU->Draw(g, viewinfo, geoRect);
		}
	}

	CRoute* pRoute = &(pAppFrame->m_Route);
	if(pRoute!=nullptr)
	{
		pRoute->Draw(pView, viewinfo, g, geoRect);
	}
}

void CGisDoc::DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	CGrfDoc::DrawTag(pView, viewinfo, g, invalidRect);
	const CPoint docPoint1 = invalidRect.TopLeft();
	const CPointF geoPoint1 = m_Datainfo.DocToWGS84(docPoint1);
	const CPoint docPoint2 = invalidRect.BottomRight();
	const CPointF geoPoint2 = m_Datainfo.DocToWGS84(docPoint2);

	CRectF geoRect;
	geoRect.left = geoPoint1.x;
	geoRect.top = geoPoint1.y;
	geoRect.right = geoPoint2.x;
	geoRect.bottom = geoPoint2.y;

	for(std::list<CPOU*>::reverse_iterator it = m_poulist.m_pous.rbegin(); it!=m_poulist.m_pous.rend(); ++it)
	{
		CPOU* pPOU = *it;
		bool drawTag = true;
		if(viewinfo.m_levelCurrent < pPOU->m_minLevelTag)
			drawTag = false;
		if(viewinfo.m_levelCurrent >= pPOU->m_maxLevelTag)
			drawTag = false;

		pPOU->DrawTag(g, viewinfo, geoRect);
	}
	const CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame->m_pAtlas!=nullptr)
	{
		for(std::list<CPOU*>::reverse_iterator it = pAppFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it!=pAppFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			pPOU->DrawTag(g, viewinfo, geoRect);
		}
	}
}

void CGisDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGrfDoc::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<(int)(m_linklist.size());
		for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
		{
			CLink* pLink = *it;
			const BYTE type = pLink->Gettype();
			ar<<type;
			pLink->Serialize(ar, dwVersion);
		}
		m_poulist.Serialize(ar, dwVersion);
	}
	else
	{
		int count1;
		ar>>count1;
		for(int index = 0; index<count1; index++)
		{
			BYTE type;
			ar>>type;
			CLink* pLink = CLink::Apply(type);
			pLink->Serialize(ar, dwVersion);
			m_linklist.push_back(pLink);
		}
		m_poulist.Serialize(ar, dwVersion);
		m_POUListCtrl.Build();

		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
		if(pTablePane!=nullptr)
		{
			pTablePane->ActivateTab('P');
		}
	}
}

#ifdef _DEBUG
void CGisDoc::AssertValid() const
{
	CGrfDoc::AssertValid();
}

void CGisDoc::Dump(CDumpContext& dc) const
{
	CGrfDoc::Dump(dc);
}
#endif //_DEBUG
