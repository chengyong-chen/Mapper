#pragma once

class CViewinfo;
class CDatum;

#define DllExport   __declspec( dllexport )

class DllExport CTriangle : public CObject
{
	DECLARE_SERIAL(CTriangle);
public:
	CTriangle();
	~CTriangle() override;

public:
	CDatum* m_pDatum1;
	CDatum* m_pDatum2;
	CDatum* m_pDatum3;

	double a11;
	double a21;
	double a31;
	double a12;
	double a22;
	double a32;

public:
	double PointTo(const CPoint& point) const;
	bool PointIn(const CPoint& point) const;
	CPoint Rectify(const CPoint& point) const;
	void CalParameter();

public:
	void Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const;
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo) const;
};
