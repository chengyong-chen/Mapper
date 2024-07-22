#include "stdafx.h"

#include "Global.h"
#include "Fuction.h"

#include <cmath>
#include <math.h>
#include <stdlib.h>

#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Imager.h"
#include "../Geometry/Group.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/OleObj.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Geometry/TextPoly.h"
#include "../Geometry/RRect.h"
#include "../Geometry/Ellipse.h"
#include "../Geometry/Tag.h"

#include "../Public/Global.h"
#include "../Public/Equation.h"
#include "../Public/Function.h"
#include "../Utility/Rect.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void GeomSerialize(CArchive& ar, const DWORD dwVersion, CTypedPtrList<CObList, CGeom*>& geomlist)
{
	if(ar.IsStoring())
	{
		const DWORD nCount = geomlist.GetCount();
		ar << nCount;
		if(nCount == 0)
			return;

		char* p1 = new char[nCount/2 + 1];
		char* p2 = p1;

		ZeroMemory(p1, nCount/2 + 1);

		POSITION pos = geomlist.GetHeadPosition();
		for(DWORD i = 0; i < nCount; i++)
		{
			CGeom* pGeom = geomlist.GetNext(pos);
			BYTE type = pGeom->m_bType;
			if(i % 2 == 0)
			{
				type = (type << 4);
				type &= 0XF0;

				*p2 |= type;
			}
			else
			{
				*p2 |= type;
				p2++;
			}
		}
		ar.Write(p1, nCount/2 + 1);
		delete[]p1;
	}
	else
	{
		DWORD nCount;
		ar >> nCount;
		if(nCount == 0)
			return;

		char* p1 = new char[nCount/2 + 1];
		char* p2 = p1;

		ar.Read(p1, nCount/2 + 1);
		for(DWORD i = 0; i < nCount; i++)
		{
			char index = *p2;
			if(i % 2 == 0)
			{
				index = (index >> 4);
			}
			else
			{
				index = (index << 4);
				index = (index >> 4);
				p2++;
			}
			index &= 0X0F;
			CGeom* pGeom = nullptr;
			switch(index)
			{
				case 0:
					//				pGeom = new CGeom;
					break;
				case 1:
					pGeom = new CPoly;
					break;
				case 2:
					pGeom = new CBezier;
					break;
				case 3:
					pGeom = new CImager;
					break;
				case 4:
					pGeom = new CDoughnut;
					break;
				case 5:
					pGeom = new CGroup;
					break;
				case 6:
					pGeom = new COleObj;
					break;
				case 7:
					pGeom = new CPRect;
					break;
				case 8:
					pGeom = new CMark;
					break;
				case 9:
					pGeom = new CText;
					break;
				case 10:
					pGeom = new CTextPoly;
					break;
				case 12:
					pGeom = new CRRect;
					break;
				case 13:
					pGeom = new CEllipse;
					break;
				default:
					break;
			}
			geomlist.AddTail(pGeom);
		}
		delete[]p1;
	}

	POSITION pos = geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(pos);
		pGeom->Serialize(ar, dwVersion);
	}
}

bool PointOnLine(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint, const unsigned long& gap, double& t)
{
	return PointOnLine(point.x, point.y, fpoint.x, fpoint.y, tpoint.x, tpoint.y, gap, t);
}

