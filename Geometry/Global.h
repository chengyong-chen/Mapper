#pragma once

#include <afxtempl.h>
#include <gdiplus.h>
#include "Geom.h"

#include "../Public/Global.h"

class CPoly;
class CGeom;
class CSpot;
enum ANCHOR : BYTE;

#include <vector>
using namespace std;

typedef struct tagSegment
{
	CPoint* pPoint;
	const unsigned int nAnchors;
	BYTE type;
} Segment;

enum Direction : BYTE
{
	DI_Left,
	DI_Right,
	DI_Up,
	DI_Down,
	DI_Horizontal,
	DI_Vertical
};
enum ANCHOR : BYTE
{
	ANCHOR_0 = 0,
	ANCHOR_1 = 1,
	ANCHOR_2,
	ANCHOR_3,
	ANCHOR_4,
	ANCHOR_5,
	ANCHOR_6,
	ANCHOR_7,
	ANCHOR_8,
	ANCHOR_9
};
enum TEXT_ALIGN : BYTE
{
	TAL_START = 0,
	TAL_END,
	TAL_LEFT,
	TAL_RIGHT,
	TAL_CENTER,
	TAL_JUSTIFY
};

enum TEXT_ANCHOR : BYTE
{
	TAC_START = 0,
	TAC_MIDDLE,
	TAC_END,
};
struct IMatrix2
{
	double m11=1; 
	double m12=0;
	double m21=0; 
	double m22=1;
	IMatrix2() 
	{

	}
	bool operator==(const IMatrix2& orther) const
	{
		return m11 ==orther.m11&&m12 ==orther.m12&&m21 ==orther.m21&&m22 ==orther.m22;
	}
};

void AFXAPI operator <<(CArchive& ar, ANCHOR& anchor);
void AFXAPI operator >>(CArchive& ar, ANCHOR& anchor);

void AFXAPI operator <<(CArchive& ar, TEXT_ALIGN& align);
void AFXAPI operator >>(CArchive& ar, TEXT_ALIGN& align);

void AFXAPI operator <<(CArchive& ar, HALIGN& align);
void AFXAPI operator >>(CArchive& ar, HALIGN& align);

void AFXAPI operator <<(CArchive& ar, VALIGN& align);
void AFXAPI operator >>(CArchive& ar, VALIGN& align);

void AFXAPI operator <<(CArchive& ar, TEXT_ANCHOR& anchor);
void AFXAPI operator >>(CArchive& ar, TEXT_ANCHOR& anchor);

__declspec(dllexport) double LineAngle(const CPoint& fpoint, const CPoint& tpoint);
__declspec(dllexport) double LineAngle(const Gdiplus::PointF& fpoint, const Gdiplus::PointF& tpoint);
__declspec(dllexport) double LineLength1(const CPoint& point1, const CPoint& point2);
__declspec(dllexport) double LineLength1(const CPointF& point1, const CPointF& point2);
__declspec(dllexport) double LineLength1(const Gdiplus::Point& point1, const Gdiplus::Point& point2);
__declspec(dllexport) double LineLength1(const Gdiplus::PointF& point1, const Gdiplus::PointF& point2);

__declspec(dllexport) unsigned long long LineLength2(const CPoint& point1, const CPoint& point2);
__declspec(dllexport) unsigned long long LineLength2(const Gdiplus::Point& point1, const Gdiplus::Point& point2);
__declspec(dllexport) unsigned long long LineLength2(const CPoint& point1, const Gdiplus::PointF& point2);
__declspec(dllexport) double LineLength2(const Gdiplus::PointF& point1, const Gdiplus::PointF& point2);

__declspec(dllexport) double GetInclination(const CPoint& pointa1, const CPoint& pointa2, CPoint pointb1, CPoint pointb2);
__declspec(dllexport) CPoint ReflectionPoint(const CPoint& point1, const CPoint& point2);
__declspec(dllexport) bool LineIntersectWithRect(const CPoint& point1, const CPoint& point2, const CRect& rect);

