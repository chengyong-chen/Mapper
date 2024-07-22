#include "stdafx.h"

#include "Clipper.h"
#include "../Public/Global.h"
#include "../Utility/LngLat.hpp"
#include "../Utility/Link.hpp"
#include "../Utility/Constants.hpp"
#include "../Utility/Cartesian.hpp"
#include "../Utility/Clipper.hpp"
#include <algorithm>
using namespace std;
using namespace projection;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Clipper::Clipper(const double lng, const double lat)
	:centerλ(Lnglat::Regularize(lng)* constants::degreeToradian), centerψ(min(constants::halfpi, max(-constants::halfpi, lat* constants::degreeToradian))), gamma(0)
{
	cosCenterψ = cos(centerψ);
	sinCenterψ = sin(centerψ);
	cosCenterγ = cos(gamma*constants::degreeToradian);
	sinCenterγ = sin(gamma*constants::degreeToradian);
	m_bSpinned = lng!=0.f||lat!=0.f;
}


void Clipper::MoveTo(const double& λ2, const double& ψ2)
{
	Link::Vertex* vertex = new Link::Vertex{λ2, ψ2, nullptr, nullptr};
	polylines.push_back(vertex);
	currentPointer = vertex;

	λ1 = λ2;
	ψ1 = ψ2;
}
void Clipper::LineTo(const double& λ2, const double& ψ2)
{
	if(abs(λ2-λ1)>constants::ε2||abs(ψ2-ψ1)>constants::ε2)
	{
		Link::Vertex* vertex = new Link::Vertex{λ2, ψ2, currentPointer, nullptr};
		currentPointer->next = vertex;
		currentPointer = vertex;
	}

	λ1 = λ2;
	ψ1 = ψ2;
}
void Clipper::Anchor(double λ, double ψ)
{
	if(m_IsProcessingPolygon)
	{
		original.push_back({λ,ψ});
	}
}
void Clipper::spin(const double ΔLng, const double ΔLat, const double alpha)
{
	centerλ -= ΔLng*constants::degreeToradian;
	Lnglat::Regularizeλ(centerλ);

	centerψ = Lnglat::Regularizeψ(centerψ-ΔLat*constants::degreeToradian);
	cosCenterψ = cos(centerψ);
	sinCenterψ = sin(centerψ);

	m_bSpinned = centerλ!=0.f||centerψ!=0.f;
}
void Clipper::convertout(double& λ, double& ψ) const
{
	if(m_bSpinned==false)
		return;

	if(centerψ!=0.f||gamma!=0)
	{
		const double cosψ = cos(ψ);
		double x = cos(λ)*cosψ;
		double y = sin(λ)*cosψ;
		const double z = sin(ψ);
		const double k = z*cosCenterγ-y*sinCenterγ;
		λ = atan2(y*cosCenterγ+z*sinCenterγ, x*cosCenterψ-k*sinCenterψ);
		ψ = asin(k*cosCenterψ+x*sinCenterψ);
	}
	if(centerλ!=0.f)
	{
		λ += centerλ;
		if(λ>constants::pi)
			λ -= 2*constants::pi;
		else if(λ<-constants::pi)
			λ += 2*constants::pi;
	}
}
void Clipper::finialize()
{
	std::list<Link::Vertex*>::iterator it = polylines.begin();
	while(it!=polylines.end())
	{
		Link::Vertex* polyline = *it;
		if(Link::GetLength(polyline)<=1)
		{
			Link::Dispose(polyline);
			it = polylines.erase(it);
		}
		else
			++it;
	}

	if(polylines.size()>=2)
	{
		Link::Vertex* head = polylines.front();
		Link::Vertex* tail = polylines.back();
		const Link::Vertex* first = head;
		Link::Vertex* last = Link::tail(tail);
		if(abs(first->λ-last->λ)<constants::ε1&&abs(first->ψ-last->ψ)<constants::ε1)
		{
			polylines.pop_back();
			polylines.erase(polylines.begin());

			first->next->prev = last;
			last->next = first->next;
			delete first;//duplicated
			polylines.push_back(tail);
		}
	}
}
std::list<Link::Vertex*> Clipper::close(bool& southin)
{
	std::list<Link::Vertex*> polygons;
	switch(polylines.size())
	{
		case 0:
			return polygons;
		case 1:
			if(clean==true&&southin)
			{
				Link::Vertex* enclosure = Rounding();
				polygons.push_back(enclosure);
				polygons.push_back(polylines.front());
				polylines.clear();
				return polygons;
			}
			else if(clean==true)
			{
				polygons.push_back(polylines.front());
				polylines.clear();
				return polygons;
			}
			break;
		default:
			break;
	}
	std::list<Crossing<Link::Vertex>*> crosses;
	for(std::list<Link::Vertex*>::iterator it = polylines.begin(); it!=polylines.end(); it++)
	{
		Link::Vertex* polyline = *it;

		Link::Vertex* vertex1 = polyline;
		Link::Vertex* vertex2 = Link::tail(polyline);
		if(std::abs(vertex1->λ-vertex2->λ)<constants::ε1&&std::abs(vertex1->ψ-vertex2->ψ)<constants::ε1)
		{
			vertex2->λ += 2*constants::ε1;
		}
		auto cross1 = new Crossing<Link::Vertex>();
		cross1->vertex = vertex1;
		cross1->isStart = true;
		crosses.emplace_back(cross1);

		auto cross2 = new Crossing<Link::Vertex>();
		cross2->vertex = vertex2;
		cross2->isStart = false;
		crosses.emplace_back(cross2);

		cross1->other = cross2;
		cross2->other = cross1;
	}
	Crossing<Link::Vertex>* cross = crosses.front();
	crosses.sort([&](const Crossing<Link::Vertex>* a, const Crossing<Link::Vertex>* b)->bool {
		double advalue = dvalue(a->vertex->λ, a->vertex->ψ);
		double bdvalue = dvalue(b->vertex->λ, b->vertex->ψ);
		return advalue<bdvalue;
	});
	for(auto it = crosses.begin(); it!=crosses.end(); it++)
	{
		(*it)->isEnter = !southin;
		southin = !southin;
	}
	auto getNext([&crosses](Crossing<Link::Vertex>* cross) {
		if(cross==(crosses.back()))
			return crosses.front();
		else
		{
			auto it = std::find(crosses.begin(), crosses.end(), cross);
			return *(++it);
		}
	});
	auto getPrev([&crosses](Crossing<Link::Vertex>* cross) {
		if(cross==crosses.front())
			return crosses.back();
		else
		{
			auto it = std::find(crosses.begin(), crosses.end(), cross);
			return *(--it);
		}
	});

	while(cross!=nullptr)
	{
		Link::Vertex* starter = cross->vertex;
		Link::Vertex* pointer = nullptr;
		do
		{
			cross->visited = true;
			if(cross->isStart)
			{
				if(pointer!=nullptr)
					pointer->next = cross->vertex;
				else
					pointer = cross->vertex;
			}
			else if(pointer==nullptr)
				pointer = Link::reverse(Link::head(cross->vertex));
			else
				pointer->next = Link::reverse(Link::head(cross->vertex));
			pointer = Link::tail(pointer);

			cross = cross->other;
			cross->visited = true;

			bool clockwise = cross->isEnter;
			auto vertex1 = cross->vertex;
			cross = clockwise ? getNext(cross) : getPrev(cross);
			auto vertex2 = cross->vertex;
			auto samples = Interpolate(vertex1->λ, vertex1->ψ, vertex2->λ, vertex2->ψ, clockwise);
			if(samples!=nullptr)
			{
				samples->prev = pointer;
				pointer->next = samples;
			}
			pointer = Link::tail(pointer);
		} while(cross->visited==false);

		if(abs(pointer->λ-starter->λ)>constants::ε1||abs(pointer->ψ-starter->ψ)>constants::ε1)
		{
			Link::Vertex* vertexe = new Link::Vertex{starter->λ,starter->ψ,pointer};
			pointer->next = vertexe;
			pointer = pointer->next;
		}
		polygons.push_back(starter);

		auto it = std::find_if(crosses.begin(), crosses.end(), [](const Crossing<Link::Vertex>* a)->bool {
			return a->visited==false&&a->isStart;
		});
		if(it==crosses.end())
			break;
		else
			cross = *it;
	}
	for(auto it = crosses.begin(); it!=crosses.end(); it++)
	{
		delete (*it);
	}
	crosses.clear();

	polylines.clear();

	if(false)
	{
		for(std::list<Link::Vertex*>::iterator it = polygons.begin(); it!=polygons.end(); it++)
		{
			OutputDebugStringA("\n\r");
			Link::Vertex* pointer = (*it);
			while(pointer!=nullptr)
			{
				CStringA string;
				string.Format("[%.7f,%.7f],", pointer->λ, pointer->ψ);
				OutputDebugStringA(string);
				pointer = pointer->next;
			}
		}
	}
	return polygons;
}