bool PointOnLine(const double& x, const double& y, double x1, double y1, double x2, double y2, const unsigned long& gap, double& t)
{
	x1 -= x;
	y1 -= y;
	x2 -= x;
	y2 -= y;
	const double sa = x1*x1 + y1*y1;
	const double sb = x2*x2 + y2*y2;
	const double sc = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
	t = (x1*x1 + y1*y1 - x1*x2 - y1*y2)/((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	if(sa <= 0.00001)//pointºÍfpointÖØºÏ
	{
		t = 0.f;
		return true;
	}
	else if(sb <= 0.00001)//pointºÍtpointÖØºÏ
	{
		t = 1.0f;
		return true;
	}
	else if(sc <= 0.00001)//fpointºÍtpointÖØºÏ
	{
		t = 0.f;
		return sqrt((sa + sb)/2) > gap ? false : true;
	}
	else if(sa >= sb + sc)//Èô½ÇAÎª¶Û½Ç£¬¼´P3µãµÄÍ¶Ó°²»ÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Øb¼´¿É
	{
		return sqrt(sb) > gap ? false : true;
	}
	else if(sb >= sa + sc)//Èô½ÇBÎª¶Û½Ç£¬¼´P3µãµÄÍ¶Ó°²»ÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Øa¼´¿É
	{
		return sqrt(sa) > gap ? false : true;
	}
	else//·ñÔòP3µãµÄÍ¶Ó°±ØÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Ø¸ß¼´¿É
	{
		const double a = sqrt(sa);
		const double b = sqrt(sb);
		const double c = sqrt(sc);
		const double factor = (a + b + c)/2;
		const double s = sqrt(factor*(factor - a)*(factor - b)*(factor - c));//º£Â×¹«Ê½£¬Çó³öÃæ»ý//Ð¡³æÔ­´´£¬×ªÔØÇëËµÃ÷³ö´¦£ºyjukh.blogcn.com

		return 2*s/c > gap ? false : true;//·µ»Ø¸ß³¤¶È
	}
}

bool PointOnLine(const CPoint& point, const CPoint& point1, const CPoint& point2)
{
	const int x1 = point1.x - point.x;
	const int y1 = point1.y - point.y;
	const int x2 = point2.x - point.x;
	const int y2 = point2.y - point.y;
	if(x1*y2 == x2*y1)//on one line
		return Within<LONG>(point.x, point1.x, point2.x);
	else
		return false;
}

bool PointOnLine(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint, const unsigned long& gap)
{
	const double x1 = (double)fpoint.x - point.x;
	const double y1 = (double)fpoint.y - point.y;
	const double x2 = (double)tpoint.x - point.x;
	const double y2 = (double)tpoint.y - point.y;
	const double sa = x1*x1 + y1*y1;
	const double sb = x2*x2 + y2*y2;
	const double sc = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
	if(sa <= 0.00001)//pointºÍfpointÖØºÏ
		return true;
	else if(sb <= 0.00001)//pointºÍtpointÖØºÏ
		return true;
	else if(sc <= 0.00001)//fpointºÍtpointÖØºÏ
		return sqrt((sa + sb)/2) > gap ? false : true;
	else if(sa >= sb + sc)//Èô½ÇAÎª¶Û½Ç£¬¼´P3µãµÄÍ¶Ó°²»ÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Øb¼´¿É
		return sqrt(sb) > gap ? false : true;
	else if(sb >= sa + sc)//Èô½ÇBÎª¶Û½Ç£¬¼´P3µãµÄÍ¶Ó°²»ÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Øa¼´¿É
		return sqrt(sa) > gap ? false : true;
	else//·ñÔòP3µãµÄÍ¶Ó°±ØÔÚÏß¶ÎÉÏ£¬Ö»Ðè·µ»Ø¸ß¼´¿É
	{
		const double a = sqrt(sa);
		const double b = sqrt(sb);
		const double c = sqrt(sc);
		const double factor = (a + b + c)/2;
		const double s = sqrt(factor*(factor - a)*(factor - b)*(factor - c));//º£Â×¹«Ê½£¬Çó³öÃæ»ý//Ð¡³æÔ­´´£¬×ªÔØÇëËµÃ÷³ö´¦£ºyjukh.blogcn.com
		return 2*s/c > gap ? false : true;//·µ»Ø¸ß³¤¶È
	}
}

bool LineOnLine(const CPoint& point11, const CPoint& point12, const CPoint& point21, const CPoint& point22)
{
	return PointOnLine(point11, point21, point22) && PointOnLine(point12, point21, point22);
}


bool LineIntersectWithRect(const CPoint& point1, const CPoint& point2, const CRect& rect)
{
	if(point1.x > rect.right && point2.x > rect.right)
		return false;
	else if(point1.x < rect.left && point2.x < rect.left)
		return false;
	else if(point1.y > rect.bottom && point2.y > rect.bottom)
		return false;
	else if(point1.y < rect.top && point2.y < rect.top)
		return false;
	else
		return true;
}

CRect UnionRect(const CRect& rect1, const CRect& rect2)
{
	if(rect1.IsRectNull())
		return rect2;
	else if(rect2.IsRectNull())
		return rect1;
	else
	{
		const int left = min(rect1.left, rect2.left);
		const int top = min(rect1.top, rect2.top);
		const int right = max(rect1.right, rect2.right);
		const int bottom = max(rect1.bottom, rect2.bottom);
		return CRect(left, top, right, bottom);
	}
}

Gdiplus::Rect UnionRect(const Gdiplus::Rect& rect1, const Gdiplus::Rect& rect2)
{
	if(rect1.IsEmptyArea())
		return rect2;
	else if(rect2.IsEmptyArea())
		return rect1;
	else
	{
		const int left = min(rect1.GetLeft(), rect2.GetLeft());
		const int top = min(rect1.GetTop(), rect2.GetTop());
		const int right = max(rect1.GetRight(), rect2.GetRight());
		const int bottom = max(rect1.GetBottom(), rect2.GetBottom());
		return Gdiplus::Rect(left, top, right, bottom);
	}
}

void SplitBezier(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, CPoint L[], CPoint R[])
{
	L[0] = fpoint;
	L[1].x = fpoint.x/2.f + fctrol.x/2.f;
	L[1].y = fpoint.y/2.f + fctrol.y/2.f;
	L[2].x = fpoint.x/4.f + 2*fctrol.x/4.f + tctrol.x/4.f;
	L[2].y = fpoint.y/4.f + 2*fctrol.y/4.f + tctrol.y/4.f;
	L[3].x = fpoint.x/8.f + 3*fctrol.x/8.f + 3*tctrol.x/8.f + tpoint.x/8.f;
	L[3].y = fpoint.y/8.f + 3*fctrol.y/8.f + 3*tctrol.y/8.f + tpoint.y/8.f;

	R[0] = L[3];
	R[1].x = fctrol.x/4.f + 2*tctrol.x/4.f + tpoint.x/4.f;
	R[1].y = fctrol.y/4.f + 2*tctrol.y/4.f + tpoint.y/4.f;
	R[2].x = tctrol.x/2.f + tpoint.x/2.f;
	R[2].y = tctrol.y/2.f + tpoint.y/2.f;
	R[3] = tpoint;
}

void SplitBezier(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, CPoint L[], CPoint R[], double t)
{
	const double s = 1 - t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;

	L[0] = fpoint;
	L[1].x = round(fpoint.x*s + fctrol.x*t);
	L[1].y = round(fpoint.y*s + fctrol.y*t);
	L[2].x = round(fpoint.x*ss + fctrol.x*s*t*2 + tctrol.x*tt);
	L[2].y = round(fpoint.y*ss + fctrol.y*s*t*2 + tctrol.y*tt);
	L[3].x = round(fpoint.x*sss + fctrol.x*ss*t*3 + tctrol.x*s*tt*3 + tpoint.x*ttt);
	L[3].y = round(fpoint.y*sss + fctrol.y*ss*t*3 + tctrol.y*s*tt*3 + tpoint.y*ttt);

	R[0] = L[3];
	R[1].x = round(fctrol.x*ss + tctrol.x*s*t*2 + tpoint.x*tt);
	R[1].y = round(fctrol.y*ss + tctrol.y*s*t*2 + tpoint.y*tt);
	R[2].x = round(tctrol.x*s + tpoint.x*t);
	R[2].y = round(tctrol.y*s + tpoint.y*t);
	R[3] = tpoint;
}

CPoint BezierPoint(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t)
{
	const double s = 1 - t;
	const double tt = t*t;
	const double ss = s*s;
	const double ttt = t*t*t;
	const double sss = s*s*s;
	const long x = round(fpoint.x*sss + fctrol.x*ss*t*3 + tctrol.x*s*tt*3 + tpoint.x*ttt);
	const long y = round(fpoint.y*sss + fctrol.y*ss*t*3 + tctrol.y*s*tt*3 + tpoint.y*ttt);
	return CPoint(x, y);
}

CSizeF BezierDerivate(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t)
{
	const double m11 = -1.0*fpoint.x + 3.0*fctrol.x - 3.0*tctrol.x + tpoint.x;
	const double m12 = -1.0*fpoint.y + 3.0*fctrol.y - 3.0*tctrol.y + tpoint.y;
	const double m21 = 3.0*fpoint.x - 6.0*fctrol.x + 3.0*tctrol.x;
	const double m22 = 3.0*fpoint.y - 6.0*fctrol.y + 3.0*tctrol.y;
	const double m31 = -3.0*fpoint.x + 3.0*fctrol.x;
	const double m32 = -3.0*fpoint.y + 3.0*fctrol.y;

	long a41 = fpoint.x;
	long a42 = fpoint.y;
	const double X = 3*t*t*m11 + 2*t*m21 + m31;
	const double Y = 3*t*t*m12 + 2*t*m22 + m32;

	//	double X1 = -3*(fpoint.x+fctrol.x-tctrol.x-tpoint.x)+6*t*(fpoint.x-2*fctrol.x+tctrol.x)+3*(fctrol.x-fpoint.x);
	//	double Y1 = -3*(fpoint.y+fctrol.y-tctrol.y-tpoint.y)+6*t*(fpoint.y-2*fctrol.y+tctrol.y)+3*(fctrol.y-fpoint.y);

	return CSizeF(X, Y);
}

bool PointOnBezier(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, const unsigned long& gap, double& t)
{
	CRect rect = FPointRect(fpoint, fctrol, tctrol, tpoint);
	rect.InflateRect(gap, gap);
	if(!rect.PtInRect(point))
		return false;
	const double m11 = -1.0*fpoint.x + 3.0*fctrol.x - 3.0*tctrol.x + tpoint.x;
	const double m12 = -1.0*fpoint.y + 3.0*fctrol.y - 3.0*tctrol.y + tpoint.y;
	const double m21 = 3.0*fpoint.x - 6.0*fctrol.x + 3.0*tctrol.x;
	const double m22 = 3.0*fpoint.y - 6.0*fctrol.y + 3.0*tctrol.y;
	const double m31 = -3.0*fpoint.x + 3.0*fctrol.x;
	const double m32 = -3.0*fpoint.y + 3.0*fctrol.y;
	const double a41 = fpoint.x;
	const double a42 = fpoint.y;

	double c[6];
	c[5] = 3*m11*m11 + 3*m12*m12;
	c[4] = 3*m11*m21 + 2*m21*m11 + 3*m12*m22 + 2*m22*m12;
	c[3] = 3*m11*m31 + 2*m21*m21 + m31*m11 + 3*m12*m32 + 2*m22*m22 + m32*m12;
	c[2] = 3*m11*a41 - 3*m11*point.x + 2*m21*m31 + m31*m21 + 3*m12*a42 - 3*m12*point.y + 2*m22*m32 + m32*m22;
	c[1] = 2*m21*a41 - 2*m21*point.x + m31*m31 + 2*m22*a42 - 2*m22*point.y + m32*m32;
	c[0] = m31*a41 - m31*point.x + m32*a42 - m32*point.y;

	double** a = new double* [5];
	for(unsigned int i = 0; i < 5; i++)
	{
		a[i] = new double[5];
		ZeroMemory(a[i], sizeof(double)*5);
	}

	for(unsigned int j = 0; j < 5; j++)
		a[0][j] = -c[5 - j - 1]/c[5];
	for(unsigned int i = 1; i < 5; i++)
		a[i][i - 1] = 1.0;

	double xr[5];
	double xi[5];

	QRroot(a, 5, xr, xi, 0.000001, 60);

	for(unsigned int i = 0; i < 5; i++)
	{
		delete a[i];
	}
	delete[] a;

	for(unsigned int i = 0; i < 5; i++)
	{
		if(xi[i] != 0.f)
			continue;
		if(xr[i] < 0 || xr[i] > 1)
			continue;

		t = xr[i];

		CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
		//		pDC->MoveTo(point);
		//		pDC->LineTo(inter);
		if(::LineLength1(point, inter) <= gap)
			return true;
	}

	return false;
}
unsigned long long Distance2(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint)
{
	const double m11 = -1.0*fpoint.x + 3.0*fctrol.x - 3.0*tctrol.x + tpoint.x;
	const double m12 = -1.0*fpoint.y + 3.0*fctrol.y - 3.0*tctrol.y + tpoint.y;
	const double m21 = 3.0*fpoint.x - 6.0*fctrol.x + 3.0*tctrol.x;
	const double m22 = 3.0*fpoint.y - 6.0*fctrol.y + 3.0*tctrol.y;
	const double m31 = -3.0*fpoint.x + 3.0*fctrol.x;
	const double m32 = -3.0*fpoint.y + 3.0*fctrol.y;
	const double a41 = fpoint.x;
	const double a42 = fpoint.y;

	double c[6];
	c[5] = 3*m11*m11 + 3*m12*m12;
	c[4] = 3*m11*m21 + 2*m21*m11 + 3*m12*m22 + 2*m22*m12;
	c[3] = 3*m11*m31 + 2*m21*m21 + m31*m11 + 3*m12*m32 + 2*m22*m22 + m32*m12;
	c[2] = 3*m11*a41 - 3*m11*point.x + 2*m21*m31 + m31*m21 + 3*m12*a42 - 3*m12*point.y + 2*m22*m32 + m32*m22;
	c[1] = 2*m21*a41 - 2*m21*point.x + m31*m31 + 2*m22*a42 - 2*m22*point.y + m32*m32;
	c[0] = m31*a41 - m31*point.x + m32*a42 - m32*point.y;

	double** a = new double* [5];
	for(unsigned int i = 0; i < 5; i++)
	{
		a[i] = new double[5];
		ZeroMemory(a[i], sizeof(double)*5);
	}

	for(unsigned int j = 0; j < 5; j++)
		a[0][j] = -c[5 - j - 1]/c[5];
	for(unsigned int i = 1; i < 5; i++)
		a[i][i - 1] = 1.0;

	double xr[5];
	double xi[5];

	QRroot(a, 5, xr, xi, 0.000001, 60);

	for(unsigned int i = 0; i < 5; i++)
		delete a[i];
	delete[] a;

	unsigned long long min2 = MAXUINT64;
	for(unsigned int i = 0; i < 5; i++)
	{
		if(xi[i] != 0.f)
			continue;
		if(xr[i] < 0 || xr[i] > 1)
			continue;
		const double t = xr[i];
		if(std::isnan(t))
			continue;

		CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
		min2 = min(min2, ::LineLength2(point, inter));
	}

	return min2;
}
long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint)
{
	return sqrt(Distance2(point, fpoint, fctrol, tctrol, tpoint));
}
long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double& t)
{
	const long m11 = -1.0*fpoint.x + 3.0*fctrol.x - 3.0*tctrol.x + tpoint.x;
	const long m12 = -1.0*fpoint.y + 3.0*fctrol.y - 3.0*tctrol.y + tpoint.y;
	const long m21 = 3.0*fpoint.x - 6.0*fctrol.x + 3.0*tctrol.x;
	const long m22 = 3.0*fpoint.y - 6.0*fctrol.y + 3.0*tctrol.y;
	const long m31 = -3.0*fpoint.x + 3.0*fctrol.x;
	const long m32 = -3.0*fpoint.y + 3.0*fctrol.y;
	const long a41 = fpoint.x;
	const long a42 = fpoint.y;

	double c[6];
	c[5] = 3*m11*m11 + 3*m12*m12;
	c[4] = 3*m11*m21 + 2*m21*m11 + 3*m12*m22 + 2*m22*m12;
	c[3] = 3*m11*m31 + 2*m21*m21 + m31*m11 + 3*m12*m32 + 2*m22*m22 + m32*m12;
	c[2] = 3*m11*a41 - 3*m11*point.x + 2*m21*m31 + m31*m21 + 3*m12*a42 - 3*m12*point.y + 2*m22*m32 + m32*m22;
	c[1] = 2*m21*a41 - 2*m21*point.x + m31*m31 + 2*m22*a42 - 2*m22*point.y + m32*m32;
	c[0] = m31*a41 - m31*point.x + m32*a42 - m32*point.y;

	double** a = new double* [5];
	for(unsigned int i = 0; i < 5; i++)
	{
		a[i] = new double[5];
		ZeroMemory(a[i], sizeof(double)*5);
	}

	for(unsigned int j = 0; j < 5; j++)
		a[0][j] = -c[5 - j - 1]/c[5];
	for(unsigned int i = 1; i < 5; i++)
		a[i][i - 1] = 1.0;

	double xr[5];
	double xi[5];

	QRroot(a, 5, xr, xi, 0.000001, 60);

	for(unsigned int i = 0; i < 5; i++)
		delete a[i];
	delete[] a;

	unsigned long  long min = ULLONG_MAX;
	for(unsigned int i = 0; i < 5; i++)
	{
		if(xi[i] != 0.f)
			continue;
		if(xr[i] < 0 || xr[i] > 1)
			continue;

		t = xr[i];

		CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
		min = min(min, ::LineLength2(point, inter));
	}

	return sqrt(min);
}

