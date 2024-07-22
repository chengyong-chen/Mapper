#pragma once




using namespace std;
#include <list>
#include <map>

class CClassification;
class CGeom;
class CDatabase;
class CATTDatasource;
class CViewinfo;


template<typename T>
class CContinous
{
public:
	CContinous(double& m_minValue, double& m_maxValue)
		: m_minValue(m_minValue), m_maxValue(m_maxValue)
	{
		m_scaling = Scaling::Constant;
	};

public:
	double& m_minValue;
	double& m_maxValue;

	T m_minT;
	T m_maxT;

	enum Scaling
	{
		Squareroot = 0,
		Constant,
		Log
	} m_scaling;

	virtual void Serialize(CArchive& ar)
	{
		if(ar.IsStoring())
		{
			ar<<m_minT;
			ar<<m_maxT;
			//	ar << m_scaling;
		}
		else
		{
			ar>>m_minT;
			ar>>m_maxT;
			//	ar >> (BYTE)m_scaling;
		}
	};

	virtual CContinous& operator =(const CContinous& other)
	{
		m_minT = other.m_minT;
		m_maxT = other.m_maxT;
		m_scaling = other.m_scaling;
		return *this;
	};

	virtual BOOL operator ==(const CContinous& other) const
	{
		if(m_minValue!=other.m_minValue)
			return FALSE;
		if(m_maxValue!=other.m_maxValue)
			return FALSE;
		if(m_scaling!=other.m_scaling)
			return FALSE;
		else
			return TRUE;
	};

	virtual BOOL operator !=(const CContinous& other) const
	{
		return !(*this==other);
	};

	T GetSize(double value)
	{
		if(value<=m_minValue)
			return m_minT;
		else if(value>=m_maxValue)
			return m_maxT;
		else if(m_minValue==m_maxValue)
			return (m_minT+m_maxT)/2;
		else if(m_scaling==Scaling::Constant)
			return m_minT+(m_maxT-m_minT)*sqrt(value/(m_maxValue-m_minValue));
		else if(m_scaling==Scaling::Log)
			return m_minT+(m_maxT-m_minT)*log(value/(m_maxValue-m_minValue));
		else// if(m_scaling == Scaling::Squareroot)
			return m_minT+(m_maxT-m_minT)*(value/(m_maxValue-m_minValue));
	};

	T GetARGB(double value)
	{
		if(value<=m_minValue)
			return m_minT;
		if(value>=m_maxValue)
			return m_maxT;

		const BYTE r1 = Gdiplus::Color(m_minT).GetR();
		const BYTE g1 = Gdiplus::Color(m_minT).GetG();
		const BYTE b1 = Gdiplus::Color(m_minT).GetB();
		const BYTE r2 = Gdiplus::Color(m_maxT).GetR();
		const BYTE g2 = Gdiplus::Color(m_maxT).GetG();
		const BYTE b2 = Gdiplus::Color(m_maxT).GetB();

		double ratio = 1;
		if(m_scaling==Scaling::Constant)
			ratio = sqrt(value/(m_maxValue-m_minValue));
		else if(m_scaling==Scaling::Log)
			ratio = log(value/(m_maxValue-m_minValue));
		else// if(m_scaling == Scaling::Squareroot)
			ratio = value/(m_maxValue-m_minValue);

		return Gdiplus::Color::MakeARGB(255, r1+(r2-r1)*ratio, g1+(g2-g1)*ratio, b1+(b2-b1)*ratio);
	};
};

void AFXAPI operator <<(CArchive& ar, CContinous<DWORD>& continous);
void AFXAPI operator >>(CArchive& ar, CContinous<DWORD>& continous);

template<typename T>
class CDivision
{
public:
	double m_minValue;
	double m_maxValue;
	T m_Value;

	virtual void Serialize(CArchive& ar, const DWORD& dwVersion)
	{
		if(ar.IsStoring())
		{
			ar<<m_minValue;
			ar<<m_maxValue;
			ar<<m_Value;
		}
		else
		{
			ar>>m_minValue;
			ar>>m_maxValue;
			ar>>m_Value;
		}
	};

	virtual BOOL operator ==(const CDivision& other) const
	{
		if(m_minValue!=other.m_minValue)
			return FALSE;
		if(m_maxValue!=other.m_maxValue)
			return FALSE;
		if(m_Value!=other.m_Value)
			return FALSE;
		else
			return TRUE;
	};

	virtual BOOL operator !=(const CDivision& other) const
	{
		return !(*this==other);
	};
};

class __declspec(dllexport) CTheme abstract
{
public:
	CTheme();
	virtual ~CTheme();

public:
	double m_minValue;
	double m_maxValue;
	std::list<CString> m_fields;
	CContinous<DWORD> m_continuous;
	std::list<CDivision<DWORD>> m_divisions;

	bool m_bIgnoreZeroAndNull;
	BYTE m_Alpha;

	enum CATEGORY : BYTE
	{
		Spot,
		Line,
		Area
	} Category;

public:
	virtual bool AddField(CDatabase& database, CATTDatasource& datasource, const CString& strField);
	virtual bool RemoveField(CDatabase& database, CATTDatasource& datasource, const CString& strField);
	virtual void RemoveAllFields();
	virtual void AddFields(CDatabase& database, CATTDatasource& datasource, std::list<CString> fields);

public:
	virtual void Clear()
	{
	};

	virtual void Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, std::map<DWORD, CGeom*>& list)
	{
	};
	virtual void Invalidate(CDocument* pDocument) const;
public:
	virtual void RedivideSize(int steps);
	virtual void RedivideARGB(int steps);
	virtual void RecalculateLimit(CDatabase& database, CATTDatasource& datasource);
	virtual int GetSize(double value);
	virtual Gdiplus::ARGB GetARGB(double value);
public:
	virtual BOOL operator ==(const CTheme& theme) const;
	virtual BOOL operator !=(const CTheme& theme) const;
	virtual CTheme* Clone();
	virtual void CopyTo(CTheme* pTheme) const;
	virtual BYTE Gettype() const { return 0; };
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	static CTheme* Apply(BYTE type);
	static Gdiplus::ARGB LightColor(Gdiplus::ARGB rgb);
	static Gdiplus::ARGB DarkColor(Gdiplus::ARGB rgb);
	static void Serialize(CArchive& ar, const DWORD dwVersion, std::list<CDivision<DWORD>>& divisions);
protected:
	virtual CString MakrScript(CDatabase& database, CATTDatasource& datasource);

public:
	enum TYPE : BYTE
	{
		Choropleth = 1,
		Graduated = 2,
		Dotdensity = 4,
		Pie = 5,
		Bar = 6,
		Cylinder = 7
	};
};
