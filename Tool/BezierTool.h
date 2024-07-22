#pragma once

class CTool;

class __declspec(dllexport) CBezierTool : public CTool
{
public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;

	unsigned short m_PrevAnchor;
	CPoint m_PrevPoint[3];
	CPoint m_InsertPoint[3];
	CPoint m_NextPoint[3];
};
