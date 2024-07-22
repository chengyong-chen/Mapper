#pragma once

#include <cmath>
template<typename T>
class CStraightLine
{
private://ax+by+c=0;
	double a;
	double b;
	double c;

public:
	CStraightLine(T x1, T y1, T x2, T y2);
	CStraightLine(double a1, double b1, double c1);

public:
	double GetAngle() const;
	bool IsPointOn(T x, T y, T tolerance);
	double GetSlope() const;

	CStraightLine* GetPerpendicular(T x, T y);
	CStraightLine* GetParallel(T x, T y);
	bool IntersectWith(CStraightLine& line, T& x, T& y);
	T DistanceFromPoint(T x, T y);
};

template<typename T>
CStraightLine<T>::CStraightLine(T x1, T y1, T x2, T y2)
{
	a = y1-y2;
	b = x2-x1;
	c = x1*y2-x2*y1;

	//a = 0 horizontal
	//b = 0 vertical
};

template<typename T>
CStraightLine<T>::CStraightLine(double a1, double b1, double c1)
{
	a = a1;
	b = b1;
	c = c1;
};

template<typename T>
double CStraightLine<T>::GetAngle() const
{
	if(a==0&&b==0)
		return 0;
	else if(a==0)
		return 0;
	else if(b==0)
		return 90;
	else
		return atan2(-a, b)*180/M_PI;
};

template<typename T>
bool CStraightLine<T>::IsPointOn(T x, T y, T tolerance)
{
	T distance = this->DistanceFromPoint(x, y);
	return distance<=tolerance;
};

template<typename T>
double CStraightLine<T>::GetSlope() const
{
	if(b==0)
		return -1;//INFINITY;
	else
		return -a/b;
};

template<typename T>
T CStraightLine<T>::DistanceFromPoint(T x, T y)
{
	if(a==0&&b==0)
		return sqrt(x*x+y*y);//return the distance between this point and original
	else
		return abs(a*x+b*y+c)/sqrt(a*a+b*b);
};

template<typename T>
CStraightLine<T>* CStraightLine<T>::GetParallel(T x, T y)//a(x - x1) + b(y - y1) = 0
{
	return new CStraightLine(a, b, -(a*x+b*y));
};

template<typename T>
CStraightLine<T>* CStraightLine<T>::GetPerpendicular(T x, T y)//b(x - x1) + a(y - y1) = 0
{
	return new CStraightLine(b, -a, a*y-b*x);
};

template<typename T>
bool CStraightLine<T>::IntersectWith(CStraightLine& line, T& x, T& y)
{
	double denom = this->a*line.b-line.a*this->b;
	if(denom==0)
		return false;
	else
	{
		x = (this->b*line.c-line.b*this->c)/denom;
		y = (line.a*this->c-this->a*line.c)/denom;

		return true;
	}
};
