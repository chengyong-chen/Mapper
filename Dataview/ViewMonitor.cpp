#include "StdAfx.h"
#include "ViewMonitor.h"
#include "Datainfo.h"
#include "Background.h"

#include "../Projection/Projection1.h"
#include "../Utility/Scale.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewMonitor::CViewMonitor(CDatainfo& foreground)
	: CViewinfo(foreground)
{
	m_pBackground = nullptr;
	m_bShowGrid = FALSE;
}


CViewMonitor::~CViewMonitor(void)
{
	this->RemoveBackground();
}
const CViewMonitor& CViewMonitor::operator=(const CViewMonitor& source)
{
	CViewinfo::operator =(source);

	return *this;
}
CPointF CViewMonitor::GetViewCenter(CWnd* pWnd) const
{
	CRect rect;
	pWnd->GetClientRect(rect);
	CPoint cliCenter = rect.CenterPoint();
	CPointF mapCenter = CViewinfo::ClientToDataMap(cliCenter);
	return  m_datainfo.m_pProjection == nullptr ? mapCenter : m_datainfo.m_pProjection->MapToGeo(mapCenter);
}
void CViewMonitor::DrawCanvas1(Gdiplus::Graphics& g) const
{
	Gdiplus::Rect cliRect;
	cliRect.X = 0;
	cliRect.Y = 0;
	cliRect.Width = m_mapCanvas.cx*m_xFactorMapToView;
	cliRect.Height = m_mapCanvas.cy*m_yFactorMapToView;
	cliRect.Offset(-m_ptViewPos.x, -m_ptViewPos.y);
	const Gdiplus::SolidBrush brush(RGB(255, 255, 254));
	g.FillRectangle(&brush, cliRect);
}
void CViewMonitor::DrawCanvas2(Gdiplus::Graphics& g) const
{
	Gdiplus::Rect cliRect(0, 0, m_mapCanvas.cx*m_xFactorMapToView, m_mapCanvas.cy*m_yFactorMapToView);
	cliRect.Offset(-m_ptViewPos.x, -m_ptViewPos.y);
	const Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1);
	g.DrawRectangle(&pen, cliRect);
}
void CViewMonitor::DrawGrid(Gdiplus::Graphics& g, CRect& inRect) const
{
	if(CViewMonitor::m_bShowGrid == TRUE)
	{
		CRect cliRect;
		cliRect.left = 0;
		cliRect.top = 0;
		cliRect.right = m_mapCanvas.cx*m_xFactorMapToView;
		cliRect.bottom = m_mapCanvas.cy*m_yFactorMapToView;
		cliRect.OffsetRect(-m_ptViewPos.x, -m_ptViewPos.y);
		const Gdiplus::Rect cliBox = CViewMonitor::DocToClient<Gdiplus::Rect>(inRect);
		CRect rect(cliBox.GetLeft(), cliBox.GetTop(), cliBox.GetRight(), cliBox.GetBottom());
		rect.IntersectRect(rect, cliRect);
		const Gdiplus::Pen pen1(Gdiplus::Color(240, 240, 240), 0.1);
		const Gdiplus::Pen pen2(Gdiplus::Color(220, 220, 220), 0.1);
		for(int x = rect.left/6*6; x <= rect.right; x += 6)
		{
			if(x < cliBox.X)
				continue;
			if(x > cliBox.GetRight())
				continue;

			if(x % 30 == 0)
				g.DrawLine(&pen2, Gdiplus::Point(x, rect.top), Gdiplus::Point(x, rect.bottom));
			else
				g.DrawLine(&pen1, Gdiplus::Point(x, rect.top), Gdiplus::Point(x, rect.bottom));
		}
		for(int y = rect.top/6*6; y <= rect.bottom; y += 6)
		{
			if(y < cliBox.Y)
				continue;
			if(y > cliBox.GetBottom())
				continue;

			if(y % 30 == 0)
				g.DrawLine(&pen2, Gdiplus::Point(rect.left, y), Gdiplus::Point(rect.right, y));
			else
				g.DrawLine(&pen1, Gdiplus::Point(rect.left, y), Gdiplus::Point(rect.right, y));
		}
	}
}

