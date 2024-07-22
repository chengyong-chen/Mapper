#pragma once

class CTool;

class __declspec(dllexport) CTopoPickTool : public CTool
{
public:
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
