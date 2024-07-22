#pragma once

#include "Gcs.h"

#include "../Public/Global.h"
#include "../Utility/Link.hpp"
#include <numbers> 
#include <optional> 

namespace boost {
	namespace json {
		class object;
	}
};
namespace pbf {
	class writer;
	using tag = uint8_t;
};
enum Interpolation : BYTE
{
	Nothing = 0,
	Linear = 1,
	GreatCircle = 4
};
typedef struct
{
	const char* key;
	const char* name;
	double a;//major semi-axis
	double b;//minor semi-axis 
	double f;//Flattening
	double Dx;
	double Dy;
	double Dz;
	double Rx;
	double Ry;
	double Rz;
	double M;
} Spheroid;

class __declspec(dllexport) CProjection abstract : public CObject
{
	DECLARE_DYNAMIC(CProjection);
public:
	CProjection();
	CProjection(double lngc);

	~CProjection() override;
public:
	CGcs GCS;
	CString m_strUnit;
	virtual double GetDilationDocToMap() const = 0;
	virtual double GetDilationMapToDoc() const = 0;

public:
	virtual bool IsInvertible() const
	{
		return true;
	}
	virtual bool IsLngLat() const = 0;
public:
	virtual double GeoToMap(double geoValue) const;
	virtual double MapToGeo(double mapValue) const;

	virtual void GeoToMap(const double& λ, const double& ψ, float& x, float& y) const = 0;
	virtual void GeoToMap(const double& λ, const double& ψ, double& x, double& y) const = 0;
	virtual void MapToGeo(const double& x, const double& y, double& λ, double& ψ) const = 0;

	virtual CPointF GeoToMap(const double& λ, const double& ψ) const = 0;
	virtual CPointF MapToGeo(const double& x, const double& y) const = 0;
	virtual CPointF GeoToMap(const CPointF& geoPoint) const = 0;
	virtual CPointF MapToGeo(const CPointF& mapPoint) const = 0;

	virtual CRectF GeoToMap(const CRectF& geoRect) const;
	virtual CRectF MapToGeo(const CRectF& mapRect) const;

	virtual std::pair<int, CPointF*> GeoToMap(Link::Vertex* link, const Interpolation& interpolation, const float tolerance) const = 0;
public:
	virtual CRectF MapToMap(CProjection* pProjection, const CRectF& mapRect) const;

public:
	virtual double Distance(double fLng, double fLat, double tLng, double tLat) const;
	virtual double CalLength(CPointF* pPoints, int nAnchors) const;
	virtual double CalArea(CPointF* pPoints, int nAnchors) const;

	virtual void Draw(CView* pView, CDC* pDC, const CRect& inRect) const;

public:
	virtual BOOL operator ==(const CProjection& projection) const;
	virtual BOOL operator !=(const CProjection& projection) const;
	virtual CProjection* Clone() const;
	virtual void CopyTo(CProjection* pProjection) const;
	virtual void Reconfig()
	{
	}
	virtual void Config(std::optional<double> lng0, std::optional<double> lat0)
	{
	}
	virtual	BYTE Gettype() const { return 0XFF; }
	virtual CString GetName() const = 0;
	virtual void Setup() = 0;
	void SetupGCS(const char* key);

public:
	virtual int UintMinX() const { return -180; };
	virtual int UintMaxX() const { return 180; };
	virtual int UintMinY() const { return -90; };
	virtual int UintMaxY() const { return 90; };
	virtual double UserToMap(double user) const = 0;
	virtual double MapToUser(double map) const = 0;
	virtual CPointF GetCenter() const { return CPointF(0, 0); }
	virtual void SetCentral(float lng, float lat) {};
	virtual optional<double> GetRadius() const = 0;
public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
	virtual void ReleaseCE(CArchive& ar) const;
	virtual void ReleaseDCOM(CArchive& ar);

	virtual void Write(CFile& file);
	virtual void Read(CFile& file);

	virtual void ReleaseWeb(CFile& file);
	virtual void ReleaseWeb(BinaryStream& stream);
	virtual void ReleaseWeb(boost::json::object& json);
	virtual void ReleaseWeb(pbf::writer& writer) const;

