#pragma once

class CTool;

class __declspec(dllexport) CSpinTool : public CTool
{
public:
	CSpinTool();

public:
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
};
