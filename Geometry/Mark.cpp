#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Resource.h"
#include "Global.h"

#include "Geom.h"
#include "Poly.h"
#include "PRect.h"
#include "Mark.h"
#include "Text.h"
#include "Tag.h"
#include "../Database/ODBCRecordset.h"
#include "../DataView/viewinfo.h"
#include "../Public/Enumeration.h"
#include "../Public/BinaryStream.h"
#include "../Style/Line.h"
#include "../Style/FillCompact.h"
#include "../Style/Spot.h"
#include "../Style/SpotCtrl.h"
#include "../Style/Hint.h"
#include "../Style/Psboard.h"

#include "../Rectify/Tin.h"

#include "../Mapper/Global.h"
#include "../Pbf/writer.hpp"
#include "../Utility/monochrome.hpp"
#include "../Utility/Math.hpp"

#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ANCHOR PrioritizedAnchors[];
__declspec(dllexport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

IMPLEMENT_SERIAL(CMark, CGeom, 0)

CMark::CMark()
	: CGeom()
{
	m_bGroup = false;
	m_Origin = CPoint(0, 0);
	m_pSpot = nullptr;

	m_bType = 8;
}

CMark::CMark(const CPoint& center)
	: CGeom(CRect(center.x-1, center.y-1, center.x+1, center.y+1), nullptr, nullptr)
{
	ASSERT_VALID(this);

	m_bGroup = false;
	m_Origin.x = center.x;
	m_Origin.y = center.y;
	m_pSpot = nullptr;

	m_bType = 8;
}

CMark::~CMark()
{
	delete m_pSpot;
}
void CMark::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CGeom::Sha1(sha1);

	sha1.process_bytes(&m_Origin.x, sizeof(LONG));
	sha1.process_bytes(&m_Origin.y, sizeof(LONG));
	if(m_pIMatrix2!=nullptr)
	{
		sha1.process_bytes(&m_pIMatrix2->m11, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m12, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m21, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m22, sizeof(double));
	}
}
void CMark::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1);

	sha1.process_bytes(&m_Origin.x-offset.cx, sizeof(LONG));
	sha1.process_bytes(&m_Origin.y-offset.cy, sizeof(LONG));
	if(m_pIMatrix2!=nullptr)
	{
		sha1.process_bytes(&m_pIMatrix2->m11, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m12, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m21, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m22, sizeof(double));
	}
}
void CMark::Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const
{
	if(m_pSpot!=nullptr)
	{
		m_pSpot->Sha1(sha1);
	}
	else if(pSpot!=nullptr)
	{
		pSpot->Sha1(sha1);
	}
}
BOOL CMark::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CMark))==FALSE)
		return FALSE;
	else if(m_Origin!=((CMark&)geom).m_Origin)
		return FALSE;
	else if(m_pIMatrix2==nullptr ^ ((CMark&)geom).m_pIMatrix2==nullptr)
		return FALSE;
	else if(m_pIMatrix2==nullptr && ((CMark&)geom).m_pIMatrix2==nullptr)
		return TRUE;
	else if(*m_pIMatrix2 != *(((CMark&)geom).m_pIMatrix2))
		return FALSE;
	else
		return TRUE;
}

void CMark::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_Origin);

	CClientDC dc(pWnd);
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 0));
	CPen* oldpen = dc.SelectObject(&pen);
	CBrush* oldbrush = (CBrush*)dc.SelectObject(&brush);
	const int radius = (nTimes%5)*2;
	if(nTimes%5==0)
	{
		CPen* oldpen1 = (CPen*)dc.SelectStockObject(NULL_PEN);
		CBrush* oldbrush1 = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);

		dc.Ellipse(CRect(point.X-10, point.Y-10, point.X+10, point.Y+10));

		dc.SelectObject(oldpen1);
		dc.SelectObject(oldbrush1);
	}

	CRect rect = CRect(point.X, point.Y, point.X, point.Y);
	rect.InflateRect(radius, radius);
	dc.Ellipse(rect);

	dc.SelectObject(oldpen);
	dc.SelectObject(oldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}
