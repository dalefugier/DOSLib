/////////////////////////////////////////////////////////////////////////////
// DosStringArray.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDosStringArray
class CDosStringArray : public CStringArray
{
public:
  CDosStringArray();
  ~CDosStringArray();
  CDosStringArray(const CDosStringArray& src);
  CDosStringArray& operator=(const CDosStringArray& src);
public:
  void Sort(bool bCaseSensitive = false, bool bAscending = true);
  void SortLogical(bool bAscending = true);
  void Cull(bool bCaseSensitive = false);
};

/////////////////////////////////////////////////////////////////////////////
// CDosNumberArray
class CDosNumberArray : public CArray<double, double>
{
public:
  void Sort(bool bIncreasing = true);
  void Cull(double tolerance = DOS_ZERO_TOLERANCE);
};

/////////////////////////////////////////////////////////////////////////////
// CDos2dPointArray
class CDos2dPointArray : public CArray<AcGePoint2d, AcGePoint2d>
{
public:
  void SortXY(bool bIncreasing = true);
  void SortYX(bool bIncreasing = true);
  void Cull(double tolerance = DOS_ZERO_TOLERANCE);
};

/////////////////////////////////////////////////////////////////////////////
// CDos3dPointArray
class CDos3dPointArray : public CArray<AcGePoint3d, AcGePoint3d>
{
public:
  // x,y,z
  void SortXYZ(bool bIncreasing = true);
  // x,z,y
  void SortXZY(bool bIncreasing = true);
  // y,x,z
  void SortYXZ(bool bIncreasing = true);
  // y,z,x
  void SortYZX(bool bIncreasing = true);
  // z,x,y
  void SortZXY(bool bIncreasing = true);
  // z,y,x
  void SortZYX(bool bIncreasing = true);
  // Cull
  void Cull(double tolerance = DOS_ZERO_TOLERANCE);
};