#include "stdafx.h"

#include "LLine.h"
#include "LBezier.h"

#include "Global.h"

#include "../Public/Equation.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static long recurse = 0;

void DoBezierPts(LBezier& V, long error, long& len, double& t, long& result)
{
	if(len==0.0)
		return;
	const long ctrolLen = V.GetCtrolLength();
	const long chordLen = V.GetChordLength();

	if(ctrolLen-chordLen>error||ctrolLen+result>len+error)
	{
		LBezier left, right;
		V.Split(left, right);

		++recurse;
		DoBezierPts(left, error, len, t, result);
		if(len!=0.0l)
		{
			DoBezierPts(right, error, len, t, result);
		}

		--recurse;
	}
	else
	{
		result += ctrolLen;
		t += 1.0/(1<<recurse);

		if(t>1)
		{
			AfxMessageBox(_T("gfdgs"));
		}
		if(abs(result-len)<=error)
		{
			len = 0.0l;
		}
	}
}

IMPLEMENT_SERIAL(LBezier, LLine, 0)

LBezier::LBezier()
{
}

LBezier::LBezier(const CPoint& pfpoint, const CPoint& pfctrol, const CPoint& ptctrol, const CPoint& ptpoint)
{
	fpoint = pfpoint;
	fctrol = pfctrol;
	tctrol = ptctrol;
	tpoint = ptpoint;
}

LBezier::LBezier(const Gdiplus::Point& pfpoint, const Gdiplus::Point& pfctrol, const Gdiplus::Point& ptctrol, const Gdiplus::Point& ptpoint)
{
	fpoint = CPoint(pfpoint.X, pfpoint.Y);
	fctrol = CPoint(pfctrol.X, pfctrol.Y);
	tctrol = CPoint(ptctrol.X, ptctrol.Y);
	tpoint = CPoint(ptpoint.X, ptpoint.Y);
}

void LBezier::SetPoint(const CPoint& pfpoint, const CPoint& pfctrol, const CPoint& ptctrol, const CPoint& ptpoint)
{
	fpoint = pfpoint;
	fctrol = pfctrol;
	tctrol = ptctrol;
	tpoint = ptpoint;
}

void LBezier::Split(LBezier& L, LBezier& R) const
{
	L.fpoint = fpoint;
	L.fctrol.x = round(fpoint.x/2.0+fctrol.x/2.0);
	L.fctrol.y = round(fpoint.y/2.0+fctrol.y/2.0);
	L.tctrol.x = round(fpoint.x/4.0+2.0*fctrol.x/4.0+tctrol.x/4.0);
	L.tctrol.y = round(fpoint.y/4.0+2.0*fctrol.y/4.0+tctrol.y/4.0);
	L.tpoint.x = round(fpoint.x/8.0+3.0*fctrol.x/8.0+3.0*tctrol.x/8.0+tpoint.x/8.0);
	L.tpoint.y = round(fpoint.y/8.0+3.0*fctrol.y/8.0+3.0*tctrol.y/8.0+tpoint.y/8);

	R.fpoint = L.tpoint;
	R.fctrol.x = round(fctrol.x/4.0+2.0*tctrol.x/4.0+tpoint.x/4.0);
	R.fctrol.y = round(fctrol.y/4.0+2.0*tctrol.y/4.0+tpoint.y/4.0);
	R.tctrol.x = round(tctrol.x/2.0+tpoint.x/2.0);
	R.tctrol.y = round(tctrol.y/2.0+tpoint.y/2.0);
	R.tpoint = tpoint;
}

void LBezier::Split(LBezier& L, LBezier& R, double t) const
{
	const double s = 1-t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;

	L.fpoint = fpoint;
	L.fctrol.x = round(fpoint.x*s+fctrol.x*t);
	L.fctrol.y = round(fpoint.y*s+fctrol.y*t);
	L.tctrol.x = round(fpoint.x*ss+fctrol.x*s*t*2+tctrol.x*tt);
	L.tctrol.y = round(fpoint.y*ss+fctrol.y*s*t*2+tctrol.y*tt);
	L.tpoint.x = round(fpoint.x*sss+fctrol.x*ss*t*3+tctrol.x*s*tt*3+tpoint.x*ttt);
	L.tpoint.y = round(fpoint.y*sss+fctrol.y*ss*t*3+tctrol.y*s*tt*3+tpoint.y*ttt);

	R.fpoint = L.tpoint;
	R.fctrol.x = round(fctrol.x*ss+tctrol.x*s*t*2+tpoint.x*tt);
	R.fctrol.y = round(fctrol.y*ss+tctrol.y*s*t*2+tpoint.y*tt);
	R.tctrol.x = round(tctrol.x*s+tpoint.x*t);
	R.tctrol.y = round(tctrol.y*s+tpoint.y*t);
	R.tpoint = tpoint;
}

CPoint LBezier::GetPoint(double t) const
{
	const double s = 1-t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;

	CPoint point;
	point.x = round(fpoint.x*sss+fctrol.x*ss*t*3+tctrol.x*s*tt*3+tpoint.x*ttt);
	point.y = round(fpoint.y*sss+fctrol.y*ss*t*3+tctrol.y*s*tt*3+tpoint.y*ttt);

	return point;
}

unsigned long LBezier::GetLength(const double& tolerance)
{
	const unsigned long ctrolLen = GetCtrolLength();
	const unsigned long chordLen = GetChordLength();

	if(ctrolLen-chordLen>tolerance)
	{
		LBezier L;
		LBezier R;
		Split(L, R);
		return L.GetLength(tolerance)+R.GetLength(tolerance);
	}
	else
		return ctrolLen;
}