Gdiplus::PointF CMark::GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const
{
	hAlign = HALIGN::HA_LEFT;
	vAlign = VALIGN::VA_CENTER;
	Gdiplus::RectF rect = viewinfo.DocToClient<Gdiplus::RectF>(m_Rect);
	rect.Inflate(5, 5);
	return CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(rect, m_pTag != nullptr ? m_pTag->GetAnchor() : ANCHOR_6);
}
void CMark::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : (CSpot*)context.pSpot;
	if(pSpot==nullptr)
		return;

	Gdiplus::PointF cliPoint;
	if(viewinfo.DocToClient_IfVisible(m_Origin, cliPoint))
	{
		g.TranslateTransform(cliPoint.X, cliPoint.Y);
		pSpot->Draw(g, library, 0, 1, context.ratioMark, viewinfo.m_sizeDPI);
		g.TranslateTransform(-cliPoint.X, -cliPoint.Y);
	}
}
void CMark::Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(m_levelObj>=viewinfo.m_levelCurrent)
		return;
	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : (CSpot*)context.pSpot;
	if(pSpot==nullptr)
		return;
	std::tuple<int, int, HBITMAP, bool> cache = pSpot->GetBitmap2(library, 0, 1, context.ratioMark, viewinfo.m_sizeDPI);
	if(std::get<2>(cache)==nullptr)
		return;

	Gdiplus::PointF cliPoint;
	if(viewinfo.DocToClient_IfVisible(m_Origin, cliPoint))
	{
		HDC hdc = g.GetHDC();
		const HBITMAP hBitmap1 = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
		g.ReleaseHDC(hdc);

		HBITMAP hBitmap2 = std::get<2>(cache);
		int left = cliPoint.X-std::get<0>(cache);
		int top = cliPoint.Y-std::get<1>(cache);

		g.TranslateTransform(left, top);

		Gdiplus::ImageAttributes attr;
		attr.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(0, 0, 0), Gdiplus::ColorAdjustTypeBitmap);
		attr.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(0, 0, 0), Gdiplus::ColorAdjustTypeBitmap);

		BITMAP bitmap2;
		::GetObject(hBitmap2, sizeof(BITMAP), &bitmap2);

		Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP(hBitmap2, nullptr);
		g.DrawImage(bmp, Gdiplus::Rect(0, 0, bitmap2.bmWidth, bitmap2.bmHeight), 0, 0, bitmap2.bmWidth, bitmap2.bmHeight, Gdiplus::UnitPixel, &attr);
		delete bmp;

		g.TranslateTransform(-left, -top);
			
		m_cliMark = Gdiplus::RectF(left, top, bitmap2.bmWidth, bitmap2.bmHeight);
	}
	if(std::get<3>(cache)==false)
	{
		::DeleteObject(std::get<2>(cache));
	}
}
void CMark::Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const
{
	if(m_levelObj<viewinfo.m_levelCurrent)
		return;
	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : (CSpot*)context.pSpot;
	if(pSpot==nullptr)
		return;
	std::tuple<int, int, HBITMAP, bool> cache = pSpot->GetBitmap2(library, 0, 1, context.ratioMark, viewinfo.m_sizeDPI);
	if(std::get<2>(cache) == nullptr)
		return;
	
	Gdiplus::PointF cliPoint;
	if(viewinfo.DocToClient_IfVisible(m_Origin, cliPoint))
	{
		HDC hdc = g.GetHDC();
		const HBITMAP hBitmap1 = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
		g.ReleaseHDC(hdc);

		HBITMAP hBitmap2 = std::get<2>(cache);
		int left = cliPoint.X-std::get<0>(cache);
		int top = cliPoint.Y-std::get<1>(cache);
		if(monochrome::percent1(hBitmap1, left, top, hBitmap2)<=tolerance)
		{
			Gdiplus::ImageAttributes attr;
			attr.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(0, 0, 0), Gdiplus::ColorAdjustTypeBitmap);
			attr.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(0, 0, 0), Gdiplus::ColorAdjustTypeBitmap);

			BITMAP bitmap2;
			::GetObject(hBitmap2, sizeof(BITMAP), &bitmap2);

			Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP(hBitmap2, nullptr);
			g.DrawImage(bmp, Gdiplus::Rect(left, top, bitmap2.bmWidth, bitmap2.bmHeight), 0, 0, bitmap2.bmWidth, bitmap2.bmHeight, Gdiplus::UnitPixel, &attr);
			delete bmp;

			m_cliMark = Gdiplus::RectF(left, top, bitmap2.bmWidth, bitmap2.bmHeight);
			m_levelObj = viewinfo.m_levelCurrent;
		}
	}
	if(std::get<3>(cache) == false)
	{
		::DeleteObject(std::get<2>(cache));
	}
}