void CViewMonitor::OnSized(CWnd* pHoster, int cx, int cy)
{
	if(cx > m_sizeView.cx)
		m_ptViewPos.x = (m_sizeView.cx - cx)/2;
	else if(m_ptViewPos.x < 0)
		m_ptViewPos.x = 0;
	else if(m_ptViewPos.x + cx > m_sizeView.cx)
		m_ptViewPos.x = m_sizeView.cx - cx;

	if(cy > m_sizeView.cy)
		m_ptViewPos.y = (m_sizeView.cy - cy)/2;
	else if(m_ptViewPos.y < 0)
		m_ptViewPos.y = 0;
	else if(m_ptViewPos.y + cy > m_sizeView.cy)
		m_ptViewPos.y = m_sizeView.cy - cy;

	if(pHoster != nullptr && m_pBackground != nullptr)
	{
		CRect rect;
		pHoster->GetClientRect(rect);
		pHoster->ClientToScreen(rect);
		m_pBackground->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
}

void CViewMonitor::MoveBy(CWnd* pWnd, int cx, int cy)
{
	if(m_pBackground != 0)
	{
		AfxGetApp()->BeginWaitCursor();
		m_pBackground->MoveBy(cx, cy);
		AfxGetApp()->EndWaitCursor();
	}
	else if(pWnd != nullptr)
	{

		CRect cliRect;
		pWnd->GetClientRect(cliRect);

		// adjust current x position
		int xNew = m_ptViewPos.x + cx;
		const int xMaximum = m_sizeView.cx - cliRect.Width();
		if(xMaximum <= 0)
			xNew = (m_sizeView.cx - cliRect.Width())/2;
		else if(xNew < 0)
			xNew = 0;
		else if(xNew > xMaximum)
			xNew = xMaximum;

		// adjust current x position
		int yNew = m_ptViewPos.y + cy;
		const int yMaximum = m_sizeView.cy - cliRect.Height();
		if(yMaximum <= 0)
			yNew = (m_sizeView.cy - cliRect.Height())/2;
		else if(yNew < 0)
			yNew = 0;
		else if(yNew > yMaximum)
			yNew = yMaximum;

		// did anything change?
		if(xNew != m_ptViewPos.x)
			m_ptViewPos.x = xNew;
		if(yNew != m_ptViewPos.y)
			m_ptViewPos.y = yNew;

		pWnd->Invalidate(FALSE);


	}
}
void CViewMonitor::SpinBy(CWnd* pWnd, double deltaLng, double deltaLat)
{
	if(m_clipper == nullptr)
		return;

	float alpha = 0;
	CPointF center = m_clipper->GetCenter();
	m_clipper->spin(deltaLng, deltaLat, alpha);
	if(pWnd != nullptr)
	{
		pWnd->Invalidate(FALSE);
	}
}
void CViewMonitor::LevelTo(CWnd* pWnd, const float& fLevel, const CPointF& mapPoint, bool force)
{
	if(m_pBackground != nullptr)
	{
		const CPointF geoCenter = m_datainfo.m_pProjection->MapToGeo(mapPoint);
		m_pBackground->LevelTo(fLevel, geoCenter);
	}
	else
		CViewinfo::LevelTo(pWnd, fLevel, mapPoint);
}

void CViewMonitor::ScaleTo(CWnd* pWnd, const float& fScale, CPointF mapPoint)
{
	if(m_pBackground != nullptr)
	{
		const CPointF geoCenter = m_datainfo.m_pProjection->MapToGeo(mapPoint);
		m_pBackground->ScaleTo(fScale, geoCenter);
	}
	else
		CViewinfo::ScaleTo(pWnd, fScale, mapPoint);
}

void CViewMonitor::RatioTo(CWnd* pWnd, float fRatio, const CPoint& cliPoint)
{
	CPoint oldFixedat = cliPoint+m_ptViewPos;
	CPoint newFixedat = CPoint(oldFixedat.x*fRatio/m_xFactorMapToView, oldFixedat.y*fRatio/m_yFactorMapToView);
	m_xFactorMapToView = fRatio;
	m_yFactorMapToView = fRatio;
	m_sizeView.cx = m_mapCanvas.cx*m_xFactorMapToView;
	m_sizeView.cy = m_mapCanvas.cy*m_yFactorMapToView;

	CViewinfo::Originate(pWnd, newFixedat, cliPoint);	
	
	if(m_pGeocentric != nullptr)
		m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_pGeocentric, m_xFactorMapToView, CPointF(m_mapOrigin.x+m_mapCanvas.cx/2, m_mapOrigin.y+m_mapCanvas.cy/2), m_sizeDPI);
	else if(m_datainfo.m_pProjection != nullptr)
		m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_datainfo.m_pProjection, m_xFactorMapToView, CPointF(m_mapOrigin.x+m_mapCanvas.cx/2, m_mapOrigin.y+m_mapCanvas.cy/2), m_sizeDPI);
	else
		m_scaleCurrent = m_sizeDPI.cx/(m_xFactorMapToView*m_sizeDPI.cx);

	m_levelCurrent = log(max(m_sizeView.cx, m_sizeView.cy)/256.f)/log(2.f);
}
void CViewMonitor::FixateAt(CWnd* pWnd, const CPointF& mapPoint, const CPoint& cliPoint)
{
	if(m_pBackground != nullptr)
	{
		const CPointF geoPoint = m_datainfo.m_pProjection->MapToGeo(mapPoint);
		AfxGetApp()->BeginWaitCursor();
		m_pBackground->JumpTo(geoPoint.x, geoPoint.y);
		AfxGetApp()->EndWaitCursor();
	}
	else if(pWnd != nullptr)
	{
		const Gdiplus::PointF viwPoint = this->DataMapToView(mapPoint.x, mapPoint.y);
		int xNew = viwPoint.X - cliPoint.x;
		int yNew = viwPoint.Y - cliPoint.y;

		CRect cliRect;
		pWnd->GetClientRect(cliRect);
		if(m_sizeView.cx <= cliRect.Width())
			xNew = (m_sizeView.cx - cliRect.Width())/2;
		else if(xNew + cliRect.Width() > m_sizeView.cx)
			xNew = m_sizeView.cx - cliRect.Width();
		else if(xNew < 0)
			xNew = 0;

		if(m_sizeView.cy <= cliRect.Height())
			yNew = (m_sizeView.cy - cliRect.Height())/2;
		else if(yNew + cliRect.Height() > m_sizeView.cy)
			yNew = m_sizeView.cy - cliRect.Height();
		else if(yNew < 0)
			yNew = 0;

		if(xNew != m_ptViewPos.x)
			m_ptViewPos.x = xNew;
		if(yNew != m_ptViewPos.y)
			m_ptViewPos.y = yNew;

		pWnd->Invalidate(FALSE);
	}
}

