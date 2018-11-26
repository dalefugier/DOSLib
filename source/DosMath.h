/////////////////////////////////////////////////////////////////////////////
// DosMath.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

// Constants
#define DOS_E               2.71828182845904523536  // e
#define DOS_LOG2E           1.44269504088896340736  // log2(e)
#define DOS_LOG10E          0.43429448190325182765  // log10(e)
#define DOS_LN2             0.69314718055994530942  // ln(2)
#define DOS_LN10            2.30258509299404568402  // ln(10)
#define DOS_PI              3.14159265358979323846  // pi
#define DOS_TWOPI           6.28318530717958647693  // 2 * pi
#define DOS_PI_2            1.57079632679489661923  // pi/2
#define DOS_PI_4            0.78539816339744830967  // pi/4
#define DOS_1_PI            0.31830988618379067154  // 1/pi
#define DOS_2_PI            0.63661977236758134308  // 2/pi
#define DOS_2_SQRTPI        1.12837916709551257390  // 2/sqrt(pi)
#define DOS_SQRT2           1.41421356237309504880  // sqrt(2)
#define DOS_SQRT1_2         0.70710678118654752440  // 1/sqrt(2)
#define DOS_SQRT_EPSILON    1.490116119385000000e-8
#define DOS_UNSET_VALUE    -1.23432101234321e+308
#define DOS_ZERO_TOLERANCE  1.0e-12
#define DOS_IS_FINITE(x)    (0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))

// Returns V, clamped to lie between L & H, making L <= V <= H
#define DOS_CLAMP(V,L,H) ( (L) < (V) ? ( (V) < (H) ? (V) : (H) ) : (L) )

// Verifies V lies within an open interval, or L < V < H
#define DOS_OPEN_INTERVAL(V,L,H) ( (L) < (V) ? ( (V) < (H) ? (true) : (false) ) : (false) )

// Verifies V lies within a closed interval, or L <= V <= H
#define DOS_CLOSED_INTERVAL(V,L,H) ( (L) <= (V) ? ( (V) <= (H) ? (true) : (false) ) : (false) )

// Prints math errors to the command line
int _matherr(struct _exception* e);

// Trigonometry helpers
double _asinh(double x);
double _acosh(double x);
double _atanh(double x);

// Scaler helpers
double _add(const double* data, int count);
double _subtract(const double* data, int count);
double _multiply(const double* data, int count);
bool _divide(const double* data, int count, double& quot);

// Statistical helpers
double _mean(const double* data, int count);
bool _moment(double data[], int n, double& ave, double& adev, double& sdev, double& var, double& skew, double& curt);

// Interpolation
double LinearInterpolate(double y0, double y1, double t);
double CosineInterpolate(double y0, double y1, double t);
double ExponentialInterpolate(double y0, double y1, double t);
double LogarithmicInterpolate(double y0, double y1, double t);
double SmoothStepInterpolate(double y0, double y1, double t);
double AccelerationInterpolate(double y0, double y1, double t);
double DecelerationInterpolate(double y0, double y1, double t);
COLORREF ColorInterpolate(double vmin, double vmax, double v);

// Miscellaneous
int _gcd(int a, int b);
int _lcm(int a, int b);
double _fact(int n);
double _trunc(double value, int decimal_places);
double _normalize(double t0, double t1, double t);
double _parameterize(double t0, double t1, double x);
int _quadratic(double a, double b, double c, double* r0, double* r1);

// Calculates the cube root.
double cbrt(double x);

// Is prime number
bool IsPrime(int n);

// Vectors
double Length2d(double x, double y);
double Length2d(const ads_point& v);
double Length3d(double x, double y, double z);
double Length3d(const ads_point& v);
bool Unitize2d(ads_point& pt);
bool Unitize3d(ads_point& pt);
bool Perpendicular2d(const ads_point& v, ads_point& p);
bool Perpendicular3d(const ads_point& v, ads_point& p);
void CrossProduct2d(const ads_point& a, const ads_point& b, ads_point& v);
void CrossProduct3d(const ads_point& a, const ads_point& b, ads_point& v);
double DotProduct2d(const ads_point& a, const ads_point& b);
double DotProduct3d(const ads_point& a, const ads_point& b);
double TripleProduct3d(const ads_point& a, const ads_point& b, const ads_point& c);
void Scale2d(ads_point& v, double d);
void Scale3d(ads_point& v, double d);
bool IsOrthogonalFrame2d(const ads_point& X, const ads_point& Y);
bool IsOrthogonalFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z);
bool IsOrthonormalFrame2d(const ads_point& X, const ads_point& Y);
bool IsOrthonormalFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z);
bool IsRightHandFrame2d(const ads_point& X, const ads_point& Y);
bool IsRightHandFrame3d(const ads_point& X, const ads_point& Y, const ads_point& Z);

// Rounding helper
class CDosRound
{
public:
  // 0. Symmetric: round up
  // Bias: away from zero
  static double ceil0(const double& value);

  // 1. Symmetric: round down
  // Bias: towards zero
  static double floor0(const double& value);

  // 2. Common: round half up
  // Bias: +Infinity
  static double roundhalfup(const double& value);

  // 3. Common: round half down
  // Bias: -Infinity
  static double roundhalfdown(const double& value);

  // 4. Symmetric: round half up
  // Bias: away from zero
  static double roundhalfup0(const double& value);

  // 5. Symmetric: round half down
  // Bias: towards zero
  static double roundhalfdown0(const double& value);

  // 6. Round half even (banker's rounding)
  // Bias: none
  static double roundhalfeven(const double& value, const double& epsilon = 0.0000001);

  // 7. Round alternate
  // Bias: none for sequential calls
  static double roundalternate(const double& value);

  // 8. Symmetric round alternate
  // Bias: none for sequential calls
  static double roundalternate0(const double& value);

  // 9. Round random
  // Bias: rand()
  static double roundrandom(const double& value);

public:
  static bool m_up;
};
