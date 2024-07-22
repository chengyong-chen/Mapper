#include "stdafx.h"

#include "Path.h"
#include "Poly.h"
#include "Compound.h"
#include "Group.h"

#include "../Style/Line.h"
#include "../Style/LineEmpty.h"
#include "../Style/LineSymbol.h"
#include "../Style/LineRiver.h"
#include "../Style/FillCompact.h"
#include "../Style/FillComplex.h"
#include "../Style/Psboard.h"
#include "../Style/ColorSpot.h"

#include "../DataView/viewinfo.h"

#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPath, CGeom)

CPath::CPath()
	: CGeom()
{
	m_bClosed = true;
}
CPath::CPath(CLine* pLine, CFillCompact* pFill)
	: CGeom(pLine, pFill)
{
	ASSERT_VALID(this);

	m_bClosed = true;
}
CPath::CPath(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CGeom(rect, pLine, pFill)
{
	ASSERT_VALID(this);

	m_bClosed = true;
}

CPath::~CPath()
{
	for(std::list<CPath*>::iterator it = m_holes.begin();it!=m_holes.end();it++)
	{
		CPath* pPath = *it;
		delete pPath;
	}
	m_holes.clear();
}
void CPath::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CGeom::Sha1(sha1);

	for(std::list<CPath*>::const_iterator it = m_holes.cbegin(); it!=m_holes.cend(); it++)
	{
		CPath* pPath = *it;
		pPath->Sha1(sha1);
	}
}
void CPath::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	for(std::list<CPath*>::const_iterator it = m_holes.cbegin(); it!=m_holes.cend(); it++)
	{
		CPath* pPath = *it;
		pPath->Sha1(sha1, offset);
	}

}
void CPath::Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const
{
	if(m_pLine!=nullptr)
	{
		m_pLine->Sha1(sha1);
	}
	else if(pLine!=nullptr)
	{
		pLine->Sha1(sha1);
	}

	if(m_bClosed == false)
	{ 
	}
	else if(m_pFill!=nullptr)
	{
		m_pFill->Sha1(sha1);
	}
	else if(pFill!=nullptr)
	{
		pFill->Sha1(sha1);
	}
}
void CPath::Purify() const
{
	CGeom::Purify();

	this->m_bMeridianCrossing = true;
}
void CPath::RecalCentroid()
{
	const int cellsize = min(m_Rect.Width(), m_Rect.Height());
	if(cellsize<=0)
		m_centroid = m_Rect.CenterPoint();
	else if(m_bClosed)//https://github.com/mapbox/polylabel/tree/master/include/mapbox
	{
		auto compare = [](const CPoly::Cell& a, const CPoly::Cell& b)
		{
			return a.max<b.max;
		};

		const CPoint centrioid = FindCentroid();
		CPoly::Cell best(centrioid.x, centrioid.y, 0, this);
		const CPoly::Cell bbox(m_Rect.CenterPoint().x, m_Rect.CenterPoint().y, 0, this);
		if(bbox.dis>best.dis)
			best = bbox;

		using Queue = std::priority_queue<CPoly::Cell, std::vector<CPoly::Cell>, decltype(compare)>;
		Queue queue(compare);

		for(int x = m_Rect.left; x<m_Rect.right; x += cellsize)
		{
			for(int y = m_Rect.top; y<m_Rect.bottom; y += cellsize)
			{
				queue.push(CPoly::Cell(x+cellsize/2, y+cellsize/2, cellsize/2, this));
			}
		}
		while(!queue.empty())
		{
			const CPoly::Cell cell = queue.top();
			queue.pop();

			if(cell.dis>best.dis)
				best = cell;
			if((cell.max-best.dis)<=1000)
				continue;
			const int half = cell.h/2;
			if(half<=1000)
				continue;

			CPoly::Cell subcell1(cell.centerx-half, cell.centery-half, half, this);
			if(subcell1.max>best.dis)
				queue.push(subcell1);
			CPoly::Cell subcell2(cell.centerx+half, cell.centery-half, half, this);
			if(subcell2.max>best.dis)
				queue.push(subcell2);
			CPoly::Cell subcell3(cell.centerx-half, cell.centery+half, half, this);
			if(subcell3.max>best.dis)
				queue.push(subcell3);
			CPoly::Cell subcell4(cell.centerx+half, cell.centery+half, half, this);
			if(subcell4.max>best.dis)
				queue.push(subcell4);
		}
		m_centroid.x = best.centerx;
		m_centroid.y = best.centery;
	}
	else
	{
		const unsigned long length = GetLength(100.f/2)/2;
		unsigned int nAnchor;
		double t;

		GetAandT(1, 0, length, nAnchor, t);

		m_centroid = GetPoint(nAnchor, t);
	}
}
CPoint CPath::GetGeogroid() const
{
	return m_geogroid != nullptr ? *m_geogroid : m_centroid;
}
CPoint CPath::GetLabeloid() const
{
	return m_labeloid != nullptr ? *m_labeloid : m_centroid;
}
void CPath::SetGeogroid(const CPoint& point)
{
	if(m_bClosed==true)
	{
		m_geogroid = m_geogroid==nullptr ? new CPoint() : m_geogroid;
		*m_geogroid = point;
	}
}
void CPath::SetLabeloid(const CPoint& point)
{
	if(m_bClosed==true)
	{
		m_labeloid = m_labeloid==nullptr ? new CPoint(point) : m_labeloid;
		*m_labeloid = point;
	}
};
void CPath::ResetGeogroid()
{
	if(m_geogroid!=nullptr)
	{
		delete m_geogroid;
		m_geogroid = nullptr;
	}
	RecalCentroid();
}
void CPath::ResetLabeloid()
{
	if(m_labeloid!=nullptr)
	{
		delete m_labeloid;
		m_labeloid = nullptr;
	}
	RecalCentroid();
}
void CPath::GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const
{
	CPoly* pPoly = this->Polylize(tolerance);
	if(pPoly!=nullptr)
	{
		Clipper2Lib::Path64 path(pPoly->m_nAnchors);
		for(int index = 0; index<pPoly->m_nAnchors; index++)
		{
			path[index].x = pPoly->m_pPoints[index].x;
			path[index].y = pPoly->m_pPoints[index].y;
		}
		paths64.push_back(path);
	}
	if(pPoly!=this)
	{
		delete pPoly;
	}
}
void CPath::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	Gdiplus::Pen* pen = nullptr;
	if(m_pLine != nullptr)
	{
		pen = m_pLine->GetPen(context.ratioLine, dpi);
	}
	Gdiplus::Brush* brush = nullptr;
	if(m_bClosed == true && m_pFill != nullptr)
	{
		brush = m_pFill->GetBrush(context.ratioFill, dpi);
	}
	if(pen == nullptr && brush == nullptr)
		return;

	Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
	AddPath(path, matrix);
	
	if(m_bClosed == true)
	{	
		if(brush != nullptr)
		{				
			if(brush->GetType() == Gdiplus::BrushTypePathGradient)
			{
				Gdiplus::Rect rect = CPath::Transform(matrix, m_Rect);
				Gdiplus::GraphicsPath enclose(Gdiplus::FillMode::FillModeAlternate);
				enclose.AddRectangle(rect);
				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, matrix, &enclose);

				g.FillPath(local, &path);
				::delete local;
			}
			else if(brush->GetType() == Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, matrix);
				g.FillPath(local, &path);
				::delete local;
			}
			else
			{
				g.FillPath(brush, &path);
			}
		}

		if(pen != nullptr)
		{
			g.DrawPath(pen, &path);
		}
	}
	else if(pen != nullptr)
	{
		g.DrawPath(pen, &path);
	}

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}
void CPath::Draw(Gdiplus::Graphics& g, CLine* pLine, const CViewinfo& viewinfo, const float& ratio) const
{
	if(pLine == nullptr)
		return;
	Gdiplus::Pen* pen = pLine->GetPen(ratio, viewinfo.m_sizeDPI);
	if(pen == nullptr)
		return;

	float tolerance = viewinfo.GetPrecision(10);
	Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
	AddPath(path, viewinfo, tolerance);
	g.DrawPath(pen, &path);		

	::delete pen;
	pen = nullptr;
}
void CPath::Preoccupy(Gdiplus::Graphics& g, CLine* pLine, const CViewinfo& viewinfo, const float& ratio) const
{
	if(pLine==nullptr)
		return;

	auto pOldColor = pLine->m_pColor==nullptr ? nullptr : pLine->m_pColor;
	if(pLine!=nullptr)
	{
		const_cast<CColor*&>(pLine->m_pColor) = CColorSpot::White();
	}
	Gdiplus::Pen* pen = pLine->GetPen(ratio, viewinfo.m_sizeDPI);
	if(pen!=nullptr)
	{
		float tolerance = viewinfo.GetPrecision(10);
		Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
		AddPath(path, viewinfo, tolerance);
		g.DrawPath(pen, &path);

		::delete pen;
		pen = nullptr;
	}
	if(pLine!=nullptr)
	{
		const_cast<CColor*&>(pLine->m_pColor) = pOldColor;
	}
}
void CPath::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	Gdiplus::Pen* pen = m_pLine != nullptr ? m_pLine->GetPen(ratio, viewinfo.m_sizeDPI) : nullptr;
	Gdiplus::Brush* brush = m_bClosed == true && m_pFill != nullptr ? m_pFill->GetBrush(ratio, viewinfo.m_sizeDPI) : nullptr;
	if(pen == nullptr && brush == nullptr)
		return;

	float tolerance = viewinfo.GetPrecision(10);
	Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
	AddPath(path,viewinfo, tolerance);

	if(m_bClosed == true)
	{
		if(brush != nullptr)
		{
			if(brush->GetType() == Gdiplus::BrushTypePathGradient)
			{
				const Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
				Gdiplus::GraphicsPath enclose(Gdiplus::FillMode::FillModeAlternate);
				enclose.AddRectangle(rect);
				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &enclose);
				g.FillPath(local, &path);
				::delete local;
			}
			else if(brush->GetType() == Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillPath(local, &path);
				::delete local;
			}
			else
			{
				g.FillPath(brush, &path);
			}
		}

		if(pen != nullptr)
		{
			g.DrawPath(pen, &path);
		}
	}
	else if(pen != nullptr)
	{
		g.DrawPath(pen, &path);
	}
	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}