bool BezierInter(const CPoint& fpoint1, const CPoint& fctrol1, const CPoint& tctrol1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& fctrol2, const CPoint& tctrol2, const CPoint& tpoint2, double* t1, double* t2, unsigned int& count)
{
	count = 1;

	CRect rect1 = FPointRect(fpoint1, fctrol1, tctrol1, tpoint1);
	CRect rect2 = FPointRect(fpoint2, fctrol2, tctrol2, tpoint2);
	rect1.NormalizeRect();
	rect2.NormalizeRect();

	if(Util::Rect::Intersect(rect1, rect2) == false)
		return false;

	long** a;
	long** b;

	a = new long* [5];
	for(unsigned int i = 0; i < 5; i++)
	{
		a[i] = new long[3];
	}

	b = new long* [5];
	for(unsigned int i = 0; i < 5; i++)
	{
		b[i] = new long[3];
	}

	a[1][1] = -1*fpoint1.x + 3*fctrol1.x - 3*tctrol1.x + tpoint1.x;
	a[1][2] = -1*fpoint1.y + 3*fctrol1.y - 3*tctrol1.y + tpoint1.y;
	a[2][1] = 3*fpoint1.x - 6*fctrol1.x + 3*tctrol1.x;
	a[2][2] = 3*fpoint1.y - 6*fctrol1.y + 3*tctrol1.y;
	a[3][1] = -3*fpoint1.x + 3*fctrol1.x;
	a[3][2] = -3*fpoint1.y + 3*fctrol1.y;
	a[4][1] = fpoint1.x;
	a[4][2] = fpoint1.y;

	b[1][1] = -1*fpoint2.x + 3*fctrol2.x - 3*tctrol2.x + tpoint2.x;
	b[1][2] = -1*fpoint2.y + 3*fctrol2.y - 3*tctrol2.y + tpoint2.y;
	b[2][1] = 3*fpoint2.x - 6*fctrol2.x + 3*tctrol2.x;
	b[2][2] = 3*fpoint2.y - 6*fctrol2.y + 3*tctrol2.y;
	b[3][1] = -3*fpoint2.x + 3*fctrol2.x;
	b[3][2] = -3*fpoint2.y + 3*fctrol2.y;
	b[4][1] = fpoint2.x;
	b[4][2] = fpoint2.y;
	const long k = Dnnse(t1[0], t2[0], 100, 2, a, b);

	for(unsigned int i = 0; i < 5; i++)
	{
		delete a[i];
	}
	delete[] a;

	for(unsigned int i = 0; i < 5; i++)
	{
		delete b[i];
	}
	delete b;

	if(k >= 0 && k < 100)
		return true;
	else
		return false;
}

