#include "stdafx.h"
#include "Fuction.h"
#include "Global.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void MatrixToHB(double** a, long n)
{
	long i, j, k;
	double d, t;
	for(k = 1; k<=n-2; k++)
	{
		d = 0.0;
		for(j = k; j<=n-1; j++)
		{
			if(std::abs(a[j][k-1])>std::abs(d))
			{
				d = a[j][k-1];
				i = j;
			}
		}

		if(d==0)
			continue;

		if(i!=k)
		{
			for(j = k-1; j<=n-1; j++)
			{
				t = a[i][j];
				a[i][j] = a[k][j];
				a[k][j] = t;
			}
			for(j = 0; j<=n-1; j++)
			{
				t = a[j][i];
				a[j][i] = a[j][k];
				a[j][k] = t;
			}
		}
		for(i = k+1; i<=n-1; i++)
		{
			t = a[i][k-1]/d;
			a[i][k-1] = 0.0;

			for(j = k; j<=n-1; j++)
				a[i][j] = a[i][j]-t*a[k][j];

			for(j = 0; j<=n-1; j++)
				a[j][k] = a[j][k]+t*a[j][i];
		}
	}
	return;
}

void QuanticToHB(const double* c, double** a, long n)
{
	a = new double* [n];

	for(long i = 0; i<n; i++)
	{
		a[i] = new double[n];
		ZeroMemory(a[i], sizeof(double)*n);
	}

	for(long j = 0; j<n; j++)
		a[0][j] = -c[n-j-1]/c[n];
	for(long i = 1; i<n; i++)
		a[i][i-1] = 1.0;
	return;
}

long QRroot(double** a, long n, double xr[], double xi[], double eps, long jt)
{
	long Row = n;
	long Col = n;

	long i, j, k, ii, jj;
	double b, c, w, g, xy, p, q, r, x, s, e, f, z, y;
	long it = 0;

	while(Row!=0)
	{
		long l = Row-1;
		while((l>0)&&(std::abs(a[l][l-1])>eps* (std::abs(a[l-1][l-1])+std::abs(a[l][l]))))
			l = l-1;

		if(l==Row-1)
		{
			xr[Row-1] = a[Row-1][Row-1];
			xi[Row-1] = 0.0f;
			Row--;
			it = 0;
		}
		else if(l==Row-2)
		{
			b = -(a[Row-1][Row-1]+a[Row-2][Row-2]);
			c = a[Row-1][Row-1]*a[Row-2][Row-2]-a[Row-1][Row-2]*a[Row-2][Row-1];
			w = b*b-4*c;
			y = sqrt(std::abs(w));
			if(w>0.0)
			{
				xy = 1.0;
				if(b<0.0)
					xy = -1.0;
				xr[Row-1] = (-b-xy*y)/2.0;
				xr[Row-2] = c/xr[Row-1];
				xi[Row-1] = 0.f;
				xi[Row-2] = 0.f;
			}
			else
			{
				xr[Row-1] = -b/2.0;
				xr[Row-2] = xr[Row-1];
				xi[Row-1] = y/2.0;
				xi[Row-2] = -xi[Row-1];
			}
			Row--;
			Row--;
			it = 0;
		}
		else
		{
			if(it>=jt)
				return false;

			it = it+1;
			for(j = l+2; j<=Row-1; j++)
				a[j][j-2] = 0;
			for(j = l+3; j<=Row-1; j++)
				a[j][j-3] = 0;
			for(k = l; k<=Row-2; k++)
			{
				if(k!=l)
				{
					p = a[k][k-1];
					q = a[k+1][k-1];
					r = 0.0;
					if(k!=Row-2)
						r = a[k+2][k-1];
				}
				else
				{
					x = a[Row-2][Row-2]+a[Row-1][Row-1];
					y = a[Row-2][Row-2]*a[Row-1][Row-1]-a[Row-1][Row-2]*a[Row-2][Row-1];
					p = a[l][l]*(a[l][l]-x)+a[l][l+1]*a[l+1][l]+y;
					q = a[l+1][l]*(a[l][l]+a[l+1][l+1]-x);
					r = a[l+1][l]*a[l+2][l+1];
				}
				if((std::abs(p)+std::abs(q)+std::abs(r))!=0.0)
				{
					xy = 1.0;
					if(p<0.0)
						xy = -1.0;
					s = xy*sqrt(p*p+q*q+r*r);
					if(k!=l)
						a[k][k-1] = -s;
					e = -q/s;
					f = -r/s;
					x = -p/s;
					y = -x-f*r/(p+s);
					g = e*r/(p+s);
					z = -x-e*q/(p+s);
					for(j = k; j<=Row-1; j++)
					{
						ii = k*n+j;
						jj = (k+1)*n+j;
						p = x*a[k][j]+e*a[k+1][j];
						q = e*a[k][j]+y*a[k+1][j];
						r = f*a[k][j]+g*a[k+1][j];
						if(k!=Row-2)
						{
							p = p+f*a[k+2][j];
							q = q+g*a[k+2][j];
							r = r+z*a[k+2][j];
							a[k+2][j] = r;
						}
						a[k+1][j] = q;
						a[k][j] = p;
					}
					j = k+3;
					if(j>=Row-1)
						j = Row-1;
					for(i = l; i<=j; i++)
					{
						p = x*a[i][k]+e*a[i][k+1];
						q = e*a[i][k]+y*a[i][k+1];
						r = f*a[i][k]+g*a[i][k+1];
						if(k!=Row-2)
						{
							p = p+f*a[i][k+2];
							q = q+g*a[i][k+2];
							r = r+z*a[i][k+2];
							a[i][k+2] = r;
						}
						a[i][k+1] = q;
						a[i][k] = p;
					}
				}
			}
		}
	}

	return true;
}