void CPath::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	Gdiplus::Pen* pen = context.penStroke;
	Gdiplus::Brush* brush = context.brushFill;

	Gdiplus::Pen* pen1 = nullptr;
	if(m_pLine != nullptr)
	{
		pen1 = m_pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
		pen = pen1;
	}
	Gdiplus::Brush* brush1 = nullptr;
	if(m_bClosed == true && m_pFill != nullptr)
	{
		brush1 = m_pFill->GetBrush(context.ratioFill, viewinfo.m_sizeDPI);
		brush = brush1;
	}
	if(pen != nullptr || brush != nullptr)
	{
		Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
		AddPath(path, viewinfo, context.tolerance);

		if(m_bClosed == true && brush != nullptr)
		{
			if(brush->GetType() == Gdiplus::BrushTypePathGradient)
			{
				const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(m_Rect);
				Gdiplus::GraphicsPath enclose(Gdiplus::FillMode::FillModeAlternate);
				enclose.AddRectangle(rect);

				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &enclose);
				g.FillPath(local, &path);
				::delete local;
			}
			else if(brush->GetType() == Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillPath(local, &path);
				::delete local;
			}
			else
			{
				g.FillPath(brush, &path);
			}
		}

		if(pen != nullptr)
		{
			g.DrawPath(pen, &path);
		}

		::delete pen1;
		pen1 = nullptr;
		::delete brush1;
		brush1 = nullptr;
	}

	if(m_pLine != nullptr && m_pLine->Gettype() == CLine::LINETYPE::Symbol)
		((CLineSymbol*)m_pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine != nullptr && m_pLine->Gettype() == CLine::LINETYPE::River)
		m_pLine->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine != nullptr)
	{
	}
	else if(context.pLine != nullptr && context.pLine->IsKindOf(RUNTIME_CLASS(CLineSymbol)) == TRUE)
		((CLineSymbol*)context.pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(context.pLine != nullptr && context.pLine->IsKindOf(RUNTIME_CLASS(CLineRiver)) == TRUE)
		m_pLine->Draw(g, viewinfo, library, this, context.ratioLine);
}
void CPath::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	Gdiplus::Pen* pen = context.penStroke;
	Gdiplus::Pen* pen1 = nullptr;
	if(m_pLine!=nullptr)
	{
		pen1 = m_pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
		pen = pen1;
	}
	if(pen!=nullptr)
	{
		Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
		AddPath(path, viewinfo, context.tolerance);

		g.DrawPath(pen, &path);

		::delete pen1;
		pen1 = nullptr;
	}
	else if(m_pLine!=nullptr&&m_pLine->Gettype()==CLine::LINETYPE::Symbol)
		((CLineSymbol*)m_pLine)->Preoccupy(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine!=nullptr&&m_pLine->Gettype()==CLine::LINETYPE::River)
		m_pLine->Preoccupy(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine!=nullptr)
	{
	}
	else if(context.pLine!=nullptr&&context.pLine->IsKindOf(RUNTIME_CLASS(CLineSymbol))==TRUE)
		((CLineSymbol*)context.pLine)->Preoccupy(g, viewinfo, library, this, context.ratioLine);
	else if(context.pLine!=nullptr&&context.pLine->IsKindOf(RUNTIME_CLASS(CLineRiver))==TRUE)
		m_pLine->Preoccupy(g, viewinfo, library, this, context.ratioLine);
}
void CPath::DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const
{
	Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
	AddPath(path, viewinfo, viewinfo.GetPrecision(10));

	if(m_bClosed == true)
	{
		const Gdiplus::HatchBrush brush(Gdiplus::HatchStyleDiagonalCross, Gdiplus::Color(100, 128, 0, 0), Gdiplus::Color(100, 255, 255, 0));
		g.FillPath(&brush, &path);
	}
	else
	{
		Gdiplus::Pen pen(Gdiplus::Color(255, 128, 0, 0), width);
		g.DrawPath(&pen, &path);
	}
}
void CPath::DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const
{
	if(m_bClosed==false)
		return;

	if(labeloid&&m_labeloid!=nullptr)
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(*m_labeloid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);

		pDC->FillSolidRect(cliRect, 0X0000FF);
	}
	if(geogroid&&m_geogroid!=nullptr)
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(*m_geogroid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);
		pDC->FillSolidRect(cliRect, 0XFF0000);
	}
	if(m_geogroid==nullptr&&m_labeloid==nullptr&&m_centroid!=CPoint(0, 0))
	{
		const Gdiplus::Point cliPoint = viewinfo.DocToClient<Gdiplus::Point>(m_centroid);
		CRect cliRect = CRect(CPoint(cliPoint.X, cliPoint.Y), CSize(0, 0));
		cliRect.InflateRect(2, 2);
		pDC->FillSolidRect(cliRect, 0X000000);
	}
}
Gdiplus::PointF CPath::GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const
{
	if(m_bClosed==true)
	{
		hAlign = HALIGN::HA_CENTER;
		vAlign = VALIGN::VA_CENTER;
		const CPoint docPoint = this->GetLabeloid();
		return viewinfo.DocToClient <Gdiplus::PointF>(docPoint);
	}
	else
		return CGeom::GetTagAnchor(viewinfo, hAlign, vAlign);
}
void CPath::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	if(this->IsValid() == false)
		return;

	CLine* line = m_pLine != nullptr ? m_pLine : aiKey.pubLine;
	CFillGeneral* fill = m_pFill != nullptr ? m_pFill : aiKey.pubFill;
	if(line == nullptr && fill == nullptr)
		return;
	const CLine::LINETYPE lineType = line == nullptr ? CLine::LINETYPE::Blank : line->Gettype();
	const CFill::FILLTYPE filltype = fill == nullptr ? CFill::FILLTYPE::Blank : fill->Gettype();
	if(m_bClosed == true)
	{
		if(filltype == CFill::FILLTYPE::Blank && lineType == CLine::LINETYPE::Alone)
		{
			aiKey.ExportStroke(file, line);
			this->ExportPts(file, matrix, _T("s"));
		}
		else if(filltype == CFill::FILLTYPE::Alone && lineType == CLine::LINETYPE::Blank)
		{
			aiKey.ExportBrush(file, fill);
			this->ExportPts(file, matrix, _T("f"));
		}
		else if(filltype == CFill::FILLTYPE::Alone && lineType == CLine::LINETYPE::Alone)
		{
			aiKey.ExportStroke(file, line);
			aiKey.ExportBrush(file, fill);
			this->ExportPts(file, matrix, _T("b"));
		}
	}
	else if(lineType == CLine::LINETYPE::Alone)
	{
		aiKey.ExportStroke(file, line);
		this->ExportPts(file, matrix, m_bClosed == true ? _T("s") : _T("S"));
	}
}
void CPath::ExportPs3(FILE* file, const CLine* pLine, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(this->IsValid() == false)
		return;
	if(pLine == nullptr)
		return;

	const CLine::LINETYPE type = pLine->Gettype();
	if(type == CLine::LINETYPE::Blank)
		return;

	aiKey.ExportStroke(file, pLine);
	this->ExportPts(file, viewinfo, m_bClosed == true ? _T("s") : _T("S"));
}
void CPath::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(this->IsValid() == false)
		return;

	CLine* line = m_pLine != nullptr ? m_pLine : aiKey.pubLine;
	CFillGeneral* fill = m_pFill != nullptr ? m_pFill : aiKey.pubFill;
	if(line == nullptr && fill == nullptr)
		return;
	const CLine::LINETYPE lineType = line == nullptr ? CLine::LINETYPE::Blank : line->Gettype();
	const CFill::FILLTYPE filltype = fill == nullptr ? CFill::FILLTYPE::Blank : fill->Gettype();
	if(m_bClosed == true)
	{
		switch(filltype)
		{
			case CFill::FILLTYPE::Blank:
				{
					if(lineType == CLine::LINETYPE::Blank)
						break;
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPs3(file, viewinfo, aiKey, this);
					else
					{
						aiKey.ExportStroke(file, line);
						this->ExportPts(file, viewinfo, _T("s"));
					}
				}
				break;
			case CFill::FILLTYPE::Symbol:
			case CFill::FILLTYPE::Pattern:
				{
					((CFillComplex*)fill)->ExportPs3(file, viewinfo, aiKey, this);

					if(lineType == CLine::LINETYPE::Blank)
						break;
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPs3(file, viewinfo, aiKey, this);
					else
					{
						aiKey.ExportStroke(file, line);
						aiKey.ExportBrush(file, fill);
						this->ExportPts(file, viewinfo, _T("s"));
					}
				}
				break;
			default:
				{
					if(lineType == CLine::LINETYPE::Blank)
					{
						aiKey.ExportBrush(file, fill);
						this->ExportPts(file, viewinfo, _T("f"));
					}
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPs3(file, viewinfo, aiKey, this);
					else
					{
						aiKey.ExportStroke(file, line);
						aiKey.ExportBrush(file, fill);
						this->ExportPts(file, viewinfo, _T("b"));
					}
				}
				break;
		}
	}
	else
	{
		switch(lineType)
		{
			case CLine::LINETYPE::Blank:
				break;
			case CLine::LINETYPE::Symbol:
			case CLine::LINETYPE::River:
				line->ExportPs3(file, viewinfo, aiKey, this);
			default:
				aiKey.ExportStroke(file, line);
				this->ExportPts(file, viewinfo, m_bClosed == true ? _T("s") : _T("S"));
				break;
		}
	}
}

