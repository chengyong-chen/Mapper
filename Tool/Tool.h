#pragma once

class CViewinfo;
class CEditGeom;

class __declspec(dllexport) CTool
{
public:
	virtual void OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint);
	virtual void OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint);
	virtual void OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint);
	virtual void OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint);

	virtual void OnRButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
	{
	}

	virtual void OnRButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
	{
	}

public:
	virtual bool CanPan();

	virtual void Recover(CWnd* pWnd, const CViewinfo& viewinfo, CTool*& pTool, CTool*& oldTool)
	{
	};
	virtual void Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC);
	virtual bool SetCursor(CWnd* pWnd, const CViewinfo& viewinfo);

public:
	static CPoint c_cliDown;
	static CPoint c_cliLast;
	static CPoint c_docDown;
	static CPoint c_docLast;

public:
	static CEditGeom* m_pEditgeom;
	static void SetEditGeom(CEditGeom* pEditobj);
	virtual void OnKeyDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool);
	virtual void OnKeyUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool);

	virtual void OnShiftDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags)
	{
	};

	virtual void OnShiftUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nRepCnt, UINT nFlags)
	{
	};
};
