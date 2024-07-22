#pragma once

class __declspec(dllexport) CVehicleTool : public CTool
{
public:
	virtual void OnMouseMove(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	virtual void OnLButtonUp(CWnd* pWnd, CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	virtual void Draw(CWnd* pWnd, CViewinfo& viewinfo, CDC* DC) override;
	virtual bool SetCursor(CWnd* pWnd, CViewinfo& viewinfo) override;
};