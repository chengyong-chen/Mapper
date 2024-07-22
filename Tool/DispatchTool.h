#pragma once

class __declspec(dllexport) CDispatchTool : public CTool
{
public:
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	void Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* DC) override;
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
