#pragma once

#include <gdiplus.h>


#include "../Public/Global.h"

class __declspec(dllexport) CRuler
{
public:
	CRuler();
	~CRuler();

public:
	bool m_bVisible;
	CPoint m_last;
	CString m_strUnit;
	short m_nClass;
	short m_pixels;
	CPointF m_origin;

	void Reset(CView* pView, CPointF origin, float factorMapToView);
	void DrawH(CView* pView, CWnd* pParent, Gdiplus::Point start);
	void DrawV(CView* pView, CWnd* pParent, Gdiplus::Point start);
	void DrawMouse(CClientDC& dcpoint, const CPoint& point) const;
	void ShowSwitch(CWnd* pParent, CView* pView);
	void OnMouseMove(CWnd* pParent, CPoint point);
};