bool LineInter(const CPoint& fpoint1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& tpoint2, double& t1, double& t2)
{
	const double deltaX1 = tpoint1.x - fpoint1.x;
	const double deltaY1 = tpoint1.y - fpoint1.y;
	const double deltaX2 = fpoint2.x - tpoint2.x;
	const double deltaY2 = fpoint2.y - tpoint2.y;
	const double dta = deltaX1*deltaY2 - deltaX2*deltaY1;
	t1 = ((fpoint2.x - fpoint1.x)*deltaY2 - (fpoint2.y - fpoint1.y)*deltaX2)/dta;
	t2 = (deltaX1*(fpoint2.y - fpoint1.y) - deltaY1*(fpoint2.x - fpoint1.x))/dta;

	if(t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
		return true;
	else
		return false;
	/*
	CPoint A = fpoint1-fpoint1;
	CPoint B = tpoint1-fpoint1;
	CPoint C = fpoint2-fpoint1;
	CPoint D = tpoint2-fpoint1;

	double xDelta = D.x-C.x;
	double yDelta = D.y-C.y;

	if(B.x*yDelta == B.y*xDelta)
	return false;

	t1 = (float)(C.x*yDelta - C.y*xDelta)/(B.x*yDelta - B.y*xDelta);
	t2 = (float)(C.x*B.y      - C.y*B.x )/(B.x*yDelta - B.y*xDelta);

	if(t1 <0 || t1 >1 || t2 <0 || t2 >1)
	return false;

	return true;
	*/
}

CRect FPointRect(const CPoint& point1, const CPoint& point2, const CPoint& point3, const CPoint& point4)
{
	CRect rect;
	rect.left = min(min(point1.x, point2.x), min(point3.x, point4.x));
	rect.top = min(min(point1.y, point2.y), min(point3.y, point4.y));
	rect.right = max(max(point1.x, point2.x), max(point3.x, point4.x));
	rect.bottom = max(max(point1.y, point2.y), max(point3.y, point4.y));
	return rect;
}

std::vector<int> XRadialBezier(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount)
{
	std::vector<int> vertices;
	for(unsigned int I = 0; I < ptCount - 1; I++)
	{
		CPoint fpoint = points[I*3 + 1];
		CPoint fctrol = points[I*3 + 2];
		CPoint tctrol = points[(I + 1)*3];
		CPoint tpoint = points[(I + 1)*3 + 1];
		if(I == 24)
		{
			//fpoint.Offset(-point);
			//fctrol.Offset(-point);
			//tctrol.Offset(-point);
			//tpoint.Offset(-point);
		}
		const CRect rect = FPointRect(fpoint, fctrol, tctrol, tpoint);
		if(point.y > rect.bottom)
			continue;
		else if(point.y < rect.top)
			continue;
		else if(direction == Direction::DI_Right && point.x > rect.right)
			continue;
		else if(direction == Direction::DI_Left && point.x < rect.left)
			continue;
		const double ay = -1.0f*fpoint.y + 3.0f*fctrol.y - 3.0f*tctrol.y + tpoint.y;
		const double by = 3.0f*fpoint.y - 6.0f*fctrol.y + 3.0f*tctrol.y;
		const double cy = -3.0f*fpoint.y + 3.0f*fctrol.y;
		const double dy = fpoint.y - point.y;

		int roots = 0;
		const double* pRoot = Q3Roots(ay, by, cy, dy, roots);
		for(unsigned int index = 0; index < roots; index++)
		{
			if(pRoot[index] == -1.f)
				continue;
			else if(pRoot[index] < 0.f)
				continue;
			else if(pRoot[index] > 1.f)
				continue;

			CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, pRoot[index]);
			const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, pRoot[index]);

			if(direction == Direction::DI_Right && inter.x < point.x)
				continue;
			else if(direction == Direction::DI_Left && inter.x > point.x)
				continue;
			else if(derivate.cy == 0.f)
				continue;

			if(inter == fpoint)
			{
				if(direction == Direction::DI_Up && derivate.cy < 0.f)
					continue;
				else if(direction == Direction::DI_Down && derivate.cy > 0.f)
					continue;
			}
			if(inter == tpoint)
			{
				if(direction == Direction::DI_Up && derivate.cy > 0.f)
					continue;
				else if(direction == Direction::DI_Down && derivate.cy < 0.f)
					continue;
			}
			vertices.push_back(inter.x);
		}

		delete[] pRoot;
	}
	return vertices;
}

