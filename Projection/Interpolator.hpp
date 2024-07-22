#pragma once
#include <cmath>

#include <numbers> 
#include <vector> 
#include <list> 
#include <functional> 
#include "Projection1.h"
#include "../Utility/Constants.hpp"
#include "../Utility/Cartesian.hpp"
#include "../Utility/Math.hpp"
using namespace std;
const double cosMinDistance = cos(30*constants::degreeToradian); // cos(minimum angular distance)
template<class T>
class __declspec(dllexport) Interpolator
{
public:
	double x1;
	double y1;
	double λ1;
	double ψ1;
	double a1;
	double b1;
	double c1;

public:
	std::vector<T>& m_points;
	float thredshold1;
	float thredshold2;
	Interpolation interpolation;

public:
	std::function<void(const double&, const double&, double&, double&)>& m_geoto;

public:
	Interpolator(std::vector<T>& points, std::function<void(const double&, const double&, double&, double&)>& geoto, const Interpolation& interpolate, float tolerance)
		:m_points(points), m_geoto(geoto), thredshold1(tolerance* tolerance), thredshold2(tolerance* tolerance/100), interpolation(interpolate)
	{
	}
	~Interpolator()
	{
	}
public:
	void LineTo(const double& λ, const double& ψ, bool clockwise = true)
	{
		switch(interpolation)
		{
			case Interpolation::Nothing:
				{
					double x;
					double y;
					m_geoto(λ, ψ, x, y);
					m_points.push_back(T(λ, ψ));
				}
				break;
			case Interpolation::Linear:
				{
					LineTo1(λ, ψ);
				}
				break;
			case Interpolation::GreatCircle:
				{
					LineTo4(λ, ψ);
				}
				break;
			default:
				break;
		}
	}
	void MoveTo(const double& λ, const double& ψ, bool skip)
	{
		double x;
		double y;
		m_geoto(λ, ψ, x, y);

		λ1 = λ;
		ψ1 = ψ;
		x1 = x;
		y1 = y;
		a1 = cos(ψ)*cos(λ);
		b1 = cos(ψ)*sin(λ);
		c1 = sin(ψ);

		if(skip == false)
		{
			m_points.push_back(T(λ, ψ));
		}
	}
private:
	void LineTo1(const double& λ, const double& ψ, bool clockwise = true)
	{
		double x;
		double y;
		m_geoto(λ, ψ, x, y);
		if(isinf(x)||isinf(y))
			return;

		const double& λ2 = λ;
		const double& ψ2 = ψ;

		const double& x2 = x;
		const double& y2 = y;

		Interpolator::Linear(x1, y1, λ1, ψ1, x2, y2, λ2, ψ2, thredshold1, 12, m_points, m_geoto);

		λ1 = λ2;
		ψ1 = ψ2;
		x1 = x2;
		y1 = y2;

		m_points.push_back(T(λ, ψ));
	}
	
	void LineTo4(const double& λ, const double& ψ, bool clockwise = true)
	{
		double x;
		double y;
		m_geoto(λ, ψ, x, y);
		if(std::isinf(x) || std::isinf(y))
			return;

		const double& λ2 = λ;
		const double& ψ2 = ψ;
		const double a2 = cos(ψ2)*cos(λ2);
		const double b2 = cos(ψ2)*sin(λ2);
		const double c2 = sin(ψ2);

		const double x2 = x;
		const double y2 = y;

		Interpolator::GreatCircle(x1, y1, λ1, a1, b1, c1, x2, y2, λ2, a2, b2, c2, thredshold1, thredshold2, 12, m_points, m_geoto);

		λ1 = λ2;
		x1 = x2;
		y1 = y2;
		a1 = a2;
		b1 = b2;
		c1 = c2;

		m_points.push_back(T(λ, ψ));
	}
	static inline void Linear(const double& x1, const double& y1, const double& λ1, const double& ψ1, const double& x2, const double& y2, const double& λ2, const double& ψ2, const float& thredshold, unsigned short depth, std::vector<T>& points, std::function<void(const double&, const double&, double&, double&)>& geoto)
	{
		if(depth == 0)
			return;
		const double dx = x2 - x1;
		const double dy = y2 - y1;
		const double d2 = dx*dx + dy*dy;
		if(d2 < thredshold)
			return;

		const double λm = (λ1 + λ2)/2;
		const double ψm = (ψ1 + ψ2)/2;
		double xm;
		double ym;
		geoto(λm, ψm, xm, ym);
		if(isinf(xm)||isinf(ym))
			return;

		const double dxm = xm - x1;
		const double dym = ym - y1;
		const double dz = dy*dxm - dx*dym;		
		Interpolator::Linear(x1, y1, λ1, ψ1, xm, ym, λm, ψm, thredshold, depth-1, points, geoto);
		points.push_back(T(λm, ψm));
		Interpolator::Linear(xm, ym, λm, ψm, x2, y2, λ2, ψ2, thredshold, depth-1, points, geoto);
	}
	static inline void GreatCircle(const double& x1, const double& y1, const double& λ1, const double& a1, const double& b1, const double& c1, const double& x2, const double& y2, const double& λ2, const double& a2, const double& b2, const double& c2, const float& thredshold1, const float& thredshold2, unsigned short depth, std::vector<T>& points, std::function<void(const double&, const double&, double&, double&)>& geoto)
	{
		if(depth == 0)
			return;

		const double dx = x2 - x1;
		const double dy = y2 - y1;
		const double d2 = dx*dx + dy*dy;
		if(d2 <= thredshold1)
			return;

		const double a = a1 + a2;
		const double b = b1 + b2;
		const double c = c1 + c2;

		const double m = sqrt(a*a + b*b + c*c);
		const double cm = c/m;
		const double ψm = asin(cm);
		const double λm = (std::abs(std::abs(cm) - 1) < constants::ε1 || std::abs(λ1 - λ2) < constants::ε1) ? (λ1 + λ2)/2 : atan2(b, a);
		double xm;
		double ym;
		geoto(λm, ψm, xm, ym);
		if(isinf(xm)||isinf(ym))
			return;

		const double dxm = xm - x1;
		const double dym = ym - y1;
		const double dz = dy*dxm - dx*dym;
		if((dz*dz/d2) > thredshold2 || std::abs((dx*dxm + dy*dym)/d2 - 0.5) > 0.3 || (a1*a2 + b1*b2 + c1*c2) < cosMinDistance) // perpendicular projected distance// midpoint close to an end// angular distance
		{
			const double am = a/m;
			const double bm = b/m;
			Interpolator::GreatCircle(x1, y1, λ1, a1, b1, c1, xm, ym, λm, am, bm, cm, thredshold1, thredshold2, depth-1, points, geoto);
			points.push_back(T(λm, ψm));
			Interpolator::GreatCircle(xm, ym, λm, am, bm, cm, x2, y2, λ2, a2, b2, c2, thredshold1, thredshold2, depth-1, points, geoto);
		}
	}
};
