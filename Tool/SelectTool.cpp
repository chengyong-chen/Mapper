#include "stdafx.h"
#include "Resource.h"

#include "Global.h"

#include "Tool.h"
#include "SelectTool.h"
#include "EditGeom.h"

#include "../Public/Function.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CSelectTool selectTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSelectTool::CSelectTool()
{
	m_actMode = ActionMode::Wandering;
	m_actReady = false;
	m_captureImage = nullptr;
	m_coveredImage = nullptr;
}

CSelectTool::~CSelectTool()
{
	Dispose();
}

void CSelectTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	time_t start;
	time(&start);

	if(m_pEditgeom!=nullptr&&m_pEditgeom->PickControl(pWnd, docPoint))
		m_actMode = ActionMode::ChangeCtrl;
	else if(pWnd->SendMessage(WM_PICKANCHOR, nFlags, (LONG)&docPoint))
		m_actMode = ActionMode::MoveAnchor;
	else if(pWnd->SendMessage(WM_PICKGEOM, nFlags, (LONG)&docPoint))
		m_actMode = ActionMode::MoveGeoms;
	else
		m_actMode = ActionMode::NetSelect;

	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	WaitForReadyParas* para = new WaitForReadyParas(pWnd, *this, viewinfo, start);
	CWinThread* pThread = AfxBeginThread(CSelectTool::ActOKProc, para, THREAD_PRIORITY_NORMAL);
}

void CSelectTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* DC)
{
	CClientDC dc(pWnd);

	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	const bool bMatch = ::GetKeyState(VK_SHIFT)<0;
	switch(m_actMode)
	{
		case ActionMode::ChangeCtrl:
			{
				m_pEditgeom->MoveContrl(&dc, c_docLast-c_docDown);
			}
			break;
		case ActionMode::MoveAnchor:
			{
				m_pEditgeom->MoveAnchor(pWnd, c_docLast-c_docDown, bMatch);
			}
			break;
		case ActionMode::MoveGeoms:
			{
				CPoint last = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(c_docDown, last, 8) : c_docLast;
				if(last!=c_docDown&&m_captureImage!=nullptr)
				{
				}
			}
			break;
		case ActionMode::NetSelect:
			{
				const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docDown.x, c_docDown.y));
				const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docLast.x, c_docLast.y));
				CRect rect = CRect(point1.X, point1.Y, point2.X, point2.Y);
				rect.NormalizeRect();

				dc.Rectangle(rect);
			}
			break;
		case ActionMode::Wandering:
			break;
		default:
			break;
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CSelectTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(m_actReady==false)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	else if(m_actMode==ActionMode::Wandering)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	else if((nFlags&MK_LBUTTON)!=MK_LBUTTON)// the mouse button is not down
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const bool bMatch = ::GetKeyState(VK_SHIFT)<0;
	switch(m_actMode)
	{
		case ActionMode::ChangeCtrl:
			{
				m_pEditgeom->MoveContrl(&dc, c_docLast-c_docDown);
				m_pEditgeom->MoveContrl(&dc, docPoint-c_docDown);
			}
			break;
		case ActionMode::MoveAnchor:
			{
				m_pEditgeom->MoveAnchor(pWnd, c_docLast-c_docDown, bMatch);
				m_pEditgeom->MoveAnchor(pWnd, docPoint-c_docDown, bMatch);
			}
			break;
		case ActionMode::MoveGeoms:
			{
				CRect client;
				pWnd->GetClientRect(client);
				CBitmap bitmap;
				if(bitmap.CreateCompatibleBitmap(&dc, client.Width(), client.Height())==TRUE)
				{
					CDC mdc;
					if(mdc.CreateCompatibleDC(&dc)==TRUE)
					{
						CBitmap* pOldBitmap = mdc.SelectObject(&bitmap);
						mdc.BitBlt(0, 0, client.Width(), client.Height(), &dc, 0, 0, SRCCOPY);
						const CPoint last = (nFlags&MK_SHIFT)!=0 ? RegulizePoint(c_docDown, c_docLast, 8) : c_docLast;
						if(last!=c_docDown&&m_coveredImage!=nullptr)
						{
							::DrawGDIHBitmapOnGDI(mdc, m_coveredOrigin, m_coveredImage);
							::DeleteObject(m_coveredImage);
							m_coveredImage = nullptr;
						}
						const CPoint local = (nFlags&MK_SHIFT)!=0 ? RegulizePoint(c_docDown, docPoint, 8) : docPoint;
						if(local!=c_docDown&&m_captureImage!=nullptr)
						{
							const Gdiplus::Point point1 = viewinfo.DocToClient <Gdiplus::Point>(local);
							const Gdiplus::Point point2 = viewinfo.DocToClient <Gdiplus::Point>(c_docDown);
							const CPoint origin = m_captureOrigin+CSize(point1.X-point2.X, point1.Y-point2.Y);
							CRect rect = CRect(origin, CSize(m_captureImage->GetWidth(), m_captureImage->GetHeight()));
							rect.IntersectRect(rect, client);
							m_coveredImage = CaptureScreen(mdc, rect);
							if(m_coveredImage!=nullptr)
							{
								m_coveredOrigin = rect.TopLeft();

								Gdiplus::Graphics g(mdc.m_hDC);
								Gdiplus::ImageAttributes imaatt;
								imaatt.SetColorKey(Gdiplus::Color(254, 254, 254), Gdiplus::Color(254, 254, 254), Gdiplus::ColorAdjustTypeDefault);
								g.DrawImage(m_captureImage, Gdiplus::Rect(origin.x, origin.y, m_captureImage->GetWidth(), m_captureImage->GetHeight()), 0, 0, m_captureImage->GetWidth(), m_captureImage->GetHeight(), Gdiplus::Unit::UnitPixel, &imaatt, nullptr, nullptr);
								g.ReleaseHDC(mdc.m_hDC);
							}
						}
						dc.BitBlt(0, 0, client.Width(), client.Height(), &mdc, 0, 0, SRCCOPY);
						mdc.SelectObject(pOldBitmap);
						mdc.DeleteDC();
					}
					bitmap.DeleteObject();
				}
			}
			break;
		case ActionMode::NetSelect:
			{
				{
					const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docDown.x, c_docDown.y));
					const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docLast.x, c_docLast.y));
					CRect rect = CRect(point1.X, point1.Y, point2.X, point2.Y);
					rect.NormalizeRect();

					dc.Rectangle(rect);
				}

				{
					const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docDown.x, c_docDown.y));
					const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(docPoint.x, docPoint.y));
					CRect rect = CRect(point1.X, point1.Y, point2.X, point2.Y);
					rect.NormalizeRect();

					dc.Rectangle(rect);
				}
			}
			break;
		default:
			break;
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CSelectTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(pWnd->GetCapture()!=pWnd||m_actReady==false)
	{
		m_actMode = ActionMode::Wandering;
		return CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	}

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const bool bMatch = ::GetKeyState(VK_SHIFT)<0;
	switch(m_actMode)
	{
		case ActionMode::ChangeCtrl:
			m_pEditgeom->ChangeContrl(pWnd, &dc, docPoint);
			break;
		case ActionMode::MoveAnchor:
			m_pEditgeom->MoveAnchor(pWnd, docPoint-c_docDown, bMatch);//hide the track lines
			m_pEditgeom->ChangeAnchor(pWnd, &dc, docPoint-c_docDown, bMatch);
			break;
		case ActionMode::MoveGeoms:
			{
				const CPoint local = bMatch==true ? RegulizePoint(c_docDown, docPoint, 8) : docPoint;
				if(local!=c_docDown)
				{
					//	if(m_coveredImage != nullptr) get rid of the last flash
					//		DrawGDIHBitmapOnGDI(dc,m_coveredOrigin,m_coveredImage);				
					const int cx = max((double)INT_MIN, min((double)INT_MAX, (double)local.x-c_docDown.x));
					const int cy = max((double)INT_MIN, min((double)INT_MAX, (double)local.y-c_docDown.y));
					CSize Δ(cx, cy);
					pWnd->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
				}
			}
			break;
		case ActionMode::NetSelect:
			{
				if(docPoint!=c_docDown)
				{
					{
						const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docDown.x, c_docDown.y));
						const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docLast.x, c_docLast.y));
						CRect rect = CRect(point1.X, point1.Y, point2.X, point2.Y);
						rect.NormalizeRect();

						dc.Rectangle(rect);
					}

					{
						CRect rect = CRect(c_docDown.x, c_docDown.y, c_docLast.x, c_docLast.y);
						rect.NormalizeRect();
						pWnd->SendMessage(WM_PICKBYRECT, nFlags, (LONG)&rect);
					}
				}
			}
			break;
		case ActionMode::Wandering:
			break;
		default:
			break;
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);

	m_actMode = ActionMode::Wandering;
	m_actReady = false;
	Dispose();

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CSelectTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	if((nFlags&MK_SHIFT)==0)
	{
		CGeom* geom = (CGeom*)pWnd->SendMessage(WM_PICKGEOM, nFlags, (LONG)&docPoint);
		if(geom!=nullptr)
		{
			geom->Open(pWnd);
		}
	}

	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
}

