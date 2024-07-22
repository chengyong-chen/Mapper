#pragma once

#include <numbers> 
#include <cmath> 

using namespace std;


namespace Cartesian
{
	inline static void cartesian(const double& λ, const double& ψ, double* ret)
	{
		double cosψ = cos(ψ);
		ret[0] = cosψ*cos(λ);
		ret[1] = cosψ*sin(λ);
		ret[2] = sin(ψ);
	}
	inline static double Dot(const double* a, const double* b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}
	inline static void Cross(const double* a, const double* b, double* ret)
	{
		ret[0] = a[1]*b[2] - a[2]*b[1];
		ret[1] = a[2]*b[0] - a[0]*b[2];
		ret[2] = a[0]*b[1] - a[1]*b[0];
	}
	inline static void AddInPlace(double* a, double* b)
	{
		a[0] += b[0];
		a[1] += b[1];
		a[2] += b[2];
	}
	inline static void Scale(const double* vector, double k, double* ret)
	{
		ret[0] = vector[0]*k;
		ret[1] = vector[1]*k;
		ret[2] = vector[2]*k;
	}
	inline static void NormalizeInPlace(double* d)
	{
		double l = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
		d[0] /= l;
		d[1] /= l;
		d[2] /= l;
	}
	inline double CircleRadius(const double& cosr, const double& λ, const double& ψ)
	{
		double ret[3];
		Cartesian::cartesian(λ, ψ, ret);

		ret[0] -= cosr;
		NormalizeInPlace(ret);
		double radius = acos(-ret[1]);
		double result = ((-ret[2] < 0 ? -radius : radius) + constants::twopi - constants::ε1);
		while(result > constants::twopi)
			result -= constants::twopi;

		return result;
	}

	inline static void spherical(const double& cartesian1, const double& cartesian2, const double& cartesian3, double& ret1, double& ret2)
	{
		ret1 = atan2(cartesian2, cartesian1);
		ret2 = asin(cartesian3);
	}
	inline static void spherical(const double& cartesian1, const double& cartesian2, const double& cartesian3, double* ret)
	{
		ret[0] = atan2(cartesian2, cartesian1);
		ret[1] = asin(cartesian3);
	}
}