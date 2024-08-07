#include "stdafx.h"

#include "Projection1.h"
#include "Geographic.h"
#include "Geocentric.h"
#include "../Utility/Lnglat.hpp"
#include "../Utility/Constants.hpp"
#include <math.h>

#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CProjection, CObject)

CProjection::CProjection()
{
	m_strUnit = "Degree";
	SetupGCS("sphere");
}

CProjection::~CProjection()
{
}

BOOL CProjection::operator==(const CProjection& projection) const
{
	if(this->Gettype() != projection.Gettype())
		return FALSE;
	else if(GCS.m_Key != nullptr && projection.GCS.m_Key != nullptr && strcmp(GCS.m_Key, projection.GCS.m_Key) != 0)
		return FALSE;
	else
		return TRUE;
}

BOOL CProjection::operator != (const CProjection& projection) const
{
	return !(*this == projection);
}

void CProjection::SetupGCS(const char* key)
{
	if(key && GCS.m_Key && strcmp(key, GCS.m_Key) == 0)
		return;

	GCS.Setup(key);
}

void CProjection::Draw(CView* pView, CDC* pDC, const CRect& inRect) const
{
}

void CProjection::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(ar.IsStoring())
	{
		ar << CStringA(GCS.m_Key);
	}
	else
	{
		CStringA strKey;
		ar >> strKey;
		SetupGCS(strKey.GetString());
	}
}

void CProjection::Write(CFile& file)
{
}

void CProjection::Read(CFile& file)
{
}

