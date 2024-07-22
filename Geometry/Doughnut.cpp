#include "stdafx.h"

#include "Global.h"
#include "Tag.h"
#include "Geom.h"
#include "Poly.h"
#include "Doughnut.h"
#include "PRect.h"

#include "../Style/Line.h"
#include "../Style/Hint.h"
#include "../Style/FillCompact.h"
#include <vector>
#include <queue>
#include <string>

using namespace std;
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CDoughnut, CCluster, 0)

CDoughnut::CDoughnut()
	: CCluster(), fillmode(Gdiplus::FillMode::FillModeAlternate)
{
	m_bType = 4;
	m_bClosed = true;
}

CDoughnut::~CDoughnut()
{
}

bool CDoughnut::IsReal() const
{
	bool bHasArea = false;
	POSITION pos1 = m_geomlist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		const CPath* pPath = m_geomlist.GetNext(pos1);
		if(pPath->m_bClosed==true)
		{
			bHasArea = true;
			break;
		}
	}
	return bHasArea;
}

void CDoughnut::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	pLine = pLine!=nullptr ? pLine : context.pLine;
	pFill = pFill!=nullptr ? pFill : context.pFill;

	Gdiplus::Pen* pen = pLine!=nullptr ? m_pLine->GetPen(context.ratioLine, dpi) : nullptr;
	Gdiplus::Brush* brush = pFill!=nullptr ? m_pFill->GetBrush(context.ratioFill, dpi) : nullptr;
	if(brush!=nullptr)
	{
		CObList polys;

		Gdiplus::GraphicsPath path(fillmode);

		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==false)
				polys.AddTail(pPath);
			else
				pPath->AddPath(path, matrix);
		}

		if(brush->GetType()==Gdiplus::BrushTypePathGradient)
		{
			const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, matrix, &path);
			g.FillPath(local, &path);
			::delete local;
		}
		else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
		{
			const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, matrix);
			g.FillPath(local, &path);
			::delete local;
		}
		else
		{
			g.FillPath(brush, &path);
		}

		if(pen!=nullptr)
		{
			g.DrawPath(pen, &path);
		}

		POSITION pos2 = polys.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = (CPath*)polys.GetNext(pos2);
			CLine* pOldLine = pPath->m_pLine;
			CFillCompact* pOldFill = pPath->m_pFill;
			CType* pOldType = pPath->m_pType;

			pPath->m_pLine = pPath->m_pLine!=nullptr ? pPath->m_pLine : m_pLine;
			pPath->m_pFill = pPath->m_pFill!=nullptr ? pPath->m_pFill : m_pFill;
			pPath->m_pType = pPath->m_pType!=nullptr ? pPath->m_pType : m_pType;

			pPath->Draw(g, matrix, context, dpi);

			pPath->m_pLine = pOldLine;
			pPath->m_pFill = pOldFill;
			pPath->m_pType = pOldType;
		}
	}
	else
		CCluster::Draw(g, matrix, context, dpi);

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}

void CDoughnut::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	float tolerance = viewinfo.GetPrecision(10);
	CLine* pLine = this->GetLine();
	CFillCompact* pFill = this->GetFill();

	Gdiplus::Pen* pen = pLine!=nullptr ? pLine->GetPen(ratio, viewinfo.m_sizeDPI) : nullptr;
	Gdiplus::Brush* brush = pFill!=nullptr ? pFill->GetBrush(ratio, viewinfo.m_sizeDPI) : nullptr;
	if(brush!=nullptr)
	{
		CObList polys;

		Gdiplus::GraphicsPath path(fillmode);

		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==false)
			{
				polys.AddTail(pPath);
			}
			else
			{
				pPath->m_pLine = nullptr;
				pPath->m_pFill = nullptr;

				pPath->AddPath(path, viewinfo, tolerance);
			}
		}

		if(path.GetPointCount()==0)
		{
		}
		else if(brush->GetType()==Gdiplus::BrushTypePathGradient)
		{
			Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);

			const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
			g.FillPath(local, &path);;
			::delete local;
		}
		else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
		{
			const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
			g.FillPath(local, &path);
			::delete local;
		}
		else
		{
			g.FillPath(brush, &path);
		}

		if(pen!=nullptr)
		{
			g.DrawPath(pen, &path);
		}

		POSITION pos2 = polys.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = (CPath*)polys.GetNext(pos2);
			CLine* pOldLine = pPath->m_pLine;
			CFillCompact* pOldFill = pPath->m_pFill;
			pPath->m_pLine = pLine;
			pPath->m_pFill = pFill;

			pPath->Draw(g, viewinfo, ratio);

			pPath->m_pLine = pOldLine;
			pPath->m_pFill = pOldFill;
		}
	}
	else
		CCluster::Draw(g, viewinfo, ratio);

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}

