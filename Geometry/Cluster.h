#pragma once

#include "Collection.h"
#include <cmath>
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
}
using namespace std;

class AFX_EXT_CLASS CCluster abstract : public CCollection<CPath>
{
protected:
	CCluster();

public:
	CPoint m_centroid;
	CPoint* m_geogroid;
	CPoint* m_labeloid;

public:
	void GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const override;

public:
	void CopyTo(CGeom* pGeom, bool ignore = false) const override;
	
public:
	CLine* GetLine() const;
	CFillCompact* GetFill() const;
	Gdiplus::PointF GetTagAnchor(const CViewinfo& viewinfo, HALIGN& hAlign, VALIGN& vAlign) const override;
		
	void ResetGeogroid() override;
	void ResetLabeloid() override;
	void SetGeogroid(const CPoint& point) override;
	void SetLabeloid(const CPoint& point) override;
	CPoint GetGeogroid() const override;
	CPoint GetLabeloid() const override;

public:
	void DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const override;
	
public:
	void Move(const int& dx, const int& dy) override;
	void Move(const CSize& delta) override;
	void Transform(const CViewinfo& viewinfo) override;
	void Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32) override;
	void Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing) override;
	CGeom* Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance) override;
	void AddMember(CPath* pPath);
	void AddMember(CGeom* pGeom) override;
	void AddMembers(std::list<CPath*>& pathes);
	void AddMembers(std::list<CPoly*>& polys);
	void AddMembers(std::list<CGeom*>& geoms) override;
	bool CreateName(CWnd* pWnd, const float& fontSize, const  float& zoomPointToDoc, CObList& geomlist) override;

public:
	~CCluster() override;
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseCE(CArchive& ar) const override;
	void ReleaseDCOM(CArchive& ar) override;
	void ReleaseWeb(CFile& file, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(BinaryStream& stream, CSize offset=CSize(0, 0)) const override;
	void ReleaseWeb(boost::json::object& json) const override;
	void ReleaseWeb(pbf::writer& writer, const CSize offset = CSize(0, 0)) const override;
	bool GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag) override;
	bool PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const override;
	DWORD PackPC(CFile* pFile, BYTE*& bytes) override;
	DWORD ReleaseCE(CFile& file, const BYTE& type) const override;
};