void CProjection::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CProjection::ReleaseDCOM(CArchive& ar)
{
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void CProjection::ReleaseWeb(CFile& file)
{
	GCS.ReleaseWeb(file);
}
void CProjection::ReleaseWeb(BinaryStream& stream)
{
	GCS.ReleaseWeb(stream);
}
void CProjection::ReleaseWeb(boost::json::object& json)
{
	GCS.ReleaseWeb(json);
}
void CProjection::ReleaseWeb(pbf::writer& writer) const
{
	GCS.ReleaseWeb(writer);
}
CProjection* CProjection::Apply(BYTE type)
{
	switch(type)
	{
		case 0XFF:
			return nullptr;
		case 0://backward compatibility only
		case 1:
			return new CGeographic();
		case 2:
			return new CGeocentric();
		default://backward compatibility only
			return new CGeocentric();
	}
}
CProjection* CProjection::Apply(const char* proj4, const char* ellps)
{
	if(proj4 == nullptr)
		return nullptr;
	else if(ellps == nullptr)
		return new CGeographic();
	else
		return new CGeocentric(proj4, ellps);
}

CProjection* CProjection::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CProjection* pClone = (CProjection*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	ASSERT_VALID(pClone);
	if(this->IsKindOf(RUNTIME_CLASS(CGeocentric)) == TRUE)
	{
		::free(((CGeocentric*)pClone)->m_keyProj4);
		((CGeocentric*)pClone)->m_keyProj4 = _strdup(((CGeocentric*)this)->m_keyProj4);
	}
	pClone->Reconfig();
	return pClone;
}

void CProjection::CopyTo(CProjection* pProjection) const
{
	if(pProjection->IsKindOf(RUNTIME_CLASS(CProjection)) == TRUE)
	{
		pProjection->SetupGCS(GCS.m_Key);
	}
}

double CProjection::GeoToMap(double geoValue) const
{
	const CPointF geoPoint1 = CPointF(0, 0);
	const CPointF geoPoint2 = CPointF(geoValue, 0);
	const CPointF mapPoint1 = this->GeoToMap(geoPoint1);
	const CPointF mapPoint2 = this->GeoToMap(geoPoint2);
	return std::abs(mapPoint2.x - mapPoint1.x);
}

double CProjection::MapToGeo(double mapValue) const
{
	const CPointF mapPoint1 = CPointF(0, 0);
	const CPointF mapPoint2 = CPointF(mapValue, 0);
	const CPointF geoPoint1 = this->MapToGeo(mapPoint1);
	const CPointF geoPoint2 = this->MapToGeo(mapPoint2);
	return std::abs(geoPoint2.x - geoPoint1.x);
}

CRectF CProjection::GeoToMap(const CRectF& geoRect) const
{
	const double minX1 = geoRect.left;
	const double maxX1 = geoRect.right;
	const double minY1 = geoRect.top;
	const double maxY1 = geoRect.bottom;
	double minX2 = +100000000000;//std::numeric_limits<double>::max();
	double maxX2 = -100000000000;//std::numeric_limits<double>::min(); 
	double minY2 = +100000000000;//std::numeric_limits<double>::max(); 
	double maxY2 = -100000000000;//std::numeric_limits<double>::min(); 
	for(int index1 = 0; index1 <= 10; index1++)//data's border is not same as view's border
	{
		for(int index2 = 0; index2 <= 10; index2++)
		{
			CPointF geoPoint;
			geoPoint.x = minX1 + (maxX1 - minX1)*index1/10;
			geoPoint.y = minY1 + (maxY1 - minY1)*index2/10;
			CPointF mapPoint = this->GeoToMap(geoPoint);
			minX2 = min(minX2, mapPoint.x);
			maxX2 = max(maxX2, mapPoint.x);
			minY2 = min(minY2, mapPoint.y);
			maxY2 = max(maxY2, mapPoint.y);
		}
	}
	return CRectF(minX2, minY2, maxX2, maxY2);
}

CRectF CProjection::MapToGeo(const CRectF& mapRect) const
{
	const CPointF geoPoint1 = this->MapToGeo(CPointF(mapRect.left, mapRect.top));
	const CPointF geoPoint2 = this->MapToGeo(CPointF(mapRect.left, mapRect.bottom));
	const CPointF geoPoint3 = this->MapToGeo(CPointF(mapRect.right, mapRect.bottom));
	const CPointF geoPoint4 = this->MapToGeo(CPointF(mapRect.right, mapRect.top));

	CRectF geoRect;
	geoRect.left = min(min(geoPoint1.x, geoPoint2.x), min(geoPoint3.x, geoPoint4.x));
	geoRect.top = min(min(geoPoint1.y, geoPoint2.y), min(geoPoint3.y, geoPoint4.y));
	geoRect.right = max(max(geoPoint1.x, geoPoint2.x), max(geoPoint3.x, geoPoint4.x));
	geoRect.bottom = max(max(geoPoint1.y, geoPoint2.y), max(geoPoint3.y, geoPoint4.y));
	return geoRect;
}

CRectF CProjection::MapToMap(CProjection* pProjection, const CRectF& mapRect) const
{
	if(pProjection == nullptr)
		return mapRect;
	else if(pProjection == this)
		return mapRect;
	else if(*pProjection == *this)
		return mapRect;

	const double minX1 = mapRect.left;
	const double maxX1 = mapRect.right;
	const double minY1 = mapRect.top;
	const double maxY1 = mapRect.bottom;
	double minX2 = +100000000000;//std::numeric_limits<double>::max();
	double maxX2 = -100000000000;//std::numeric_limits<double>::min(); 
	double minY2 = +100000000000;//std::numeric_limits<double>::max(); 
	double maxY2 = -100000000000;//std::numeric_limits<double>::min(); 
	for(int index1 = 0; index1 <= 10; index1++)//data'sborderisotsameasview'sborder
	{
		for(int index2 = 0; index2 <= 10; index2++)
		{
			CPointF mapPoint1;
			mapPoint1.x = minX1 + (maxX1 - minX1)*index1/10;
			mapPoint1.y = minY1 + (maxY1 - minY1)*index2/10;
			CPointF geoPoint1 = this->MapToGeo(mapPoint1);
			const CPointF geoPoint2 = this->GCS.GeoToGeo(pProjection->GCS, geoPoint1);
			if(isinf(geoPoint2.x) || isinf(geoPoint2.y))
				continue;

			CPointF mapPoint2 = pProjection->GeoToMap(geoPoint2);
			minX2 = min(minX2, mapPoint2.x);
			maxX2 = max(maxX2, mapPoint2.x);
			minY2 = min(minY2, mapPoint2.y);
			maxY2 = max(maxY2, mapPoint2.y);
		}
	}

	return CRectF(minX2, minY2, maxX2, maxY2);
}

double CProjection::Distance(double fλ, double fψ, double tλ, double tψ) const
{
	double t = 0.f;
	if(tλ == fλ && tψ == fψ)
		t = 0.0f;
	else if(tλ == fλ)
		t = std::abs(tψ - fψ);
	else
	{
		const double g = atan(1 / (tan(tψ) * cos(fψ) * (1 / sin(tλ - fλ)) - (1 / tan(tλ - fλ)) * sin(fψ)));
		t = std::abs(asin(cos(tψ) * sin(tλ - fλ) / sin(g)));
	}
	return std::abs(1000 * t* constants::radianTodegree * 60 * 6371.118 / 3437.75);
}

double CProjection::CalLength(CPointF* pPoints, int nAnchors) const
{
	if(nAnchors >= 2)
	{
		double length = 0;

		for(long i = 0; i < nAnchors - 1; i++)
		{
			const double λ1 = pPoints[i].x;
			const double ψ1 = pPoints[i].y;
			const double λ2 = pPoints[i + 1].x;
			const double ψ2 = pPoints[i + 1].y;

			double t = 0;
			if(λ2 == λ1 && ψ2 == ψ1)
			{
				t = 0.0f;
			}
			else if(λ2 == λ1)
			{
				t = std::abs(ψ2 - ψ1);
			}
			else
			{
				const double g = sin(ψ1)*sin(ψ2) + cos(ψ1)*cos(ψ2)*cos(std::abs(λ2 - λ1));
				t = acos(g);
				//double g = atan(1/(tan(ψ2)*cos(ψ1)/sin(λ2-λ1)-sin(ψ1)/tan(λ2-λ1)));
				//t = fstd::abs(asin(cos(ψ2)*sin(λ2-λ1)/sin(g)));
			}

			length += t*180/M_PI*60*6371.118/3437.75;
		}

		return length;
	}

	return 0;
}

double CProjection::CalArea(CPointF* pPoints, int nAnchors) const
{
	if(nAnchors >= 2)
	{
		double λ1 = pPoints[0].x;
		double ψ1 = pPoints[0].y;
		double areaall = 0;
		for(long i = 0; i < nAnchors; i++)
		{
			double λ2 = pPoints[i].x;
			double ψ2 = pPoints[i].y;
			if(λ1 != λ2)
			{
				if(ψ1 > ψ2)
				{
					double latitemp;
					latitemp = ψ1;
					ψ1 = ψ2;
					ψ2 = latitemp;
				}

				//f21=(f2-f1)*(λ2-λ1)
				const double sinlati1 = sin(ψ1), sinlati2 = sin(ψ2);
				const double area2 = GCS.ellps.a*GCS.ellps.a*(1 - GCS.ellps.fe2)*(sinlati2/(1 - GCS.ellps.fe2*sinlati2*sinlati2) + 1/(2*GCS.ellps.fe1)*log((1 + GCS.ellps.fe1*sinlati2)/1 - GCS.ellps.fe1*sinlati2));
				const double area1 = GCS.ellps.a*GCS.ellps.a*(1 - GCS.ellps.fe2)*(sinlati1/(1 - GCS.ellps.fe2*sinlati1*sinlati1) + 1/(2*GCS.ellps.fe1)*log((1 + GCS.ellps.fe1*sinlati1)/1 - GCS.ellps.fe1*sinlati1));
				double area = (area2 - area1)*(λ2 - λ1);
				const double distance1 = std::abs(GCS.ellps.a*cos(ψ1)*(λ2 - λ1)/sqrt(1 - GCS.ellps.fe2*sinlati1*sinlati1));
				const double distance2 = std::abs(GCS.ellps.a*cos(ψ2)*(λ2 - λ1)/sqrt(1 - GCS.ellps.fe2*sinlati2*sinlati2));

				area = area*distance1/(distance1 + distance2) + area1*(λ2 - λ1);//»¡ÏßÖÁ³àµÀµÄÃæ»ý
				areaall += area;
			}
			λ1 = λ2;
			ψ1 = ψ2;
		}

		return std::abs(areaall);
	}

	return 0;
}

void CProjection::ExportMapInfoTab(FILE* fileMif, double left, double top, double right, double bottom)
{
	fprintf(fileMif, "CoordSys Earth Units \"m\" Bounds (%d, %d) (%d, %d)\n", left, top, right, bottom);
}

void CProjection::ExportMapInfoMif(FILE* fileMif, double left, double top, double right, double bottom)
{
	fprintf(fileMif, "CoordSys Earth Units \"m\" Bounds (%d, %d) (%d, %d)\n", left, top, right, bottom);
}