std::vector<int> YRadialBezier(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount)
{
	std::vector<int> vertices;
	for(unsigned int I = 0; I < ptCount - 1; I++)
	{
		CPoint fpoint = points[I*3 + 1];
		CPoint fctrol = points[I*3 + 2];
		CPoint tctrol = points[(I + 1)*3];
		CPoint tpoint = points[(I + 1)*3 + 1];
		const CRect rect = FPointRect(fpoint, fctrol, tctrol, tpoint);
		if(point.x < rect.left)
			continue;
		else if(point.x > rect.right)
			continue;
		else if(direction == Direction::DI_Up && point.y > rect.bottom)
			continue;
		else if(direction == Direction::DI_Down && point.y < rect.top)
			continue;
		const double ay = -1.0f*fpoint.x + 3.0f*fctrol.x - 3.0f*tctrol.x + tpoint.x;
		const double by = 3.0f*fpoint.x - 6.0f*fctrol.x + 3.0f*tctrol.x;
		const double cy = -3.0f*fpoint.x + 3.0f*fctrol.x;
		const double dy = fpoint.x - point.x;

		int roots = 0;
		const double* pRoot = Q3Roots(ay, by, cy, dy, roots);
		for(unsigned int index = 0; index < roots; index++)
		{
			if(pRoot[index] == -1.f)
				continue;
			else if(pRoot[index] < 0.f)
				continue;
			else if(pRoot[index] > 1.f)
				continue;

			CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, pRoot[index]);
			const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, pRoot[index]);

			if(direction == Direction::DI_Up && inter.y < point.y)
				continue;
			else if(direction == Direction::DI_Down && inter.y > point.y)
				continue;
			else if(derivate.cx == 0.f)
				continue;

			if(inter == fpoint)
			{
				if(direction == Direction::DI_Up && derivate.cx < 0.f)
					continue;
				else if(direction == Direction::DI_Down && derivate.cx > 0.f)
					continue;
			}
			if(inter == tpoint)
			{
				if(direction == Direction::DI_Up && derivate.cx > 0.f)
					continue;
				else if(direction == Direction::DI_Down && derivate.cx < 0.f)
					continue;
			}
			vertices.push_back(inter.y);
		}

		delete[] pRoot;
	}
	return vertices;
}

std::vector<int> XRadialPoly(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount)
{
	std::vector<int> vertices;
	for(unsigned int I = 0; I < ptCount - 1; I++)
	{
		const CPoint fpoint = points[I];
		const CPoint tpoint = points[I + 1];

		CRect rect = CRect(fpoint.x, fpoint.y, tpoint.x, tpoint.y);
		rect.NormalizeRect();
		if(direction == Direction::DI_Left && point.x < rect.left)
			continue;
		else if(direction == Direction::DI_Right && point.x > rect.right)
			continue;
		else if((double)(point.y - rect.top)*(point.y - rect.bottom) > 0)
			continue;
		else if(tpoint.y == fpoint.y)
		{
			if(fpoint.y == point.y)
				vertices.push_back(fpoint.x);
			continue;
		}
		const double t = (double)(point.y - fpoint.y)/(tpoint.y - fpoint.y);
		if(t < 0 || t > 1)
			continue;
		const long x = fpoint.x + round(t*(tpoint.x - fpoint.x));
		vertices.push_back(x);
	}
	return vertices;
}

std::vector<int> YRadialPoly(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount)
{
	std::vector<int> vertices;
	for(unsigned int I = 0; I < ptCount - 1; I++)
	{
		const CPoint fpoint = points[I];
		const CPoint tpoint = points[I + 1];

		CRect rect = CRect(fpoint.x, fpoint.y, tpoint.x, tpoint.y);
		rect.NormalizeRect();
		if(direction == Direction::DI_Up && point.y > rect.bottom)
			continue;
		else if(direction == Direction::DI_Down && point.y < rect.top)
			continue;
		else if((double)(point.x - rect.left)*(point.x - rect.right) > 0)
			continue;
		else if(tpoint.x == fpoint.x)
		{
			if(fpoint.x == point.x)
				vertices.push_back(fpoint.y);
			continue;
		}
		const double t = (double)(point.x - fpoint.x)/(tpoint.x - fpoint.x);
		if(t < 0 || t > 1)
			continue;
		const long y = fpoint.y + round(t*(tpoint.y - fpoint.y));
		vertices.push_back(y);
	}
	return vertices;
}

unsigned long long LineLength2(const CPoint& point1, const CPoint& point2)
{
	const unsigned long long dx = abs(point1.x - point2.x);
	const unsigned long long dy = abs(point1.y - point2.y);
	return dx*dx + dy*dy;
}
unsigned long long LineLength2(const Gdiplus::Point& point1, const Gdiplus::Point& point2)
{
	const unsigned long long dx = abs(point1.X - point2.X);
	const unsigned long long dy = abs(point1.Y - point2.Y);
	return dx*dx + dy*dy;
}
double LineLength2(const Gdiplus::PointF& point1, const Gdiplus::PointF& point2)
{
	const double dx = point1.X - point2.X;
	const double dy = point1.Y - point2.Y;
	return dx*dx + dy*dy;
}

double LineLength1(const CPoint& point1, const CPoint& point2)
{
	const double dx = (double)point1.x - point2.x;
	const double dy = (double)point1.y - point2.y;
	return sqrt(dx*dx + dy*dy);
}
double LineLength1(const CPointF& point1, const CPointF& point2)
{
	const double dx = (double)point1.x - point2.x;
	const double dy = (double)point1.y - point2.y;
	return sqrt(dx*dx + dy*dy);
}
double LineLength1(const CPoint& point1, const Gdiplus::Point& point2)
{
	const double dx = (double)point1.x - point2.X;
	const double dy = (double)point1.y - point2.Y;
	return sqrt(dx*dx + dy*dy);
}
double LineLength1(const Gdiplus::Point& point1, const Gdiplus::Point& point2)
{
	const double dx = (double)point1.X - point2.X;
	const double dy = (double)point1.Y - point2.Y;
	return sqrt(dx*dx + dy*dy);
}
double LineLength1(const Gdiplus::PointF& point1, const Gdiplus::PointF& point2)
{
	const double dx = (double)point1.X - point2.X;
	const double dy = (double)point1.Y - point2.Y;
	return sqrt(dx*dx + dy*dy);
}

double LineAngle(const CPoint& fpoint, const CPoint& tpoint)
{
	const long Dy = tpoint.y - fpoint.y;
	const long Dx = tpoint.x - fpoint.x;
	if(Dy == 0 && Dx == 0)
		return 0;

	double dAngle = atan2((double)Dy, (double)Dx);
	if(dAngle < 0.0f)
		dAngle += 2*M_PI;

	return dAngle;
}

