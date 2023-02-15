/////////////////////////////////////////////////////////////////////////////
// DosMath.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosMath.h"

int _matherr(struct _exception* e)
{
  if (e)
  {
    e->retval = 0.0;
    switch (e->type)
    {
    case _DOMAIN:
      DOS_PrintError(L"argument domain error");
      break;
    case _SING:
      DOS_PrintError(L"argument singularity");
      break;
    case _OVERFLOW:
      DOS_PrintError(L"overflow range error");
      break;
    case _PLOSS:
      DOS_PrintError(L"partial loss of significance");
      break;
    case _TLOSS:
      DOS_PrintError(L"total loss of significance");
      break;
    case _UNDERFLOW:
      DOS_PrintError(L"the result is too small to be represented");
      break;
    }
  }
  return RSERR;
}

double _asinh(double x)
{
  return log(x + sqrt(x * x + 1.0));
}

double _acosh(double x)
{
  if (!(x >= 1.0))
    return DOS_UNSET_VALUE;

  return log(x + sqrt(x * x - 1.0));
}

double _atanh(double x)
{
  if (!(x > -1.0 && x < 1.0))
    return DOS_UNSET_VALUE;

  return (0.5 * log((1.0 + x) / (1.0 - x)));
}

double _add(const double* data, int count)
{
  ASSERT(data);
  ASSERT(count > 0);
  double sum = data[0];
  for (int i = 1; i < count; i++)
    sum += data[i];
  return sum;
}

double _subtract(const double* data, int count)
{
  ASSERT(data);
  ASSERT(count > 0);
  double diff = 0.0;
  if (1 == count)
  {
    diff = 0.0 - data[0];
  }
  else
  {
    diff = data[0];
    for (int i = 1; i < count; i++)
      diff -= data[i];
  }
  return diff;
}

double _multiply(const double* data, int count)
{
  ASSERT(data);
  ASSERT(count > 0);
  double prod = data[0];
  for (int i = 1; i < count; i++)
    prod *= data[i];
  return prod;
}

bool _divide(const double* data, int count, double& quot)
{
  ASSERT(data);
  ASSERT(count > 0);
  quot = data[0];
  for (int i = 1; i < count; i++)
  {
    if (0.0 == data[i])
    {
      DOS_PrintError(L"divide by zero");
      return false;
    }
    quot /= data[i];
  }
  return true;
}

double _mean(const double* data, int count)
{
  ASSERT(data);
  double s = _add(data, count);
  return (s / count);
}

bool _moment(double data[], int n, double& ave, double& adev, double& sdev, double& var, double& skew, double& curt)
{
  if (n < 2)
    return false;

  int j = 0;
  double ep = 0.0, s = 0.0, p = 0.0;

  for (j = 0; j < n; j++)
    s += data[j];
  ave = s / n;

  adev = var = skew = curt = 0.0;
  for (j = 0; j < n; j++)
  {
    adev += fabs(s = data[j] - (ave));
    ep += s;
    var += (p = s * s);
    skew += (p *= s);
    curt += (p *= s);
  }

  adev /= n;
  var = (var - ep * ep / n) / (n - 1);
  sdev = sqrt(var);

  if (var)
  {
    skew /= (n * var * sdev);
    curt = curt / (n * var * var) - 3.0;
  }
  else
  {
    // no skewness or kurtosis when variance = 0
    skew = DOS_UNSET_VALUE;
    curt = DOS_UNSET_VALUE;
  }

  return true;
}

double _trunc(double value, int decimal_places)
{
  double integer = 0, fractional = 0, output = 0;
  int i = 0, j = 0, places = 1;

  fractional = modf(value, &output);
  for (i = 0; i < decimal_places + 1; i++)
  {
    fractional = modf(fractional, &integer);
    for (j = 0; j < i; j++)
      places *= 10;
    output += integer / places;
    places = 1;
    fractional *= 10;
  }

  return output;
}

double _normalize(double t0, double t1, double t)
{
  double x = t0;
  if (t0 != t1)
    x = (t == t1) ? 1.0 : (t - t0) / (t1 - t0);
  return x;
}

double _parameterize(double t0, double t1, double x)
{
  double t = ((1.0 - x) * t0 + x * t1);
  return t;
}