void CPath::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid() == false)
		return;

	if(m_pLine == nullptr && m_pFill == nullptr)
		return;
	const CLine::LINETYPE lineType = m_pLine == nullptr ? CLine::LINETYPE::Blank : m_pLine->Gettype();
	const CFill::FILLTYPE filltype = m_pFill == nullptr ? CFill::FILLTYPE::Blank : m_pFill->Gettype();
	if(m_bClosed == true)
	{
		if(filltype == CFill::FILLTYPE::Blank && lineType == CLine::LINETYPE::Alone)
		{
			m_pLine->ExportPdf(page);
			this->ExportPts(page, matrix);
			HPDF_Page_Stroke(page);
		}
		else if(filltype == CFill::FILLTYPE::Alone && lineType == CLine::LINETYPE::Blank)
		{
			m_pFill->ExportPdf(page);
			this->ExportPts(page, matrix);
			HPDF_Page_Fill(page);
		}
		else if(filltype == CFill::FILLTYPE::Alone && lineType == CLine::LINETYPE::Alone)
		{
			m_pLine->ExportPdf(page);
			m_pFill->ExportPdf(page);
			this->ExportPts(page, matrix);
			m_pLine->m_nWidth == 0 ? HPDF_Page_Fill(page) : HPDF_Page_FillStroke(page);
		}
	}
	else if(lineType == CLine::LINETYPE::Alone)
	{
		m_pLine->ExportPdf(page);
		this->ExportPts(page, matrix);
		HPDF_Page_Stroke(page);
	}
}