void CViewMonitor::ZoomBy(CWnd* pWnd, float fRatio, const CPoint& cliPoint)
{
	if(fRatio > 1.0f && m_scaleCurrent <= m_datainfo.m_scaleMaximum)
	{
	}
	else if(fRatio < 1.0f && m_scaleCurrent >= m_datainfo.m_scaleMinimum)
	{
	}
	else if(m_pBackground != 0)
	{
		fRatio = min(max(fRatio, m_scaleCurrent/m_datainfo.m_scaleMinimum), m_scaleCurrent/m_datainfo.m_scaleMaximum);
		AfxGetApp()->BeginWaitCursor();
		m_pBackground->ZoomBy(cliPoint, fRatio);
		AfxGetApp()->EndWaitCursor();
	}
	else if(m_scaleCurrent/fRatio < m_datainfo.m_scaleMaximum)
	{
		const CPointF mapPoint = this->ClientToDataMap(cliPoint);
		ScaleTo(pWnd, m_datainfo.m_scaleMaximum, mapPoint);
	}
	else if(m_scaleCurrent/fRatio > m_datainfo.m_scaleMinimum)
	{
		const CPointF mapPoint = this->ClientToDataMap(cliPoint);
		ScaleTo(pWnd, m_datainfo.m_scaleMinimum, mapPoint);
	}
	else
	{
		const CPointF mapPoint = this->ClientToDataMap(cliPoint);
		ScaleTo(pWnd, m_scaleCurrent/fRatio, mapPoint);
	}
}
void CViewMonitor::RatioBy(CWnd* pWnd, float fRatio, const CPoint& cliPoint)
{
	if(fRatio > 1.0f && m_scaleCurrent <= m_datainfo.m_scaleMaximum)
	{
	}
	else if(fRatio < 1.0f && m_scaleCurrent >= m_datainfo.m_scaleMinimum)
	{
	}
	else if(m_pBackground != 0)
	{
		fRatio = min(max(fRatio, m_scaleCurrent/m_datainfo.m_scaleMinimum), m_scaleCurrent/m_datainfo.m_scaleMaximum);
		AfxGetApp()->BeginWaitCursor();
		m_pBackground->ZoomBy(cliPoint, fRatio);
		AfxGetApp()->EndWaitCursor();
	}
	else if(m_scaleCurrent/fRatio < m_datainfo.m_scaleMaximum)
	{
		fRatio = m_scaleCurrent/m_datainfo.m_scaleMaximum;
		RatioTo(pWnd, fRatio*min(m_xFactorMapToView, m_yFactorMapToView), cliPoint);
	}
	else if(m_scaleCurrent/fRatio > m_datainfo.m_scaleMinimum)
	{
		fRatio = m_scaleCurrent/m_datainfo.m_scaleMinimum;
		RatioTo(pWnd, fRatio*min(m_xFactorMapToView, m_yFactorMapToView), cliPoint);
	}
	else
	{	
		RatioTo(pWnd, fRatio*min(m_xFactorMapToView, m_yFactorMapToView), cliPoint);
	}
}