void CMark::PositeTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, bool reset) const
{
	if(m_levelObj>viewinfo.m_levelCurrent)
		return;
	if(m_strName.IsEmpty()==TRUE)
		return;
	if(reset == false &&m_levelTag<viewinfo.m_levelCurrent && m_bTag != 0X00)
		return;

	CHint* pHint = m_pHint!=nullptr ? m_pHint : context.pHint;
	if(pHint==nullptr)
		return;
	if(m_strName==_T("Nantong")||m_strName==_T("Changzhou"))
	{
		int i = 0;
	}
	HDC hdc = g.GetHDC();
	HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
	g.ReleaseHDC(hdc);
	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	const Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, m_strName, 0);

	const bool bHasEnglish = true;
	unsigned int total = textSize.Width*textSize.Height;
	unsigned int minOne1 = total;
	BYTE Align = 0;
	ANCHOR Anchor = ANCHOR::ANCHOR_0;
	for(int i = 0; i<8; i++)
	{
		ANCHOR anchor = PrioritizedAnchors[i];
		HALIGN hAlign;
		VALIGN vAlign;
		CTag::GetDefaultAlign(anchor, hAlign, vAlign);
		const Gdiplus::RectF tagRect = ::GetTagRect(m_cliMark, textSize, anchor, hAlign, vAlign);
		int one1 = monochrome::count1(bitmap, (int)tagRect.GetLeft(), (int)tagRect.GetTop(), (int)tagRect.Width, (int)tagRect.Height);
		if(one1==0)
		{
			Anchor = anchor;
			minOne1 = 0;
			break;
		}
		else if(one1<minOne1)
		{
			Anchor = anchor;
			minOne1 = one1;
		}
	}
	float tolerance = (m_bTag != 0X00 && reset) ? 1.0 : 0.1;
	if(minOne1<(total*tolerance))
	{
		HALIGN hAlign;
		VALIGN vAlign;
		CTag::GetDefaultAlign(Anchor, hAlign, vAlign);
		m_bTag = 0X00;
		m_bTag |= hAlign;
		m_bTag |= (vAlign<<2);
		m_bTag |= (Anchor<<4);
		CMark::FillTag(g, viewinfo, pHint, m_bTag);

		m_levelTag = min(m_levelTag, viewinfo.m_levelCurrent);
	}
}
void CMark::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const BYTE& bTag) const
{
	if(m_strName.IsEmpty()==TRUE)
		return;
	pHint = m_pHint!=nullptr ? m_pHint : pHint;
	if(pHint==nullptr)
		return;

	ANCHOR anchor = (ANCHOR)(bTag>>4);
	HALIGN hAlign = (HALIGN)(bTag&0B00000011);
	VALIGN vAlign = (VALIGN)((bTag&0B00001100)>>2);
	Gdiplus::PointF point = CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(m_cliMark, anchor);
	pHint->DrawString(g, viewinfo, m_strName, point, 0, hAlign, vAlign);
}
void CMark::FillTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const BYTE& bTag) const
{
	if(m_strName.IsEmpty()==TRUE)
		return;
	pHint = m_pHint!=nullptr ? m_pHint : pHint;
	if(pHint==nullptr)
		return;

	ANCHOR anchor = (ANCHOR)(bTag>>4);
	HALIGN hAlign = (HALIGN)(bTag&0B00000011);
	VALIGN vAlign = (VALIGN)((bTag&0B00001100)>>2);
	const Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, m_strName, 0);
	const Gdiplus::RectF tagRect = ::GetTagRect(m_cliMark, textSize, anchor, hAlign, vAlign);
	Gdiplus::SolidBrush brush(Gdiplus::Color::White);
	g.FillRectangle(&brush, tagRect);
}
void CMark::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const
{
	if(m_strName.IsEmpty()==TRUE)
		return;
	pHint = m_pHint!=nullptr ? m_pHint : pHint;
	if(pHint==nullptr)
		return;

	if(m_bTag!=0X00)
	{
		CMark::DrawTag(g, viewinfo, pHint, m_bTag);
	}
	else
	{
		//, HALIGN::HA_LEFT, VALIGN::VA_CENTER
		const Gdiplus::SizeF textSize = pHint->MeasureText<Gdiplus::SizeF>(g, viewinfo, m_strName, 0);
		const Gdiplus::RectF markRect = viewinfo.DocToClient<Gdiplus::RectF>(m_Rect);
		Gdiplus::RectF maxRect = markRect;
		const bool bHasEnglish = true;//used to use lucene to detected, removed, need to use a new way
		if(bHasEnglish==true)
		{
			maxRect.Inflate(textSize.Width, textSize.Height);
		}
		else
		{
			const Gdiplus::REAL ex = max(textSize.Width, textSize.Height);
			maxRect.Inflate(ex, ex);
		}

		CArray<Gdiplus::RectF, Gdiplus::RectF&> rectarray;
		const int count = d_oRectArray.GetSize();
		for(int index = 0; index<count; index++)
		{
			Gdiplus::RectF rect2 = d_oRectArray.GetAt(index);
			if(rect2.IntersectsWith(maxRect)==TRUE)
				rectarray.Add(rect2);
		}

		double minOne1 = 9999999999999999.0f;
		BYTE Align = 0;
		ANCHOR Anchor = ANCHOR::ANCHOR_0;
		for(int i = 0; i<8; i++)
		{
			ANCHOR anchor = PrioritizedAnchors[i];
			HALIGN hAlign;
			VALIGN vAlign;
			CTag::GetDefaultAlign(anchor, hAlign, vAlign);
			Gdiplus::RectF rect = ::GetTagRect(markRect, textSize, anchor, hAlign, vAlign);
			const int count = rectarray.GetSize();
			double area = 0.0f;
			for(int j = 0; j<count; j++)
			{
				Gdiplus::RectF rect2 = rectarray.GetAt(j);
				rect2.Intersect(rect);
				if(rect2.IsEmptyArea()==FALSE)
				{
					area += (double)rect2.Width*(double)rect2.Height;
				}
			}

			if(area==0.0f)
			{
				Anchor = anchor;
				minOne1 = 0;
				break;
			}
			else if(area<minOne1)
			{
				minOne1 = area;
			}
		}
		rectarray.RemoveAll();

		if(minOne1<(pHint->m_fSize*pHint->m_fSize))
		{
			HALIGN hAlign;
			VALIGN vAlign;
			CTag::GetDefaultAlign(Anchor, hAlign, vAlign);
			Gdiplus::PointF point = CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(markRect, Anchor);
			pHint->DrawString(g, viewinfo, m_strName, point, 0, hAlign, vAlign);
		}
	}
}
void CMark::DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_Origin);
	point.X -= 16;
	point.Y -= 16;
	Gdiplus::Bitmap* hBitmap = Gdiplus::Bitmap::FromResource(hInst, MAKEINTRESOURCE(IDB_PIN));
	if(hBitmap!=nullptr)
	{
		Gdiplus::ImageAttributes imaatt;
		imaatt.SetColorKey(Gdiplus::Color(255, 0, 255), Gdiplus::Color(255, 0, 255), Gdiplus::ColorAdjustTypeDefault);
		g.DrawImage(hBitmap, Gdiplus::Rect(point.X, point.Y, 32, 32), 0, 0, 32, 32, Gdiplus::UnitPixel, &imaatt);;
		delete hBitmap;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CMark::SetSpot(CWnd* pWnd, CLibrary& library, const CViewinfo& viewinfo)
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CSpotCtrl dlg(nullptr, true, library, m_pSpot);
	if(dlg.DoModal()==IDOK)
	{
		if(m_pSpot!=nullptr)
		{
			m_pSpot->Decrease(library);
			delete m_pSpot;
			m_pSpot = nullptr;
		}
		m_pSpot = dlg.d_pSpot;
		dlg.d_pSpot = nullptr;

		Invalidate(pWnd, viewinfo, 3);
		CalCorner(m_pSpot, viewinfo.m_datainfo, library);
		Invalidate(pWnd, viewinfo, 3);

		pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CMark::CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* pSpot, const CType* m_pType) const
{
	spots.CountIn(m_pSpot!=nullptr ? m_pSpot : pSpot);
}

bool CMark::HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const
{
	pSpot2 = m_pSpot!=nullptr ? m_pSpot : pSpot2;

	if(activeall==ACTIVEALL::Spot)
	{
		if(pSpot1==nullptr)
			return false;
		else if(pSpot2==nullptr)
			return false;
		else if(pSpot1==pSpot2)
			return true;
		else
			return (*pSpot1==*pSpot2);
	}

	return false;
}

bool CMark::IsDissident() const
{
	if(m_pSpot!=nullptr)
		return true;

	return false;
}

void CMark::Regress(CWnd* pWnd, CLibrary& library)
{
	CGeom::Regress(pWnd, library);

	if(m_pSpot!=nullptr)
	{
		m_pSpot->Decrease(library);
		delete m_pSpot;
		m_pSpot = nullptr;
	}
}

bool CMark::CreateName(CWnd* pWnd, const float& fontSize, const  float& zoomPointToDoc, CObList& oblist)
{
	if(m_strName.IsEmpty()==FALSE)
	{
		const CPoint point = m_Rect.BottomRight();

		CText* text = new CText(point);
		text->m_strName = m_strName;
		text->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
		oblist.AddTail(text);
		return true;
	}

	return false;
}
void CMark::Move(const int& dx, const int& dy)
{
	m_Origin.x += dx;
	m_Origin.y += dy;

	CGeom::Move(dx, dy);
}
void CMark::Move(const CSize& Δ)
{
	m_Origin.x += Δ.cx;
	m_Origin.y += Δ.cy;

	CGeom::Move(Δ);
}

void CMark::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE)
	{
		CMark* pMark = (CMark*)pGeom;

		pMark->m_Origin = m_Origin;
		if(m_pIMatrix2!=nullptr)
		{
			delete pMark->m_pIMatrix2;
			pMark->m_pIMatrix2 = new IMatrix2();
			pMark->m_pIMatrix2->m11 = m_pIMatrix2->m11;
			pMark->m_pIMatrix2->m12 = m_pIMatrix2->m12;
			pMark->m_pIMatrix2->m21 = m_pIMatrix2->m21;
			pMark->m_pIMatrix2->m22 = m_pIMatrix2->m22;
		}
		delete pMark->m_pSpot;
		if(m_pSpot!=nullptr)
			pMark->m_pSpot = m_pSpot->Clone();

		pMark->m_levelObj = m_levelObj;
		pMark->m_levelTag = m_levelTag;
		pMark->m_bTag = m_bTag;
	}
}

