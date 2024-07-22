#pragma once			

template <typename T>
class RectangleWalker
{
public:
	RectangleWalker(T left, T top, T right, T bottom, unsigned int prec = 1)
	{								//	^
		minX = left;				//	|			0		bottom			1*prec	
		maxX = right;				//	|			   |----------------|
		minY = top;					//	|			   |				|
		maxY = bottom;				//	|	   left    |				| right
		//	|			   |				|
		cx = right-left;			//	|			   |				|
		cy = bottom-top;			//	|			   |----------------|	
		//	|		     3*prec	   top		    2*prec
		step = 1.f/prec;			    //--+------------------------------------------>
		peri = 4*prec;			    //	|		
	}
private:
	T minX;
	T maxX;
	T minY;
	T maxY;
	T cx;
	T cy;
	float step;
	unsigned int peri;
private:
	float NextNod(const float d, bool clockwise)
	{
		if(std::fmod(d, 1)==0)
			return std::fmod(d+(clockwise ? +1 : -1)+peri, peri);
		else if(clockwise)
			return std::fmod(std::floor(d)+1+peri, peri);
		else
			return std::fmod(std::floor(d)+peri, peri);
	}
	float PrevNod(const float d, bool clockwise)
	{
		if(std::fmod(d, 1)==0)
			return std::fmod(d+(clockwise ? -1 : +1)+peri, peri);
		else if(clockwise)
			return std::fmod(std::floor(d)+peri, peri);
		else
			return std::fmod(std::ceil(d)+peri, peri);
	}
public:
	inline float CalculateD(const T x, const T y) const
	{
		if(abs(y-maxY)<constants::ε1)
			return 0/step+(x-minX)/(cx*step);
		else if(abs(x-maxX)<constants::ε1)
			return 1/step+(maxY-y)/(cy*step);
		else if(abs(y-minY)<constants::ε1)
			return 2/step+(maxX-x)/(cx*step);
		else if(abs(x-minX)<constants::ε1)
			return 3/step+(y-minY)/(cy*step);
		else
			return 0;
	}
	inline bool GetPosition(float d, T& x, T& y) const
	{
		int corner = (int)floor(d*step)%4;
		float leftover = d*step-(int)floor(d*step);
		switch(corner)
		{
			case 0:
				x = minX+leftover*cx;
				y = maxY;
				return true;
			case 1:
				x = maxX;
				y = maxY-leftover*cy;
				return true;
			case 2:
				x = maxX-leftover*cx;
				y = minY;
				return true;
			case 3:
				x = minX;
				y = minY+leftover*cy;
				return true;
			default:
				return false;
		}
	}
	inline void interplate(float d1, float d2, bool clockwise, std::function<void(T, T)> callback)
	{
		if(abs(d2-d1)<constants::ε2)
			return;
		if(std::floor(d1)!=std::floor(d2))
		{
		}
		else if(d2>d1&&clockwise==true)
			return;
		else if(d1>d2&&clockwise==false)
			return;
		if(std::ceil(d1)!=std::ceil(d2))
		{
		}
		else if(d1==0&&clockwise==false)
			return;
		else if(d2==0&&clockwise==true)
			return;
		else if(d2>d1&&clockwise==true)
			return;
		else if(d1>d2&&clockwise==false)
			return;

		float start = NextNod(d1, clockwise);
		float stop = NextNod(d2, clockwise);
		do
		{
			T x;
			T y;
			GetPosition(start, x, y);
			callback(x, y);

			start = NextNod(start, clockwise);
		} while(start!=stop);
	}
	inline void Rounding(std::function<void(T, T)> callback) const
	{
		float d = 0;
		while(d<=peri)
		{
			double x;
			double y;
			if(GetPosition(d, x, y))
			{
				callback(x, y);
			}
			d += step;
		}
	}
	template<typename C>
	inline static C* getNext(std::list<C*>& crosses, C* cross, bool clockwise)
	{
		if(clockwise)
		{
			if(cross==(crosses.back()))
				return crosses.front();
			else
			{
				auto it = std::find(crosses.begin(), crosses.end(), cross);
				return *(++it);
			}
		}
		else
		{
			if(cross==(crosses.front()))
				return crosses.back();
			else
			{
				auto it = std::find(crosses.begin(), crosses.end(), cross);
				return *(--it);
			}
		}
	}
};

