#pragma once

class CTool;

class __declspec(dllexport) CPolyTool : public CTool
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

	unsigned short m_PrevAnchor;
	CPoint m_PrevPoint;
	CPoint m_NextPoint;
};