void CPath::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(this->IsValid() == false)
		return;

	CLine* line = m_pLine != nullptr ? m_pLine : (CLine*)context.pLine;
	CFillCompact* fill = m_pFill != nullptr ? m_pFill : (CFillCompact*)context.pFill;
	if(line == nullptr && fill == nullptr)
		return;
	const CLine::LINETYPE lineType = line == nullptr ? CLine::LINETYPE::Blank : line->Gettype();
	const CFill::FILLTYPE filltype = fill == nullptr ? CFill::FILLTYPE::Blank : fill->Gettype();
	if(m_bClosed == true)
	{
		switch(filltype)
		{
			case CFill::FILLTYPE::Blank:
				{
					if(lineType == CLine::LINETYPE::Blank)
						break;
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPdf(pdf, page, viewinfo, library, this);
					else
					{
						line->ExportPdf(page);
						this->ExportPts(page, viewinfo);
						HPDF_Page_Stroke(page);
					}
				}
				break;
			case CFill::FILLTYPE::Symbol:
			case CFill::FILLTYPE::Pattern:
				{
					fill->ExportPdf(pdf, page, viewinfo, library, this);

					if(lineType == CLine::LINETYPE::Blank)
						break;
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPdf(pdf, page, viewinfo, library, this);
					else
					{
						line->ExportPdf(page);
						this->ExportPts(page, viewinfo);
						HPDF_Page_Stroke(page);
					}
				}
				break;
			default:
				{
					if(lineType == CLine::LINETYPE::Blank)
					{
						fill->ExportPdf(page);
						this->ExportPts(page, viewinfo);
						HPDF_Page_Fill(page);
					}
					else if(lineType == CLine::LINETYPE::Symbol || lineType == CLine::LINETYPE::River)
						line->ExportPdf(pdf, page, viewinfo, library, this);
					else
					{
						line->ExportPdf(page);
						fill->ExportPdf(page);
						this->ExportPts(page, viewinfo);
						line->m_nWidth == 0 ? HPDF_Page_Fill(page) : HPDF_Page_FillStroke(page);
					}
				}
				break;
		}
	}
	else
	{
		switch(lineType)
		{
			case CLine::LINETYPE::Blank:
				break;
			case CLine::LINETYPE::Symbol:
			case CLine::LINETYPE::River:
				line->ExportPdf(pdf, page, viewinfo, library, this);
			default:
				line->ExportPdf(page);
				this->ExportPts(page, viewinfo);
				HPDF_Page_Stroke(page);
				break;
		}
	}
}