double LineAngle(const Gdiplus::PointF& fpoint, const Gdiplus::PointF& tpoint)
{
	const long Dy = tpoint.Y - fpoint.Y;
	const long Dx = tpoint.X - fpoint.X;
	if(Dy == 0 && Dx == 0)
		return 0;

	double dAngle = atan2((double)Dy, (double)Dx);
	if(dAngle < 0.0f)
		dAngle += 2*M_PI;

	return dAngle;
}

double GetInclination(const CPoint& pointa1, const CPoint& pointa2, CPoint pointb1, CPoint pointb2)
{//´Ëº¯ÊýÎªÇóa1µ½a2µÄÓÐÏòÏßÓëb1µ½b2µÄÓÐÏòÏß¼äµÄÆ«×ª½Ç
	double angle = 0;
	const CPoint pointoffset = pointb1 - pointa1;
	pointb2 = pointb2 - pointoffset;
	pointb1 = pointa1;
	//Çó½»½Ç
	const double powa1a2 = pow((double)(pointa1.x - pointa2.x), 2) + pow((double)(pointa1.y - pointa2.y), 2);
	const double a1a2 = sqrt(powa1a2);
	const double powb1b2 = pow((double)(pointb1.x - pointb2.x), 2) + pow((double)(pointb1.y - pointb2.y), 2);
	const double b1b2 = sqrt(powb1b2);
	const double a2b2 = pow((double)(pointa2.x - pointb2.x), 2) + pow((double)(pointa2.y - pointb2.y), 2);
	angle = acos((powa1a2 + powb1b2 - a2b2)/(2*a1a2*b1b2));
	//Ê×ÏÈÅÐ¶ÏÊÇÕý½Ç»¹ÊÇ¸º½Ç
	if(pointa2.x != pointa1.x)
	{
		const double ka12 = (double)(pointa2.y - pointa1.y)/(pointa2.x - pointa1.x);
		double equationvalue = (pointa2.y - pointb2.y)*(pointa2.x - pointa1.x) - (pointa2.x - pointb2.x)*(pointa2.y - pointa1.y);
		if(pointa2.x > pointa1.x)
			equationvalue = -equationvalue;
		if(ka12 >= 0)
		{
			if(pointa2.y >= pointa1.y)
			{
				if(equationvalue > 0)
					angle = -angle;
			}
			else
			{
				if(equationvalue < 0)
					angle = -angle;
			}
		}
		else
		{
			if(pointa2.y >= pointa1.y)
			{
				if(equationvalue < 0)
					angle = -angle;
			}
			else
			{
				if(equationvalue > 0)
					angle = -angle;
			}
		}
	}
	return angle;
}
unsigned long long Distance2(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint)
{
	const int vx = point.x - fpoint.x;
	const int vy = point.y - fpoint.y;
	const int dx = tpoint.x - fpoint.x;
	const int dy = tpoint.y - fpoint.y;
	const double dot = (double)vx*dx + (double)vy*dy;
	const double lensq = (double)dx*dx + (double)dy*dy;
	const double ratio = lensq == 0 ? -1 : dot/lensq;

	int x;
	int y;
	if(ratio < 0) {
		x = fpoint.x;
		y = fpoint.y;
	}
	else if(ratio > 1) {
		x = tpoint.x;
		y = tpoint.y;
	}
	else {
		x = fpoint.x + ratio*dx;
		y = fpoint.y + ratio*dy;
	}
	const int deltax = point.x - x;
	const int deltay = point.y - y;
	return (unsigned long long)deltax*deltax+ (unsigned long long)deltay*deltay;
}

long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint)
{
	const double A = (double)(point.x - fpoint.x)*(point.x - fpoint.x) + (double)(point.y - fpoint.y)*(point.y - fpoint.y);
	const double B = (double)(point.x - tpoint.x)*(point.x - tpoint.x) + (double)(point.y - tpoint.y)*(point.y - tpoint.y);
	const double C = (double)(fpoint.x - tpoint.x)*(fpoint.x - tpoint.x) + (double)(fpoint.y - tpoint.y)*(fpoint.y - tpoint.y);
	const double min = min(A, B);
	const double max = max(A, B);
	if(min + C > max)
	{
		const long distance = ((fpoint.y - tpoint.y)*(point.x - fpoint.x) - (fpoint.x - tpoint.x)*(point.y - fpoint.y))/sqrt((double)C);
		return abs(distance);
	}
	else
		return sqrt(min);
}
double Distance1(const CPointF& point, const CPointF& fpoint, const CPointF& tpoint)
{
	const double A = pow((double)(point.x - fpoint.x), 2) + pow((double)(point.y - fpoint.y), 2);
	const double B = pow((double)(point.x - tpoint.x), 2) + pow((double)(point.y - tpoint.y), 2);
	const double C = pow((double)(fpoint.x - tpoint.x), 2) + pow((double)(fpoint.y - tpoint.y), 2);
	const double min = min(A, B);
	const double max = max(A, B);
	if(min + C > max)
	{
		const double distance = ((double)(fpoint.y - tpoint.y)*(point.x - fpoint.x) + (double)(tpoint.x - fpoint.x)*(point.y - fpoint.y))/sqrt(C);
		return abs(distance);
	}
	else
		return sqrt(min);
}

long Vertical(CPoint& point, const CPoint& point1, const CPoint& point2)
{
	const CPoint point0 = point;
	const double D1 = pow((double)(point1.x - point0.x), 2) + pow((double)(point1.y - point0.y), 2);
	const double D2 = pow((double)(point2.x - point0.x), 2) + pow((double)(point2.y - point0.y), 2);
	const double D0 = pow((double)(point2.x - point1.x), 2) + pow((double)(point2.y - point1.y), 2);

	if((D1 >= D2 && D1 < D2 + D0) || (D2 >= D1 && D2 < D1 + D0))
	{
		const double A = point1.y - point2.y;
		const double B = point2.x - point1.x;
		const double C = B*point1.y + A*point1.x;
		const double distance = (A*point0.x + B*point0.y - C)/sqrt(A*A + B*B);

		point.x = (B*B*point0.x - A*B*point0.y + A*C)/(A*A + B*B);
		point.y = (A*A*point0.y - A*B*point0.x + B*C)/(A*A + B*B);

		return abs(distance);
	}
	else if(D1 < D2)
	{
		point = point1;

		return sqrt(D1);
	}
	else
	{
		point = point2;

		return sqrt(D2);
	}
}

