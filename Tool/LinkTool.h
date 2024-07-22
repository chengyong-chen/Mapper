#pragma once

class CTool;
class CLink;

class __declspec(dllexport) CLinkTool : public CTool
{
public:
	CLinkTool();

public:
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	virtual void OnTimerMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint);
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	CLink* m_pLink;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
