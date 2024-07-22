#pragma once

namespace math
{
	static inline  bool between(const float pos1, const float pos2, const float& pos)
	{
		return (pos>=pos1 && pos<=pos2) ||(pos<=pos1 && pos>=pos2);
	}
	static inline void findIMatrix2(const CPoint& point1, const CPoint& point2, const CPoint& point3, const CPoint& point4, double& m11, double& m12, double& m21, double& m22)
	{
		double sin = 1.0;
		double cos = 1.0;
		const double cx = point4.x - point1.x;
		const double cy = point4.y - point1.y;
		if(cx != 0 || cy != 0)
		{
			sin = cy/sqrt(cx*cx + cy*cy);
			cos = cx/sqrt(cx*cx + cy*cy);
		}
		else
		{
			sin = 0.0f;
			cos = 1.0f;
		}
		double tg;
		const double cx2 = point2.x - point1.x;
		const double cy2 = point2.y - point1.y;
		const double x = cx*cos + cy*sin;
		const double y = -cx*sin + cy*cos;
		if(y != 0.0f)
			tg = x/y;
		else
			tg = 0.0f;

		m11 = cos;
		m12 = -sin;
		m21 = -tg*cos+sin;
		m22 = tg* sin+cos;
	}
}