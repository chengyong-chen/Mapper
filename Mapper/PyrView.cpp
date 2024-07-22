#include "stdafx.h"
#include "PyrView.h"
#include "PyrDoc.h"

#include "Global.h"

#include "../Tool/Tool.h"
#include "../Tool/Global.h"
#include "../Tool/PanTool.h"

#include "../Projection/Mapinfo.h"
#include "../Projection/Projection1.h"

extern __declspec(dllimport) CTool*      pTool;
extern __declspec(dllimport) CPanTool    panTool;

IMPLEMENT_DYNCREATE(CPyrView, CGisView)

BEGIN_MESSAGE_MAP(CPyrView, CGisView)
	//{{AFX_MSG_MAP(CPyrView)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ZOOMIN,    OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT,   OnZoomOut)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPyrView, CGisView)	
	ON_EVENT(CPyrView, AFX_IDW_PANE_FIRST-10, 150, OnFSCommond, VTS_BSTR VTS_BSTR)
	ON_EVENT(CPyrView, AFX_IDW_PANE_FIRST-10, 197, OnFlashCall, VTS_BSTR) 
END_EVENTSINK_MAP()

CPyrView::CPyrView()
{
}

CPyrView::~CPyrView()
{
}


#ifdef _DEBUG
void CPyrView::AssertValid() const
{
	CGisView::AssertValid();
}

void CPyrView::Dump(CDumpContext& dc) const
{
	CGisView::Dump(dc);
}
#endif //_DEBUG

void CPyrView::OnInitialUpdate()
{
	CGisView::OnInitialUpdate();	
	
	if(m_GoogleMap.Create(nullptr,nullptr,WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_EX_NOPARENTNOTIFY,CRect(0,0,0,0),this->GetParent(),AFX_IDW_PANE_FIRST-10,nullptr) == TRUE)
	{
		m_GoogleMap.LoadMovie(0,"D:\\My Flexs\\GoogleMap\\GoogleMap\\bin-release\\GoogleMap.swf");
		m_GoogleMap.Play();
	}

	this->ModifyStyleEx(0,WS_EX_TRANSPARENT);
}

void CPyrView::OnSize(UINT nType, int cx, int cy) 
{
	CGisView::OnSize(nType, cx, cy);

	if(m_GoogleMap.m_hWnd != nullptr)
	{
		m_GoogleMap.MoveWindow(2, 2, cx, cy);
	}
	this->BringWindowToTop();
	this->SetScrollSizes(MM_TEXT, CSize(cx,cy),CSize(0,0),CSize(0,0));
}

BOOL CPyrView::PreTranslateMessage(MSG* pMsg) 
{
	if(pTool == &panTool)
	{
		switch(pMsg->message)
		{
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_LBUTTONDBLCLK:
			case WM_SETCURSOR:
				{
					return  m_GoogleMap.SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
				}
		}
	}

	return CGisView::PreTranslateMessage(pMsg);
}

BOOL CPyrView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(pTool == &panTool)
		return TRUE;

	return CGisView::OnSetCursor(pWnd, nHitTest, message);	
}

BOOL CPyrView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}


LONG CPyrView::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
	if(m_GoogleMap.m_hWnd != nullptr)
	{
		CRect zoomRect = *(CRect*)WPARAM;
		DocToClient(zoomRect);
		CPoint  cliPoint = zoomRect.CenterPoint() ;
	
		if(zoomRect.Width() < 5 || zoomRect.Height() < 5)
		{
			CString strParam; 
			strParam.Format("<number>2</number><number>%d</number><number>%d</number>",cliPoint.x,cliPoint.y);
			CString strXml = MakeInvokeXml("ZoomMap", strParam);
			m_GoogleMap.CallFunction(strXml);
		}
		else
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			cliPoint = clientRect.CenterPoint() ;
			
			float xRrato = (float) clientRect.Width() /zoomRect.Width();
			float yRatio = (float) clientRect.Height()/zoomRect.Height();
			float fRatio = min(xRrato, yRatio);
			
			CString strParam; 
			strParam.Format("<number>%g</number><number>%d</number><number>%d</number>",fRatio,cliPoint.x,cliPoint.y);
			CString strXml = MakeInvokeXml("ZoomMap", strParam);
			m_GoogleMap.CallFunction(strXml);
		}
		return 0L;
	}

	return CGisView::OnZoomIn(WPARAM,LPARAM);
}


