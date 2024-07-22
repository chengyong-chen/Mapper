#pragma once

#include "../Public/Global.h"
#include "../Dataview/Viewinfo.h"

#include "Segment.h"

class CTopology;
class CRoadTopo;
class CRoadNode;
class CRoadEdge;
class CPoly;
class CNode;

class __declspec(dllexport) CRoute : public CObject
{
protected:
	DECLARE_SERIAL (CRoute);

	CRoute ();

	~CRoute () override;
public:
	static CImageList m_imagelist;

public:
	CPointF m_ptStart;
	CPointF m_ptEnd;
	CArray<CPointF, CPointF> m_Pole;

public:
	static CPointF m_geoCurrent;

public:
	void SetStart (CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& point);
	void SetPass (CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& point);
	void SetEnd (CWnd* pWnd, const CViewinfo& viewinfo, const CPointF& point);

	void Stop (CWnd* pWnd) const;
	void Over (CWnd* pWnd);
public:
	void DrawMark (CWnd* pWnd, const CViewinfo& viewinfo);
	void Move (CWnd* pWnd, const CViewinfo& viewinfo, const double& lng, const double& lat, const double& altitude);
	void ViewWillDraw (CWnd* pWnd, const CViewinfo& viewinfo);
	void ViewDrawOver (CWnd* pWnd, const CViewinfo& viewinfo);

public:
	int GetNextPole (const double& lng, const double& lat, const double& altitude);
	float GetLeftLength ();

	float GetShortcutLength ();
	float GetBeelineLength ();

	static float GetBeelineLength (CPointF point1, CPointF point2);

public:
	//	void Release(CTopology* pTopology);

public:
	CSegmentList m_segmentlist;

	bool Search (CRoadTopo* pTopology);
	bool Search (CRoadTopo* pTopology, CPointF point1, CPointF point2);
	void Draw (CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRectF& geoRect);
};
