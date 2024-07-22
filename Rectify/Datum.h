#pragma once

class CViewinfo;

#define DllExport   __declspec( dllexport )

class DllExport CDatum : public CObject
{
	DECLARE_SERIAL(CDatum);
public:
	CDatum();
	CDatum(const CPoint& point);
	~CDatum() override;
public:
	CPoint m_rPoint;
	CPoint m_bPoint;
public:
	void Moving(CWnd* pWnd, const CViewinfo& viewinfo, const CSize& delta);
	void Move(const CSize& delta);
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo) const;
	void Draw(CWnd* pWnd, const CViewinfo& viewinfo) const;
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const;
	bool Pick(const CViewinfo& viewinfo, const CPoint& docPoint) const;
public:
	mutable bool m_rActive;
	mutable bool m_bActive;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};

long GetSymbol(double a);