CGeom* CPath::Convert(Clipper2Lib::Paths64& paths, bool closed)
{
	std::list<CPoly*> polys;
	for(auto it=paths.begin();it!=paths.end();it++)
	{
		Clipper2Lib::Path64& path = *it;
		if(path.size() < 2)
			continue;

		CPoly* pPoly = new CPoly();
		pPoly->m_nAnchors = path.size();
		pPoly->m_pPoints = new CPoint[pPoly->m_nAnchors+1];
		for(int index = 0; index < path.size(); index++)
		{
			pPoly->m_pPoints[index].x = path[index].x;
			pPoly->m_pPoints[index].y = path[index].y;
		}
		if(closed && pPoly->m_pPoints[pPoly->m_nAnchors - 1] != pPoly->m_pPoints[0])
		{
			pPoly->m_nAnchors++;
			pPoly->m_pPoints[pPoly->m_nAnchors - 1] = pPoly->m_pPoints[0];
		}
		pPoly->m_bClosed = closed;
		pPoly->RecalRect();
		polys.push_back(pPoly);
	}
	if(polys.size() == 0)
		return nullptr;
	else if(polys.size() == 1)
	{
		CPoly* pPoly = polys.front();
		return pPoly;
	}
	else if(closed)
	{
		CCompound* pCompound = new CCompound();
		pCompound->AddMembers(polys);
		return pCompound;
	}
	else
	{
		CGroup* pGroup = new CGroup();
		pGroup->AddMembers(polys);
//		CGeom::CopyTo(pGroup);
		return pGroup;
	}
}