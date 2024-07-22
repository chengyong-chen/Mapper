#pragma once

#include <list>
#include <optional>
#include <functional>
#include "Clipper.h"

#include "../Utility/Link.hpp"
#include "../Utility/Constants.hpp"

using namespace std;
using namespace Link;
namespace projection
{
	class __declspec(dllexport) Circler : public Clipper
	{
	public:
		Circler(const double lng, const double lat, double r);

	protected:
		double radius;

	protected:
		std::optional<bool> visible1;

	private:
		double cosr;
		bool smallCircle;
		bool notHemisphere;
		BYTE code1;
		BYTE code2;
	private:
		BYTE Code(const double& λ, const double& ψ);
	public:
		void Anchor(double λ, double ψ) override;
		double GetRadius() const
		{
			return radius;
		}	
	public:
		virtual inline bool IsVisible(double λ, double ψ) const override
		{
			return cos(λ) * cos(ψ) > cosr;
		}
		void start(bool isPolygon) override
		{
			Clipper::start(isPolygon);

			code1 = 0x00;
			code2 = 0x00;
			clean = true;
			visible1.reset();
		}

		void reradius(const double& r)
		{
			radius = r;
			cosr = cos(radius);
			smallCircle = cosr > 0;
			notHemisphere = abs(cosr) > constants::ε1;
		}
	public:
		Link::Vertex* Rounding() const override;
		std::list<Link::Vertex*> close(optional<bool>& southpolin) override;
	protected:		
		Link::Vertex* Interpolate(const double& λ1, const double& ψ1, const double& λ2, const double& ψ2, bool clockwise) const override;
	protected:
		template<typename T>
		static int Intersect(const double& cosr, const T& λ1, const T& ψ1, const T& λ2, const T& ψ2, bool two, T& x1, T& y1, T& x2, T& y2);
	};
}