#include "stdafx.h"

#include "Antimeridian.h"

#include "../Public/Global.h"
#include "../Utility/LngLat.hpp"
#include "../Utility/Link.hpp"
#include "../Utility/Constants.hpp"
#include "../Utility/RectangleWalker.hpp"

#include <algorithm>
using namespace std;
using namespace projection;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Antimeridian::Antimeridian(const double lng, const double lat)
	:Clipper(lng, lat)
{
}
void Antimeridian::Anchor(double λ2, double ψ2)
{
	Clipper::Anchor(λ2, ψ2);
	convertin(λ2, ψ2);
	
	if(currentPointer == nullptr)
		MoveTo(λ2, ψ2);
	else if(abs(λ2-λ1)<constants::ε2&&abs(ψ2-ψ1)<constants::ε2)
	{
		λ1 = λ2;
		ψ1 = ψ2;
	}
	else if(abs(abs(λ2 - λ1) - constants::pi) < constants::ε2 && abs(abs(ψ2)-constants::halfpi)>constants::ε2 && abs(ψ2-ψ1)>constants::ε1)//cross pole
	{
		LineTo(λ1, (ψ2 + ψ1)/2 > 0 ? constants::halfpi : -constants::halfpi);
		LineTo(λ2, ψ2);
	}
	else if(abs(λ2 - λ1) <= constants::pi)
		LineTo(λ2, ψ2);
//	else if(abs(abs(λ1) - constants::pi) < constants::ε2 && abs(abs(λ2) - constants::pi) < constants::ε2)//still on meridian
//	{
//		LineTo(λ1, ψ2);
//	}
//	else if(abs(abs(λ2) - constants::pi) < constants::ε2)//this is on meridian
//	{
//		LineTo(λ1 > 0 ? constants::pi : -constants::pi, ψ2);
//	}
//	else if(abs(abs(λ1) - constants::pi) < constants::ε2)//previous on meridian
//	{
//		Link::Vertex* vextex = currentPointer->prev;
//		while(vextex != nullptr && abs(abs(vextex->λ) - constants::pi) < constants::ε2)
//		{
//			vextex = vextex->prev;
//		}
//		if(vextex == nullptr)//all previous points are on meridian 
//			LineTo(λ2, ψ2);
//		else if(λ1>0&&λ2>0)
//			LineTo(λ2, ψ2);
//		else if(λ1<0&&λ2<0)
//			LineTo(λ2, ψ2);
//		else
//		{
//			MoveTo(λ2>0 ? constants::pi : -constants::pi, ψ1);
//			LineTo(λ2, ψ2);
//			clean = false;
//		}
//	}
	else if(λ1 < 0 && λ2 > 0)//cross meridian
	{
		const double sinλΔ = sin(λ1 - λ2);
		const double sinλ1 = sin(λ1);
		const double sinλ2 = sin(λ2);
		const double sinψ1 = sin(ψ1);
		const double sinψ2 = sin(ψ2);
		const double cosψ1 = cos(ψ1);
		const double cosψ2 = cos(ψ2);
		const double inter = atan((sinψ1*cosψ2*sinλ2 - sinψ2*cosψ1*sinλ1)/(cosψ1*cosψ2*sinλΔ));
		LineTo(-constants::pi, inter);
		MoveTo(constants::pi, inter);
		LineTo(λ2, ψ2);

		clean = false;
	}
	else if(λ1 > 0 && λ2 < 0)//cross meridian 
	{
		const double sinλΔ = sin((λ1 - λ2));
		const double sinλ1 = sin(λ1);
		const double sinλ2 = sin(λ2);
		const double sinψ1 = sin(ψ1);
		const double sinψ2 = sin(ψ2);
		const double cosψ1 = cos(ψ1);
		const double cosψ2 = cos(ψ2);
		const double inter = atan((sinψ1*cosψ2*sinλ2 - sinψ2*cosψ1*sinλ1)/(cosψ1*cosψ2*sinλΔ));

		LineTo(constants::pi, inter);
		MoveTo(-constants::pi, inter);
		LineTo(λ2, ψ2);
		clean = false;
	}
	else
		LineTo(λ2, ψ2);
}
std::list<Link::Vertex*> Antimeridian::close(optional<bool>& southpolin)
{
	if(southpolin.has_value() == false)
	{
		southpolin = Clipper::SouthPoleIn(original);
	}
	bool starterIn = southpolin.value();
	if(IsSpinned())
	{
		double spλ = -constants::pi;
		double spψ = -constants::halfpi;
		convertout(spλ, spψ);
		starterIn = Clipper::PointIn(original, spλ, spψ, southpolin.value());
	}

	return Clipper::close(starterIn);
}
Link::Vertex* Antimeridian::Rounding() const
{	
	Link::Vertex* start = nullptr;
	Link::Vertex* prev = nullptr;
	auto callback = [&](double λ, double ψ) {
		Link::Vertex* vertex = new Link::Vertex{λ, ψ, prev, nullptr};
		if(start == nullptr)
			start = vertex;
		if(prev != nullptr)
			prev->next = vertex;
		prev = vertex;
	};
	RectangleWalker<double> walker(-constants::pi, -constants::halfpi, constants::pi, constants::halfpi, 2);
	walker.Rounding(callback);
	return start;
}

Link::Vertex* Antimeridian::Interpolate(const double& λ1, const double& ψ1, const double& λ2, const double& ψ2, bool clockwise) const
{
	std::vector<CPointF> points;
	if(abs(λ2-λ1)<constants::ε1&&abs(ψ2-ψ1)<constants::ε1)
		return nullptr;

	Link::Vertex* start = nullptr;
	Link::Vertex* prev = nullptr;
	auto callback = [&](double λ, double ψ) {
		Link::Vertex* vertex = new Link::Vertex{λ, ψ, prev, nullptr};
		if(start == nullptr)
			start = vertex;
		if(prev != nullptr)
			prev->next = vertex;
		prev = vertex;
	};
	static RectangleWalker<double>  walker(-constants::pi, -constants::halfpi, constants::pi, constants::halfpi, 2);
	float d1 = walker.CalculateD(λ1, ψ1);
	float d2 = walker.CalculateD(λ2, ψ2);
	walker.interplate(d1, d2, clockwise, callback);
	return start;
}