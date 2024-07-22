#pragma once

#include "../Public/Global.h"
#include "../utility/LngLat.hpp"
#include "../utility/constants.hpp"
#include "Clipper.h"

#include <vector>
#include <functional>
#include <forward_list>

using namespace Link;
using namespace std;
namespace projection
{
	class __declspec(dllexport) Antimeridian : public Clipper
	{
	public:
		Antimeridian(const double lng, const double lat);

	public:
		void Anchor(double λ, double ψ) override;
		inline bool IsVisible(double λ, double ψ) const override
		{
			return true;
		}
	protected:				
		Link::Vertex* Interpolate(const double& λ1, const double& ψ1, const double& λ2, const double& ψ2, bool clockwise) const override;
	public:
		Link::Vertex* Rounding() const override;
		std::list<Link::Vertex*> close(optional<bool>& southpolin) override;
	};
}