double LinearInterpolate(double y0, double y1, double t)
{
  return (y0 * (1.0 - t) + y1 * t);
}

double CosineInterpolate(double y0, double y1, double t)
{
  double t0 = (1.0 - cos(t * DOS_PI)) / 2.0;
  return LinearInterpolate(y0, y1, t0);
}

double ExponentialInterpolate(double y0, double y1, double t)
{
  if (y0 <= 0.0 || y1 <= 0.0)
    return DOS_UNSET_VALUE;
  return (y0 * pow(y1 / y0, t));
}

double LogarithmicInterpolate(double y0, double y1, double t)
{
  if (y0 == 0.0 || y1 == 0.0 || t == 0.0)
    return DOS_UNSET_VALUE;
  return (log(t) - log(y0) / log(y1) - log(y0));
}

double SmoothStepInterpolate(double y0, double y1, double t)
{
  double t0 = (pow(t, 2) * (3 - 2 * t));
  return LinearInterpolate(y0, y1, t0);
}

double AccelerationInterpolate(double y0, double y1, double t)
{
  double t0 = pow(t, 2);
  return LinearInterpolate(y0, y1, t0);
}

double DecelerationInterpolate(double y0, double y1, double t)
{
  double t0 = (1 - pow(1 - t, 2));
  return LinearInterpolate(y0, y1, t0);
}

COLORREF ColorInterpolate(double vmin, double vmax, double v)
{
  double r = 1.0, g = 1.0, b = 1.0; // white

  if (v < vmin) v = vmin;
  if (v > vmax) v = vmax;
  double dv = vmax - vmin;

  if (v < (vmin + 0.25 * dv))
  {
    r = 0;
    g = 4 * (v - vmin) / dv;
  }
  else if (v < (vmin + 0.5 * dv))
  {
    r = 0;
    b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
  }
  else if (v < (vmin + 0.75 * dv))
  {
    r = 4 * (v - vmin - 0.5 * dv) / dv;
    b = 0;
  }
  else
  {
    g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
    b = 0;
  }

  r = _parameterize(0, 255, r);
  g = _parameterize(0, 255, g);
  b = _parameterize(0, 255, b);

  return RGB((int)r, (int)r, (int)b);
}

int _gcd(int a, int b)
{
  while (b > 0)
  {
    a = a % b;
    a ^= b ^= a ^= b; // swap
  }
  return a;
}

int _lcm(int a, int b)
{
  return (a * b) / _gcd(a, b);
}

double _fact(int n)
{
  if (n <= 0)
    return 0;

  double f = 1.0;
  for (int i = 1; i <= n; i++)
    f *= i;

  return f;
}

double Length2d(double x, double y)
{
  double len = 0.0;
  x = fabs(x);
  y = fabs(y);
  if (y > x) {
    len = x; x = y; y = len;
  }

  if (x > DBL_MIN)
  {
    len = 1.0 / x;
    y *= len;
    len = x*sqrt(1.0 + y*y);
  }
  else if (x > 0.0 && DOS_IS_FINITE(x))
    len = x;
  else
    len = 0.0;

  return len;
}

double Length2d(const ads_point& v)
{
  return Length2d(v[0], v[1]);
}

double Length3d(double x, double y, double z)
{
  double len = 0.0;
  x = fabs(x);
  y = fabs(y);
  z = fabs(z);
  if (y >= x && y >= z) {
    len = x; x = y; y = len;
  }
  else if (z >= x && z >= y) {
    len = x; x = z; z = len;
  }

  if (x > DBL_MIN)
  {
    len = 1.0 / x;
    y *= len;
    z *= len;
    len = x*sqrt(1.0 + y*y + z*z);
  }
  else if (x > 0.0 && DOS_IS_FINITE(x))
    len = x;
  else
    len = 0.0;

  return len;
}

double Length3d(const ads_point& v)
{
  return Length3d(v[0], v[1], v[2]);
}

