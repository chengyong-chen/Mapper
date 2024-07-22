#pragma once

#include <map>
#include "Laypt.h"
#include "../Geometry/Mark.h"
class CGeom;
class CMark;
class CODBCRecordset;

class AFX_EXT_CLASS CLaypp : public CLaypt
{
private:
	class CMarker sealed : public CMark
	{
	public:
		CMarker();
		CMarker(const CPoint& origin);

		~CMarker() override;

	public:
		//	char m_strType[4];
		CStringA m_strType;

	public:
		bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	};

public:
	CLaypp(CTree<CLayer>& tree);
	CLaypp(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag);

	~CLaypp() override;

	CString m_tableType;
	CString m_strIconFile;

	Gdiplus::Bitmap* m_Bitmap;
	CImageList m_Iconlist;
	int m_nIconHeight;
	int m_nIconCount;

	std::map<CStringA, int> m_mapTypeIcon;

public:
	BYTE Gettype() const override { return 8; };
public:
	void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const override;
	void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, const bool showCreated, const bool showDynamic) const override;

public:
	BOOL LoadIcons(CString strIconFile);

public:
	void Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms = false) override;
	void Reload(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRectF& mapRect) override;
};