LONG CPyrView::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	if(m_GoogleMap.m_hWnd != nullptr)
	{
		CRect zoomRect = *(CRect*)WPARAM;
		DocToClient(zoomRect);
		CPoint cliPoint = zoomRect.CenterPoint() ;
	
		if(zoomRect.Width() < 5 || zoomRect.Height() < 5)
		{
			CString strParam; 
			strParam.Format("<number>0.5</number><number>%d</number><number>%d</number>",cliPoint.x,cliPoint.y);
			CString strXml = MakeInvokeXml("ZoomMap", strParam);
			m_GoogleMap.CallFunction(strXml); 
		}
		else
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			
			float xRatio = (float) zoomRect.Width() /clientRect.Width();
			float yRatio = (float) zoomRect.Height()/clientRect.Height();
			float fRatio = max(xRatio, yRatio);
			
			CString strParam; 
			strParam.Format("<number>%g</number><number>%d</number><number>%d</number>",fRatio,cliPoint.x,cliPoint.y);
			CString strXml = MakeInvokeXml("ZoomMap", strParam);
			m_GoogleMap.CallFunction(strXml);
		}

		return 0L;
	}

	return CGisView::OnZoomOut(WPARAM,LPARAM);
}

void CPyrView::OnFSCommond(LPCSTR commond, LPCSTR args)
{
	try
	{
		CString strCommand = commond;
		if(strCommand == "MapChanged")
		{
			CString str = args;
			
			int level;
			int wrap;
			double minX;
			double minY;
			double maxX;
			double maxY;
			
			_stscanf(str,"Level:%d;Wrap:%d;MinX:%lf;MinY:%lf;MaxX:%lf;MaxY:%lf;",&level,&wrap,&minX,&minY,&maxX,&maxY);

			CPyrDoc* pDoc = this->GetDocument();
			if(pDoc != nullptr)
			{
				CRect rect;
				this->GetClientRect(rect);

				if(wrap<rect.Width())
				{
					CString strParam; 
					strParam.Format("<number>%d</number>",level+1);
					CString strXml = MakeInvokeXml("CenterMap", strParam);
					m_GoogleMap.CallFunction(strXml);				
				}
				else
				{
					
					pDoc->m_Mapinfo.m_mapOrigin = CPointF(minX,minY);
					
					pDoc->m_Mapinfo.m_mapCanvas.cy = maxY-minY;
					pDoc->m_Mapinfo.m_mapCanvas.cx = 360+(maxX-minX) > 360 ? maxX-minX : 360+(maxX-minX);
				}
			
				m_doctoviewZoomX = rect.Width() /(pDoc->m_Mapinfo.m_mapCanvas.cx*pDoc->m_Mapinfo.m_pProjection->m_dilationMapToDoc);
				m_doctoviewZoomY = rect.Height()/(pDoc->m_Mapinfo.m_mapCanvas.cy*pDoc->m_Mapinfo.m_pProjection->m_dilationMapToDoc);

				CSize docCanvas = pDoc->m_Mapinfo.GetDocCanvas();
				CSize docOffset = pDoc->m_Mapinfo.GetDocOrigin();

				{
					double m11 = m_doctoviewZoomX;
					double m12 = 0;		
					double m21 = 0;
					double m22 =-m_doctoviewZoomY;
					double dx =                               - docOffset.cx*m11;
					double dy = docCanvas.cy*m_doctoviewZoomY - docOffset.cy*m22;

					matrixDoctoWin.SetElements(m11,0,0,m22,dx,dy);
				}

				{
					double m11 = 1.0f/m_doctoviewZoomX;
					double m12 = 0;
					double m21 = 0;
					double m22 =-1.0f/m_doctoviewZoomY;
					double dx =              +docOffset.cx;
					double dy =  docCanvas.cy+docOffset.cy;

					matrixWintoDoc.SetElements(m11,0,0,m22,dx,dy);
				}

				m_currentScale = pDoc->m_Mapinfo.GetScale(m_doctoviewZoomX,m_doctoviewZoomY,CPoint(docOffset.cx+docCanvas.cx/2,docOffset.cx+docCanvas.cx/2));
				pDoc->m_Mapinfo.m_scaleSource = m_currentScale;
				this->Invalidate();
			}

		}
		else if(true)
		{
		}
	}
	catch(CException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
}

void CPyrView::OnFlashCall(LPCTSTR request)  
{ 
 AfxMessageBox(request); 
} 

//void CPyrView::OnMouseMove(UINT nFlags, CPoint point)
//{
//	CGisView::OnMouseMove(nFlags,point);
//
//	try 
//	{ 
//		CString strParam; 
//		strParam.Format("<number>%d</number><number>%d</number>",point.x,point.y);
//		CString strXml = MakeInvokeXml("CliToGeo", strParam);
//		CString strOut = m_GoogleMap.CallFunction(strXml); 
//		double lat;
//		double log;
//		sscanf(strOut,_T("<string>Log:%lf;Lat:%lf;</string>"),&log,&lat);
//		AfxGetMainWnd()->SendMessage(WM_SETMOUSECOORDINATE, (UINT)&log, (UINT)&lat);	
//	} 
//	catch(COleDispatchException* ex) 
//	{ 
//		
//	} 
//}

CString CPyrView::MakeInvokeXml(CString strFunc,CString strParam)
{
	CString str;
	str.Format("<invoke name=\"%s\" returntype=\"xml\"><arguments>%s</arguments></invoke>",strFunc,strParam);
	return str;
}

