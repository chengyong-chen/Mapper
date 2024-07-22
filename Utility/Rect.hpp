#pragma once

namespace Util
{
	namespace Rect
	{
		inline bool Intersect(const CRect& rect1, const CRect& rect2)
		{
			if(rect2.left > rect1.right)
				return false;
			else if(rect2.right < rect1.left)
				return false;
			else if(rect2.top > rect1.bottom)
				return false;
			else if(rect2.bottom < rect1.top)
				return false;
			else
				return true;
		}

		inline bool Inside(const CRect& rect1, const CRect& rect2)
		{
			return rect1.left >= rect2.left && rect1.right <= rect2.right && rect1.top >= rect2.top && rect1.bottom <= rect2.bottom;
		}
		inline bool	PointIn(const CRect& rect, const CPoint& point)
		{
			if(point.x<rect.left)
				return false;
			else if(point.x>rect.right)
				return false;
			else if(point.y<rect.top)
				return false;
			else if(point.y>rect.bottom)
				return false;
			else
				return true;
		}
	}
}