__declspec(dllexport) CPoint RegulizePoint(const CPoint& point1, const CPoint& point2, const unsigned short& nDivCount);
__declspec(dllexport) long Vertical(CPoint& point, const CPoint& fpoint, const CPoint& tpoint);
__declspec(dllexport) long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint);
__declspec(dllexport) double Distance1(const CPointF& point, const CPointF& fpoint, const CPointF& tpoint);
__declspec(dllexport) long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint);
__declspec(dllexport) long Distance1(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double& t);

__declspec(dllexport) unsigned long long Distance2(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint);
__declspec(dllexport) unsigned long long Distance2(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint);
//__declspec(dllexport) unsigned long long Distance2(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double& t);
__declspec(dllexport) bool PointOnLine(const CPoint& point, const CPoint& point1, const CPoint& point2);
__declspec(dllexport) bool PointOnLine(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint, const unsigned long& gap);
__declspec(dllexport) bool PointOnLine(const CPoint& point, const CPoint& fpoint, const CPoint& tpoint, const unsigned long& gap, double& t);
__declspec(dllexport) bool PointOnLine(const double& x, const double& y, double x1, double y1, double x2, double y2, const unsigned long& gap, double& t);

__declspec(dllexport) bool LineOnLine(const CPoint& point11, const CPoint& point12, const CPoint& point21, const CPoint& point22);

__declspec(dllexport) std::vector<int> XRadialPoly(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount);
__declspec(dllexport) std::vector<int> YRadialPoly(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount);
__declspec(dllexport) std::vector<int> XRadialBezier(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount);
__declspec(dllexport) std::vector<int> YRadialBezier(const Direction& direction, const CPoint& point, CPoint* points, const unsigned int& ptCount);

__declspec(dllexport) void SplitBezier(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, CPoint L[], CPoint R[]);
__declspec(dllexport) void SplitBezier(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, CPoint L[], CPoint R[], double t);

__declspec(dllexport) CPoint BezierPoint(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t);
__declspec(dllexport) CSizeF BezierDerivate(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t);
__declspec(dllexport) bool PointOnBezier(const CPoint& point, const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, const unsigned long& gap, double& t);

__declspec(dllexport) CRect FPointRect(const CPoint& point1, const CPoint& point2, const CPoint& point3, const CPoint& point4);

__declspec(dllexport) CRect UnionRect(const CRect& rect1, const CRect& rect2);
__declspec(dllexport) Gdiplus::Rect UnionRect(const Gdiplus::Rect& rect1, const Gdiplus::Rect& rect2);

__declspec(dllexport) bool LineInter(const CPoint& fpoint1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& tpoint2, double& t1, double& t2);
__declspec(dllexport) bool BezierInter(const CPoint& fpoint1, const CPoint& fctrol1, const CPoint& tctrol1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& fctrol2, const CPoint& tctrol2, const CPoint& tpoint2, double* t1, double* t2, unsigned int& count);
__declspec(dllexport) bool BezierLineInter(const CPoint& fpoint1, const CPoint& fctrol1, const CPoint& tctrol1, const CPoint& tpoint1, const CPoint& fpoint2, const CPoint& tpoint2, double* t1, double* t2, unsigned int& count);

__declspec(dllexport) bool LineClip(const CRect& clipper, CPoint& point1, CPoint& point2);

__declspec(dllexport) void GeomSerialize(CArchive& ar, CTypedPtrList<CObList, CGeom*>& geomlist);

void GetMoreBezierT(double st, double et, double A_x, double B_x, double C_x, double D_x, double A_y, double B_y, double C_y, double D_y, CPoint fpoint, CPoint tpoint, double* RecordT, const unsigned int& tolerance);

__declspec(dllexport) Gdiplus::RectF GetTagRect(const Gdiplus::RectF& markRect, const Gdiplus::SizeF& textSize, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign);
__declspec(dllexport) Gdiplus::RectF GetTagRect(const Gdiplus::PointF& origin, const Gdiplus::SizeF& textSize, const HALIGN& hAlign, const VALIGN& vAlign);

