#include "stdafx.h"
#include "Equation.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CBRT_2 1.2599210498948731648
#define CBRT_2_SQR 1.5874010519681994748
#define T_PI  2.0*M_PI
#define F_PI  4.0*M_PI

static const double cbrt_factors[5] =
{
	1.0/CBRT_2_SQR,
	1.0/CBRT_2,
	1.0,
	CBRT_2,
	CBRT_2_SQR
};

double cbrt(double x)
{
	int xe;
	const double xm = frexp(std::abs(x), &xe);
	const double u = (+0.354895765043919860
		+((+1.50819193781584896
			+((-2.11499494167371287
				+((+2.44693122563534430
					+((-1.83469277483613086
						+(+0.784932344976639262-
							0.145263899385486377*xm)*xm)
						*xm))
					*xm))
				*xm))
			*xm));
	const double t2 = u*u*u;
	const double ym = u*(t2+2.0*xm)/(2.0*t2+xm)*cbrt_factors[2+xe%3];

	return ldexp(x>0.0 ? +ym : -ym, xe/3);
}

double* Q2Roots(double a, double b, double c, int& nRoots)
{
	nRoots = 0;
	double* pRoots = nullptr;
	if(a!=0.f)
	{
		const double Δ = b*b-4.0*a*c;
		if(Δ>=0)
		{
			nRoots = 2;
			pRoots = new double[2];
			pRoots[0] = (-b+sqrt(Δ))/(2*a);
			pRoots[1] = (-b-sqrt(Δ))/(2*a);
		}
	}
	else if(b!=0.f)
	{
		nRoots = 1;
		pRoots = new double[1];
		pRoots[0] = -c/b;
	}
	return pRoots;
}

//ax3+bx2+cx+d=0
double* Q3Roots(double a, double b, double c, double d, int& nRoots)
{
	if(a==0.f)
	{
		return Q2Roots(b, c, d, nRoots);
	}
	else
	{
		double* root = new double[3]{-1,-1,-1};
		const double A = b/a;//turn to: x3+Ax2+Bx+C=0		
		const double B = c/a;
		const double C = d/a;
		const double a_over_3 = A/3.0;
		const double Q1 = (3*B-A*A)/9.0;
		const double Q3 = Q1*Q1*Q1;
		const double R = (9*A*B-27*C-2*A*A*A)/54.0;
		const double Δ = Q3+R*R;

		if(Δ<0.0)// Three unequal real roots.
		{
			nRoots = 3;
			const double theta = acos(R/sqrt(-Q3));
			const double SQ = sqrt(-Q1);
			root[0] = 2.0*SQ*cos(theta/3.0)-a_over_3;
			root[1] = 2.0*SQ*cos((theta+T_PI)/3.0)-a_over_3;
			root[2] = 2.0*SQ*cos((theta+F_PI)/3.0)-a_over_3;
		}
		else if(Δ>0.0)// One real root.
		{
			nRoots = 1;
			const double SΔ = sqrt(Δ);
			const double S = cbrt(R+SΔ);
			const double T = cbrt(R-SΔ);
			root[0] = (S+T)-a_over_3;
		}
		else// Three real roots, at least two equal.
		{
			nRoots = 2;
			const double CBRT_R = cbrt(R);
			root[0] = 2*CBRT_R-a_over_3;
			root[1] = CBRT_R-a_over_3;
		}
		return root;
	}
}

double* Q4Roots(double a, double b, double c, double d, double e, int& nRoots)
{
	if(a==0.f)
		return Q3Roots(b, c, d, e, nRoots);
	else
	{
	}
}

double* Q5Roots(double a, double b, double c, double d, double e, double f, int& nRoots)
{
	if(a==0.f)
		return Q4Roots(b, c, d, e, f, nRoots);
	else
	{
	}
}
