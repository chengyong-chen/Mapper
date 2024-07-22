#include "Stdafx.h"
#include "DataHandler.h"
#include "Import.h"
#include <cmath>
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Text.h"
#include "../Geometry/Group.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Clip.h"

#include "../Style/Line.h"
#include "../Style/FillCompact.h"
#include "../Style/LineEmpty.h"
#include "../Style/FillEmpty.h"
#include "../Style/FillAlone.h"

IDataHandler::IDataHandler(DWORD& maxID)
	: m_maxId(maxID)
{
	bBezier = false;
}

IDataHandler::~IDataHandler()
{
}

void IDataHandler::onPathRect(float x, float y, float dx, float dy)
{
	const Gdiplus::PointF point1 = CurrentState.matrix.TransformPoint(x, y);
	const Gdiplus::PointF point2 = CurrentState.matrix.TransformPoint(x+dx, y);
	const Gdiplus::PointF point3 = CurrentState.matrix.TransformPoint(x+dx, y+dy);
	const Gdiplus::PointF point4 = CurrentState.matrix.TransformPoint(x, y+dy);

	CPoly* pPoly = new CPoly();
	pPoly->AddAnchor(Change(point1));
	pPoly->AddAnchor(Change(point2));
	pPoly->AddAnchor(Change(point3));
	pPoly->AddAnchor(Change(point4));
	pPoly->AddAnchor(Change(point1));
	pPoly->m_bClosed = true;
	m_activePaths.push(pPoly);
}
void IDataHandler::onPathMoveTo(float x, float y)
{
	if(m_activePaths.empty()==false)
	{
		CPath* pPath = m_activePaths.top();
		pPath->Close(false, 0, nullptr);
		if(this->bBezier==false)
		{
			const auto poly = pPath->Polylize(0);
			if(poly!=nullptr)
			{
				m_activePaths.pop();
				m_activePaths.push(poly);
				delete pPath;
			}
		}
	}
	{
		bBezier = false;
		const Gdiplus::PointF point = CurrentState.matrix.TransformPoint(x, y);
		CBezier* pBezier = new CBezier();
		pBezier->AddAnchor(Change(point), true, true);
		pBezier->m_geoId = ++m_maxId;
		m_activePaths.push(pBezier);
	}
}

void IDataHandler::onPathLineTo(float x, float y)
{
	CBezier* pPath = (CBezier*)m_activePaths.top();
	const Gdiplus::PointF point = CurrentState.matrix.TransformPoint(x, y);
	pPath->AddAnchor(Change(point), true, true);
}

void IDataHandler::onPathCurvetoc(float x1, float y1, float x2, float y2, float x3, float y3)
{
	CPath* pPath = m_activePaths.top();
	const Gdiplus::PointF point1 = CurrentState.matrix.TransformPoint(x1, y1);
	const Gdiplus::PointF point2 = CurrentState.matrix.TransformPoint(x2, y2);
	const Gdiplus::PointF point3 = CurrentState.matrix.TransformPoint(x3, y3);
	((CBezier*)pPath)->SetContrl(((CBezier*)pPath)->m_nAnchors, +1, Change(point1), true);
	((CBezier*)pPath)->AddAnchor(Change(point3), true, true);
	((CBezier*)pPath)->SetContrl(((CBezier*)pPath)->m_nAnchors, -1, Change(point2), true);
	bBezier = true;
}

void IDataHandler::onPathCurvetov(float x2, float y2, float x3, float y3)
{
	CBezier* pPath = (CBezier*)m_activePaths.top();
	if(pPath->m_nAnchors>=1)
	{
		const CPoint control1 = pPath->m_pPoints[(pPath->m_nAnchors-1)*3+0];
		const CPoint anchor = pPath->m_pPoints[(pPath->m_nAnchors-1)*3+1];
		const CPoint control2 = ReflectionPoint(control1, anchor);
		pPath->SetContrl(pPath->m_nAnchors, +1, control2, true);
	}
	const Gdiplus::PointF point2 = CurrentState.matrix.TransformPoint(x2, y2);
	const Gdiplus::PointF point3 = CurrentState.matrix.TransformPoint(x3, y3);
	pPath->AddAnchor(Change(point3), true, true);
	pPath->SetContrl(pPath->m_nAnchors, -1, Change(point2), true);
	bBezier = true;
}

void IDataHandler::onPathCurvetoy(float x1, float y1, float x3, float y3)
{
	CBezier* pPath = (CBezier*)m_activePaths.top();

	const Gdiplus::PointF point1 = CurrentState.matrix.TransformPoint(x1, y1);
	const Gdiplus::PointF point3 = CurrentState.matrix.TransformPoint(x3, y3);
	pPath->SetContrl(pPath->m_nAnchors, +1, Change(point1), true);
	pPath->AddAnchor(Change(point3), true, true);
	bBezier = true;
}

