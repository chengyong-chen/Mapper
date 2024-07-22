#pragma once

#include "Tool.h"

class CDatum;
class CViewInof;
class CTin;

class __declspec(dllexport) CDatumTool : public CTool
{
public:
//	CDatumTool();
	CDatumTool(CTin& tin);

public:
	// Implementation
	void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint) override;
	void OnRButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;
	void OnRButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint) override;

public:
	bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo) override;
public:
	CTin& m_tin;
	CDatum* m_pDatum;
};
