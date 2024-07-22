#pragma once

class CTool;

class __declspec(dllexport) CPRectTool : public CTool
{
public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	void OnKeyDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool) override;
	void OnKeyUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
	virtual void DrawTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point1, CPoint point2, bool bSquare, bool bCentral);
};