double LBezier::SqrtDiffFunc(const double& t) const
{
	double x = 0;
	double y = 0;
	x = 0;
	y = 0;
	const double s = 1-t;
	const double ss = s*s;

	x += fpoint.x*(-3*ss);
	y += fpoint.y*(-3*ss);

	x += fctrol.x*(-6*s*t);
	y += fctrol.y*(-6*s*t);

	x += fctrol.y*3*ss;
	y += fctrol.y*3*ss;

	x += tctrol.x*3*(2*t*s-t*t);
	y += tctrol.y*3*(2*t*s-t*t);

	x += tpoint.x*3*t*t;
	y += tpoint.y*3*t*t;

	return sqrt(x*x+y*y);
}

unsigned long LBezier::GetLengthByT(const double& t, const double& tolerance)
{
	if(t==0.f)
		return 0;
	if(t==1.f)
		return GetLength(tolerance);

	LBezier L, R;
	Split(L, R, t);

	return L.GetLength(tolerance);
}

bool LBezier::PointOn(const CPoint& point, const unsigned long& gap, double& t) const
{
	return PointOnBezier(point, fpoint, fctrol, tctrol, tpoint, gap, t);
}

bool LBezier::Inter(LBezier* bezier, double* t1, double* t2, unsigned int& count)
{
	return BezierInter(fpoint, fctrol, tctrol, tpoint, bezier->fpoint, bezier->fctrol, bezier->tctrol, bezier->tpoint, t1, t2, count);
}

bool LBezier::Inter(LLine* line, double* t1, double* t2, unsigned int& count)
{
	if(line->IsKindOf(RUNTIME_CLASS(LBezier)))
		return Inter((LBezier*)line, t1, t2, count);
	else
		return BezierLineInter(fpoint, fctrol, tctrol, tpoint, line->fpoint, line->tpoint, t1, t2, count);
}

double LBezier::GetTByLength(unsigned long& length, const double& tolerance)
{
	double t = 0.0f;
	long result = 0.0f;
	const float adjust = 0.0f;
	long tempLength = length;

	if(length<tolerance)
	{
		return 0;
		//      LDPoint rnd(p[0].GetPointF());
		//      adjust = p[0].DistFrom(p[3]) - rnd.DistFrom(p[3]); 
		//      tempLength += adjust;
	}
	else
		DoBezierPts(*this, tolerance, tempLength, t, result);

	length -= std::lround(result-adjust);
	return t;
}

long LBezier::GetCtrolLength() const
{
	long ctrolLen = 0;

	ctrolLen += ::LineLength1(fpoint, fctrol);
	ctrolLen += ::LineLength1(fctrol, tctrol);
	ctrolLen += ::LineLength1(tctrol, tpoint);

	return ctrolLen;
}

long LBezier::GetChordLength() const
{
	return ::LineLength1(fpoint, tpoint);
}

int LBezier::BezierX(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t)
{
	const double s = 1-t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;

	return round(fpoint.x*sss+fctrol.x*ss*t*3+tctrol.x*s*tt*3+tpoint.x*ttt);
}

int LBezier::BezierY(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t)
{
	const double s = 1-t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;

	return round(fpoint.y*sss+fctrol.y*ss*t*3+tctrol.y*s*tt*3+tpoint.y*ttt);
}

CRect LBezier::Box(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint)
{
	CRect rect(fpoint, tpoint);
	rect.NormalizeRect();

	{
		const double a = -1.0*fpoint.x+3.0*fctrol.x-3.0*tctrol.x+tpoint.x;
		const double b = 3.0*fpoint.x-6.0*fctrol.x+3.0*tctrol.x;
		const double c = -3.0*fpoint.x+3.0*fctrol.x;
		int nRoots = 0;
		double* pRoots = Q2Roots(3*a, 2*b, c, nRoots);
		if(pRoots!=nullptr&&nRoots>0)
		{
			for(int index = 0; index<nRoots; index++)
			{
				if(pRoots[index]<=0)
					continue;
				else if(pRoots[index]>=1)
					continue;
				else
				{
					const int X = LBezier::BezierX(fpoint, fctrol, tctrol, tpoint, pRoots[index]);
					rect.left = min(rect.left, X);
					rect.right = max(rect.right, X);
				}
			}

			delete[] pRoots;
			pRoots = nullptr;
		}
	}
	{
		const double a = -1.0*fpoint.y+3.0*fctrol.y-3.0*tctrol.y+tpoint.y;
		const double b = 3.0*fpoint.y-6.0*fctrol.y+3.0*tctrol.y;
		const double c = -3.0*fpoint.y+3.0*fctrol.y;
		int nRoots = 0;
		double* pRoots = Q2Roots(3*a, 2*b, c, nRoots);
		if(pRoots!=nullptr&&nRoots>0)
		{
			for(int index = 0; index<nRoots; index++)
			{
				if(pRoots[index]<=0)
					continue;
				else if(pRoots[index]>=1)
					continue;
				else
				{
					const int Y = LBezier::BezierY(fpoint, fctrol, tctrol, tpoint, pRoots[index]);
					rect.top = min(rect.top, Y);
					rect.bottom = max(rect.bottom, Y);
				}
			}
			delete[] pRoots;
			pRoots = nullptr;
		}
	}

	return rect;
}
