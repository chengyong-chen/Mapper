#pragma once

class CLLine;

class AFX_EXT_CLASS LBezier : public LLine
{
protected:
	DECLARE_SERIAL(LBezier);
public:
	LBezier();
	LBezier(const CPoint& pfpoint, const CPoint& pfctrol, const CPoint& ptctrol, const CPoint& ptpoint);
	LBezier(const Gdiplus::Point& pfpoint, const Gdiplus::Point& pfctrol, const Gdiplus::Point& ptctrol, const Gdiplus::Point& ptpoint);

public:
	CPoint fctrol;
	CPoint tctrol;

public:
	void SetPoint(const CPoint& pfpoint, const CPoint& pfctrol, const CPoint& ptctrol, const CPoint& ptpoint);

public:
	unsigned long GetLength(const double& tolerance) override;
	double GetTByLength(unsigned long& len, const double& tolerance);
	unsigned long GetLengthByT(const double& t, const double& tolerance);
	void Split(LBezier& L, LBezier& R) const;
	void Split(LBezier& L, LBezier& R, double t) const;
	CPoint GetPoint(double t) const;
	bool PointOn(const CPoint& point, const unsigned long& gap, double& t) const;
	virtual bool Inter(LLine* line, double* t1, double* t2, unsigned int& count);
	bool Inter(LBezier* bezier, double* t1, double* t2, unsigned int& count) override;
	double SqrtDiffFunc(const double& t) const;
public:
	long GetCtrolLength() const;
	long GetChordLength() const;

public:
	static int BezierX(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t);
	static int BezierY(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint, double t);
	static CRect Box(const CPoint& fpoint, const CPoint& fctrol, const CPoint& tctrol, const CPoint& tpoint);
};