void CMark::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE)
	{
		CMark* pMark = (CMark*)pGeom;

		Swapclass<CPoint>(pMark->m_Origin, m_Origin);
		Swapclass<CSpot*>(pMark->m_pSpot, m_pSpot);
	}

	CGeom::Swap(pGeom);
}

void CMark::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	const CPoint oldCenter = m_Origin;
	const long x = round(m_Origin.x*m11+m_Origin.y*m21+m31);
	const long y = round(m_Origin.x*m12+m_Origin.y*m22+m32);
	m_Origin.x = x;
	m_Origin.y = y;

	CGeom::Transform(pDC, viewinfo, m11, m21, m31, m12, m22, m32);

	m_Origin = oldCenter;
}

void CMark::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	const long x = m_Origin.x*m11+m_Origin.y*m21+m31;
	const long y = m_Origin.x*m12+m_Origin.y*m22+m32;

	CGeom::Move(CSize(x-m_Origin.x, y-m_Origin.y));

	m_Origin.x = x;
	m_Origin.y = y;
}

void CMark::Transform(const CViewinfo& viewinfo)
{
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_Origin);
	m_Origin.x = point.X;
	m_Origin.y = point.Y;

	CGeom::Transform(viewinfo);
}
void CMark::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	const CPoint center = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_Origin);

	CSize Δ;
	Δ.cx = center.x - m_Origin.x;
	Δ.cy = center.y - m_Origin.y;

	m_Origin.x = center.x;
	m_Origin.y = center.y;
	CGeom::Move(Δ);
}