bool BezierLineInter(const CPoint& fpoint1, const CPoint& fctrol1, const CPoint& tctrol1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& tpoint2, double* t1, double* t2, unsigned int& count)
{
	double m11 = -1.0*fpoint1.x + 3.0*fctrol1.x - 3.0*tctrol1.x + tpoint1.x;
	double m12 = -1.0*fpoint1.y + 3.0*fctrol1.y - 3.0*tctrol1.y + tpoint1.y;

	double m21 = 3.0*fpoint1.x - 6.0*fctrol1.x + 3.0*tctrol1.x;
	double m22 = 3.0*fpoint1.y - 6.0*fctrol1.y + 3.0*tctrol1.y;

	double m31 = -3.0*fpoint1.x + 3.0*fctrol1.x;
	double m32 = -3.0*fpoint1.y + 3.0*fctrol1.y;

	double a41 = fpoint1.x;
	double a42 = fpoint1.y;
	const CPoint A = fpoint2;
	const CPoint B = tpoint2 - fpoint2;
	if(B.x == 0 && B.y == 0)
		return false;

	m11 *= B.y;
	m12 *= B.x;

	m21 *= B.y;
	m22 *= B.x;

	m31 *= B.y;
	m32 *= B.x;

	a41 *= B.y;
	a42 *= B.x;
	const double ay = m11 - m12;
	const double by = m21 - m22;
	const double cy = m31 - m32;
	const double dy = a41 - a42 - (A.x*B.y - A.y*B.x);

	int roots = 0;
	const double* pRoot = Q3Roots(ay, by, cy, dy, roots);

	count = 0;
	for(unsigned int index = 0; index < roots; index++)
	{
		if(pRoot[index] == -1.f)
			continue;
		else if(pRoot[index] < 0.f)
			continue;
		else if(pRoot[index] > 1.f)
			continue;

		t1[count] = pRoot[index];
		const CPoint point = BezierPoint(fpoint1, fctrol1, tctrol1, tpoint1, pRoot[index]);
		if(B.x != 0)
			t2[count] = (double)(point.x - A.x)/B.x;
		else
			t2[count] = (double)(point.y - A.y)/B.y;

		if(t2[count] < 0 || t2[count] > 1)
			continue;

		count++;
	}

	delete[] pRoot;

	if(count > 0)
		return true;
	else
		return false;
}

bool LineClip(const CRect& rect, CPoint& point1, CPoint& point2)
{
	BYTE b0 = 0, b1 = 0, bQ = 0;

	// CHECK FIRST POINT
	if(point1.x < rect.left)
		b0 |= 1;
	else if(point1.x > rect.right)
		b0 |= 2;
	if(point1.y > rect.bottom)
		b0 |= 4;
	else if(point1.y < rect.top)
		b0 |= 8;

	// CHECK SECOND POINT
	if(point2.x < rect.left)
		b1 |= 1;
	else if(point2.x > rect.right)
		b1 |= 2;
	if(point2.y > rect.bottom)
		b1 |= 4;
	else if(point2.y < rect.top)
		b1 |= 8;

	if(b0 != 0 && b1 == 0)
	{
		if(point2.x == rect.left && (b0 & 1) == 1)
			return false;
		if(point2.x == rect.right && (b0 & 2) == 2)
			return false;
		if(point2.y == rect.bottom && (b0 & 4) == 4)
			return false;
		if(point2.y == rect.top && (b0 & 8) == 8)
			return false;
	}
	if(b1 != 0 && b0 == 0)
	{
		if(point1.x == rect.left && (b1 & 1) == 1)
			return false;
		if(point1.x == rect.right && (b1 & 2) == 2)
			return false;
		if(point1.y == rect.bottom && (b1 & 4) == 4)
			return false;
		if(point1.y == rect.top && (b1 & 8) == 8)
			return false;
	}

	CPoint point;
	while(b0 || b1)
	{
		if(b0 & b1)
			return false;
		bQ = b0 ? b0 : b1;

		if(1 & bQ)
		{
			point.x = rect.left;
			point.y = round(point1.y + (double)(point2.y - point1.y)*(rect.left - point1.x)/(point2.x - point1.x));
		}
		else if(2 & bQ)
		{
			point.x = rect.right;
			point.y = round(point1.y + (double)(point2.y - point1.y)*(rect.right - point1.x)/(point2.x - point1.x));
		}
		else if(4 & bQ)
		{
			point.x = round(point2.x + (double)(point2.x - point1.x)*(rect.bottom - point2.y)/(point2.y - point1.y));
			point.y = rect.bottom;
		}
		else if(8 & bQ)
		{
			point.x = round(point2.x + (double)(point2.x - point1.x)*(rect.top - point2.y)/(point2.y - point1.y));
			point.y = rect.top;
		}

		if(bQ == b0)
		{
			point1 = point;

			b0 = 0;
			if(point.x < rect.left)
				b0 |= 1;
			else if(point.x > rect.right)
				b0 |= 2;
			if(point.y > rect.bottom)
				b0 |= 4;
			else if(point.y < rect.top)
				b0 |= 8;
		}
		else
		{
			point2 = point;

			b1 = 0;
			if(point.x < rect.left)
				b1 |= 1;
			else if(point.x > rect.right)
				b1 |= 2;
			if(point.y > rect.bottom)
				b1 |= 4;
			else if(point.y < rect.top)
				b1 |= 8;
		}
	}

	return true;
}
void GetMoreBezierT(double st, double et, double A_x, double B_x, double C_x, double D_x, double A_y, double B_y, double C_y, double D_y, CPoint fpoint, CPoint tpoint, double* RecordT, const unsigned int& tolerance)
{
	//	ASSERT(tpoint.x-fpoint.x!=0);//Îª0Ê±²»¿Éµ¼
	const double k0 = tpoint.x - fpoint.x == 0 ? (double)(tpoint.y - fpoint.y) : (double)(tpoint.y - fpoint.y)/(double)(tpoint.x - fpoint.x);//µÃµ½Ê×Ä©µãÐ±ÂÊ

	//»¯¼ò·½³ÌµÃ(µ¼ÊýÏµÊý)
	const double A = 3*A_x*k0 - 3*A_y;
	const double B = 2*B_x*k0 - 2*B_y;
	const double C = C_x*k0 - C_y;
	//get first t
	const double Delta = B*B - 4*A*C;
	double root1 = 0, root2 = 0;
	double t;
	const double LA = k0;
	const double LB = -1;
	const double LC = (double)(fpoint.y) - k0*(double)(fpoint.x);//LA~LCÎªÆðÄ©µãµãÐ±Ê½·½³ÌµÄÏµÊý
	if(Delta >= 0)
	{
		//		ASSERT(A!=0);
		if(A == 0)//ÍË»¯·½³Ì
		{
			if(B != 0)
				root1 = root2 = -C/B;//¶þ´Î·½³Ì
			else
				return;//Ò»´Î·½³Ì¼´Ö±Ïß
		}
		else
		{
			root1 = (-B + sqrt(Delta))/(2*A);
			root2 = (-B - sqrt(Delta))/(2*A);
		}

		if(root1 >= et || root1 <= st)
		{
			if(root2 >= et || root2 <= st)
				return;
			else
				t = root2;
		}
		else if(root2 >= et || root2 <= st)
			t = root1;
		else
		{
			CPoint point1, point2;
			point1.x = round((double)(A_x*root1*root1*root1 + B_x*root1*root1 + C_x*root1 + D_x));
			point1.y = round((double)(A_y*root1*root1*root1 + B_y*root1*root1 + C_y*root1 + D_y));
			point2.x = round((double)(A_x*root2*root2*root2 + B_x*root2*root2 + C_x*root2 + D_x));
			point2.y = round((double)(A_y*root2*root2*root2 + B_y*root2*root2 + C_y*root2 + D_y));
			double h1, h2;
			if(LA*LA + LB*LB != 0)
			{
				h1 = std::abs(LA*point1.x + LB*point1.y + LC)/sqrt(LA*LA + LB*LB);//|point1*x0+b*y0+c|/sqrt(a*a+b*b)Îªµãµ½Ö±Ïß¾àÀë
				h2 = std::abs(LA*point2.x + LB*point2.y + LC)/sqrt(LA*LA + LB*LB);//|a*x0+b*y0+c|/sqrt(a*a+b*b)Îªµãµ½Ö±Ïß¾àÀë
				t = (h1 > h2) ? root1 : root2;
			}
			else
			{
				AfxMessageBox(_T("Divided by zero"));
				return;
			}
		}

		if(LA*LA + LB*LB != 0)
		{
			CPoint point0;
			point0.x = round((double)(A_x*t*t*t + B_x*t*t + C_x*t + D_x));
			point0.y = round((double)(A_y*t*t*t + B_y*t*t + C_y*t + D_y));
			const double h = std::abs(LA*point0.x + LB*point0.y + LC)/sqrt(LA*LA + LB*LB);//|a*x0+b*y0+c|/sqrt(a*a+b*b)Îªµãµ½Ö±Ïß¾àÀë
			if(h > tolerance)//µÃµ½ÖÐ¼äµã
			{
				if(RecordT[0] >= RecordT[1] - 2)
				{
					RecordT = (double*)realloc(RecordT, sizeof(double)*((int)(RecordT[1]) + 10));
					RecordT[1] += 10;
				}
				RecordT[(int)(RecordT[0]) + 2] = t;
				RecordT[0]++;
				//·Ö³ÉÇ°ºóÁ½¶Î½øÐÐµÝ¹é
				GetMoreBezierT(st, t, A_x, B_x, C_x, D_x, A_y, B_y, C_y, D_y, fpoint, point0, RecordT, tolerance);//Ç°¶Î
				GetMoreBezierT(t, et, A_x, B_x, C_x, D_x, A_y, B_y, C_y, D_y, point0, tpoint, RecordT, tolerance);//ºó¶Î
			}
		}
		else
			AfxMessageBox(_T("Divided by zero!"));
	}
}

