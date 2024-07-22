#pragma once

#include "PRectTool.h"

class __declspec(dllexport) CRRectTool : public CPRectTool
{
public:
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	void DrawTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point1, CPoint point2, bool bSquare, bool bCentral) override;
};
