#pragma once

#include "Projection1.h"
#include "../Public/Global.h"
#include "../Utility/Constants.hpp"
#include <optional> 

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
}
struct PJconsts;
typedef struct PJconsts PJ; 
struct pj_ctx;
typedef struct pj_ctx PJ_CONTEXT;

class __declspec(dllexport) CGeocentric : public CProjection
{
protected:
	DECLARE_DYNCREATE(CGeocentric);
public:
	CGeocentric();
	CGeocentric(const char* proj4, const char* ellps = nullptr);
	~CGeocentric() override;

public:
	char* m_keyProj4 = nullptr;
	PJ_CONTEXT* m_ctx;
	PJ* m_pProj4;

public:
	double m_lng0;
	double m_lng1;
	double m_lng2;
	double m_lat0;
	double m_lat1;
	double m_lat2;
	double m_lats;
	double m_lngc;
	double m_k0;
	BOOL m_south;
	int m_zone;
	double m_h;
	double m_alpha;
	double m_gamma;
	int UintMinX() const override { return -20000000; };
	int UintMaxX() const override { return 20000000; };
	int UintMinY() const override { return -20000000; };
	int UintMaxY() const override { return 20000000; };
public:
	BYTE Gettype() const override { return 2; };
	CString GetName() const override;
	bool IsInvertible() const override;
	double GetDilationDocToMap() const override
	{
		return 0.01;
	};
	double GetDilationMapToDoc() const override
	{
		return 100;
	};
public:
	bool IsLngLat() const override
	{
		return false;
	}
public:
	inline void GeoToMap(const double& λ, const double& ψ, float& x, float& y) const override;
	inline void GeoToMap(const double& λ, const double& ψ, double& x, double& y) const override;
	inline void MapToGeo(const double& x, const double& y, double& λ, double& ψ) const override;

	inline CPointF GeoToMap(const double& λ, const double& ψ) const override;
	inline CPointF MapToGeo(const double& x, const double& y) const override;

	inline CPointF GeoToMap(const CPointF& geoPoint) const override;
	inline CPointF MapToGeo(const CPointF& mapPoint) const override;

	std::pair<int, CPointF*> GeoToMap(Link::Vertex* link, const Interpolation& interpolation, const float tolerance) const override;
public:
	BOOL operator == (const CProjection& projection) const override;
	void CopyTo(CProjection* pProjection) const override;
	void Setup() override;
	void Reconfig() override;
	void Config(std::optional<double> lng0, std::optional<double> lat0) override;
	CPointF GetCenter() const override
	{
		return CPointF(m_lng0, m_lat0);
	}
	double UserToMap(double user) const override
	{
		return (double)user;
	}
	double MapToUser(double map) const override
	{
		return (double)map;
	}
	void SetCentral(float lng, float lat) override
	{
		if(lng != m_lng0 || lat != m_lat0)
		{
			Config(lng, lat);
		}
	}
	optional<double> GetRadius() const
	{
		ProjType* projtype = CGeocentric::GetProjType(m_keyProj4);
		return projtype==nullptr ? (optional<double>)std::nullopt : (projtype->clip == 0 ? (std::optional<double>)std::nullopt : projtype->clip);
	}
public:
	void Serialize(CArchive& ar, const DWORD& dwVersion) override;
	void ReleaseWeb(CFile& file) override;
	void ReleaseWeb(BinaryStream& stream) override;
	void ReleaseWeb(boost::json::object& json) override;
	void ReleaseWeb(pbf::writer& writer) const override;
public:
	typedef struct
	{
		const char* key;
		const char* name;
		bool b_lng0;
		bool b_lat0;
		bool b_lat1;
		bool b_lat2;
		bool b_lats;
		bool b_lngc;
		bool b_lng1;
		bool b_lng2;
		bool b_k0;
		bool b_south;
		bool b_zone;
		bool b_h;
		bool b_alpha;
		bool b_gamma;
		bool invertible;
		double clip;
	} ProjType;