bool Unitize2d(ads_point& pt)
{
  bool rc = false;
  double d = Length2d(pt[0], pt[1]);
  if (d > DBL_MIN)
  {
    d = 1.0 / d;
    pt[0] *= d;
    pt[1] *= d;
    rc = true;
  }
  else if (d > 0.0 && DOS_IS_FINITE(d))
  {
    ads_point tmp;
    tmp[0] = pt[0] * 8.9884656743115795386465259539451e+307;
    tmp[1] = pt[1] * 8.9884656743115795386465259539451e+307;
    d = Length2d(tmp[0], tmp[1]);
    if (d > DBL_MIN)
    {
      d = 1.0 / d;
      pt[0] = tmp[0] * d;
      pt[1] = tmp[1] * d;
      rc = true;
    }
    else
    {
      pt[0] = 0.0;
      pt[1] = 0.0;
    }
  }
  else
  {
    pt[0] = 0.0;
    pt[1] = 0.0;
  }

  return rc;
}

bool Unitize3d(ads_point& pt)
{
  bool rc = false;
  double d = Length3d(pt[0], pt[1], pt[2]);
  if (d > DBL_MIN)
  {
    d = 1.0 / d;
    pt[0] *= d;
    pt[1] *= d;
    pt[2] *= d;
    rc = true;
  }
  else if (d > 0.0 && DOS_IS_FINITE(d))
  {
    ads_point tmp;
    tmp[0] = pt[0] * 8.9884656743115795386465259539451e+307;
    tmp[1] = pt[1] * 8.9884656743115795386465259539451e+307;
    tmp[2] = pt[2] * 8.9884656743115795386465259539451e+307;
    d = Length3d(tmp[0], tmp[1], tmp[2]);
    if (d > DBL_MIN)
    {
      d = 1.0 / d;
      pt[0] = tmp[0] * d;
      pt[1] = tmp[1] * d;
      pt[2] = tmp[2] * d;
      rc = true;
    }
    else
    {
      pt[0] = 0.0;
      pt[1] = 0.0;
      pt[2] = 0.0;
    }
  }
  else
  {
    pt[0] = 0.0;
    pt[1] = 0.0;
    pt[2] = 0.0;
  }

  return rc;
}

bool Perpendicular2d(const ads_point& v, ads_point& p)
{
  p[1] = v[0];
  p[0] = -v[1];
  return (p[0] != 0.0 || p[1] != 0.0) ? true : false;
}

bool Perpendicular3d(const ads_point& v, ads_point& p)
{
  int i, j, k;
  double a, b;
  k = 2;
  if (fabs(v[1]) > fabs(v[0])) {
    if (fabs(v[2]) > fabs(v[1])) {
      // |v[2]| > |v[1]| > |v[0]|
      i = 2;
      j = 1;
      k = 0;
      a = v[2];
      b = -v[1];
    }
    else if (fabs(v[2]) >= fabs(v[0])) {
      // |v[1]| >= |v[2]| >= |v[0]|
      i = 1;
      j = 2;
      k = 0;
      a = v[1];
      b = -v[2];
    }
    else {
      // |v[1]| > |v[0]| > |v[2]|
      i = 1;
      j = 0;
      k = 2;
      a = v[1];
      b = -v[0];
    }
  }
  else if (fabs(v[2]) > fabs(v[0])) {
    // |v[2]| > |v[0]| >= |v[1]|
    i = 2;
    j = 0;
    k = 1;
    a = v[2];
    b = -v[0];
  }
  else if (fabs(v[2]) > fabs(v[1])) {
    // |v[0]| >= |v[2]| > |v[1]|
    i = 0;
    j = 2;
    k = 1;
    a = v[0];
    b = -v[2];
  }
  else {
    // |v[0]| >= |v[1]| >= |v[2]|
    i = 0;
    j = 1;
    k = 2;
    a = v[0];
    b = -v[1];
  }
  p[i] = b;
  p[j] = a;
  p[k] = 0.0;
  return (a != 0.0) ? true : false;
}

void CrossProduct2d(const ads_point& a, const ads_point& b, ads_point& v)
{
  v[0] = 0.0;
  v[1] = 0.0;
  v[2] = a[0] * b[1] - b[0] * a[1];
}

void CrossProduct3d(const ads_point& a, const ads_point& b, ads_point& v)
{
  v[0] = a[1] * b[2] - b[1] * a[2];
  v[1] = a[2] * b[0] - b[2] * a[0];
  v[2] = a[0] * b[1] - b[0] * a[1];
}