void CDoughnut::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	pLine = pLine!=nullptr ? pLine : context.pLine;
	pFill = pFill!=nullptr ? pFill : context.pFill;

	Gdiplus::Pen* pen1 = pLine!=nullptr ? pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI) : nullptr;
	Gdiplus::Brush* brush1 = pFill!=nullptr ? pFill->GetBrush(context.ratioFill, viewinfo.m_sizeDPI) : nullptr;
	Gdiplus::Pen* pen = pLine!=nullptr ? pen1 : context.penStroke;
	Gdiplus::Brush* brush = pFill!=nullptr ? brush1 : context.brushFill;
	if(brush!=nullptr)
	{
		float tolerance = viewinfo.GetPrecision(10);

		CObList polys;
		Gdiplus::GraphicsPath path(fillmode);
		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==false)
				polys.AddTail(pPath);
			else
				pPath->AddPath(path, viewinfo, tolerance);
		}
		if(path.GetPointCount()==0)
		{
		}
		else if(brush->GetType()==Gdiplus::BrushTypePathGradient)
		{
			const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
			g.FillPath(local, &path);;
			::delete local;
		}
		else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
		{
			const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
			g.FillPath(local, &path);
			::delete local;
		}
		else
		{
			g.FillPath(brush, &path);
		}

		if(pen!=nullptr)
		{
			g.DrawPath(pen, &path);
		}

		Gdiplus::Pen* oldpen = context.penStroke;
		Gdiplus::Brush* oldbrush = context.brushFill;
		context.penStroke = pen;
		context.brushFill = brush;

		POSITION pos2 = polys.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = (CPath*)polys.GetNext(pos2);
			pPath->Draw(g, viewinfo, library, context);
		}
		context.penStroke = oldpen;
		context.brushFill = oldbrush;
		polys.RemoveAll();
	}
	else
		CCluster::Draw(g, viewinfo, library, context);

	::delete pen1;
	pen1 = nullptr;
	::delete brush1;
	brush1 = nullptr;
}
void CDoughnut::DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const
{
	float tolerance = viewinfo.GetPrecision(10);

	const Gdiplus::HatchBrush brush(Gdiplus::HatchStyleDiagonalCross, Gdiplus::Color(100, 128, 0, 0), Gdiplus::Color(100, 255, 255, 0));
	Gdiplus::Pen pen(RGB(0, 128, 0), 10);
	pen.SetBrush(&brush);

	Gdiplus::GraphicsPath path(fillmode);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_geomlist.GetNext(pos);
		if(pPath->m_bClosed == false)
			pPath->DrawHatch(g, viewinfo, width);
		else
			pPath->AddPath(path, viewinfo, tolerance);
	}
	if(path.GetPointCount() > 0)
	{
		g.FillPath(&brush, &path);
	}
}
void CDoughnut::Normalize()
{
	CPath* pOuter = CDoughnut::GetOuter();
	pOuter = pOuter==nullptr ? m_geomlist.GetHead() : pOuter;
	if(pOuter!= nullptr)
	{
		pOuter->Normalize(false);
	}
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_geomlist.GetNext(pos);
		if(pPath == pOuter)
			continue;
		pPath->Normalize(true);
	}
}
void CDoughnut::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	if(this->IsValid()==false)
		return;

	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	pLine = pLine!=nullptr ? pLine : aiKey.pubLine;
	pFill = pFill!=nullptr ? pFill : aiKey.pubFill;
	if(pFill!=nullptr)
	{
		_ftprintf(file, _T("u\n"));

		_ftprintf(file, _T("*u\n"));
		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==FALSE)
				continue;
			else
				pPath->ExportPs3(file, matrix, aiKey);
		}
		_ftprintf(file, _T("*U\n"));

		POSITION pos2 = m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos2);
			if(pPath->m_bClosed==TRUE)
				continue;
			else
				pPath->ExportPs3(file, matrix, aiKey);
		}

		_ftprintf(file, _T("U\n"));
	}
	else
		return CCluster::ExportPs3(file, matrix, aiKey);
}

