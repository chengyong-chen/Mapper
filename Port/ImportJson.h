#pragma once
#include <list>
#include "ImportGdal.h"
namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
class CTopology;
class __declspec(dllexport) CImportJson final : public CImportGdal
{
public:
	CImportJson(CDatainfo& datainfo, CLayerTree& layertree, std::list<CTopology*>& topolist, DWORD& dwMaxGeomID);
	~CImportJson() override;

private:
	double m_jsonres = 1.f;
	double m_scale = 1.f;
	double m_scaleX = 1.f;
	double m_scaleY = 1.f;
	CSizeF m_margin = CSizeF(0.f, 0.f);
	std::list<CTopology*>& m_topolist;
public:
	BOOL Import(CString& file, CString& dbname, bool into=false) override;
	CProjection* GetProjection(CString& strFile);
	CPoint Change(const double& x, const double& y) const override;
	CPoly* ReadPoly(boost::json::array coordinates);

private:
	void CreateTable(boost::json::array jgeoms, CDaoDatabase* pMDBDataBase, CString table, std::map<string, string>& fieldtype, CString& fieldAnno, CString& fieldGeocode, CString& fieldAid, CString& fieldCentroidX, CString& fieldCentroidY);
};
