#include "stdafx.h"
#include <cmath>
#include <numbers> 
#include "Circler.h"

#include "../Public/Global.h"
#include "../Utility/Link.hpp"
#include "../Utility/Lnglat.hpp"
#include "../Utility/Constants.hpp"
#include "../Utility/Cartesian.hpp"
using namespace projection;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Circler::Circler(const double lng, const double lat, double  r)
	:Clipper(lng, lat)
{
	reradius(r);
}

BYTE Circler::Code(const double& λ, const double& ψ)
{
	double r = smallCircle ? radius : (constants::pi - radius);
	BYTE	code = 0;
	if(λ < -r)
		code |= 0x01; // left
	else if(λ > r)
		code |= 0x02; // right

	if(ψ < -r)
		code |= 0x04; // below
	else if(ψ > r)
		code |= 0x08; // above
	return code;
}

void Circler::Anchor(double λ2, double ψ2)
{
	Clipper::Anchor(λ2, ψ2);
	convertin(λ2, ψ2);
	if(abs(λ2 - λ1) < constants::ε2 && abs(ψ2 - ψ1) < constants::ε2)
		return;

	bool visible = Circler::IsVisible(λ2, ψ2);
	BYTE code2 = smallCircle ? (visible ? 0 : Code(λ2, ψ2)) : (visible ? Code(λ2 + (λ2 < 0 ? constants::pi : -constants::pi), ψ2) : 0);
	if(visible1.has_value() == false && visible == false)
	{
	}
	else if(visible1.has_value() == false && visible == true)
	{
		MoveTo(λ2, ψ2);
	}
	else if(visible1.has_value() == false && visible == false)
	{
	}
	else if(visible1 == true && visible == false)//inside going out
	{
		double iλ1, iψ1, iλ2, iψ2;
		if(Intersect<double>(cosr, λ1, ψ1, λ2, ψ2, false, iλ1, iψ1, iλ2, iψ2))
			LineTo(iλ1, iψ1);
		else
		{
			//to do
		}
		clean = false;
		currentPointer = nullptr;
	}
	else if(visible1 == false && visible == true)//outside going in
	{
		double iλ1, iψ1, iλ2, iψ2;
		if(Intersect<double>(cosr, λ2, ψ2, λ1, ψ1, false, iλ1, iψ1, iλ2, iψ2))
			Clipper::MoveTo(iλ1, iψ1);
		else
		{
			//to do
		}
		Clipper::LineTo(λ2, ψ2);
		clean = false;
	}
	else if((visible ^ (notHemisphere && smallCircle)) && (code1&code2)==0)//still outside or inside
	{
		double iλ1, iψ1, iλ2, iψ2;
		if(Intersect<double>(cosr, λ2, ψ2, λ1, ψ1, true, iλ1, iψ1, iλ2, iψ2))
		{
			if(smallCircle)
			{
				Clipper::MoveTo(iλ1, iψ1);
				Clipper::LineTo(iλ2, iψ2);
				currentPointer = nullptr;
			}
			else
			{
				Clipper::LineTo(iλ2, iψ2);
				Clipper::MoveTo(iλ1, iψ1);
			}
			clean = false;
		}
		if(visible)
		{
			Clipper::LineTo(λ2, ψ2);
		}
	}
	else if(visible)
	{
		Clipper::LineTo(λ2, ψ2);
	}

	λ1 = λ2;
	ψ1 = ψ2;
	visible1 = visible;
	code1 = code2;
}
std::list<Link::Vertex*> Circler::close(optional<bool>& southpolin)
{
	if(southpolin.has_value() == false)
	{
		southpolin = Clipper::SouthPoleIn(original);
	}

	double spλ = -constants::pi;
	double spψ = radius - constants::pi;
	bool starterIn = southpolin.value();
	if(IsSpinned())
	{
		convertout(spλ, spψ);
		starterIn = Clipper::PointIn(original, spλ, spψ, southpolin.value());
	}
	else
	{
		starterIn = abs(radius-constants::halfpi)>constants::ε1 ? Clipper::PointIn(original, spλ, spψ, southpolin.value()) : starterIn;
	}

	return Clipper::close(starterIn);
}
template<typename T>
int Circler::Intersect(const double& cosr, const T& λ1, const T& ψ1, const T& λ2, const T& ψ2, bool two, T& iλ1, T& iψ1, T& iλ2, T& iψ2)
{
	double λa = λ1;
	double ψa = ψ1;
	double λb = λ2;
	double ψb = ψ2;

	double pa[3];
	Cartesian::cartesian(λa, ψa, pa);
	double pb[3];
	Cartesian::cartesian(λb, ψb, pb);

	// We have two planes, n1.p = d1 and n2.p = d2.
	// Find intersection line p(t) = c1 n1 + c2 n2 + t (n1 ⨯ n2).
	double n1[3] = {1, 0, 0}; // normal
	double n2[3];
	Cartesian::Cross(pa, pb, n2);
	double n2n2 = Cartesian::Dot(n2, n2);
	double n1n2 = n2[0];
	double determinant = n2n2 - n1n2*n1n2;

	// Two polar points.
	if(!determinant)
	{
		if(two)
			return 0;
		else {
			iλ1 = λ1;
			iψ1 = ψ1;
			return 1;
		}
	}
	double c1 = cosr*n2n2/determinant;
	double c2 = -cosr*n1n2/determinant;
	double n1xn2[3];
	double A[3];
	double B[3];
	Cartesian::Cross(n1, n2, n1xn2);
	Cartesian::Scale(n1, c1, A);
	Cartesian::Scale(n2, c2, B);
	Cartesian::AddInPlace(A, B);

	// Solve |p(t)|^2 = 1.
	double* u = n1xn2;
	double w = Cartesian::Dot(A, u);
	double uu = Cartesian::Dot(u, u);
	double t2 = w*w - uu*(Cartesian::Dot(A, A) - 1);
	if(t2 < 0)
		return 0;

	double t = sqrt(t2);
	double q[3];
	Cartesian::Scale(u, (-w - t)/uu, q);
	Cartesian::AddInPlace(q, A);
	Cartesian::spherical(q[0], q[1], q[2], q);
	if(two == false)
	{
		iλ1 = q[0];
		iψ1 = q[1];
		return 1;
	}
	// Two intersection points. 
	if(λb < λa)
	{
		double z = λa;
		λa = λb;
		λb = z;
	}
	double Δλ = λb - λa;
	bool polar = abs(Δλ - constants::pi) < constants::ε1;
	bool meridian = polar || Δλ < constants::ε1;
	if(!polar && ψb < ψa)
	{
		double z = ψa;
		ψa = ψb;
		ψb = z;
	};

	// Check that the first point is between a and b.
	if(meridian ? polar ? ψa + ψb > 0 ^ q[1] < (abs(q[0] - λa) < constants::ε1 ? ψa : ψb) : ψa <= q[1] && q[1] <= ψb : Δλ > constants::pi ^ (λa <= q[0] && q[0] <= λb))
	{
		double q1[3];
		Cartesian::Scale(u, (-w + t)/uu, q1);
		Cartesian::AddInPlace(q1, A);
		iλ1 = q[0];
		iψ1 = q[1];

		double q2[2];
		Cartesian::spherical(q1[0], q1[1], q1[2], q2);
		iλ2 = q2[0];
		iψ2 = q2[1];
		return 2;
	}
	else
		return 0;
}
Link::Vertex* Circler::Rounding() const
{
	Link::Vertex* start = nullptr;
	Link::Vertex* prev = nullptr;

	double sinr = sin(radius);
	double step = 6*constants::degreeToradian;
	double t1 = radius + constants::twopi;
	double t2 = radius;
	while(t1>t2)
	{
		double ret[2];
		Cartesian::spherical(cosr, -sinr * cos(t1), -sinr * sin(t1), ret);

		Link::Vertex* vertex = new Link::Vertex{ret[0], ret[1], prev, nullptr};
		if(start == nullptr)
			start = vertex;
		if(prev != nullptr)
			prev->next = vertex;
		prev = vertex;

		t1 -= step;
	}
	return start;
}
Link::Vertex* Circler::Interpolate(const double& λ1, const double& ψ1, const double& λ2, const double& ψ2, bool clockwise) const
{
	double sinr = sin(radius);
	double step = (clockwise ? -6 : 6)*constants::degreeToradian;

	double t1 = Cartesian::CircleRadius(cosr, λ1, ψ1);
	double t2 = Cartesian::CircleRadius(cosr, λ2, ψ2);
	if(clockwise && t1 < t2)
		t1 += constants::twopi;
	else if(!clockwise && t1 > t2)
		t1 -= constants::twopi;

	Link::Vertex* start = nullptr;
	Link::Vertex* prev = nullptr;
	while(clockwise ? t1>t2: t1<t2)
	{
		double ret[2];
		Cartesian::spherical(cosr, -sinr * cos(t1), -sinr * sin(t1), ret);
		Link::Vertex* vertex = new Link::Vertex{ret[0], ret[1], prev, nullptr};
		if(start == nullptr)
			start = vertex;
		if(prev != nullptr)
			prev->next = vertex;
		prev = vertex;

		t1 += step;
	}
	return start;
}