void CDoughnut::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(this->IsValid()==false)
		return;

	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	pLine = pLine!=nullptr ? pLine : aiKey.pubLine;
	pFill = pFill!=nullptr ? pFill : aiKey.pubFill;
	if(pFill!=nullptr)
	{
		_ftprintf(file, _T("u\n"));

		_ftprintf(file, _T("*u\n"));
		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==FALSE)
				continue;
			else
				pPath->ExportPs3(file, viewinfo, aiKey);
		}
		_ftprintf(file, _T("*U\n"));

		POSITION pos2 = m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos2);
			if(pPath->m_bClosed==TRUE)
				continue;
			else
				pPath->ExportPs3(file, viewinfo, aiKey);
		}

		_ftprintf(file, _T("U\n"));
	}
	else		
		CCluster::ExportPs3(file, viewinfo, aiKey);
}

void CDoughnut::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid()==false)
		return;

	CLine* pLine = this->GetLine();
	CFillCompact* pFill = this->GetFill();
	if(pFill!=nullptr&&IsReal()==TRUE)
	{
		pFill->ExportPdf(page);
		if(pLine!=nullptr)
			pLine->ExportPdf(page);

		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==FALSE)
				continue;

			pPath->ExportPts(page, matrix);
		}

		HPDF_Page_ClosePath(page);
		if(pLine!=nullptr)
			HPDF_Page_FillStroke(page);
		else
			HPDF_Page_Fill(page);

		POSITION pos2 = m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos2);
			if(pPath->m_bClosed==TRUE)
				continue;
			else
				pPath->ExportPdf(pdf, page, matrix);
		}
	}
	else
		return CCluster::ExportPdf(pdf, page, matrix);
}

void CDoughnut::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(this->IsValid()==false)
		return;

	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	pLine = pLine!=nullptr ? pLine : context.pLine;
	pFill = pFill!=nullptr ? pFill : context.pFill;
	if(pFill!=nullptr&&IsReal()==TRUE)
	{
		pFill->ExportPdf(page);
		if(pLine!=nullptr)
			pLine->ExportPdf(page);

		POSITION pos1 = m_geomlist.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos1);
			if(pPath->m_bClosed==FALSE)
				continue;
			else
				pPath->ExportPts(page, viewinfo);
		}

		HPDF_Page_ClosePath(page);
		if(pLine!=nullptr)
			HPDF_Page_EofillStroke(page);
		else
			HPDF_Page_Eofill(page);

		POSITION pos2 = m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CPath* pPath = m_geomlist.GetNext(pos2);
			if(pPath->m_bClosed==TRUE)
				continue;
			else
				pPath->ExportPdf(pdf, page, viewinfo, library, context);
		}
	}
	else
		return CCluster::ExportPdf(pdf, page, viewinfo, library, context);
}