	virtual void ExportMapInfoTab(FILE* fileMif, double left, double top, double right, double bottom);
	virtual void ExportMapInfoMif(FILE* fileMif, double left, double top, double right, double bottom);

public:
	static CProjection* Apply(const BYTE type);
	static CProjection* Apply(const char* proj4, const char* ellps);

public:
	inline static Spheroid SpheroidList[] = {
		//key			name											a				b 					f				Dx	Dy	Dz	Rx	Ry	Rz	M
		{"sphere",		"Normal Sphere(r=6370997)",					6370997,		6370997,			0,					0,	0,	0,	0,	0,	0,	1},
		{"GRS80",		"GRS 1980(IUGG, 1980)",						6378137.0,		6356752.314,		1/298.257222101,	0,	0,	0,	0,	0,	0,	1},
		{"Beijing54",	"Beijing 54",								6378245.0,		6356863.0188,		1/298.3,			-49.704, -52.426, 29.700, 0, 0, 0, 1},
		{"WGS84",		"WGS 84",									6378137.0,		6356752.314,		1/298.257223563,	0,	0,	0,	0,	0,	0,	1},
		{"krass",		"Krassovsky 1942",							6378245.0,		6356863.019,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"intl",		"International 1909(Hayford)",				6378388.0,		6356911.946,		1/297.0,			0,	0,	0,	0,	0,	0,	1},
		{"clrk80",		"Clarke 1880",								6378249.145,	6356514.870,		1/293.465,			0,	0,	0,	0,	0,	0,	1},
		{"clrk66",		"Clarke 1866",								6378206.4,		6356583.800,		1/294.9786982,		0,	0,	0,	0,	0,	0,	1},
		{"airy",		"Airy 1830",								6377563.396,	6356256.909,		1/299.3249646,		0,	0,	0,	0,	0,	0,	1},
		{"bessel",		"Bessel 1841",								6377397.155,	6356078.963,		1/299.1528128,		0,	0,	0,	0,	0,	0,	1},
		{"evrst30",		"Everest 1830",								6377276.345,	6356075.413,		1/300.8017,			0,	0,	0,	0,	0,	0,	1},		
		{"bessnam",		"Bessel 1841 (modified for Namibia)",		6377483.865,	6356165.383,		1/299.1528128,		0,	0,	0,	0,	0,	0,	1},
		{"modairy",		"Airy 1930(modified for Ireland 1965)",		6377340.189,	6356034.448,		1/299.3249646,		0,	0,	0,	0,	0,	0,	1},
		{"evrst48",		"Everest 1948",								6377304.063,	6356103.038993,		1/300.8017,			0,	0,	0,	0,	0,	0,	1},
		{"fschr60",		"Fischer 1960",								6378166.0,		6356784.283666,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"fschr60m",	"Fischer 1960(modified for South Asia)",	6378155.0,		6356773.320,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"fschr68",		"Fischer 1968",								6378150.0,		6356768.337303,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"GRS67",		"GRS 67(IUGG 1967)",						6378160.0,		6356774.516,		1/298.247167427,	0,	0,	0,	0,	0,	0,	1},
		{"helmert",		"Helmert 1906",								6378200.0,		6356818.170,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"hough",		"Hough",									6378270.0,		6356794.343,		1/297.0,			0,	0,	0,	0,	0,	0,	1},
		{"WGS60",		"WGS 60",									6378165.0,		6356783.286959,		1/298.3,			0,	0,	0,	0,	0,	0,	1},
		{"WGS66",		"WGS 66",									6378145.0,		6356759.769,		1/298.25,			0,	0,	0,	0,	0,	0,	1},
		{"MERIT",		"MERIT 83",									6378137.0,		6356772.2,			1/298.257,			0,	0,	0,	0,	0,	0,	1},
		{"newintl",		"New International 1967",					6378157.5,		6356772.2,			1/298.25,			0,	0,	0,	0,	0,	0,	1},
		{"walbeck",		"Walbeck",									6376896.0,		6355834.8467,		1/302.78,			0,	0,	0,	0,	0,	0,	1},
		{"evrstSS",		"Everest (Sabah & Sarawak)",				6377298.556,	6356097.550,		1/300.8017,			0,	0,	0,	0,	0,	0,	1},
		{"evrst56",		"Everest 1956",								6377301.243,	6356100.228,		1/300.80174,		0,	0,	0,	0,	0,	0,	1},
		{"NWL9D",		"Naval Weapons Lab., 1965",					6378145.0,		6356759.76948868,	1/298.25,			0,	0,	0,	0,	0,	0,	1},
		{"plessis",		"Plessis 1817",								6376523.0,		6355862.933,		1/308.64,			0,	0,	0,	0,	0,	0,	1},
		{"evrst69",		"Everest 1969",								6377295.664,	6356103.039,		1/300.8017,			0,	0,	0,	0,	0,	0,	1},
		{"WGS72",		"WGS 72",									6378135.0,		6356750.520,		1/298.26,			0,	0,	0,	0,	0,	0,	1}, //was 1 
		{"SGS85",		"Soviet Geodetic System 85",				6378136.0,		0,					1/298.257,			0,	0,	0,	0,	0,	0,	1},
		{"IAU76",		"IAU 1976",									6378140.0,		0,					1/298.257,			0,	0,	0,	0,	0,	0,	1},
		{"airy",		"Airy 1830",								6377563.396,	6356256.910,		1/299.3249646,		0,	0,	0,	0,	0,	0,	1},
		{"APL4.9",		"Appl. Physics. 1965",						6378137.0,		0,					1/298.25,			0,	0,	0,	0,	0,	0,	1},
		{"andrae",		"Andrae 1876 (Den., Iclnd.)",				6377104.43,		0,					1/300.0,			0,	0,	0,	0,	0,	0,	1},
		{"CPM",			"Comm. des Poids et Mesures 1799",			6375738.7,		0,					1/334.29,			0,	0,	0,	0,	0,	0,	1},
		{"delmbr",		"Delambre 1810 (Belgium)",					6376428,		0,					1/311.5,			0,	0,	0,	0,	0,	0,	1},
		{"engelis",		"Engelis 1985",								6378136.05,		0,					1/298.2566,			0,	0,	0,	0,	0,	0,	1},
		{"kaula",		"Kaula 1961",								6378163.0,		0,					1/298.24,			0,	0,	0,	0,	0,	0,	1},
		{"lerch",		"Lerch 1979",								6378139.0,		0,					1/298.257,			0,	0,	0,	0,	0,	0,	1},
		{"mprts",		"Maupertius 1738",							6397300.0,		0,					1/191.0,			0,	0,	0,	0,	0,	0,	1},
		{"aust_SA",		"Australian  Natl & S. Amer. 1969",			6378160.0,		6356774.719,		1/298.25,			0,	0,	0,	0,	0,	0,	1}, //was 2
		{nullptr,		nullptr,									0,				0,					0,					0,	0,	0,	0,	0,	0,	1}
	};
	static Spheroid* GetSpheroidList()
	{
		return SpheroidList;
	}
};