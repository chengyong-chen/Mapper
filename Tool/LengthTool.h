#pragma once

class CTool;

class __declspec(dllexport) CLengthTool : public CTool
{
public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override
	{
	};
	void OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;
public:
	void Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC) override;
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
	bool CanPan() override;
	CArray<CPoint, CPoint&> m_points;
};
