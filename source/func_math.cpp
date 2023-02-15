/////////////////////////////////////////////////////////////////////////////
// func_math.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosMath.h"
#include "DosStringArray.h"

////////////////////////////////////////////////////////////////
// dos_pi
int CDOSLibApp::ads_dos_pi()
{
  acedRetReal(DOS_PI);
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_e
int CDOSLibApp::ads_dos_e()
{
  acedRetReal(DOS_E);
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_sin
int CDOSLibApp::ads_dos_sin()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(sin(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cos
int CDOSLibApp::ads_dos_cos()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(cos(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_tan
int CDOSLibApp::ads_dos_tan()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(tan(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_asin
int CDOSLibApp::ads_dos_asin()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(asin(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acos
int CDOSLibApp::ads_dos_acos()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(acos(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_atan
int CDOSLibApp::ads_dos_atan()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(atan(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_sinh
int CDOSLibApp::ads_dos_sinh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(sinh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cosh
int CDOSLibApp::ads_dos_cosh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(cosh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_tanh
int CDOSLibApp::ads_dos_tanh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(tanh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_asinh
int CDOSLibApp::ads_dos_asinh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(_asinh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acosh
int CDOSLibApp::ads_dos_acosh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(_acosh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_atanh 
int CDOSLibApp::ads_dos_atanh()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(_atanh(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_atan2
int CDOSLibApp::ads_dos_atan2()
{
  CAdsArgs args;

  double y = 0.0;
  if (!args.GetDouble(y))
    return RSERR;

  args.Next();

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  try
  {
    acedRetNum(atan2(y, x));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hypot
int CDOSLibApp::ads_dos_hypot()
{
  CAdsArgs args;

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  args.Next();

  double y = 0.0;
  if (!args.GetDouble(y))
    return RSERR;

  try
  {
    acedRetNum(_hypot(x, y));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_log
int CDOSLibApp::ads_dos_log()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(log(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_log2
int CDOSLibApp::ads_dos_log2()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(log(d) / log(2.0));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_log10
int CDOSLibApp::ads_dos_log10()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(log10(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_exp
int CDOSLibApp::ads_dos_exp()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(exp(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pow
int CDOSLibApp::ads_dos_pow()
{
  CAdsArgs args;

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  args.Next();

  double y = 0.0;
  if (!args.GetDouble(y))
    return RSERR;

  try
  {
    acedRetNum(pow(x, y));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_sqrt
int CDOSLibApp::ads_dos_sqrt()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(sqrt(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_div
int CDOSLibApp::ads_dos_div()
{
  CAdsArgs args;

  int n = 0;
  if (!args.GetInteger(n))
    return RSERR;

  args.Next();

  int d = 0;
  if (!args.GetInteger(d))
    return RSERR;

  try
  {
    div_t t = div(n, d);
    CAdsRetList result;
    result.Add(t.quot);
    result.Add(t.rem);
    acedRetList(result);
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_gcd
int CDOSLibApp::ads_dos_gcd()
{
  CAdsArgs args;

  int a = 0;
  if (!args.GetInteger(a))
    return RSERR;

  args.Next();

  int b = 0;
  if (!args.GetInteger(b))
    return RSERR;

  try
  {
    acedRetNum(_gcd(a, b));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_lcm
int CDOSLibApp::ads_dos_lcm()
{
  CAdsArgs args;

  int a = 0;
  if (!args.GetInteger(a))
    return RSERR;

  args.Next();

  int b = 0;
  if (!args.GetInteger(b))
    return RSERR;

  try
  {
    acedRetNum(_lcm(a, b));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_fmod
int CDOSLibApp::ads_dos_fmod()
{
  CAdsArgs args;

  double n = 0.0;
  if (!args.GetDouble(n))
    return RSERR;

  args.Next();

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(fmod(n, d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_modf
int CDOSLibApp::ads_dos_modf()
{
  CAdsArgs args;

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  try
  {
    double n = 0;
    double y = modf(x, &n);

    CAdsRetList result;
    result.Add(n);
    result.Add(y);
    acedRetList(result);
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_abs
int CDOSLibApp::ads_dos_abs()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(fabs(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rtd
int CDOSLibApp::ads_dos_rtd()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  acedRetNum(d * (180.0 / DOS_PI));

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dtr
int CDOSLibApp::ads_dos_dtr()
{
  CAdsArgs args;

  if (args.IsInteger())
  {
    int deg = 0, min = 0, sec = 0;
    double dir = 1.0;

    if (!args.GetInteger(deg) || !DOS_CLOSED_INTERVAL(deg, 0, 360))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(min) || !DOS_CLOSED_INTERVAL(min, 0, 60))
        return RSERR;

      args.Next();

      if (!args.IsEmpty())
      {
        if (!args.GetInteger(sec) || !DOS_CLOSED_INTERVAL(sec, 0, 60))
          return RSERR;

        args.Next();

        if (!args.IsEmpty())
        {
          if (args.IsNil())
            dir = -1.0;
          else
            dir = 1.0;
        }
      }
    }

    double angle = ((double)deg + ((double)min * (1.0 / 60.0)) + ((double)sec * (1.0 / 3600.0)));
    acedRetNum(dir * (angle * (DOS_PI / 180.0)));
  }
  else
  {
    double angle = 0.0;
    if (!args.GetDouble(angle))
      return RSERR;

    acedRetNum(angle * (DOS_PI / 180.0));
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ceil
int CDOSLibApp::ads_dos_ceil()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  acedRetNum(ceil(d));

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_floor
int CDOSLibApp::ads_dos_floor()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  acedRetNum(floor(d));

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_round 
int CDOSLibApp::ads_dos_round()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  args.Next();

  int mode = 6; // roundhalfeven
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(mode))
      return RSERR;
  }

  switch (mode)
  {
  case 0:
    acedRetNum(CDosRound::ceil0(d));
    break;
  case 1:
    acedRetNum(CDosRound::floor0(d));
    break;
  case 2:
    acedRetNum(CDosRound::roundhalfup(d));
    break;
  case 3:
    acedRetNum(CDosRound::roundhalfdown(d));
    break;
  case 4:
    acedRetNum(CDosRound::roundhalfup0(d));
    break;
  case 5:
    acedRetNum(CDosRound::roundhalfdown0(d));
    break;
  case 6:
    acedRetNum(CDosRound::roundhalfeven(d));
    break;
  case 7:
    acedRetNum(CDosRound::roundalternate(d));
    break;
  case 8:
    acedRetNum(CDosRound::roundalternate0(d));
    break;
  case 9:
    acedRetNum(CDosRound::roundrandom(d));
    break;
  default:
    acedRetNil();
    break;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_trunc
int CDOSLibApp::ads_dos_trunc()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  args.Next();

  int n = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(n))
      return RSERR;
    n = DOS_CLAMP(n, 0, 16);
  }

  acedRetNum(_trunc(d, n));

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_min
int CDOSLibApp::ads_dos_min()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;
      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  list.Sort(true);
  acedRetNum(list[0]);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_max
int CDOSLibApp::ads_dos_max()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;
      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  list.Sort(false);
  acedRetNum(list[0]);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_sum
int CDOSLibApp::ads_dos_sum()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    CDosNumberArray list;
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }

    acedRetNum(_add(list.GetData(), (int)list.GetCount()));
  }
  else
  {
    CDosNumberArray list;
    if (!args.GetDoubleList(list))
      return RSERR;

    const int list_count = (int)list.GetCount();

    args.Next();

    if (!args.IsEmpty())
    {
      while (!args.IsEmpty())
      {
        CDosNumberArray next;
        if (!args.GetDoubleList(next))
          return RSERR;

        if (list_count != (int)next.GetCount())
          return RSERR;

        for (int i = 0; i < list_count; i++)
          list[i] += next[i];

        args.Next();
      }

      CAdsRetList result;
      result.Add(list.GetData(), (int)list.GetCount());
      acedRetList(result);
    }
    else
    {
      acedRetNum(_add(list.GetData(), (int)list.GetCount()));
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_difference
int CDOSLibApp::ads_dos_difference()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    CDosNumberArray list;
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }

    acedRetNum(_subtract(list.GetData(), (int)list.GetCount()));
  }
  else
  {
    CDosNumberArray list;
    if (!args.GetDoubleList(list))
      return RSERR;

    const int list_count = (int)list.GetCount();

    args.Next();

    if (!args.IsEmpty())
    {
      while (!args.IsEmpty())
      {
        CDosNumberArray next;
        if (!args.GetDoubleList(next))
          return RSERR;

        if (list_count != (int)next.GetCount())
          return RSERR;

        for (int i = 0; i < list_count; i++)
          list[i] -= next[i];

        args.Next();
      }

      CAdsRetList result;
      result.Add(list.GetData(), (int)list.GetCount());
      acedRetList(result);
    }
    else
    {
      acedRetNum(_subtract(list.GetData(), (int)list.GetCount()));
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_product
int CDOSLibApp::ads_dos_product()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    CDosNumberArray list;
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }

    acedRetNum(_multiply(list.GetData(), (int)list.GetCount()));
  }
  else
  {
    CDosNumberArray list;
    if (!args.GetDoubleList(list))
      return RSERR;

    const int list_count = (int)list.GetCount();

    args.Next();

    if (!args.IsEmpty())
    {
      while (!args.IsEmpty())
      {
        CDosNumberArray next;
        if (!args.GetDoubleList(next))
          return RSERR;

        if (list_count != (int)next.GetCount())
          return RSERR;

        for (int i = 0; i < list_count; i++)
          list[i] *= next[i];

        args.Next();
      }

      CAdsRetList result;
      result.Add(list.GetData(), (int)list.GetCount());
      acedRetList(result);
    }
    else
    {
      acedRetNum(_multiply(list.GetData(), (int)list.GetCount()));
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_quotient
int CDOSLibApp::ads_dos_quotient()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    CDosNumberArray list;
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }

    double rc = 0.0;
    if (_divide(list.GetData(), (int)list.GetCount(), rc))
      acedRetNum(rc);
    else
      return RSERR;
  }
  else
  {
    CDosNumberArray list;
    if (!args.GetDoubleList(list))
      return RSERR;

    const int list_count = (int)list.GetCount();

    args.Next();

    if (!args.IsEmpty())
    {
      while (!args.IsEmpty())
      {
        CDosNumberArray next;
        if (!args.GetDoubleList(next))
          return RSERR;

        if (list_count != (int)next.GetCount())
          return RSERR;

        for (int i = 0; i < list_count; i++)
        {
          if (0.0 == next[i])
          {
            DOS_PrintError(L"divide by zero");
            return RSERR;
          }
          list[i] /= next[i];
        }

        args.Next();
      }

      CAdsRetList result;
      result.Add(list.GetData(), (int)list.GetCount());
      acedRetList(result);
    }
    else
    {
      double rc = 0.0;
      if (_divide(list.GetData(), (int)list.GetCount(), rc))
        acedRetNum(rc);
      else
        return RSERR;
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_mean
int CDOSLibApp::ads_dos_mean()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    CDosNumberArray list;
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }

    acedRetNum(_mean(list.GetData(), (int)list.GetCount()));
  }
  else
  {
    CDosNumberArray list;
    if (!args.GetDoubleList(list))
      return RSERR;

    const int list_count = (int)list.GetCount();
    int list_items = 1;

    args.Next();

    if (!args.IsEmpty())
    {
      while (!args.IsEmpty())
      {
        CDosNumberArray next;
        if (!args.GetDoubleList(next))
          return RSERR;

        if (list_count != (int)next.GetCount())
          return RSERR;

        for (int i = 0; i < list_count; i++)
          list[i] += next[i];

        list_items++;

        args.Next();
      }

      for (int i = 0; i < list_count; i++)
        list[i] /= list_items;

      CAdsRetList result;
      result.Add(list.GetData(), (int)list.GetCount());
      acedRetList(result);
    }
    else
    {
      acedRetNum(_mean(list.GetData(), (int)list.GetCount()));
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_median
int CDOSLibApp::ads_dos_median()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  int count = (int)list.GetCount();
  list.Sort(true);

  double d = 0.0;
  if (count > 1)
  {
    if (count % 2)
      d = list[count / 2];
    else
      d = (list[count / 2 - 1] + list[count / 2]) / 2;
  }
  else
    d = list[0];

  acedRetNum(d);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_mode
int CDOSLibApp::ads_dos_mode()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  CMap<double, double, int, int> map;
  double d = 0.0;
  int i, count = 0;
  for (i = 0; i < list.GetCount(); i++)
  {
    d = list[i];
    map[d]++;
    if (map[d] > count)
      count = map[d];
  }

  CDosNumberArray values;
  POSITION pos = map.GetStartPosition();
  while (pos)
  {
    map.GetNextAssoc(pos, d, i);
    if (i == count)
      values.Add(d);
  }

  values.Sort(true);

  CAdsRetList result;
  for (i = 0; i < values.GetCount(); i++)
    result.Add(d);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_range
int CDOSLibApp::ads_dos_range()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  int count = (int)list.GetCount();
  list.Sort(true);

  double d = 0.0;
  if (count > 1)
    d = list[count - 1] - list[0];
  else
    d = 0.0;

  acedRetNum(d);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_moment
int CDOSLibApp::ads_dos_moment()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  if (list.GetCount() < 2)
    return RSERR;

  double ave, adev, sdev, var, skew, curt;
  ave = adev = sdev = var = skew = curt = 0.0;

  if (_moment(list.GetData(), (int)list.GetCount(), ave, adev, sdev, var, skew, curt))
  {
    CAdsRetList result;
    result.Add(ave);
    result.Add(adev);
    result.Add(sdev);
    result.Add(var);
    result.Add(skew == DOS_UNSET_VALUE ? false : skew);
    result.Add(curt == DOS_UNSET_VALUE ? false : curt);
    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_interp
int CDOSLibApp::ads_dos_interp()
{
  CAdsArgs args;

  double y0 = 0.0;
  if (!args.GetDouble(y0))
    return RSERR;

  args.Next();

  double y1 = 0.0;
  if (!args.GetDouble(y1))
    return RSERR;

  if (y0 >= y1)
    return RSERR;

  args.Next();

  double t = 0.0;
  if (!args.GetDouble(t))
    return RSERR;

  args.Next();

  int mode = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(mode))
      return RSERR;
  }

  try
  {
    switch (mode)
    {
    case 0:
      acedRetNum(LinearInterpolate(y0, y1, t));
      break;
    case 1:
      acedRetNum(CosineInterpolate(y0, y1, t));
      break;
    case 2:
      acedRetNum(SmoothStepInterpolate(y0, y1, t));
      break;
    case 3:
      acedRetNum(AccelerationInterpolate(y0, y1, t));
      break;
    case 4:
      acedRetNum(DecelerationInterpolate(y0, y1, t));
      break;
    case 5:
      //acedRetNum( ExponentialInterpolate(y0, y1, t) );
      acedRetNil();
      break;
    case 6:
      //acedRetNum( LogarithmicInterpolate(y0, y1, t) );
      acedRetNil();
      break;
    default:
      acedRetNil();
      break;
    }
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_clamp
int CDOSLibApp::ads_dos_clamp()
{
  CAdsArgs args;

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  args.Next();

  double a = 0.0;
  if (!args.GetDouble(a))
    return RSERR;

  args.Next();

  double b = 0.0;
  if (!args.GetDouble(b))
    return RSERR;

  if (a >= b)
    return RSERR;

  acedRetNum(DOS_CLAMP(x, a, b));

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_normalize
int CDOSLibApp::ads_dos_normalize()
{
  CAdsArgs args;

  double t = 0.0;
  if (!args.GetDouble(t))
    return RSERR;

  args.Next();

  double t0 = 0.0;
  if (!args.GetDouble(t0))
    return RSERR;

  args.Next();

  double t1 = 0.0;
  if (!args.GetDouble(t1))
    return RSERR;

  if (t0 >= t1)
    return RSERR;

  acedRetNum(_normalize(t0, t1, t));

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_parameterize
int CDOSLibApp::ads_dos_parameterize()
{
  CAdsArgs args;

  double x = 0.0;
  if (!args.GetDouble(x))
    return RSERR;

  args.Next();

  double t0 = 0.0;
  if (!args.GetDouble(t0))
    return RSERR;

  args.Next();

  double t1 = 0.0;
  if (!args.GetDouble(t1))
    return RSERR;

  if (t0 >= t1)
    return RSERR;

  acedRetNum(_parameterize(t0, t1, x));

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_fact
int CDOSLibApp::ads_dos_fact()
{
  CAdsArgs args;

  int n = 0;
  if (!args.GetInteger(n))
    return RSERR;

  acedRetNum(_fact(n));

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_equal
int CDOSLibApp::ads_dos_equal()
{
  CAdsArgs args;

  if (args.IsNumber())
  {
    double a = 0.0;
    if (!args.GetDouble(a))
      return RSERR;

    args.Next();

    double b = 0.0;
    if (!args.GetDouble(b))
      return RSERR;

    args.Next();

    double t = DOS_ZERO_TOLERANCE;
    if (args.IsEmpty())
    {
      struct resbuf rb;
      acedGetVar(L"LUPREC", &rb);
      int lu = rb.resval.rint;
      if (lu <= 0)
        t = DOS_ZERO_TOLERANCE;
      else
        t = (1 / pow(10.0, lu));
    }
    else
    {
      if (!args.GetDouble(t))
        return RSERR;

      if (t < 0.0)
        t = DOS_ZERO_TOLERANCE;
    }

    if (fabs(a - b) < t)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    CDosNumberArray list_a;
    if (!args.GetDoubleList(list_a))
      return RSERR;

    args.Next();

    CDosNumberArray list_b;
    if (!args.GetDoubleList(list_b))
      return RSERR;

    if (list_a.GetCount() != list_b.GetCount())
      return RSERR;

    args.Next();

    double t = DOS_ZERO_TOLERANCE;
    if (args.IsEmpty())
    {
      struct resbuf rb;
      acedGetVar(L"LUPREC", &rb);
      int lu = rb.resval.rint;
      if (lu <= 0)
        t = DOS_ZERO_TOLERANCE;
      else
        t = (1 / pow(10.0, lu));
    }
    else
    {
      if (!args.GetDouble(t))
        return RSERR;

      if (t < 0.0)
        t = DOS_ZERO_TOLERANCE;
    }

    bool b = true;
    for (int i = 0; i < list_a.GetCount(); i++)
    {
      if (fabs(list_a[i] - list_b[i]) >= t)
      {
        b = false;
        break;
      }
    }

    if (b)
      acedRetT();
    else
      acedRetNil();

  }

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_copysign
int CDOSLibApp::ads_dos_copysign()
{
  CAdsArgs args;

  double a = 0.0;
  if (!args.GetDouble(a))
    return RSERR;

  args.Next();

  double b = 0.0;
  if (!args.GetDouble(b))
    return RSERR;

  acedRetNum(_copysign(a, b));

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_chgsign
int CDOSLibApp::ads_dos_chgsign()
{
  CAdsArgs args;

  double a = 0.0;
  if (!args.GetDouble(a))
    return RSERR;

  acedRetNum(_chgsign(a));

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_sign
int CDOSLibApp::ads_dos_sign()
{
  CAdsArgs args;

  double a = 0.0;
  if (!args.GetDouble(a))
    return RSERR;

  int count = args.Count();

  if (4 == count)
  {
    if (a < 0.0) // -1
    {
      args.Next();
      acedRetVal(args.Resbuf());
    }
    else if (a > 0.0) // 1
    {
      args.Next();
      args.Next();
      args.Next();
      acedRetVal(args.Resbuf());
    }
    else // 0
    {
      args.Next();
      args.Next();
      acedRetVal(args.Resbuf());
    }
  }
  else
  {
    if (a < 0.0)
      acedRetInt(-1);
    else if (a > 0.0)
      acedRetInt(1);
    else
      acedRetInt(0);
  }

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_sortnumbers
int CDOSLibApp::ads_dos_sortnumbers()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (!args.GetDoubleList(list))
    return RSERR;

  args.Next();

  bool bIncreasing = true;
  if (!args.IsEmpty())
  {
    if (!args.GetBoolean(bIncreasing))
      return RSERR;
  }

  list.Sort(bIncreasing);

  CAdsRetList result;
  for (int i = 0; i < list.GetCount(); i++)
    result.Add(list[i]);
  acedRetList(result);

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_cullnumbers
int CDOSLibApp::ads_dos_cullnumbers()
{
  CAdsArgs args;

  CDosNumberArray numbers;
  if (!args.GetDoubleList(numbers))
    return RSERR;

  args.Next();

  double tolerance = DOS_ZERO_TOLERANCE;
  if (!args.IsEmpty())
  {
    if (!args.GetDouble(tolerance))
      return RSERR;

    if (tolerance < DOS_ZERO_TOLERANCE)
      tolerance = DOS_ZERO_TOLERANCE;
  }

  numbers.Cull();

  CAdsRetList result;
  for (int i = 0; i < numbers.GetCount(); i++)
    result.Add(numbers[i]);

  acedRetList(result);

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_permute
static void permute_print(CAdsRetList* result, const CAdsResbuf* v, const int size)
{
  if (result && v)
  {
    result->AddLB();
    for (int i = 0; i < size; i++)
    {
      if (v[i].m_d == DOS_UNSET_VALUE)
        result->Add(v[i].m_s);
      else
        result->Add(v[i].m_d);
    }
    result->AddLE();
  }
}

static void permute_swap(CAdsResbuf* v, const int i, const int j)
{
  CAdsResbuf t = v[i];
  v[i] = v[j];
  v[j] = t;
}

static void permute_rotate_left(CAdsResbuf* v, const int start, const int n)
{
  CAdsResbuf tmp = v[start];
  for (int i = start; i < n - 1; i++)
    v[i] = v[i + 1];
  v[n - 1] = tmp;
}

static void permute(CAdsRetList* result, CAdsResbuf* v, const int start, const int n)
{
  permute_print(result, v, n);
  if (start < n)
  {
    int i, j;
    for (i = n - 2; i >= start; i--)
    {
      for (j = i + 1; j < n; j++)
      {
        permute_swap(v, i, j);
        permute(result, v, i + 1, n);
      }
      permute_rotate_left(v, i, n);
    }
  }
}

int CDOSLibApp::ads_dos_permute()
{
  CAdsArgs args;

  CArray<CAdsResbuf, CAdsResbuf> list;
  if (!args.GetResbufList(list))
    return RSERR;

  CAdsRetList result;
  permute(&result, list.GetData(), 0, (int)list.GetCount());
  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_vector
int CDOSLibApp::ads_dos_vector()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  CAdsRetList result;
  if (b2d)
  {
    result.Add(a[0] - b[0]);
    result.Add(a[1] - b[1]);
  }
  else
  {
    result.Add(a[0] - b[0]);
    result.Add(a[1] - b[1]);
    result.Add(a[2] - b[2]);
  }

  acedRetList(result);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_unitize
int CDOSLibApp::ads_dos_unitize()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  CAdsRetList result;
  if (b2d)
  {
    if (Unitize2d(a))
    {
      result.Add(a[0]);
      result.Add(a[1]);
    }
  }
  else
  {
    if (Unitize3d(a))
    {
      result.Add(a[0]);
      result.Add(a[1]);
      result.Add(a[2]);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_length
int CDOSLibApp::ads_dos_length()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();
  if (b2d)
    acedRetNum(Length2d(a));
  else
    acedRetNum(Length3d(a));

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_negate
int CDOSLibApp::ads_dos_negate()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (args.IsNumber())
  {
    while (!args.IsEmpty())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;

      list.Add(d);
      args.Next();
    }
  }
  else
  {
    if (!args.GetDoubleList(list))
      return RSERR;
  }

  CAdsRetList result;
  for (int i = 0; i < list.GetCount(); i++)
    result.Add(-list[i]);

  acedRetList(result);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_crossproduct
int CDOSLibApp::ads_dos_crossproduct()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  ads_point v;
  if (b2d)
    CrossProduct2d(a, b, v);
  else
    CrossProduct3d(a, b, v);

  CAdsRetList result;
  result.Add(v[0]);
  result.Add(v[1]);
  result.Add(v[2]);
  acedRetList(result);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_dotproduct
int CDOSLibApp::ads_dos_dotproduct()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  if (b2d)
    acedRetNum(DotProduct2d(a, b));
  else
    acedRetNum(DotProduct3d(a, b));

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_wedgeproduct
int CDOSLibApp::ads_dos_wedgeproduct()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    acedRetNum(a[0] * b[1] - a[1] * b[0]);
  else
    return RSERR; // 2d only

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_tripleproduct
int CDOSLibApp::ads_dos_tripleproduct()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  ads_point c;
  if (!args.GetPoint(c))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  if (!b2d)
    acedRetNum(TripleProduct3d(a, b, c));
  else
    return RSERR; // 3d only

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_perpendicularto
int CDOSLibApp::ads_dos_perpendicularto()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  CAdsRetList result;
  ads_point b;
  if (b2d)
  {
    if (Perpendicular2d(a, b))
    {
      result.Add(b[0]);
      result.Add(b[1]);
    }
  }
  else
  {
    if (Perpendicular3d(a, b))
    {
      result.Add(b[0]);
      result.Add(b[1]);
      result.Add(b[2]);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_zerop
int CDOSLibApp::ads_dos_zerop()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();
  if (b2d)
  {
    if (a[0] == 0.0 && a[1] == 0.0)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    if (a[0] == 0.0 && a[1] == 0.0 && a[2] == 0.0)
      acedRetT();
    else
      acedRetNil();
  }

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_tinyp
int CDOSLibApp::ads_dos_tinyp()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  double tiny_tol = DOS_ZERO_TOLERANCE;
  if (!args.IsEmpty())
  {
    if (!args.GetDouble(tiny_tol))
      return RSERR;
  }

  if (b2d)
  {
    if (fabs(a[0]) <= tiny_tol && fabs(a[1]) <= tiny_tol)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    if (fabs(a[0]) <= tiny_tol && fabs(a[1]) <= tiny_tol && fabs(a[2]) <= tiny_tol)
      acedRetT();
    else
      acedRetNil();
  }

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_unitp
int CDOSLibApp::ads_dos_unitp()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();
  if (b2d)
  {
    if (fabs(Length2d(a) - 1.0) <= DOS_SQRT_EPSILON)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    if (fabs(Length3d(a) - 1.0) <= DOS_SQRT_EPSILON)
      acedRetT();
    else
      acedRetNil();
  }

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_parallelp
int CDOSLibApp::ads_dos_parallelp()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  double ang_tol = DOS_PI / 180.0;
  if (!args.IsEmpty())
  {
    if (!args.GetDouble(ang_tol))
      return RSERR;
  }

  int rc = 0;
  if (b2d)
  {
    const double ll = Length2d(a) * Length2d(b);
    if (ll > 0.0)
    {
      const double cos_angle = (a[0] * b[0] + a[1] * b[1]) / ll;
      const double cos_tol = cos(ang_tol);
      if (cos_angle >= cos_tol)
        rc = 1;
      else if (cos_angle <= -cos_tol)
        rc = -1;
    }
  }
  else
  {
    const double ll = Length3d(a) * Length3d(b);
    if (ll > 0.0)
    {
      const double cos_angle = (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / ll;
      const double cos_tol = cos(ang_tol);
      if (cos_angle >= cos_tol)
        rc = 1;
      else if (cos_angle <= -cos_tol)
        rc = -1;
    }
  }

  acedRetInt(rc);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_perpendicularp
int CDOSLibApp::ads_dos_perpendicularp()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  double ang_tol = DOS_PI / 180.0;
  if (!args.IsEmpty())
  {
    if (!args.GetDouble(ang_tol))
      return RSERR;
  }

  bool rc = false;
  if (b2d)
  {
    const double ll = Length2d(a) * Length2d(b);
    if (ll > 0.0)
    {
      if (fabs((a[0] * b[0] + a[1] * b[1]) / ll) <= sin(ang_tol))
        rc = true;
    }
  }
  else
  {
    const double ll = Length3d(a) * Length3d(b);
    if (ll > 0.0)
    {
      if (fabs((a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / ll) <= sin(ang_tol))
        rc = true;
    }
  }

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_orthogonalp
int CDOSLibApp::ads_dos_orthogonalp()
{
  CAdsArgs args;
  bool rc = false;

  ads_point X;
  if (!args.GetPoint(X))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point Y;
  if (!args.GetPoint(Y))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  if (args.IsEmpty())
  {
    if (b2d)
      rc = IsOrthogonalFrame2d(X, Y);
    else
      return RSERR;
  }
  else
  {
    ads_point Z;
    if (!args.GetPoint(Z))
      return RSERR;

    if (b2d)
      b2d = args.Is2dPoint();

    if (!b2d)
      rc = IsOrthogonalFrame3d(X, Y, Z);
    else
      return RSERR;
  }

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_orthonormalp
int CDOSLibApp::ads_dos_orthonormalp()
{
  CAdsArgs args;
  bool rc = false;

  ads_point X;
  if (!args.GetPoint(X))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point Y;
  if (!args.GetPoint(Y))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  if (args.IsEmpty())
  {
    if (b2d)
      rc = IsOrthonormalFrame2d(X, Y);
    else
      return RSERR;
  }
  else
  {
    ads_point Z;
    if (!args.GetPoint(Z))
      return RSERR;

    if (b2d)
      b2d = args.Is2dPoint();

    if (!b2d)
      rc = IsOrthonormalFrame3d(X, Y, Z);
    else
      return RSERR;
  }

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_righthandp
int CDOSLibApp::ads_dos_righthandp()
{
  CAdsArgs args;
  bool rc = false;

  ads_point X;
  if (!args.GetPoint(X))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point Y;
  if (!args.GetPoint(Y))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  args.Next();

  if (args.IsEmpty())
  {
    if (b2d)
      rc = IsRightHandFrame2d(X, Y);
    else
      return RSERR;
  }
  else
  {
    ads_point Z;
    if (!args.GetPoint(Z))
      return RSERR;

    if (b2d)
      b2d = args.Is2dPoint();

    if (!b2d)
      rc = IsRightHandFrame3d(X, Y, Z);
    else
      return RSERR;
  }

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_anglebetween
int CDOSLibApp::ads_dos_anglebetween()
{
  CAdsArgs args;

  ads_point a;
  if (!args.GetPoint(a))
    return RSERR;

  bool b2d = args.Is2dPoint();

  args.Next();

  ads_point b;
  if (!args.GetPoint(b))
    return RSERR;

  if (b2d)
    b2d = args.Is2dPoint();

  double radians = 0.0;
  if (b2d)
  {
    Unitize2d(a);
    Unitize2d(b);
    double dot = DOS_CLAMP(DotProduct2d(a, b), -1.0, 1.0);
    radians = acos(dot);
  }
  else
  {
    Unitize3d(a);
    Unitize3d(b);
    double dot = DOS_CLAMP(DotProduct3d(a, b), -1.0, 1.0);
    radians = acos(dot);
  }
  acedRetNum(radians);

  return RTNORM;
}

///////////////////////////////////////////////////////////////
// dos_scale
int CDOSLibApp::ads_dos_scale()
{
  CAdsArgs args;

  CDosNumberArray list;
  if (!args.GetDoubleList(list))
    return RSERR;

  args.Next();

  double scale = 1.0;
  if (!args.GetDouble(scale))
    return RSERR;

  CAdsRetList result;
  for (int i = 0; i < list.GetCount(); i++)
    result.Add(list[i] * scale);
  acedRetList(result);

  return RSRSLT;
}

///////////////////////////////////////////////////////////////
// dos_quadratic
int CDOSLibApp::ads_dos_quadratic()
{
  CAdsArgs args;

  double a = 1.0;
  if (!args.GetDouble(a))
    return RSERR;

  args.Next();

  double b = 1.0;
  if (!args.GetDouble(b))
    return RSERR;

  args.Next();

  double c = 1.0;
  if (!args.GetDouble(c))
    return RSERR;

  double r0 = 0.0, r1 = 0.0;

  int rc = _quadratic(a, b, c, &r0, &r1);

  CAdsRetList result;
  result.Add(rc);
  result.Add(r0);
  result.Add(r1);
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_phi
int CDOSLibApp::ads_dos_phi()
{
  acedRetReal((1.0 + sqrt(5.0)) / 2.0);
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cbrt
int CDOSLibApp::ads_dos_cbrt()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  try
  {
    acedRetNum(cbrt(d));
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isprime
int CDOSLibApp::ads_dos_isprime()
{
  CAdsArgs args;

  int n = 0;
  if (!args.GetInteger(n))
    return RSERR;

  try
  {
    if (IsPrime(n))
      acedRetT();
    else
      acedRetNil();
  }
  catch (...)
  {
    return RSERR;
  }

  return RSRSLT;
}