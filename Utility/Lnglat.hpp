#pragma once

#include <cmath>
#include <numbers> 
#include "constants.hpp" 

namespace Lnglat
{
	//the follow function is used for translate angle to radian
	static double RadiantoDegree(double radian)
	{
		return radian*constants::radianTodegree;
	}
	static inline double Regularize(const double& lng)
	{
		return max(-180, min(180, lng > 180 ? std::fmod(lng, 360) - 360 : (lng < -180 ? std::fmod(lng, 360) + 360 : lng)));
	}
	static inline double Rotate(const double& lng, const double& delta)
	{
		return Lnglat::Regularize(lng + delta);
	}
	static CPointF Regularize(const CPointF& point)
	{
		double lng = point.x > 180 ? point.x - 360 : point.x < -180 ? point.x + 360 : point.x;
		double lat = point.y > 90 ? point.y - 180 : point.y < -90 ? point.y + 180 : point.y;
		lng = max(-180, min(180, point.x));
		lat = max(-90, min(90, point.y));
		return CPointF(lng, lat);
	}
	static inline void Regularizeλ(double& λ)
	{
		if(λ > constants::pi)
			λ -= constants::twopi;
		else if(λ < -constants::pi)
			λ += constants::twopi;
	}
	static inline double Regularizeψ(const double& ψ)
	{
		if(ψ > constants::halfpi)
			return ψ - constants::pi;
		else if(ψ < -constants::halfpi)
			return ψ +  constants::pi;
		else
			return ψ;
	}
	static CPointF Regularize(double x, double y)
	{
		double lng = x > 180 ? x - 360 : x < -180 ? x + 360 : x;
		double lat = y > 90 ? y - 180 : y < -90 ? y + 180 : y;
		lng = max(-180, min(180, lng));
		lat = max(-90, min(90, lat));

		return CPointF(lng, lat);
	}
	//the follow function is used for translate angle to radian
	static inline double DegreetoRadian(double angle)
	{
		return angle*constants::degreeToradian;
	}
	static CRectF Rotate(const CRectF& rect, const double& delta, const float ε)
	{
		double lng1 = Lnglat::Regularize(rect.left + delta + ε);
		double lng2 = Lnglat::Regularize(rect.right + delta - ε);
		return CRectF(lng1, rect.top, lng2, rect.bottom);
	}
}