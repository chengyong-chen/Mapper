#pragma once

class CDatainfo;
class CViewinfo;

class __declspec(dllexport) CRuler
{
public:
	CRuler();
	~CRuler();

public:
	bool m_bVisible;
	unsigned short m_nClass;
	unsigned short m_nXpixels;
	unsigned short m_nYpixels;
	bool m_bUnit;
	bool Reset(CWnd* pWnd, const CViewinfo& viewinfo);
	void DrawH(CView* pView, CMDIChildWnd* pChildfrm, CPoint start) const;
	void DrawV(CView* pView, CMDIChildWnd* pChildfrm, CPoint start) const;

	void ShowSwitch(CView* pView, const CViewinfo& viewinfo);
};
