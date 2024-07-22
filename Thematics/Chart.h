#pragma once

#include "Theme.h"

using namespace std;
#include <list>
#include <map>

class CViewinfo;

class CChart abstract : public CTheme
{
public:
	CChart();

	~CChart() override;

public:
	std::map<CString, Gdiplus::ARGB> m_fieldcolors;

	enum Style : BYTE
	{
		TwoD,
		ThreeD,
	} m_style;

	COLORREF m_colorBorder;
	float m_widthBorder;
	COLORREF m_colorAxis;
	float m_widthAxis;
	int m_angleIncline;
	float m_deepth;

public:
	void Draw(Gdiplus::Graphics& g, CDatabase& database, CATTDatasource& datasource, const CViewinfo& viewinfo, std::map<DWORD, CGeom*>& list) override;

	virtual void Draw(Gdiplus::Graphics& g, const double fRatio, Gdiplus::Point point, std::map<CString, double>& elements)
	{
	};

public:
	bool AddField(CDatabase& database, CATTDatasource& datasource, const CString& strField) override;
	bool RemoveField(CDatabase& database, CATTDatasource& datasource, const CString& strField) override;
	void RemoveAllFields() override;
	void AddFields(CDatabase& database, CATTDatasource& datasource, std::list<CString> fields) override;

public:
	virtual Gdiplus::ARGB DefaultColor(std::map<CString, Gdiplus::ARGB>& exsiting) { return rand()|0XFF000000; };

	BOOL operator ==(const CTheme& theme) const override;
	void CopyTo(CTheme* pTheme) const override;
	BYTE Gettype() const override { return 3; };
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;

public:
	void RecalculateLimit(CDatabase& database, CATTDatasource& datasource) override;

public:
	static int Incline(int size, int angle);
	static bool ComparePair(std::pair<Gdiplus::Point, std::map<CString, double>>& pair1, std::pair<Gdiplus::Point, std::map<CString, double>>& pair2);
};

template<class T>
struct CompareMapData
{
public:
	bool operator()(typename T::value_type& pair, typename T::mapped_type i) const
	{
		return pair.second==i;
	}
};