CGeom* CMark::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	const CPoint center = CDatainfo::Project(fMapinfo, tMapinfo, m_Origin);

	CSize Δ;
	Δ.cx = center.x-m_Origin.x;
	Δ.cy = center.y-m_Origin.y;

	m_Origin.x = center.x;
	m_Origin.y = center.y;
	CGeom::Move(Δ);
	return nullptr;
}

void CMark::Rectify(CTin& tin)
{
	const CPoint center = tin.Rectify(m_Origin);

	CSize Δ;
	Δ.cx = center.x-m_Origin.x;
	Δ.cy = center.y-m_Origin.y;

	m_Origin.x = center.x;
	m_Origin.y = center.y;
	CGeom::Move(Δ);
}

void CMark::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		if(m_pIMatrix2!=nullptr)
		{
			ar<<false;
			ar<<m_pIMatrix2->m11;
			ar<<m_pIMatrix2->m12;
			ar<<m_pIMatrix2->m21;
			ar<<m_pIMatrix2->m22;
		}
		else
		{
			ar<<true;
		}
		ar<<m_Origin.x;
		ar<<m_Origin.y;
		const BYTE typeSpot = m_pSpot==nullptr ? 0XFF : m_pSpot->Gettype();
		ar<<typeSpot;
	}
	else
	{
		bool bRegular;
		ar >> bRegular;
		if(bRegular==false)
		{
			m_pIMatrix2 = new IMatrix2();
			if(dwVersion<=2)
			{
				CPoint points[4];
				ar.Read(points, 4*sizeof(CPoint));
				math::findIMatrix2(points[0], points[1], points[2], points[3], m_pIMatrix2->m11, m_pIMatrix2->m12, m_pIMatrix2->m21, m_pIMatrix2->m22);
			}
			else
			{
				ar>>(m_pIMatrix2->m11);
				ar>>(m_pIMatrix2->m12);
				ar>>(m_pIMatrix2->m21);
				ar>>(m_pIMatrix2->m22);
			}
		}

		ar>>m_Origin.x;
		ar>>m_Origin.y;
		BYTE spottype;
		ar>>spottype;

		m_pSpot = CSpot::Apply(spottype);
	}

	if(m_pSpot!=nullptr)
		m_pSpot->Serialize(ar, dwVersion);
}