void Clipper::Connect(std::list<Link::Vertex*>& polylines, Link::Vertex* node1, Link::Vertex* node2)
{
	Link::Vertex* polyline1 = nullptr;
	Link::Vertex* polyline2 = nullptr;
	for(std::list<Link::Vertex*>::iterator it = polylines.begin(); it!=polylines.end(); it++)
	{
		Link::Vertex* head = (*it);
		Link::Vertex* tail = Link::tail(head);
		if(tail==node1||tail==node2)
			polyline1 = head;
		if(head==node1||head==node2)
			polyline2 = head;
	}

	if(polyline1==nullptr||polyline2==nullptr)
	{
	}
	else if(polyline1==polyline2)
	{
		Link::Vertex* tail = Link::tail(polyline1);
		const Link::Vertex* head = polyline1;
		Link::Vertex* vertex = new Link::Vertex{head->λ, head->ψ, tail};
		tail->next = vertex;
	}
	else
	{
		Link::Vertex* tail = Link::tail(polyline1);
		Link::Vertex* head = polyline2;
		tail->next = head;
		head->prev = tail;
		polylines.remove(polyline2);
	}
}
bool Clipper::SouthPoleIn(std::vector<std::pair<double, double>>& data)//can't use d3.polygon.contains function, becuase south pole is always out of the bound of the polygon
{
	if(data.size()<3*2)
		return false;

	double rounding = 0;
	double area = 0;

	auto it = data.begin();
	double λ1 = (*it).first;
	double ψ1 = (*it).second/2+constants::quarterpi;
	double sinψ1 = sin(ψ1);
	double cosψ1 = cos(ψ1);
	it++;
	while(it!=data.end())
	{
		double λ2 = (*it).first;
		double ψ2 = (*it).second/2+constants::quarterpi;
		double sinψ2 = sin(ψ2);
		double cosψ2 = cos(ψ2);
		double Δ = λ2-λ1;
		bool crossmeridian = abs(Δ)>constants::pi;
		rounding += crossmeridian ? (Δ>0 ? (Δ-constants::twopi) : (Δ+constants::twopi)) : Δ;
		double k = sinψ1*sinψ2;
		area += atan2(k*sin(Δ), cosψ1*cosψ2+k*cos(Δ));

		λ1 = λ2;
		ψ1 = ψ2;
		it++;
	}

	if(abs(rounding-constants::twopi)<constants::ε1) //arounding south pole in a clockwise way
		return true;
	//	else if(abs(rounding+constants::twopi)<constants::ε1 && area < -constants::ε2)//arounding north pole in a anticlockwise way,d3 geo only check it is a anticlock
	//		return true;
	else
		return false;
}
bool Clipper::PointIn(std::vector<std::pair<double, double>>& data, double λ, double ψ, bool southpolein)//can't use d3.polygon.contains function, becuase south pole is always out of the bound of the polygon
{
	double normal[3] = {sin(λ), -cos(λ), 0};
	double sinψ = sin(ψ);
	if(sinψ==1)
		ψ = constants::halfpi+constants::ε1;
	else if(sinψ==-1)
		ψ = -constants::halfpi-constants::ε1;

	int winding = 0;
	auto it = data.begin();
	double λ1 = (*it).first;
	double ψ1 = (*it).second;
	it++;
	while(it!=data.end())
	{
		double λ2 = (*it).first;
		double ψ2 = (*it).second;
		double Δ = λ2-λ1;
		bool crossmeridian = abs(Δ)>constants::pi;
		if(crossmeridian^(λ<=λ1)^(λ<=λ2))
		{
			double ret1[3];
			Cartesian::cartesian(λ1, ψ1, ret1);
			double ret2[3];
			Cartesian::cartesian(λ2, ψ2, ret2);
			double arc[3];
			Cartesian::Cross(ret1, ret2, arc);
			Cartesian::NormalizeInPlace(arc);
			double intersection[3];
			Cartesian::Cross(normal, arc, intersection);
			Cartesian::NormalizeInPlace(intersection);
			double arcψ = ((crossmeridian^(Δ>=0)) ? -1 : 1)*asin(intersection[2]);
			if(ψ>arcψ||ψ==arcψ&&(arc[0]||arc[1]))
			{
				winding += 1;
			}
		}

		λ1 = λ2;
		ψ1 = ψ2;
		it++;
	}
	return southpolein^(winding%2==1);
}
double Clipper::dvalue(const double& λ, const double& ψ)
{
	return λ<0 ? ψ+constants::halfpi-constants::ε1 : constants::pi+(constants::halfpi-ψ);
}