double DotProduct2d(const ads_point& a, const ads_point& b)
{
  return (a[0] * b[0] + a[1] * b[1]);
}

double DotProduct3d(const ads_point& a, const ads_point& b)
{
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

double TripleProduct3d(const ads_point& a, const ads_point& b, const ads_point& c)
{
  return (a[0] * (b[1] * c[2] - b[2] * c[1]) + a[1] * (b[2] * c[0] - b[0] * c[2]) + a[2] * (b[0] * c[1] - b[1] * c[0]));
}

void Scale2d(ads_point& v, double d)
{
  v[0] *= d;
  v[1] *= d;
}

void Scale3d(ads_point& v, double d)
{
  v[0] *= d;
  v[1] *= d;
  v[2] *= d;
}

bool IsOrthogonalFrame2d(const ads_point& X, const ads_point& Y)
{
  // returns true if X, Y, Z is an orthogonal frame
  double lx = Length2d(X);
  double ly = Length2d(Y);
  if (lx <= DOS_SQRT_EPSILON)
    return false;
  if (ly <= DOS_SQRT_EPSILON)
    return false;
  lx = 1.0 / lx;
  ly = 1.0 / ly;
  double x = DotProduct2d(X, Y)*lx*ly;
  if (fabs(x) > DOS_SQRT_EPSILON)
    return false;
  return true;
}

bool IsOrthogonalFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z)
{
  // returns true if X, Y, Z is an orthogonal frame
  double lx = Length3d(X);
  double ly = Length3d(Y);
  double lz = Length3d(Z);
  if (lx <= DOS_SQRT_EPSILON)
    return false;
  if (ly <= DOS_SQRT_EPSILON)
    return false;
  if (lz <= DOS_SQRT_EPSILON)
    return false;
  lx = 1.0 / lx;
  ly = 1.0 / ly;
  lz = 1.0 / lz;
  double xy = (X[0] * Y[0] + X[1] * Y[1] + X[2] * Y[2])*lx*ly;
  double yz = (Y[0] * Z[0] + Y[1] * Z[1] + Y[2] * Z[2])*ly*lz;
  double zx = (Z[0] * X[0] + Z[1] * X[1] + Z[2] * X[2])*lz*lx;
  if (fabs(xy) > DOS_SQRT_EPSILON
    || fabs(yz) > DOS_SQRT_EPSILON
    || fabs(zx) > DOS_SQRT_EPSILON
    )
  {
    double t = 0.0000152587890625;
    if (fabs(xy) >= t || fabs(yz) >= t || fabs(zx) >= t)
      return false;

    // do a more careful (and time consuming check)
    // This fixes RR 22219 and 22276
    ads_point V;
    CrossProduct3d(X, Y, V);
    Scale3d(V, lx*ly);
    t = fabs((V[0] * Z[0] + V[1] * Z[1] + V[2] * Z[2])*lz);
    if (fabs(t - 1.0) > DOS_SQRT_EPSILON)
      return false;

    CrossProduct3d(Y, Z, V);
    Scale3d(V, ly*lz);
    t = fabs((V[0] * X[0] + V[1] * X[1] + V[2] * X[2])*lx);
    if (fabs(t - 1.0) > DOS_SQRT_EPSILON)
      return false;

    CrossProduct3d(Z, X, V);
    Scale3d(V, lz*lx);
    t = fabs((V[0] * Y[0] + V[1] * Y[1] + V[2] * Y[2])*ly);
    if (fabs(t - 1.0) > DOS_SQRT_EPSILON)
      return false;
  }
  return true;
}

bool IsOrthonormalFrame2d(const ads_point& X, const ads_point& Y)
{
  // returns true if X, Y, Z is an orthonormal frame
  if (!IsOrthogonalFrame2d(X, Y))
    return false;
  double x = Length2d(X);
  if (fabs(x - 1.0) > DOS_SQRT_EPSILON)
    return false;
  x = Length2d(Y);
  if (fabs(x - 1.0) > DOS_SQRT_EPSILON)
    return false;
  return true;
}

