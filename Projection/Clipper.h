#pragma once

#include "../Public/Global.h"
#include "../utility/LngLat.hpp"
#include "../utility/constants.hpp"
#include "../utility/Link.hpp"

#include <vector>
#include <optional>
#include <functional>
#include <forward_list>

using namespace std;
using namespace Link;

enum Interpolation : BYTE;
namespace projection
{
	class __declspec(dllexport) Clipper abstract
	{
	public:
		Clipper(const double lng, const double lat);
		~Clipper()
		{
			original.clear();

			for(std::list<Link::Vertex*>::iterator it = polylines.begin(); it != polylines.end(); it++)
			{
				Link::Vertex* polyline = *it;
				Link::Dispose(polyline);
			}
			polylines.clear();
			currentPointer = nullptr;
		}
	private:
		double centerλ;
		double centerψ;
		double gamma;

		double cosCenterψ;
		double sinCenterψ;
		double cosCenterγ;
		double sinCenterγ;

	protected:
		double λ1 = 0;
		double ψ1 = 0;
	protected:
		void MoveTo(const double& λ2, const double& ψ2);
		void LineTo(const double& λ2, const double& ψ2);
		
	public:
		virtual void Anchor(double λ, double ψ);
		std::list<Link::Vertex*> polylines;
		Link::Vertex* currentPointer = nullptr;

	protected:
		bool clean = true;
		bool m_bSpinned = false;
		bool m_IsProcessingPolygon;
		std::vector<std::pair<double, double>> original;
	public:
		inline bool IsSpinned()
		{
			return m_bSpinned;
		}
		virtual inline bool IsVisible(double λ, double ψ) const = 0;		
		inline void convertin(double& λ, double& ψ) const
		{
			if(centerλ != 0.f)
			{
				λ -= centerλ;
				Lnglat::Regularizeλ(λ);
			}
			if(centerψ != 0.f || gamma != 0.f)
			{
				const double cosψ = cos(ψ);

				const double x = cos(λ)*cosψ;
				const double y = sin(λ)*cosψ;
				const double z = sin(ψ);
				const double k = z*cosCenterψ - x*sinCenterψ;
				λ = atan2(y*cosCenterγ - k*sinCenterγ, x*cosCenterψ + z*sinCenterψ);
				ψ = asin(k*cosCenterγ + y*sinCenterγ);
				Lnglat::Regularizeλ(λ);
			}
		}
		void convertout(double& λ, double& ψ) const;
	public:
		CPointF GetCenter() const
		{
			return CPointF(centerλ*constants::radianTodegree, centerψ*constants::radianTodegree);
		}
		void recenter(const double& lng, const double& lat)
		{
			m_bSpinned = lng!=0.f || lat!=0.f;


			centerλ = lng*constants::degreeToradian;


			centerψ = lat*constants::degreeToradian;
			cosCenterψ = cos(centerψ);
			sinCenterψ = sin(centerψ);
		}
		void spin(const double deltaLng, const double deltaLat, const double alpha);
		void finialize();
		virtual void start(bool isPolygon)
		{
			for(std::list<Link::Vertex*>::iterator it = polylines.begin(); it != polylines.end(); it++)
			{
				Link::Vertex* polyline = *it;
				Link::Dispose(polyline);
			}
			polylines.clear();
			currentPointer = nullptr;

			original.clear();
			λ1 = 0;
			ψ1 = 0;
			m_IsProcessingPolygon = isPolygon;
			clean = true;
		}
		virtual std::list<Link::Vertex*> close(optional<bool>& southin)=0;
		std::list<Link::Vertex*> close(bool& southin);
		virtual Link::Vertex* Rounding() const = 0;

	protected:				
		virtual Link::Vertex* Interpolate(const double& λ1, const double& ψ1, const double& λ2, const double& ψ2, bool clockwise) const = 0;

	protected:
		static double dvalue(const double& λ, const double& ψ);
		static bool SouthPoleIn(std::vector<std::pair<double, double>>& data);
		static bool PointIn(std::vector<std::pair<double, double>>& data, double λ, double ψ, bool southpolein);
		static void Connect(std::list<Link::Vertex*>& polylines, Link::Vertex* node1, Link::Vertex* node2);
	};
}