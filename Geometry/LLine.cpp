#include "stdafx.h"
#include "LLine.h"
#include "LBezier.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(LLine, CObject, 0)

LLine::LLine()
{
}

LLine::LLine(const CPoint& pfpoint, const CPoint& ptpoint)
{
	fpoint = pfpoint;
	tpoint = ptpoint;
}

void LLine::SetPoint(const CPoint& pfpoint, const CPoint& ptpoint)
{
	fpoint = pfpoint;
	tpoint = ptpoint;
}

void LLine::Split(LLine& L, LLine& R) const
{
	L.fpoint = fpoint;
	L.tpoint.x = fpoint.x/2.f+tpoint.x/2.f;
	L.tpoint.y = fpoint.y/2.f+tpoint.y/2.f;

	R.fpoint = L.tpoint;
	R.tpoint = tpoint;
}

void LLine::Split(LLine& L, LLine& R, double t) const
{
	L.fpoint = fpoint;
	L.tpoint.x = fpoint.x+round((tpoint.x-fpoint.x)*t);
	L.tpoint.y = fpoint.y+round((tpoint.y-fpoint.y)*t);

	R.fpoint = L.tpoint;
	R.tpoint = tpoint;
}

CPoint LLine::GetPoint(const CPoint& fpoint, const CPoint& tpoint, double t)
{
	CPoint point;

	point.x = round(fpoint.x+(tpoint.x-fpoint.x)*t);
	point.y = round(fpoint.y+(tpoint.y-fpoint.y)*t);

	return point;
}

CPoint LLine::GetMiddle(const CPoint& fpoint, const CPoint& tpoint)
{
	CPoint point;

	point.x = round(fpoint.x+(tpoint.x-fpoint.x)/2.f);
	point.y = round(fpoint.y+(tpoint.y-fpoint.y)/2.f);

	return point;
}

unsigned long LLine::GetLength(const double& tolerance)
{
	unsigned long len;

	len = (tpoint.x-fpoint.x)*(tpoint.x-fpoint.x)+(tpoint.y-fpoint.y)*(tpoint.y-fpoint.y);
	len = (long)sqrt((double)len);

	return len;
}

bool LLine::PointOn(CPoint& point, const unsigned long& gap)
{
	CRect rect = CRect(fpoint.x, fpoint.y, tpoint.x, tpoint.y);
	rect.InflateRect(gap, gap);
	if(!rect.PtInRect(point))
		return false;

	if((point.x==fpoint.x&&point.y==fpoint.y))
		return true;

	if((point.x==tpoint.x&&point.y==tpoint.y))
		return true;
	const long A = fpoint.y-tpoint.y;
	const long B = tpoint.x-fpoint.x;
	if(abs(A)<2&&abs(B)<2)
	{
		if(::LineLength1(fpoint, point)<gap)
		{
			point.x = fpoint.x;
			point.y = fpoint.y;
			return true;
		}
		else
		{
			return false;
		}
	}
	const long C = -(B*fpoint.y+A*fpoint.x);
	const long D = A*A+B*B;
	const long distance = (long)((A*point.x+B*point.y+C)/sqrt((double)D));
	if(abs(distance)>gap)
	{
		const long x = long((B*B*point.x-A*B*point.y-A*C)/D);
		const long y = long((A*A*point.y-A*B*point.x-B*C)/D);
		point.x = x;
		point.y = y;
		return false;
	}
	else
	{
		return true;
	}
	return false;
}

bool LLine::Inter(LBezier* bezier, double* t1, double* t2, unsigned int& count)
{
	return BezierLineInter(bezier->fpoint, bezier->fctrol, bezier->tctrol, bezier->tpoint, fpoint, tpoint, t2, t1, count);
}

bool LLine::Inter(LLine* line, double& t1, double& t2)
{
	if(line->IsKindOf(RUNTIME_CLASS(LBezier)))
		return Inter((LBezier*)line, t1, t2);
	else
	{
		return LineInter(fpoint, tpoint, line->fpoint, line->tpoint, t1, t2);
	}
}