bool IsOrthonormalFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z)
{
  // returns true if X, Y, Z is an orthonormal frame
  if (!IsOrthogonalFrame3d(X, Y, Z))
    return false;
  double x = Length3d(X);
  if (fabs(x - 1.0) > DOS_SQRT_EPSILON)
    return false;
  x = Length3d(Y);
  if (fabs(x - 1.0) > DOS_SQRT_EPSILON)
    return false;
  x = Length3d(Z);
  if (fabs(x - 1.0) > DOS_SQRT_EPSILON)
    return false;
  return true;
}

bool IsRightHandFrame2d(const ads_point& X, const ads_point& Y)
{
  // returns true if X, Y is an orthonormal right hand frame
  if (!IsOrthonormalFrame2d(X, Y))
    return false;
  ads_point V;
  CrossProduct2d(X, Y, V);
  ads_point Z;
  Z[0] = 0.0;
  Z[1] = 0.0;
  Z[2] = 1.0;
  double x = DotProduct3d(V, Z); // Yes 3d...
  if (x <= DOS_SQRT_EPSILON)
    return false;
  return true;
}

bool IsRightHandFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z)
{
  // returns true if X, Y, Z is an orthonormal right hand frame
  if (!IsOrthonormalFrame3d(X, Y, Z))
    return false;
  ads_point V;
  CrossProduct3d(X, Y, V);
  double x = DotProduct3d(V, Z);
  if (x <= DOS_SQRT_EPSILON)
    return false;
  return true;
}

bool CDosRound::m_up = false;

double CDosRound::ceil0(const double& value)
{
  double result = ceil(fabs(value));
  return (value < 0.0) ? -result : result;
}

double CDosRound::floor0(const double& value)
{
  double result = floor(fabs(value));
  return (value < 0.0) ? -result : result;
}

double CDosRound::roundhalfup(const double& value)
{
  return floor(value + 0.5);
}

double CDosRound::roundhalfdown(const double& value)
{
  return ceil(value - 0.5);
}

double CDosRound::roundhalfup0(const double& value)
{
  double result = CDosRound::roundhalfup(fabs(value));
  return (value < 0.0) ? -result : result;
}

double CDosRound::roundhalfdown0(const double& value)
{
  double result = CDosRound::roundhalfdown(fabs(value));
  return (value < 0.0) ? -result : result;
}

double CDosRound::roundhalfeven(const double& value, const double& epsilon)
{
  if (value < 0.0)
    return -CDosRound::roundhalfeven(-value, epsilon);

  double ipart = 0.0;
  modf(value, &ipart);

  if ((value - (ipart + 0.5)) < epsilon)
  {
    if (fmod(ipart, 2.0) < epsilon)
      return ipart;

    return ceil0(ipart + 0.5);
  }

  return CDosRound::roundhalfup0(value);
}

double CDosRound::roundalternate(const double& value)
{
  m_up = !m_up;
  if (m_up)
    return CDosRound::roundhalfup(value);
  return CDosRound::roundhalfdown(value);
}

double CDosRound::roundalternate0(const double& value)
{
  m_up = !m_up;
  if (m_up)
    return CDosRound::roundhalfup0(value);
  return CDosRound::roundhalfdown0(value);
}

double CDosRound::roundrandom(const double& value)
{
  if (rand() >= RAND_MAX / 2)
    return CDosRound::roundhalfup0(value);
  return CDosRound::roundhalfdown0(value);
}

