#pragma once

#include "Projection1.h"
#include "../Utility/Constants.hpp"
class __declspec(dllexport) CGeographic : public CProjection
{
protected:
	DECLARE_DYNCREATE(CGeographic);
public:
	CGeographic();

public:
	bool IsLngLat() const override
	{
		return true;
	}
public:
	inline void GeoToMap(const double& λ, const double& ψ, float& x, float& y) const override;
	inline void GeoToMap(const double& λ, const double& ψ, double& x, double& y) const override;
	inline void MapToGeo(const double& x, const double& y, double& λ, double& ψ) const override;

	inline CPointF GeoToMap(const double& λ, const double& ψ) const override;
	inline CPointF MapToGeo(const double& x, const double& y) const override;
	std::pair<int, CPointF*> GeoToMap(Link::Vertex* link, const Interpolation& interpolation, const float tolerance) const override;
	inline CPointF GeoToMap(const CPointF& geoPoint) const override
	{
		return geoPoint;
	};
	inline CPointF MapToGeo(const CPointF& mapPoint) const override
	{
		return mapPoint;
	}
	
public:
	BYTE Gettype() const override { return 1; };
	CString GetName() const override { return _T("Geographic"); };
	double UserToMap(double user) const override
	{
		return user*constants::pi/180;
	};
	double MapToUser(double map) const override
	{
		return map*180/constants::pi;
	};
	double GetDilationDocToMap() const override
	{
		return 0.0000001*constants::pi/180;
		
	};
	double GetDilationMapToDoc() const override
	{
		return 10000000*180/constants::pi;
	};
	optional<double> GetRadius() const override
	{
		return constants::pi;
	}
public:
	void ReleaseWeb(boost::json::object& json) override;
	void ReleaseWeb(pbf::writer& writer) const override;
public:
	BOOL operator ==(const CProjection& projection) const override;
	void Setup() override
	{
	};
};

