#pragma once


#include "../Database/Datasource.h"
#include "../Public/Global.h"
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CPoupin;
class CHint;
class CSpot;
class CLibrary;
class CODBCDatabase;
class CPOUList;
class CDatainfo;
class CViewinfo;
class BinaryStream;
class CPointF;
class CRectF;
class __declspec(dllexport) CPOU : public CObject
{
public:
	CPOU(CPOUList& poulist);

	~CPOU() override;

public:
	CPOUList& m_poulist;
	DWORD m_dwId;
	CString m_strName;

	CPOUDatasource m_datasource;
public:
	BOOL m_bVisible;
	BOOL m_bShowTag;

	float m_minLevelTag;
	float m_maxLevelTag;

public:
	CRectF m_ValidRect;
	CTypedPtrList<CObList, CPoupin*> m_Pinlist;
	CSpot* m_pSpot;
	CHint* m_pHint;

public:
	void LoadPinlist(const CRectF& geoRect);
	void Clear();
	void UpdatePosition(const DWORD& dwId, const CPointF& point);

	virtual void Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CRectF& inGeoRect);
	virtual void DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CRectF& geoRect);

	virtual int Pick(CWnd* pWnd, CPoint& docPoint, const CViewinfo& viewinfo) const;

	virtual void Invalidate(CDocument* pDocument) const;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseWeb(CFile& file, const CDatainfo& datainfo);
	virtual void ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo);
	virtual void ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo);
	virtual void ReleaseWeb(pbf::writer& writer, const CDatainfo& datainfo) const;
	bool SetSpot(CDocument* pDocument);
	bool SetHint(CDocument* pDocument);
	bool SetWhere(CDocument* pDocument);
	bool SetTag(CDocument* pDocument);

	bool IsFiledNumeric(CString strTable, CString strField);
};
