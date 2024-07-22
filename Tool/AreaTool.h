#pragma once

class __declspec(dllexport) CAreaTool : public CBoundTool
{
public:
	void OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