void CSelectTool::OnShiftDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags)
{
	CSelectTool::OnMouseMove(pWnd, viewinfo, nFlags, c_cliLast, c_docLast);
}

void CSelectTool::OnShiftUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags)
{
	CSelectTool::OnMouseMove(pWnd, viewinfo, nFlags, c_cliLast, c_docLast);
}

bool CSelectTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_SELECT);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}

void CSelectTool::Dispose()
{
	::delete m_captureImage;
	m_captureImage = nullptr;
	::DeleteObject(m_coveredImage);
	m_coveredImage = nullptr;
}

UINT CSelectTool::ActOKProc(LPVOID lpVoid)
{
	const WaitForReadyParas* para = (WaitForReadyParas*)lpVoid;
	para->selecttool.m_actReady = false;
	if(para->selecttool.m_coveredImage!=nullptr)
	{
		::DeleteObject(para->selecttool.m_coveredImage);
		para->selecttool.m_coveredImage = nullptr;
	}
	::delete para->selecttool.m_captureImage;
	para->selecttool.m_captureImage = nullptr;

	while(para->selecttool.m_actReady==false)
	{
		if((GetAsyncKeyState(VK_LBUTTON)&0X8000)!=0X8000) // VK_LBUTTON is not down
			break;

		time_t now;
		time(&now);
		bool doit = difftime(now, para->downtime)>1;
		if(doit==false)
		{
			CClientDC dc(para->pWnd);
			const Gdiplus::Point point1 = para->viewinfo.DocToClient<Gdiplus::Point>(CPoint(para->selecttool.c_docLast.x, para->selecttool.c_docLast.y));
			const Gdiplus::Point point2 = para->viewinfo.DocToClient<Gdiplus::Point>(CPoint(para->selecttool.c_docDown.x, para->selecttool.c_docDown.y));
			doit |= abs(point1.X-point2.X)>10||abs(point1.Y-point2.Y)>10;
		}
		if(doit==true)
		{
			para->selecttool.SetCursor(nullptr, para->viewinfo);
			CSelectTool::c_docLast = CSelectTool::c_docDown;

			if(para->selecttool.m_actMode==ActionMode::MoveGeoms)
			{
				Gdiplus::Rect* cliActive = (Gdiplus::Rect*)(para->pWnd->SendMessage(WM_GETACTIVEAREA, 0, 0));
				if(cliActive!=nullptr)
				{
					const Gdiplus::Point cliOrigin = para->viewinfo.DocToClient<Gdiplus::Point>(para->selecttool.c_docDown);
					CPoint scrOrigin(cliOrigin.X, cliOrigin.Y);
					para->pWnd->ClientToScreen(&scrOrigin); //the coordinate is on the whole desktop  
					const int cx = GetSystemMetrics(SM_CXMAXTRACK);//this is for multiple monitors
					const int cy = GetSystemMetrics(SM_CYMAXTRACK);//this is for multiple monitors
					Gdiplus::Rect range(scrOrigin.x-cx, scrOrigin.y-cy, cx*2, cy*2);
					if(cliActive->Intersect(range)==TRUE)
					{
						para->selecttool.m_captureOrigin = CPoint(cliActive->X, cliActive->Y);
						para->selecttool.m_captureImage = (Gdiplus::Bitmap*)(para->pWnd->SendMessage(WM_CAPTUREIMAGE, (UINT)cliActive, 0));
					}
				}
			}

			para->selecttool.m_actReady = true;
		}
	}
	delete para;
	return 1;
}