long Dnetn(long n, double eps, double t, double h, float x[], long k, long q[], long p[])
{
	double z, beta, d;
	double* y = new double[n];
	double** a = new double* [n];
	for(long m = 0; m<n; m++)
	{
		a[m] = new double[n];
	}

	double* b = new double[n];
	long l = k;
	double am = 1.0+eps;
	while(am>=eps)
	{
		Dnetnf(x, b, n, q, p);
		am = 0.0;
		for(int i = 0; i<=n-1; i++)
		{
			z = std::abs(b[i]);
			if(z>am)
			{
				am = z;
			}
		}
		if(am>=eps)
		{
			l = l-1;
			if(l==0)
			{
				delete[] y;
				delete[] b;
				for(int i = 0; i<n; i++)
					delete a[i];
				delete[] a;
				printf("fail\n");
				return (0);
			}
			for(int j = 0; j<=n-1; j++)
			{
				z = x[j];
				x[j] = x[j]+h;
				Dnetnf(x, y, n, q, p);
				for(int i = 0; i<=n-1; i++)
					a[i][j] = y[i];
				x[j] = z;
			}
			if(Agaus(a, b, n)==0)
			{
				delete[] y;
				delete[] b;
				for(int i = 0; i<n; i++)
					delete a[i];
				delete[] a;
				return (-1);
			}

			beta = 1.0;
			for(int i = 0; i<=n-1; i++)
				beta = beta-b[i];

			if(std::abs(beta)+1.0==1.0)
			{
				delete[] y;
				delete[] b;
				for(int i = 0; i<n; i++)
					delete a[i];
				delete[] a;
				printf("fail\n");
				return (-2);
			}

			d = h/beta;
			for(int i = 0; i<=n-1; i++)
				x[i] = x[i]-d*b[i];
			h = t*h;
		}
	}
	delete[] y;
	delete[] b;
	for(int i = 0; i<n; i++)
		delete a[i];
	delete[] a;
	return (k-l);
}

long Agaus(double** a, double b[], long n)
{
	long* js, l, k, i, j, is, p, q;
	double d, t;
	js = new long[n];
	l = 1;
	for(k = 0; k<=n-2; k++)
	{
		d = 0.0;
		for(i = k; i<=n-1; i++)
			for(j = k; j<=n-1; j++)
			{
				t = std::abs(a[i][j]);
				if(t>d)
				{
					d = t;
					js[k] = j;
					is = i;
				}
			}
		if(d+1.0==1.0)
			l = 0;
		else
		{
			if(js[k]!=k)
				for(i = 0; i<=n-1; i++)
				{
					p = i*n+k;
					q = i*n+js[k];
					t = a[i][k];
					a[i][k] = a[i][js[k]];
					a[i][js[k]] = t;
				}
			if(is!=k)
			{
				for(j = k; j<=n-1; j++)
				{
					p = k*n+j;
					q = is*n+j;
					t = a[k][j];
					a[k][j] = a[is][j];
					a[is][j] = t;
				}
				t = b[k];
				b[k] = b[is];
				b[is] = t;
			}
		}
		if(l==0)
		{
			delete js;
			printf("fail\n");
			return (0);
		}

		d = a[k][k];
		for(j = k+1; j<=n-1; j++)
		{
			p = k*n+j;
			a[k][j] = a[k][j]/d;
		}
		b[k] = b[k]/d;
		for(i = k+1; i<=n-1; i++)
		{
			for(j = k+1; j<=n-1; j++)
			{
				p = i*n+j;
				a[i][j] = a[i][j]-a[i][k]*a[k][j];
			}
			b[i] = b[i]-a[i][k]*b[k];
		}
	}
	d = a[n-1][n-1];
	if(std::abs(d)+1.0==1.0)
	{
		delete js;
		printf("fail\n");
		return (0);
	}
	b[n-1] = b[n-1]/d;
	for(i = n-2; i>=0; i--)
	{
		t = 0.0;
		for(j = i+1; j<=n-1; j++)
			t = t+a[i][j]*b[j];
		b[i] = b[i]-t;
	}
	js[n-1] = n-1;
	for(k = n-1; k>=0; k--)
		if(js[k]!=k)
		{
			t = b[k];
			b[k] = b[js[k]];
			b[js[k]] = t;
		}
	delete js;
	return (1);
}