	static ProjType* GetProjType(const char* pszKey) 
	{
		if(pszKey == nullptr)
			return nullptr;

		for(int index = 0; CGeocentric::ProjTypes[index].key != nullptr; index++)
		{
			if(strcmp(CGeocentric::ProjTypes[index].key, pszKey) == 0)
				return &CGeocentric::ProjTypes[index];
		}
		return nullptr;
	};
	static ProjType* GetProjTypes()
	{
		return ProjTypes;
	}
	static void proj4log(void* data, int logLevel, const char* message);
	inline static ProjType ProjTypes[] = {
		//	Key			Name									lng0	lat0	lat1	lat2	lats	lngc	lng1	lng2	k		south	zone	h		α		γ		invertible clip			
		{"adams_hemi",	"Adams Hemisphere in a Square",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			constants::halfpi},
		{"adams_ws1",	"Adams World in a Square I",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"adams_ws2",	"Adams World in a Square II",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"aea",			"Albers Equal Area",					1,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"aeqd",		"Azimuthal Equidistant",				1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			constants::pi-0.001*constants::degreeToradian},
		{"airy",		"Airy",									1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			constants::halfpi-0.1},
		{"aitoff",		"Aitoff",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"apian",		"Apian Globular I",						1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"august",		"August Epicycloidal",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"bacon",		"Bacon Globular",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
	//	{"bertin1953",	"Bertin 1953",							0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0}, hardcoded rotation in the code
	//	{"bipc",		"Bipolar conic of western hemisphere",	1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"boggs",		"Boggs Eumorphic",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
	//	{"cass",		"Cassini",								1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"cc",			"Central Cylindrical",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"ccon",		"Central Conic",						1,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"cea",			"Equal Area Cylindrical",				1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"chamb",		"Chamberlin Trimetric",					1,		0,		1,		1,		0,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,			constants::halfpi},
		{"collg",		"Collignon",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"comill",		"Compact Miller",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"crast",		"Craster Parabolic (Putnins P4)",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"denoy",		"Denoyer Semi-Elliptical",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"eck1",		"Eckert I",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eck2",		"Eckert II",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eck3",		"Eckert III",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eck4",		"Eckert IV",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eck5",		"Eckert V",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eck6",		"Eckert VI",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eqc",			"Equidistant Cylindrical(Plate Caree)",	1,		1,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eqdc",		"Equidistant Conic",					1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"eqearth",		"Equal Earth",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"euler",		"Euler",								1,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"fahey",		"Fahey",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"fouc",		"Foucaut",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"fouc_s",		"Foucaut Sinusoidal",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"gall",		"Gall(Gall Stereographic)",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"geos",		"Geostationary Satellite View",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		0,		0,		1,			0},
		{"gins8",		"Ginsburg VIII (TsNIIGAiK)",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"gn_sinu",		"General Sinusoidal Series",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"gnom",		"Gnomonic",								1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			60*constants::degreeToradian},
		{"goode",		"Goode Homolosine",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"guyou",		"Guyou",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			constants::halfpi},
		{"hammer",		"Hammer & Eckert-Greifendorff",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"hatano",		"Hatano Asymmetrical Equal Area",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"healpix",		"HEALPix",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"rhealpix",	"rHEALPix",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"igh",			"Interrupted Goode Homolosine",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"igh_o",		"Interrupted Goode Homolosine(Oceanic View)",1,	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"imoll",		"Interrupted Mollweide",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"imoll_o",		"Interrupted Mollweide (Oceanic View)",	1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"imw_p",		"Internation Map of the World Polyconic",1,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"isea",		"Icosahedral Snyder Equal Area",		1,		0,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"kav5",		"Kavraisky V",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"kav7",		"Kavraisky VII",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"laea",		"Lambert Azimuthal Equal Area",			1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			constants::halfpi},
		{"lagrng",		"Lagrange",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"larr",		"Larrivee",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"lask",		"Laskowski",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"lcc",			"Lambert Conformal Conic",				1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"lcca",		"Lambert Conformal Conic Alternative",	1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"leac",		"Lambert Equal Area Conic",				1,		0,		1,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		1,			0},
	//	{"lee_os",		"Lee Oblated Stereographic",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"loxim",		"Loximuthal",							1,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"lsat",		"Space oblique for LANDSAT",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mbt_s",		"McBryde-Thomas Flat-Polar Sine",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mbt_fps",		"McBryde-Thomas Flat-Pole Sine (No. 2)",1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mbtfpp",		"McBride-Thomas Flat-Polar Parabolic",	1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mbtfpq",		"McBryde-Thomas Flat-Polar Quartic",	1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mbtfps",		"McBryde-Thomas Flat-Polar Sinusoidal", 1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"merc",		"Mercator",								1,		0,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		0,		1,			0},
	//	{"mil_os",		"Miller Oblated Stereographic",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"mill",		"Miller Cylindrical",					1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"misrsom",		"Space oblique for MISR",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"moll",		"Mollweide",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"murd1",		"Murdoch I",							1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"murd2",		"Murdoch II",							1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"murd3",		"Murdoch III",							1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"natearth",    "Natural Earth",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"natearth2",   "Natural Earth II",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"nell",		"Nell",									1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"nell_h",		"Nell-Hammer",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"nicol",		"Nicolosi Globular",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"nsper",		"Near-sided perspective",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"ob_tran",		"General Oblique Transformation",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"ocea",		"Oblique Cylindrical Equal Area",		1,		0,		1,		1,		0,		1,		1,		1,		0,		0,		0,		0,		1,		0,		1,			0},
		{"oea",			"Oblated Equal Area",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"omerc",		"Oblique Mercator",						1,		1,		0,		0,		0,		1,		0,		0,		1,		0,		0,		0,		1,		1,		1,			0},
		{"ortel",		"Ortelius Oval",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"ortho",		"Orthographic",							1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			constants::halfpi},
		{"patterson",	"Patterson",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			constants::halfpi+constants::ε1*constants::degreeToradian},
	//	{"pconic",		"Perspective Conic",					1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"peirce_q",	"Peirce Quincuncial",					1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"poly",		"Polyconic (American)",					1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp1",		"Putnins P1",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp2",		"Putnins P2",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp3",		"Putnins P3",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp3p",		"Putnins P3p",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp4p",		"Putnins P4",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp5",		"Putnins P5",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp5p",		"Putnins P5p",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp6",		"Putnins P6",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"putp6p",		"Putnins P6p",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"qua_aut",		"Quartic Authalic",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"qsc",			"Quadrilateralized Spherical Cube",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"robin",		"Robinson",								1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"rouss",		"Roussilhe Stereographic",				1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"rpoly",		"Rectangular Polyconic",				1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
	//	{"s2",			"S2",									1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
	//	{"som",			"Space Oblique Mercator (SOM)",			1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"stere",		"Stereographic",						1,		1,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		0,		1,			142*constants::degreeToradian},
	//	{"sterea",		"Oblique Stereographic Alternative",	1,		1,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		0,		1,			0},
	//	{"gstmerc",		"Gauss-Schreiber Transverse Mercator",	1,		1,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		0,		1,			0},
		{"tcc",			"Transverse Central Cylindrical",		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"tcea",		"Transverse Cylindrical Equal Area",	1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"times",		"Times",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"tissot",		"Tissot Conic",							1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"tmerc",		"Transverse Mercator",					1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"tobmerc",		"Tobler-Mercator",						1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			constants::halfpi},
		{"tpeqd",		"Two Point Equidistant",				1,		0,		1,		1,		0,		0,		1,		1,		0,		0,		0,		0,		0,		0,		1,			130*constants::degreeToradian},
		{"tpers",		"Tilted perspective",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"ups",			"Universal Polar Stereographic",		1,		0,		0,		0,		0,		0,		0,		0,		0,		1,		0,		0,		0,		0,		1,			0},
		{"urm5",		"Urmaev V",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"urmfps",		"Urmaev Flat-Polar Sinusoidal",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"utm",			"Universal Transverse Mercator (UTM)",	0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		1,		0,		0,		0,		1,			0},
		{"vandg",		"van der Grinten (I)",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"vandg2",		"van der Grinten II",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"vandg3",		"van der Grinten III",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"vandg4",		"van der Grinten IV",					1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
		{"vitk1",		"Vitkovsky I",							1,		1,		1,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag1",		"Wagner I (Kavraisky VI)",				1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag2",		"Wagner II",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag3",		"Wagner III",							1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag4",		"Wagner IV",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag5",		"Wagner V",								1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag6",		"Wagner VI",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wag7",		"Wagner VII",							1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,			0},
	//	{"webmerc",		"Web Mercator/Pseudo Mercator",			1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wink1",		"Winkel I",								1,		0,		0,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wink2",		"Winkel II",							1,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"wintri",		"Winkel Tripel",						1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{"google",		"Transverse Mercator",					1,		0,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0},
		{nullptr,		"",										1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,			0}
	};
};
