#pragma once

void MatrixToHB(double** a, long n);

long QRroot(double** a, long n, double xr[], double xi[], double eps, long jt);
void QuanticToHB(double* c, double** a, long n);
long Dnetn(long n, double eps, double t, double h, float x[], long k, long q[], long p[]);
void Dnetnf(float t[], double f[], long n, long a[], long b[]);
long Agaus(double** a, double b[], long n);

void Dnmtc(float& t1, float& t2, float ab, long m, float eps, long a[], long b[]);
float Dnmtcf(float t1, float t2, long** a, long** b);
float mrnd1(float* r);

long Dnnse(double& t1, double& t2, long m, long eps, long** a, long** b);
long Dnnsef(float t1, float t2, double& df1, double& df2, long** a, long** b);
