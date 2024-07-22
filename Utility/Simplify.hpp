#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Polyline_simplification_2/Squared_distance_cost.h>
#include <CGAL/IO/WKT.h>


#include <boost/format.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<int> point;
typedef bg::model::polygon<point> polygon;
typedef bg::model::multi_polygon<polygon> mpolygon;
typedef bg::model::ring<point> Ring;

#include "correct.hpp"
#include "psimpl.h"


namespace Simplify
{
	void Simplify1(CPoint* points, unsigned int anchors, const int& tolerance, std::vector<int>& result)//created self interseted sections
	{
		std::vector<int> polyline;
		for(unsigned int index = 0; index < anchors; index++)
		{
			polyline.push_back(points[index].x);
			polyline.push_back(points[index].y);
		}	
		psimpl::simplify_douglas_peucker<2>(polyline.begin(), polyline.end(), tolerance, std::back_inserter(result));
	}
	void Simplify2(CPoint* points, unsigned int anchors, const int& tolerance, bool clockwise, std::list<std::vector<int>>& outers, std::list<std::vector<int>>& inners)//doesn't work, always return multiple polygons
	{				
		polygon polygon;
		for(unsigned int index = 0; index < anchors; index++)
		{
			polygon.outer().push_back(point(points[index].x, points[index].y));
		}
		mpolygon result;
		geometry1::correct(polygon, result, tolerance*tolerance);
		if(boost::geometry::is_valid(result))
		{
			for(auto it1 = result.begin(); it1 != result.end(); it1++)
			{
				std::vector<int> outer;
				for(auto it2 = (*it1).outer().begin(); it2 != (*it1).outer().end(); it2++)
				{
					outer.push_back((*it2).x());
					outer.push_back((*it2).y());
				}
				outers.push_back(outer);
			}
		}
	}
	void Correct(CPoint* points, unsigned int anchors, bool clockwise, const int& tolerance, std::list<std::vector<int>>& result)//doesn't work, always return multiple polygons
	{
		Ring ring;
		for(unsigned int index = 0; index < anchors; index++)
		{
			ring.push_back(point(points[index].x, points[index].y));
		}		
		std::vector<std::pair<Ring, double>> pairs = geometry1::impl::correct(ring, clockwise ? boost::geometry::order_selector::clockwise : boost::geometry::order_selector::counterclockwise, 2*tolerance*tolerance);
		for(auto it1 = pairs.begin(); it1 < pairs.end(); it1++)
		{
			std::vector<int> ring;
			for(auto it2 = it1->first.begin(); it2 != it1->first.end(); it2++)
			{
				ring.push_back(it2->x());
				ring.push_back(it2->y());
			}
			result.push_back(ring);
		}
	}
	namespace PS = CGAL::Polyline_simplification_2;
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef PS::Vertex_base_2<K> Vb;
	typedef CGAL::Constrained_triangulation_face_base_2<K> Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb> TDS;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, CGAL::Exact_predicates_tag> CDT;
	typedef CGAL::Constrained_triangulation_plus_2<CDT> CT;
	typedef CGAL::Polygon_2<K> Polygon;	
	void Simplify4(CPoint* points, unsigned int anchors, const int& tolerance, std::vector<int>& result)//	solution1, doesn't work, error in etree
	{
		Polygon polygon1;
		for(unsigned int index = 0; index < anchors; index++)
		{
			polygon1.push_back(CT::Point(points[index].x, points[index].y));
		}
		Polygon polygon2 = PS::simplify(polygon1, PS::Squared_distance_cost(), PS::Stop_above_cost_threshold((double)tolerance*tolerance));
		for(auto it = boost::begin(polygon2); it != boost::end(polygon2); ++it)
		{
			result.push_back((int)(*it).x());
			result.push_back((int)(*it).y());
		}	
	}
}