void CViewMonitor::OnFSCommond(CWnd* pWnd, LPCSTR commond, LPCSTR args)
{
	if(m_pGeocentric == nullptr)
		return;
	if(m_pBackground == nullptr)
		return;

	try
	{
		CStringA strCommand = commond;
		if(commond == CStringA("ViewChanged"))
		{
			const CStringA str = args;
			double factorMapToView;
			sscanf(str, "VSX:%d;VSY:%d;VPX:%d;VPY:%d;MapToView:%lf", &m_sizeView.cx, &m_sizeView.cy, &m_ptViewPos.x, &m_ptViewPos.y, &factorMapToView);
			m_xFactorMapToView = factorMapToView;
			m_yFactorMapToView = factorMapToView;
			m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_pGeocentric, m_xFactorMapToView, m_datainfo.GetMapRect().CenterPoint(), 72);
			m_levelCurrent = log(max(m_sizeView.cx, m_sizeView.cy)/256.f)/log(2.f);

			pWnd->Invalidate(TRUE);
			pWnd->UpdateWindow();
		}
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
}

void CViewMonitor::RemoveBackground()
{
	if(m_pBackground != nullptr)
	{
		m_pGeocentric = m_pBackground->m_bufGeocentric;
		m_mapOrigin = m_pBackground->m_bufmapOrigin;
		m_mapCanvas = m_pBackground->m_bufmapCanvas;

		delete m_pBackground;
		m_pBackground = nullptr;
	}
}

void CViewMonitor::SetBackground(CBackground* pBackground)
{
	if(pBackground == m_pBackground)
		return;

	RemoveBackground();

	if(pBackground != nullptr)
	{
		pBackground->m_bufGeocentric = m_pGeocentric;
		pBackground->m_bufmapOrigin = m_mapOrigin;
		pBackground->m_bufmapCanvas = m_mapCanvas;

		m_pGeocentric = &pBackground->m_geocentric;
		m_mapOrigin = pBackground->m_mapOrigin;
		m_mapCanvas = pBackground->m_mapCanvas;

		m_pBackground = pBackground;
	}
}

void CViewMonitor::Refresh(CWnd* pWnd, const CPointF& geoCenter)
{
	float level = min(m_levelMaximum, max(m_levelCurrent, m_levelMinimum));	
	CPointF mapCenter = m_pGeocentric!=nullptr ? m_pGeocentric->GeoToMap(geoCenter) : (this->m_datainfo.m_pProjection!=nullptr ? this->m_datainfo.m_pProjection->GeoToMap(geoCenter) : geoCenter);
	if(m_pGeocentric!=nullptr)
	{
		CRectF rect =CViewinfo::TransformRange(m_pGeocentric);
		m_mapOrigin.x = rect.left;
		m_mapOrigin.y = rect.top;
		m_mapCanvas.cx = rect.right - rect.left;
		m_mapCanvas.cy = rect.bottom - rect.top;
	}
	CViewinfo::LevelTo(pWnd, level, mapCenter, true);
}