void CMark::ReleaseCE(CArchive& ar) const
{
	CGeom::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		CPoint center = m_Origin;
		center.x /= 10000;
		center.y /= 10000;
		ar<<center.x;
		ar<<center.y;
	}
	else
	{
	}
}

void CMark::ReleaseDCOM(CArchive& ar)
{
	//	CGeom::ReleaseDCOM(ar);
	CGeom::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		BYTE cSwitch = 0X00;
		if(m_pSpot!=nullptr) cSwitch |= 0X02;

		ar<<cSwitch;
		ar<<m_Origin.x;
		ar<<m_Origin.y;


		if(m_pSpot!=nullptr)
		{
			const BYTE type = m_pSpot->Gettype();
			ar<<type;
			m_pSpot->ReleaseDCOM(ar);
		}
	}
	else
	{
		BYTE cSwitch;
		ar>>cSwitch;
		ar>>m_Origin.x;
		ar>>m_Origin.y;



		if((cSwitch&0X02)==0X02)
		{
			BYTE spottype;
			ar>>spottype;
			m_pSpot = CSpot::Apply(spottype);
			if(m_pSpot!=nullptr)
				m_pSpot->ReleaseDCOM(ar);
		}
	}
}

bool CMark::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	try
	{
		m_geoId = rs.Field(_T("GID")).AsLong();
		m_Origin.x = datainfo.MapToDoc(rs.Field(_T("X")).AsDouble());
		m_Origin.y = datainfo.MapToDoc(rs.Field(_T("Y")).AsDouble());
		if(strTag.IsEmpty()==FALSE)
		{
			m_strName = rs.Field(strTag).AsString();
		}

		m_Rect = CRect(m_Origin.x, m_Origin.y, m_Origin.x, m_Origin.y);
		return true;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return false;
}

bool CMark::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	const double x = datainfo.DocToMap(m_Origin.x);
	const double y = datainfo.DocToMap(m_Origin.y);

	rs.Field(_T("X")) = x;
	rs.Field(_T("Y")) = y;
	return false;
}

DWORD CMark::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CGeom::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		BYTE cSwitch = 0;
		if(m_pSpot!=nullptr) cSwitch |= 0X04;

		pFile->Write(&cSwitch, sizeof(BYTE));
		size += sizeof(BYTE);
		const DWORD nCode1 = (((m_geoId<<10)+m_geoId)^0X76586778);
		const DWORD nCode2 = (((m_geoId<<11)+m_geoId)^0X34535467);
		CPoint center = m_Origin;
		center.x = (center.x^nCode1);
		center.y = (center.y^nCode2);

		pFile->Write(&center, sizeof(CPoint));
		size += sizeof(CPoint);

		if(m_pSpot!=nullptr)
		{
			const BYTE spottype = m_pSpot->Gettype();
			pFile->Write(&spottype, sizeof(BYTE));
			size += sizeof(BYTE);
		}

		if(m_pSpot!=nullptr)
		{
			size += m_pSpot->PackPC(pFile, bytes);
		}
		return size;
	}
	else
	{
		const BYTE cSwitch = *(BYTE*)bytes;
		bytes += sizeof(BYTE);

		m_Origin = *(CPoint*)bytes;
		const DWORD nCode1 = (((m_geoId<<10)+m_geoId)^0X76586778);
		const DWORD nCode2 = (((m_geoId<<11)+m_geoId)^0X34535467);
		m_Origin.x = (m_Origin.x^nCode1);
		m_Origin.y = (m_Origin.y^nCode2);

		bytes += sizeof(CPoint);

		if(m_pSpot!=nullptr)
		{
			//			m_pSpot->Decrease(nullptr);
			delete m_pSpot;
			m_pSpot = nullptr;
		}

		if((cSwitch&0X04)==0X04)
		{
			const BYTE spotindex = *(BYTE*)bytes;
			bytes += sizeof(BYTE);

			m_pSpot = CSpot::Apply(spotindex);
		}

		if(m_pSpot!=nullptr)
		{
			m_pSpot->PackPC(nullptr, bytes);
		}
		return 0;
	}
}

DWORD CMark::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);

	CPoint point = m_Origin;
	point.x /= 10000;
	point.y /= 10000;
	file.Write(&point, sizeof(CPoint));
	size += sizeof(CPoint);

	return size;
}

void CMark::ReleaseWeb(CFile& file, CSize offset) const
{
	CGeom::ReleaseWeb(file, offset);;

	short x = m_Origin.x-offset.cx;
	short y = m_Origin.y-offset.cy;
	ReverseOrder(x);
	file.Write(&x, sizeof(short));
	ReverseOrder(y);
	file.Write(&y, sizeof(short));

	const BYTE type = m_pSpot==nullptr ? -1 : m_pSpot->Gettype();
	file.Write(&type, sizeof(BYTE));
	if(m_pSpot!=nullptr)
	{
		m_pSpot->ReleaseWeb(file);
	}
}

