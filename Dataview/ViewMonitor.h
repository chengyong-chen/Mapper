#pragma once

#include "Viewinfo.h"

class CDatainfo;
class CBackground;

class __declspec(dllexport) CViewMonitor : public CViewinfo
{
public:
	CViewMonitor(CDatainfo& foreground);
	~CViewMonitor(void);

public:
	bool m_bShowGrid;

public:
	using CViewinfo::ClientToDoc;
	using CViewinfo::DocToClient;
	using CViewinfo::DataMapToClient;

public:
	CBackground* m_pBackground = nullptr;
	void RemoveBackground();
	void SetBackground(CBackground* pBackground);
	void Refresh(CWnd* pWnd, const CPointF& geoCenter);
	CPointF GetViewCenter(CWnd* pWnd) const;
public:
	void DrawCanvas1(Gdiplus::Graphics& g) const;
	void DrawCanvas2(Gdiplus::Graphics& g) const;
	void DrawGrid(Gdiplus::Graphics& g, CRect& rect) const;

public:
	void FixateAt(CWnd* pWnd, const CPointF& mapPoint, const CPoint& cliPoint);
	void MoveBy(CWnd* pWnd, int cx, int cy);
	void SpinBy(CWnd* pWnd, double deltaLng, double deltaLat);
	void ZoomBy(CWnd* pWnd, float fRatio, const CPoint& cliPoint);	
	void RatioBy(CWnd* pWnd, float fRatio, const CPoint& cliPoint);

public:
	void LevelTo(CWnd* pWnd, const float& fLevel, const CPointF& mapPoint, bool force=false) override;
	void ScaleTo(CWnd* pWnd, const float& fScale, CPointF mapPoint) override;	
	void RatioTo(CWnd* pWnd, float fRatio, const CPoint& cliPoint);

public:
	void OnSized(CWnd* pHoster, int cx, int cy);
	void OnFSCommond(CWnd* pWnd, LPCSTR commond, LPCSTR args);
public:	
	const CViewMonitor& operator =(const CViewMonitor& viewinfo);
};
