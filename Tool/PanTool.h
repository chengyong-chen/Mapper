#pragma once

class CTool;

class __declspec(dllexport) CPanTool : public CTool
{
public:
	CPanTool();

public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
	void Recover(CWnd* pWnd, const CViewinfo& viewinfo, CTool*& pTool, CTool*& oldTool) override;

public:
	bool m_bContinuously;
	CBitmap m_bitmap;
};
