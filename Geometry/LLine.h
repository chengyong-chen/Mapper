#pragma once

class LBezier;

class AFX_EXT_CLASS LLine : public CObject
{
protected:
	DECLARE_SERIAL(LLine);
public:
	LLine();
	LLine(const CPoint& pfpoint, const CPoint& ptpoint);
public:
	CPoint fpoint;
	CPoint tpoint;

public:
	void SetPoint(const CPoint& pfpoint, const CPoint& ptpoint);

public:
	static CPoint GetPoint(const CPoint& pfpoint, const CPoint& ptpoint, double t);
	static CPoint GetMiddle(const CPoint& fpoint, const CPoint& tpoint);

	virtual unsigned long GetLength(const double& tolerance);
	void Split(LLine& L, LLine& R) const;
	void Split(LLine& L, LLine& R, double t) const;
	virtual bool PointOn(CPoint& point, const unsigned long& gap);
	virtual bool Inter(LLine* line, double& t1, double& t2);
	virtual bool Inter(LBezier* bezier, double* t1, double* t2, unsigned int& count);
};
