#include "stdafx.h"
#include "Geographic.h"


#include "../Utility/Constants.hpp"
#include "../Utility/Link.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGeographic, CProjection)
CGeographic::CGeographic()
	: CProjection()
{
	m_strUnit = "Degree";	
	SetupGCS("sphere");
}

BOOL CGeographic::operator==(const CProjection& projection) const
{
	CRuntimeClass* pRuntimeClass1 = this->GetRuntimeClass();
	CRuntimeClass* pRuntimeClass2 = projection.GetRuntimeClass();
	if(pRuntimeClass1 != pRuntimeClass2)
		return FALSE;
	else
		return CProjection::operator==(projection);
}


void CGeographic::GeoToMap(const double& λ, const double& ψ, float& x, float& y) const
{
	x = λ;
	y = ψ;
}
void CGeographic::GeoToMap(const double& λ, const double& ψ, double& x, double& y) const
{
	x = λ;
	y = ψ;
}
void CGeographic::MapToGeo(const double& x, const double& y, double& λ, double& ψ) const
{
	λ = x;
	ψ = y;
}
CPointF CGeographic::GeoToMap(const double& λ, const double& ψ) const
{
	return CPointF(λ, ψ);
}

CPointF CGeographic::MapToGeo(const double& x, const double& y) const
{
	return CPointF(x, y);
}
std::pair<int, CPointF*> CGeographic::GeoToMap(Link::Vertex* link, const Interpolation& interpolation, const float tolerance) const
{
	unsigned int count = Link::GetLength(link);
	CPointF* points = new CPointF[count];
	Link::Vertex* pointer1 = link;
	CPointF* pointer2 = points;
	while(pointer1!=nullptr&&pointer2!=nullptr)
	{
		this->GeoToMap(pointer1->λ, pointer1->ψ, pointer2->x, pointer2->y);
		pointer1 = pointer1->next;
		pointer2++;
	}
	return std::pair<int, CPointF*>(count, points);;	
};
void CGeographic::ReleaseWeb(boost::json::object& json)
{
	CProjection::ReleaseWeb(json);
}
void CGeographic::ReleaseWeb(pbf::writer& writer) const
{
	CProjection::ReleaseWeb(writer);
}