bool CDoughnut::PickIn(const CPoint& point) const
{
	if(CGeom::PickIn(point)==false)
		return false;

	long nIn = 0;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CPath* pPath = m_geomlist.GetNext(pos);
		if(pPath->PickIn(point)==true)
			nIn++;
	}

	return nIn%2==0 ? false : true;
}

CPath* CDoughnut::GetOuter() const
{
	CPath* largestGeom = nullptr;
	double largestArae = 0;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPath* pPoly = m_geomlist.GetNext(pos);
		if(pPoly->m_bClosed==false)
			continue;
		const double area = pPoly->GetArea();
		if(abs(area)>largestArae)
		{
			largestArae = abs(area);
			largestGeom = pPoly;
		}
	}
	return largestGeom;
}

bool CDoughnut::ComparePair(std::pair<CPath*, double>& pair1, std::pair<CPath*, double>& pair2)
{
	return abs(pair1.second)>abs(pair2.second);
}
void CDoughnut::RecalCentroid()
{
	const int width = m_Rect.Width();
	const int height = m_Rect.Height();
	const int cellsize = max(width, height);
	if(cellsize<=0)
		m_centroid = m_Rect.CenterPoint();
	else if(m_bClosed)
	{
		auto compare = [](const CPath::Cell& a, const CPath::Cell& b)
		{
			return a.max<b.max;
		};
		using Queue = std::priority_queue<CPath::Cell, std::vector<CPath::Cell>, decltype(compare)>;
		Queue queue(compare);
		const int h = cellsize/2;
		for(int x = m_Rect.left; x<m_Rect.right; x += cellsize)
		{
			for(int y = m_Rect.top; y<m_Rect.bottom; y += cellsize)
			{
				queue.push(CPath::Cell(x+cellsize/2, y+cellsize/2, cellsize/2, this));
			}
		}
		const CPoint centrioid = FindCentroid();
		CPath::Cell best(centrioid.x, centrioid.y, 0, this);
		const CPath::Cell bboxcell(m_Rect.CenterPoint().x, m_Rect.CenterPoint().y, 0, this);
		if(bboxcell.dis>best.dis)
			best = bboxcell;

		while(!queue.empty())
		{
			const CPath::Cell cell = queue.top();
			queue.pop();

			if(cell.dis>best.dis)
				best = cell;

			if((cell.max-best.dis)<=1000)
				continue;
			const int half = cell.h/2;
			if(half<=1000)
				continue;

			CPath::Cell subcell1(cell.centerx-half, cell.centery-half, half, this);
			if(subcell1.max>best.dis)
				queue.push(subcell1);
			CPath::Cell subcell2(cell.centerx+half, cell.centery-half, half, this);
			if(subcell2.max>best.dis)
				queue.push(subcell2);
			CPath::Cell subcell3(cell.centerx-half, cell.centery+half, half, this);
			if(subcell3.max>best.dis)
				queue.push(subcell3);
			CPath::Cell subcell4(cell.centerx+half, cell.centery+half, half, this);
			if(subcell4.max>best.dis)
				queue.push(subcell4);
		}
		m_centroid.x = best.centerx;
		m_centroid.y = best.centery;
	}
	else
		m_centroid = m_Rect.CenterPoint();
}

unsigned long long CDoughnut::Distance2(const CPoint& point) const
{
	unsigned long long min = MAXUINT64;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPath* pPath = m_geomlist.GetNext(pos);
		if(pPath->IsKindOf(RUNTIME_CLASS(CPoly))==TRUE)
			min = min(min, ((CPoly*)pPath)->Distance2(point));
	}
	return min;
}
CPoint CDoughnut::FindCentroid() const
{
	double finalX = 0;
	double finalY = 0;
	double totalArea = 0;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CPath* pPath = m_geomlist.GetNext(pos);
		const double area = pPath->GetArea();
		const CPoint coid = pPath->FindCentroid();
		finalX += coid.x*area;
		finalY += coid.y*area;
		totalArea += area;
	}
	return CPoint(finalX/totalArea, finalY/totalArea);
}