int _quadratic(double a, double b, double c, double *r0, double *r1)
/* Find solutions of a quadratic equation
 *
 * INPUT:
 *   a, b, c  coefficients defining the quadratic equation
 *            a*t^2 + b*t + c = 0
 *   r0, r1   address of doubles
 * OUTPUT:
 *      0: successful - two distinct real roots (*r0 < *r1)
 *      1: successful - one real root (*r0 = *r1)
 *      2: successful - two complex conjugate roots (*r0 +/- (*r1)*sqrt(-1))
 *     -1: failure - a = 0, b != 0        (*r0 = *r1 = -c/b)
 *     -2: failure - a = 0, b  = 0 c != 0 (*r0 = *r1 = 0.0)
 *     -3: failure - a = 0, b  = 0 c  = 0 (*r0 = *r1 = 0.0)
 *
 * COMMENTS:
 *   The quadratic equation is solved using the formula
 *   roots = q/a, c/q, q = 0.5*(b + sgn(b)*sqrt(b^2 - 4ac)).
 *
 *   When |b^2 - 4*a*c| <= b*b*DBL_EPSILON, the discriminant
 *   is numerical noise and is assumed to be zero.
 *
 *   If it is really important to have the best possible answer,
 *   you sould probably tune up the returned roots using
 *   Brent's algorithm.
 *
 * REFERENCE:
 *   Numerical Recipes in C, section 5.5
 */
{
  double q, x0, x1, y0, y1, y;

  if (a == 0.0) {
    if (b == 0.0)
    {
      *r0 = *r1 = 0.0; return (c == 0.0) ? -3 : -2;
    }
    *r0 = *r1 = -c / b; return -1;
  }

  if (c == 0.0) {
    if (b == 0.0)
    {
      *r0 = *r1 = 0.0; return 1;
    }
    b /= -a;
    if (b < 0.0)
    {
      *r0 = b; *r1 = 0.0;
    }
    else
    {
      *r0 = 0.0; *r1 = b;
    }
    return 0;
  }

  if (b == 0.0) {
    c /= -a;
    *r1 = sqrt(fabs(c));
    if (c < 0.0)
    {
      *r0 = 0.0; return 2;
    }
    *r0 = -(*r1);
    return 0;
  }
  q = b*b - 4.0*a*c;
  if (fabs(q) <= b*b* DBL_EPSILON)
    q = 0.0; /* q is noise - set it to zero */
  if (q <= 0.0) {
    /* multiple real root or complex conjugate roots */
    *r0 = -0.5*b / a;
    if (q == 0.0)
    {
      *r1 = *r0; return 1;
    }

    /* complex conjugate roots (probably) */
    *r1 = fabs(0.5*sqrt(fabs(q)) / a);
    x0 = *r0;
    x1 = *r1;
    y = (a*x0 + b)*x0 + c;            /* y = quadratic evaluated at -b/2a */
    if ((a > 0.0 && y <= 0.0) || (a < 0.0 && y >= 0.0))
    {
      *r1 = *r0; return 1;
    }
    y0 = y - a*x1*x1;                 /* y0 = real part of "zero" */
    y1 = (2.0*a*x0 + b)*x1;           /* y1 = imaginary part of "zero" */
    if (fabs(y) <= fabs(y0) || fabs(y) <= fabs(y1))
    {
      *r1 = *r0; return 1;
    }
    return 2;
  }

  /* distinct roots (probably) */
  q = 0.5*(fabs(b) + sqrt(q));
  if (b > 0.0) q = -q;
  x0 = q / a;
  x1 = c / q;
  if (x0 == x1)
  {
    *r0 = *r1 = x0; return 1;
  }

  if (x0 > x1)
  {
    y = x0; x0 = x1; x1 = y;
  }

  /* quick test to see if roots are numerically distinct from extrema */
  y = -0.5*b / a;
  if (x0 <= y && y <= x1) {
    y = (a*y + b)*y + c;              /* y = quadratic evaluated at -b/2a */
    y0 = (a*x0 + b)*x0 + c;
    y1 = (a*x1 + b)*x1 + c;
    if (fabs(y) <= fabs(y0) || fabs(y) <= fabs(y1)
      || (a > 0.0 && y > 0.0) || (a < 0.0 && y < 0.0))
    {
      *r0 = *r1 = -0.5*b / a; return 1;
    }
  }

  /* distinct roots */
  *r0 = x0;
  *r1 = x1;
  return 0;
}

double cbrt(double x)
{
  if (fabs(x) < DBL_EPSILON)
    return 0.0;
  if (x > 0.0)
    return pow(x, 1.0 / 3.0);
  return -pow(-x, 1.0 / 3.0);
}

bool IsPrime(int n)
{
  if (n < 2)
    return false;

  if (n == 2)
    return true;

  if (n % 2 == 0)
    return false;

  for (int i = 3; (i*i) <= n; i += 2)
  {
    if (n % i == 0)
      return false;
  }

  return true;
}