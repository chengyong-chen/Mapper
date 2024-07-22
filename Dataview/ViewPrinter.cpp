#include "StdAfx.h"
#include "ViewPrinter.h"
#include "Datainfo.h"
#include "MyPrintDlg.h"

#include "../Projection/Projection1.h"
#include "../Utility/Scale.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewPrinter::CViewPrinter(CDatainfo& foreground)
	: CViewinfo(foreground)
{
	m_marginTop = 0;
	m_marginBottom = 0;
	m_marginLeft = 0;
	m_marginRight = 0;

	m_bHCenter = FALSE;
	m_bVCenter = FALSE;
}

CViewPrinter::~CViewPrinter(void)
{
}

BOOL CViewPrinter::DoPreparePrinting(CView* pView, CPrintInfo* pInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD dwFlags = PD_ALLPAGES|PD_USEDEVMODECOPIES|PD_HIDEPRINTTOFILE|PD_ENABLEPRINTTEMPLATE|PD_RETURNDC;
	if(m_docPrint.IsRectNull()==TRUE)
		dwFlags |= PD_NOSELECTION;
	else
		dwFlags |= PD_SELECTION;

	if(pInfo->m_pPD!=nullptr)
	{
		delete pInfo->m_pPD;
		pInfo->m_pPD = nullptr;
	}
	HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	pInfo->m_pPD = new CMyPrintDlg(FALSE, dwFlags, pView);
	((CMyPrintDlg*)pInfo->m_pPD)->m_viewPrinter = this;
	const BOOL oldMode = pInfo->m_bPreview;
	pInfo->m_bPreview = FALSE;
	pInfo->m_pPD->m_pd.nMinPage = 1;
	pInfo->m_pPD->m_pd.nMaxPage = 0xffff;

	if(pView->DoPreparePrinting(pInfo)==FALSE)
	{
		pInfo->m_pPD->PostMessage(WM_DESTROY, 0, 0);
		pInfo->m_pPD->DestroyWindow();
		delete pInfo->m_pPD;
		pInfo->m_pPD = nullptr;

		pInfo->m_bPreview = oldMode;

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return FALSE;
	}
	else
	{
		pInfo->m_bPreview = oldMode;
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return TRUE;
	}
}

void CViewPrinter::BeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	m_sizeDPI.cx = pDC->GetDeviceCaps(LOGPIXELSX);
	m_sizeDPI.cy = pDC->GetDeviceCaps(LOGPIXELSY);

	CRectF rect = this->m_datainfo.GetMapRect();
	CViewinfo::ScaleTo(nullptr, m_scalePrint, rect.CenterPoint());

	
	const CRectF mapPrint = m_datainfo.DocToMap(m_docPrint);
	const int cxView = mapPrint.Width()*m_xFactorMapToView;
	const int cyView = mapPrint.Height()*m_yFactorMapToView;
	const int cxPage = pDC->GetDeviceCaps(HORZRES)-(m_marginLeft+m_marginRight)*m_sizeDPI.cx;
	const int cyPage = pDC->GetDeviceCaps(VERTRES)-(m_marginTop+m_marginBottom)*m_sizeDPI.cy;
	const int nOverlap = ((CMyPrintDlg*)pInfo->m_pPD)->m_nOverlap*m_sizeDPI.cx/24.5;//Pixels
	const int colCount = cxView<=cxPage ? 1 : (cxView-nOverlap)%(cxPage-nOverlap)==0 ? (cxView-nOverlap)/(cxPage-nOverlap) : (cxView-nOverlap)/(cxPage-nOverlap)+1;
	const int rowCount = cyView<=cyPage ? 1 : (cyView-nOverlap)%(cyPage-nOverlap)==0 ? (cyView-nOverlap)/(cyPage-nOverlap) : (cyView-nOverlap)/(cyPage-nOverlap)+1;
	const BOOL bRange = pInfo->m_pPD->PrintRange();
	if(bRange==TRUE)
	{
		long fromPage = pInfo->m_pPD->GetFromPage();
		long toPage = pInfo->m_pPD->GetToPage();
		if(fromPage>colCount*rowCount)
			fromPage = colCount*rowCount;
		if(toPage>colCount*rowCount)
			toPage = colCount*rowCount;

		pInfo->SetMinPage(fromPage);
		pInfo->SetMaxPage(toPage);
		pInfo->m_nCurPage = fromPage;
	}
	else
	{
		pInfo->SetMinPage(1);
		pInfo->SetMaxPage(colCount*rowCount);
		pInfo->m_nCurPage = 1;
	}
}

void CViewPrinter::Print(CView* pView, CDC* pDC, CPrintInfo* pInfo)
{
	if(pInfo->m_bContinuePrinting==FALSE)
		return;

	const CRectF mapPrint = m_datainfo.DocToMap(m_docPrint);
	const CRectF mapTotal = m_datainfo.GetMapRect();
	const int cxView = mapPrint.Width()*m_xFactorMapToView;
	const int cyView = mapPrint.Height()*m_yFactorMapToView;
	const int cxPage = pDC->GetDeviceCaps(HORZRES)-(m_marginLeft+m_marginRight)*m_sizeDPI.cx;
	const int cyPage = pDC->GetDeviceCaps(VERTRES)-(m_marginTop+m_marginBottom)*m_sizeDPI.cy;
	const int nOverlap = ((CMyPrintDlg*)pInfo->m_pPD)->m_nOverlap*m_sizeDPI.cx/24.5;//Pixels
	const int colCount = cxView<=cxPage ? 1 : (cxView-nOverlap)%(cxPage-nOverlap)==0 ? (cxView-nOverlap)/(cxPage-nOverlap) : (cxView-nOverlap)/(cxPage-nOverlap)+1;
	int rowCount = cyView<=cyPage ? 1 : (cyView-nOverlap)%(cyPage-nOverlap)==0 ? (cyView-nOverlap)/(cyPage-nOverlap) : (cyView-nOverlap)/(cyPage-nOverlap)+1;
	const long col = (pInfo->m_nCurPage-1)%colCount;
	const long row = (pInfo->m_nCurPage-1)/colCount;

	m_ptViewPos.x = (mapPrint.left-mapTotal.left)*m_xFactorMapToView;
	m_ptViewPos.y = (mapTotal.bottom-mapPrint.bottom)*m_yFactorMapToView;

	CRect vewTotal = CRect(CPoint(0, 0), m_sizeView);
	CRect vewPrint = CRect(m_ptViewPos, CSize(cxView, cyView));

	m_ptViewPos.x += col*(cxPage-nOverlap);
	m_ptViewPos.y += row*(cyPage-nOverlap);

	CRect vewPage(m_ptViewPos, CSize(cxPage, cyPage));
	vewPage.IntersectRect(vewPage, vewPrint);
	vewPage.IntersectRect(vewPage, vewTotal);

	int xOffset = m_marginLeft*m_sizeDPI.cx;
	int yOffset = m_marginTop*m_sizeDPI.cy;
	if(m_bHCenter==TRUE&&vewPage.Width()<cxPage)
	{
		xOffset += (cxPage-vewPage.Width())/2;
	}
	if(m_bVCenter==TRUE&&vewPage.Height()<cyPage)
	{
		yOffset += (cyPage-vewPage.Height())/2;
	}
	const CRect clip = CRect(CPoint(xOffset, yOffset), vewPage.Size());
	int ret1 = pDC->SelectClipRgn(nullptr);
	int ret2 = pDC->IntersectClipRect(&clip);
	pInfo->m_rectDraw = clip;

	m_ptViewPos.x -= xOffset;
	m_ptViewPos.y -= yOffset;
}