void IDataHandler::onPathArc(float x1, float y1, float x2, float y2, float rx, float ry, float angle, bool bLarge, bool bSweep)
{
	if(x1==x2&&y1==y2)
		return;
	else if(rx==0.0f&&ry==0.0f)
		onPathLineTo(x1, y1);
	else
	{
		const float radian = angle*M_PI/180.f;
		const double x1dash = cos(radian)*(x1-x2)/2.0+sin(radian)*(y1-y2)/2.0;
		const double y1dash = -sin(radian)*(x1-x2)/2.0+cos(radian)*(y1-y2)/2.0;
		const double numerator = rx*rx*ry*ry-rx*rx*y1dash*y1dash-ry*ry*x1dash*x1dash;
		double root;
		if(numerator<0.0)
		{
			const float s = sqrt(1.0-numerator/(rx*rx*ry*ry));
			rx *= s;
			ry *= s;
			root = 0.0;
		}
		else
		{
			root = ((bLarge==true&&bSweep==true)||(bLarge==0&&bSweep==false) ? -1.0 : 1.0)*sqrt(numerator/(rx*rx*y1dash*y1dash+ry*ry*x1dash*x1dash));
		}
		const double cxdash = root*rx*y1dash/ry;
		const double cydash = -root*ry*x1dash/rx;
		const double cx = cos(radian)*cxdash-sin(radian)*cydash+(x1+x2)/2.0;
		const double cy = sin(radian)*cxdash+cos(radian)*cydash+(y1+y2)/2.0;

		double theta1 = CalculateVectorAngle(1.0, 0.0, (x1dash-cxdash)/rx, (y1dash-cydash)/ry);
		double dtheta = CalculateVectorAngle((x1dash-cxdash)/rx, (y1dash-cydash)/ry, (-x1dash-cxdash)/rx, (-y1dash-cydash)/ry);

		if(bSweep==0&&dtheta>0)
			dtheta -= 2.0*M_PI;
		else if(bSweep==1&&dtheta<0)
			dtheta += 2.0*M_PI;
		const int segments = ceil(dtheta/(M_PI/2.0));
		const double Δ = dtheta/segments;
		const double t = 8.0/3.0*sin(Δ/4.0)*sin(Δ/4.0)/sin(Δ/2.0);

		double startX = x1;
		double startY = y1;

		for(int i = 0; i<segments; ++i)
		{
			const double cosTheta1 = cos(theta1);
			const double sinTheta1 = sin(theta1);
			const double theta2 = theta1+Δ;
			const double cosTheta2 = cos(theta2);
			const double sinTheta2 = sin(theta2);
			const double endpointX = cos(radian)*rx*cosTheta2-sin(radian)*ry*sinTheta2+cx;
			const double endpointY = sin(radian)*rx*cosTheta2+cos(radian)*ry*sinTheta2+cy;
			const double dx1 = t*(-cos(radian)*rx*sinTheta1-sin(radian)*ry*cosTheta1);
			const double dy1 = t*(-sin(radian)*rx*sinTheta1+cos(radian)*ry*cosTheta1);
			const double dxe = t*(cos(radian)*rx*sinTheta2+sin(radian)*ry*cosTheta2);
			const double dye = t*(sin(radian)*rx*sinTheta2-cos(radian)*ry*cosTheta2);

			IDataHandler::onPathCurvetoc((float)(startX+dx1), (float)(startY+dy1), (float)(endpointX+dxe), (float)(endpointY+dye), (float)endpointX, (float)endpointY);

			theta1 = theta2;
			startX = (float)endpointX;
			startY = (float)endpointY;
		}
	}
}

void IDataHandler::onPathClose(bool ignore)
{
	if(m_activePaths.empty())
		return;

	if(CPath* pPath = m_activePaths.top(); pPath!=nullptr)
	{
		pPath->Close(true, 0, nullptr, ignore);
	}

	std::stack<CPath*> buffer;
	while(m_activePaths.empty()==false)
	{
		CPath* pPath = m_activePaths.top();
		pPath->Close(true, 0, nullptr);
		m_activePaths.pop();
		buffer.push(pPath);
	}
	while(buffer.empty()==false)
	{
		CPath* pPath = buffer.top();
		buffer.pop();
		m_activePaths.push(pPath);
	}
}