void CMark::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CGeom::ReleaseWeb(stream, offset);;
	const short x = m_Origin.x-offset.cx;
	const short y = m_Origin.y-offset.cy;
	stream<<x;
	stream<<y;
	const BYTE type = m_pSpot==nullptr ? -1 : m_pSpot->Gettype();
	stream<<type;
	if(m_pSpot!=nullptr)
	{
		m_pSpot->ReleaseWeb(stream);
	}
}
void CMark::ReleaseWeb(boost::json::object& json) const
{
	CGeom::ReleaseWeb(json);

	std::vector<int> child1;
	child1.push_back(m_Origin.x);
	child1.push_back(m_Origin.y);
	json["Center"] = boost::json::value_from(child1);
	if(m_pSpot!=nullptr)
	{
		boost::json::object child2;
		const BYTE type = m_pSpot->Gettype();
		child2["Type"] = type;
		m_pSpot->ReleaseWeb(child2);
		json["Spot"] = child2;
	}
}
void CMark::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CGeom::ReleaseWeb(writer, offset);

	writer.add_variant_sint32(m_Origin.x - offset.cx);
	writer.add_variant_sint32(m_Origin.y - offset.cy);
	if(m_pIMatrix2!=nullptr)
	{
		writer.add_bool(false);
		writer.add_double(m_pIMatrix2->m11);
		writer.add_double(m_pIMatrix2->m12);
		writer.add_double(m_pIMatrix2->m21);
		writer.add_double(m_pIMatrix2->m22);
	}
	else
	{
		writer.add_bool(true);
	}
	if(m_pSpot != nullptr)
	{
		writer.add_bool(true);
		const BYTE type = m_pSpot->Gettype();
		writer.add_byte(type);
		m_pSpot->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);

	{
		BYTE levelObj = (BYTE)(m_levelObj*10);
		BYTE levelTag = (BYTE)(m_levelTag*10);
		writer.add_byte(levelObj);
		writer.add_byte(levelTag);
		writer.add_byte(m_bTag);
	}
}
//void CMark::ExportEps(FILE* file,double fZoom)
//{
//	_ftprintf(file, _T("q\n"));
//	_ftprintf(file, _T("%g %g translate\n"), m_Origin.x*fZoom,m_Origin.y*fZoom);
//
//	if(m_fSin != 0)
//	{
//		double m11 = m_fCos;
//		double m12 = m_fSin;
//		double m21 = m_fCos-m_fSin;  
//		double m22 = m_fSin+m_fCos;
//
//		double m31 = 0; 
//		double m32 = 0; 
//
////		_ftprintf(file, _T("[%g %g %g %g %g %g] concat\n"), m11,m12,m21,m22,m31,m32);
//		_ftprintf(file, _T("[%g %g %g %g %g %g] concat\n"), m_fCos,m_fSin,-m_fSin,m_fCos,0,0);
//	}
//	_ftprintf(file, _T("SpotSymbol\n"));	
//	_ftprintf(file, _T("Q\n"));
//}

void CMark::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(this->IsValid()==false)
		return;

	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : aiKey.pubSpot;
	if(pSpot==nullptr)
		return;

	double m11 = 1;
	double m12 = 0;
	double m21 = 0;
	double m22 = 1;
	if(m_pIMatrix2!=nullptr)
	{
		m11 = m_pIMatrix2->m11;
		m12 = m_pIMatrix2->m12;
		m21 = m_pIMatrix2->m21;
		m22 = m_pIMatrix2->m22;
	}
	const Gdiplus::PointF center = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);

	Gdiplus::Matrix matrix(m11, m12, m21, m22, center.X, center.Y);
	pSpot->ExportPs3(file, matrix, aiKey);
}

void CMark::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(this->IsValid()==false)
		return;

	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : (CSpot*)context.pSpot;
	if(pSpot==nullptr)
		return;

	double m11 = 1;
	double m12 = 0;
	double m21 = 0;
	double m22 = 1;
	if(m_pIMatrix2!=nullptr)
	{
		m11 = m_pIMatrix2->m11;
		m12 = m_pIMatrix2->m12;
		m21 = m_pIMatrix2->m21;
		m22 = m_pIMatrix2->m22;
	}
	const Gdiplus::PointF center = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);

	Gdiplus::Matrix matrix(m11, m12, m21, m22, center.X, center.Y);
	pSpot->ExportPdf(pdf, page, library, matrix);
}