void Dnetnf(float t[], double f[], long n, long a[], long b[])
{
	f[0] = t[0]*t[0]*t[0]*a[7]+t[0]*t[0]*a[6]+t[0]*a[5]+a[4]-t[1]*t[1]*t[1]*a[3]-t[1]*t[1]*a[2]-t[1]*a[1]-a[0];
	f[1] = t[0]*t[0]*t[0]*b[7]+t[0]*t[0]*b[6]+t[0]*b[5]+b[4]-t[1]*t[1]*t[1]*b[3]-t[1]*t[1]*b[2]-t[1]*b[1]-b[0];

	return;
}

float Dnmtcf(float t1, float t2, long a[], long b[])
{
	float f;
	float f1;
	float f2;

	f1 = t1*t1*t1*a[7]+t1*t1*a[6]+t1*a[5]+a[4]-t2*t2*t2*a[3]-t2*t2*a[2]-t2*a[1]-a[0];
	f2 = t1*t1*t1*b[7]+t1*t1*b[6]+t1*b[5]+b[4]-t2*t2*t2*b[3]-t2*t2*b[2]-t2*b[1]-b[0];

	f = (sqrt(f1*f1+f2*f2));

	return (long)f;
}

long Dnnse(double& t1, double& t2, long m, long eps, long** a, long** b)
{
	long k;
	long F;
	double D, s;
	double df1;
	double df2;

	k = m;
	F = Dnnsef(t1, t2, df1, df2, a, b);

	while(F>=eps)
	{
		k--;
		if(k==0)
			return m;

		D = 0.0;
		D = D+df1*df1;
		D = D+df2*df2;

		if(D+1.0==1.0)
			return -1;

		s = F/D;

		t1 = t1-s*df1;
		t2 = t2-s*df2;

		F = Dnnsef(t1, t2, df1, df2, a, b);
	}

	if(t1<0||t1 > 1||t2<0||t2 > 1)
		return -1;

	return m-k;
}

long Dnnsef(float t1, float t2, double& df1, double& df2, long** a, long** b)
{
	long F;

	long f1;
	long f2;

	float dft1;
	float dft2;

	f1 = t1*t1*t1*a[1][1]+t1*t1*a[2][1]+t1*a[3][1]+a[4][1]-t2*t2*t2*b[1][1]-t2*t2*b[2][1]-t2*b[3][1]-b[4][1];
	f2 = t1*t1*t1*a[1][2]+t1*t1*a[2][2]+t1*a[3][2]+a[4][2]-t2*t2*t2*b[1][2]-t2*t2*b[2][2]-t2*b[3][2]-b[4][2];

	F = f1*f1+f2*f2;

	dft1 = 3*t1*t1*a[1][1]+2*t1*a[2][1]+a[3][1];
	dft2 = 3*t1*t1*a[1][2]+2*t1*a[2][2]+a[3][2];
	df1 = 2*(f1*dft1+f2*dft2);

	dft1 = -1*(3*t2*t2*b[1][1]+2*t2*b[2][1]+b[3][1]);
	dft2 = -1*(3*t2*t2*b[1][2]+2*t2*b[2][2]+b[3][2]);
	df2 = 2*(f1*dft1+f2*dft2);

	return F;
}

//13260531
void Dnmtc(float& t1, float& t2, float ab, long m, float eps, long a[], long b[])
{
	long k = 1;
	float r = 1;
	float u1;
	float u2;
	float F = Dnmtcf(t1, t2, a, b);
	float F1;

	while(ab>=eps)
	{
		u1 = t1-ab+2*ab*mrnd1(&r);
		u2 = t2-ab+2*ab*mrnd1(&r);
		F1 = Dnmtcf(u1, u2, a, b);
		k = k+1;
		if(F1>=F)
		{
			if(k>m)
			{
				k = 1;
				ab = ab/2;
			}
		}
		else
		{
			k = 1;
			t1 = u1;
			t2 = u2;
			F = F1;

			if(F<2)
				return;
		}
	}
	return;
}

float mrnd1(float* r)
{
	long m;
	float s, u, v, p;

	s = 65536.0;
	u = 2053.0;
	v = 13849.0;

	m = (long)(*r/s);
	*r = *r-m*s;
	*r = u*(*r)+v;
	m = (long)(*r/s);
	*r = *r-m*s;
	p = *r/s;

	return p;
}

CPoint RegulizePoint(const CPoint& point1, const CPoint& point2, const unsigned short& nDivCount)
{
	const long nDivAng = 360/nDivCount;
	double fAngle = LineAngle(point1, point2);
	long nAngle = fAngle*180/M_PI;
	const long DivNum = (long)((nAngle+nDivAng/2.)/nDivAng);

	nAngle = DivNum*nDivAng;
	nAngle = nAngle%360;
	fAngle = (nAngle*M_PI/180);
	const double dis = ::LineLength1(point1, point2);
	const int x = round(point1.x+dis*cos(fAngle));
	const int y = round(point1.y+dis*sin(fAngle));
	return CPoint(x, y);
}