void IDataHandler::onPathEnd(bool line, bool fill)
{
	if(m_activePaths.empty()==false)
	{
		CPath* pPath = m_activePaths.top();
		if(this->bBezier==false)
		{
			const auto poly = pPath->Polylize(0);
			if(poly!=nullptr)
			{
				m_activePaths.pop();
				m_activePaths.push(poly);
				delete pPath;
			}
		}
	}
	CTypedPtrList<CObList, CPath*> geoms;
	while(m_activePaths.empty()==false)
	{
		CPath* pPath = m_activePaths.top();
		geoms.AddHead((CPath*)pPath);
		m_activePaths.pop();
	}
	const auto pLine = line ? GetLine() : new CLineEmpty();
	const auto pFill = fill ? GetFill() : new CFillEmpty();
	if(geoms.GetCount()==1)
	{
		CPath* pPath1 = geoms.GetHead();
		delete pPath1->m_pLine;
		delete pPath1->m_pFill;
		pPath1->m_pLine = pLine;
		pPath1->m_pFill = pFill;
		pPath1->Close(fill, 0, nullptr);
		m_geomHolders.top()->AddTail(pPath1);
	}
	else if(fill)
	{
		CDoughnut* pDonut = new CDoughnut();
		pDonut->m_geoId = ++m_maxId;
		POSITION pos = geoms.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* pPath1 = geoms.GetNext(pos);
			pPath1->Close(true, 0, nullptr);
			pDonut->AddMember(pPath1);
		}
		delete pDonut->m_pLine;
		delete pDonut->m_pFill;
		pDonut->m_pLine = pLine;
		pDonut->m_pFill = pFill;
		pDonut->m_bClosed = true;
		pDonut->fillmode = CurrentState.fillmode;
		m_geomHolders.top()->AddTail(pDonut);
	}
	else if(line)
	{
		CGroup* pGroup = new CGroup();
		pGroup->m_geoId = ++m_maxId;
		POSITION pos = geoms.GetHeadPosition();
		while(pos!=nullptr)
		{
			CPath* pPath1 = geoms.GetNext(pos);
			pGroup->AddMember(pPath1);
		}
		delete pGroup->m_pLine;
		delete pGroup->m_pFill;
		pGroup->m_pLine = pLine;
		pGroup->m_pFill = pFill;
		m_geomHolders.top()->AddTail(pGroup);
	}
}
void IDataHandler::onClipStart(Gdiplus::FillMode mode)
{
	CClip* pClip = new CClip(mode);
	while(m_activePaths.empty()==false)
	{
		CPath* pPath = m_activePaths.top();
		m_activePaths.pop();
		if(pPath->GetAnchors()>3)
			pClip->m_paths.AddTail(pPath);
		else
			delete pPath;
	}
	pClip->RecalRect();
	CurrentState.clippingpath = pClip;

	m_geomHolders.top()->AddTail(pClip);
	m_geomHolders.push(&pClip->m_geomlist);
}
void IDataHandler::onGroupBegin()
{
	CGroup* pGroup = new CGroup();
	pGroup->m_geoId = ++m_maxId;
	m_geomHolders.top()->AddTail(pGroup);

	m_geomHolders.push(&pGroup->m_geomlist);
}
void IDataHandler::onCollectionEnd()
{
	if(m_geomHolders.empty())
	{
	}
	else if(m_geomHolders.size()>1)
		m_geomHolders.pop();
}
void IDataHandler::onPathDiscard()
{
	while(m_activePaths.empty()==false)
	{
		const CGeom* pPath = m_activePaths.top();
		m_activePaths.pop();
		delete pPath;
	}
}

CPoint IDataHandler::Change(const double& x, const double& y) const
{
	return (m_instance->Change)(x, y);
}
CPoint IDataHandler::Change(const Gdiplus::PointF& point) const
{
	return (m_instance->Change)(point.X, point.Y);
}

float IDataHandler::CalculateVectorAngle(double ux, double uy, double vx, double vy)
{
	const double ta = atan2(uy, ux);
	const double tb = atan2(vy, vx);
	return tb>=ta ? tb-ta : 2*M_PI-(ta-tb);
}

CLine* IDataHandler::GetLine()
{
	if(CurrentState.line_color!=nullptr)
	{
		CLine* pLine = new CLine();
		pLine->m_pColor = CurrentState.line_color->Clone();
		if(CurrentState.line_dash.GetCount()>1)
		{
			pLine->m_dash = CurrentState.line_dash;
		}
		pLine->m_nWidth = CurrentState.line_width*10;
		pLine->m_bCapindex = CurrentState.line_cap;
		pLine->m_bJoinindex = CurrentState.line_join;
		pLine->m_bMiterlimit = CurrentState.line_miterlimit;
		return pLine;
	}
	else
		return nullptr;
}

CFillCompact* IDataHandler::GetFill() const
{
	return CurrentState.fill_color!=nullptr ? new CFillAlone(CurrentState.fill_color->Clone()) : nullptr;
}

void IDataHandler::onGeomParsed(CGeom* pGeom)
{
	m_geomHolders.top()->AddTail(pGeom);

}