Gdiplus::RectF CMark::GetVewBoundary(const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CSpot* pSpot = m_pSpot!=nullptr ? m_pSpot : (CSpot*)context.pSpot;
	CRect rect = pSpot!=nullptr ? pSpot->GetWanRect(library) : CRect(0, 0, 0, 0);
	rect.left = (long)(rect.left*context.ratioMark*viewinfo.m_sizeDPI.cx/(10000*72.f));
	rect.top = (long)(rect.top*context.ratioMark*viewinfo.m_sizeDPI.cy/(10000*72.f));
	rect.right = (long)(rect.right*context.ratioMark*viewinfo.m_sizeDPI.cx/(10000*72.f));
	rect.bottom = (long)(rect.bottom*context.ratioMark*viewinfo.m_sizeDPI.cy/(10000*72.f));
	if(m_pIMatrix2!=nullptr)
	{
		//to update	CPoint point1;
		//to update	point1.x = round(rect.left*m_fCos-rect.top*m_fSin);
		//to update	point1.y = round(rect.left*m_fSin+rect.top*m_fCos);
		//to update	CPoint point2;
		//to update	point2.x = round(rect.right*m_fCos-rect.top*m_fSin);
		//to update	point2.y = round(rect.right*m_fSin+rect.top*m_fCos);
		//to update	CPoint point3;
		//to update	point3.x = round(rect.right*m_fCos-rect.bottom*m_fSin);
		//to update	point3.y = round(rect.right*m_fSin+rect.bottom*m_fCos);
		//to update	CPoint point4;
		//to update	point4.x = round(rect.left*m_fCos-rect.bottom*m_fSin);
		//to update	point4.y = round(rect.left*m_fSin+rect.bottom*m_fCos);
		//to update
		//to update	rect.left = min(min(point1.x, point2.x), min(point3.x, point4.x));
		//to update	rect.top = min(min(point1.y, point2.y), min(point3.y, point4.y));
		//to update	rect.right = max(max(point1.x, point2.x), max(point3.x, point4.x));
		//to update	rect.bottom = max(max(point1.y, point2.y), max(point3.y, point4.y));
	}
	const Gdiplus::PointF point = viewinfo.DocToClient <Gdiplus::PointF>(m_Origin);
	return Gdiplus::RectF(point.X-rect.Width()/2.f, point.Y-rect.Height()/2.f, rect.Width(), rect.Height());
}

void CMark::CalCorner(const CSpot* pSpot, const CDatainfo& datainfo, const CLibrary& library)
{
	CRect border(-10000, -10000, 10000, 10000);
	if(m_pSpot!=nullptr)
		border = m_pSpot->GetDocRect(datainfo, library);
	else if(pSpot!=nullptr)
		border = pSpot->GetDocRect(datainfo, library);
	else if(m_pIMatrix2!=nullptr)
	{
		border = m_Rect;
		border.OffsetRect(-m_Rect.CenterPoint());
	}
	//to do when it is not regular
	{
		border.OffsetRect(m_Origin.x, m_Origin.y);
		m_Rect.left = border.left;
		m_Rect.top = border.top;
		m_Rect.right = border.right;
		m_Rect.bottom = border.bottom;
	}
}

CGeom* CMark::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(in && Clipper2Lib::PointInPolygon<int64_t>(Clipper2Lib::Point64(m_Origin.x, m_Origin.y), clips.front())==Clipper2Lib::PointInPolygonResult::IsInside)
		return (CGeom*)this;
	else if(!in && Clipper2Lib::PointInPolygon<int64_t>(Clipper2Lib::Point64(m_Origin.x, m_Origin.y), clips.front())==Clipper2Lib::PointInPolygonResult::IsOutside)
		return (CGeom*)this;
	else
		return nullptr;
}

void CMark::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(this->IsValid()==false)
		return;

	if(bTransGeo==true)
	{
		const CPointF geoPoint = datainfo.DocToGeo(m_Origin);
		_ftprintf(fileMif, _T("CPoint\n"));
		_ftprintf(fileMif, _T("%g %g\n"), geoPoint.x, geoPoint.y);
		_ftprintf(fileMif, _T("Symbol (83,37440,8,\"CASWLEGEND\",0,0)\n"));
	}
	else
	{
		_ftprintf(fileMif, _T("CPoint\n"));
		_ftprintf(fileMif, _T("%d %d\n"), m_Origin.x, m_Origin.y);
		_ftprintf(fileMif, _T("Symbol (83,37440,8,\"CASWLEGEND\",0,0)\n"));
	}
}

double CMark::Shortcut(const CPoint& point, CPoint& trend) const
{
	double distance = ::LineLength1(point, m_Origin);
	distance = abs(distance);
	trend.x = m_Origin.x;
	trend.y = m_Origin.y;

	return distance;
}