CArchive& AFXAPI operator <<(CArchive& ar, CPointF& point)
{
	ar.Write(&point, sizeof(CPointF));
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CPointF& point)
{
	ar.Read(&point, sizeof(CPointF));
	return ar;
}
Gdiplus::RectF GetTagRect(const Gdiplus::PointF& origin, const Gdiplus::SizeF& textSize, const HALIGN& hAlign, const VALIGN& vAlign)
{
	float x = origin.X;
	float y = origin.Y;
	switch(hAlign)
	{
		case HALIGN::HA_LEFT:
			break;
		case HALIGN::HA_CENTER:
			x -= textSize.Width/2;
			break;
		case HALIGN::HA_RIGHT:
			x -= textSize.Width;
			break;
	}
	switch(vAlign)
	{
		case VALIGN::VA_TOP:
			y += textSize.Height;
			break;
		case VALIGN::VA_CENTER:
			y += textSize.Height/2;
			break;
		case VALIGN::VA_BOTTOM:
			break;
	}
	return Gdiplus::RectF(x, y-textSize.Height, textSize.Width, textSize.Height);
}
Gdiplus::RectF GetTagRect(const Gdiplus::RectF& markRect, const Gdiplus::SizeF& textSize, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	Gdiplus::PointF origin;
	switch(anchor)
	{
		case ANCHOR_1:
			origin.X = markRect.GetLeft();
			origin.Y = markRect.GetTop();
			break;
		case ANCHOR_2:
			origin.X = markRect.GetLeft()+markRect.Width/2;
			origin.Y = markRect.GetTop();
			break;
		case ANCHOR_3:
			origin.X = markRect.GetRight();
			origin.Y = markRect.GetTop();
			break;
		case ANCHOR_4:
			origin.X = markRect.GetLeft();
			origin.Y = markRect.GetTop()+markRect.Height/2;
			break;
		case ANCHOR_5:
			origin.X = markRect.GetLeft()+markRect.Width/2;
			origin.Y = markRect.GetTop()+markRect.Height/2;
			break;
		case ANCHOR_6:
			origin.X = markRect.GetRight();
			origin.Y = markRect.GetTop()+markRect.Height/2;
			break;
		case ANCHOR_7:
			origin.X = markRect.GetLeft();
			origin.Y = markRect.GetBottom();
			break;
		case ANCHOR_8:
			origin.X = markRect.GetLeft()+markRect.Width/2;
			origin.Y = markRect.GetBottom();
			break;
		case ANCHOR_9:
			origin.X = markRect.GetRight();
			origin.Y = markRect.GetBottom();
			break;
	}
	return ::GetTagRect(origin, textSize, hAlign, vAlign);
}

CPoint ReflectionPoint(const CPoint& point1, const CPoint& point2)
{
	const int x = 2*point2.x - point1.x;
	const int y = 2*point2.y - point1.y;
	return CPoint(x, y);
}

void AFXAPI operator <<(CArchive& ar, TEXT_ALIGN& align)
{
	ar << (BYTE)align;
}

void AFXAPI operator >>(CArchive& ar, TEXT_ALIGN& align)
{
	BYTE byte;
	ar >> byte;
	align = (TEXT_ALIGN)byte;
}

void AFXAPI operator <<(CArchive& ar, ANCHOR& anchor)
{
	ar << (BYTE)anchor;
}

void AFXAPI operator >>(CArchive& ar, ANCHOR& anchor)
{
	BYTE byte;
	ar >> byte;
	anchor = (ANCHOR)byte;
}

void AFXAPI operator <<(CArchive& ar, HALIGN& align)
{
	ar << (BYTE)align;
}

void AFXAPI operator >>(CArchive& ar, HALIGN& align)
{
	BYTE byte;
	ar >> byte;
	align = (HALIGN)byte;
}

void AFXAPI operator <<(CArchive& ar, VALIGN& align)
{
	ar << (BYTE)align;
}

void AFXAPI operator >>(CArchive& ar, VALIGN& align)
{
	BYTE byte;
	ar >> byte;
	align = (VALIGN)byte;
}

void AFXAPI operator <<(CArchive& ar, TEXT_ANCHOR& anchor)
{
	ar << (BYTE)anchor;
}

void AFXAPI operator >>(CArchive& ar, TEXT_ANCHOR& anchor)
{
	BYTE byte;
	ar >> byte;
	anchor = (TEXT_ANCHOR)byte;
}

__declspec(dllexport) ANCHOR PrioritizedAnchors[] = {ANCHOR_6, ANCHOR_4, ANCHOR_2, ANCHOR_8, ANCHOR_3, ANCHOR_9, ANCHOR_1, ANCHOR_7};
