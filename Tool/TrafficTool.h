#pragma once

class CTool;

class __declspec(dllexport) CTrafficTool : public CTool
{
public:
	virtual void OnLButtonDown(CWnd* pWnd, UINT nFlags, const CPoint& point) override;
	virtual void OnLButtonUp(CWnd* pWnd, UINT nFlags, const CPoint& point) override;

public:
	virtual bool SetCursor(CWnd* pWnd,CViewinfo& viewinfo) override;

public:
	static bool m_bStart;
};