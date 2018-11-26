/////////////////////////////////////////////////////////////////////////////
// DosStringArray.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosStringArray.h"

static int __cdecl CompareAscending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return a->Compare(*b);
}

static int __cdecl CompareDescending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return (-1 * a->Compare(*b));
}

static int __cdecl CompareNoCaseAscending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return a->CompareNoCase(*b);
}

static int __cdecl CompareNoCaseDescending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return (-1 * a->CompareNoCase(*b));
}

static int __cdecl CompareLogicalAscending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return (StrCmpLogicalW(a->GetBuffer(), b->GetBuffer()));
}

static int __cdecl CompareLogicalDescending(const void* pa, const void* pb)
{
  CString* a = (CString*)pa;
  CString* b = (CString*)pb;
  if (0 == a && 0 == b)
    return 0;
  if (0 == a)
    return -1;
  if (0 == b)
    return 1;
  return (-1 * StrCmpLogicalW(a->GetBuffer(), b->GetBuffer()));
}

CDosStringArray::CDosStringArray()
{
}

CDosStringArray::~CDosStringArray()
{
}

CDosStringArray::CDosStringArray(const CDosStringArray& src)
{
  Append(src);
}

CDosStringArray& CDosStringArray::operator=(const CDosStringArray& src)
{
  if (this != &src)
  {
    RemoveAll();
    Append(src);
  }
  return *this;
}

void CDosStringArray::Sort(bool bCaseSensitive, bool bAscending)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(CString),
    bAscending
    ? (bCaseSensitive ? CompareAscending : CompareNoCaseAscending)
    : (bCaseSensitive ? CompareDescending : CompareNoCaseDescending)
    );
}

void CDosStringArray::SortLogical(bool bAscending)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(CString),
    bAscending
    ? CompareLogicalAscending
    : CompareLogicalDescending
    );
}

void CDosStringArray::Cull(bool bCaseSensitive)
{
  if (GetSize() <= 1)
    return;

  const int count = (int)GetCount();
  CDosStringArray temp;
  temp.Copy(*this);
  RemoveAll();

  int* delete_list = (int*)malloc(count * sizeof(int));
  memset(delete_list, 0, count * sizeof(int));

  int i, j;
  for (i = 0; i < count; i++)
  {
    if (!delete_list[i])
    {
      Add(temp[i]);
      for (j = i + 1; j < count; j++)
      {
        if (delete_list[j])
          continue;
        if (bCaseSensitive)
        {
          if (0 == temp[j].Compare(temp[i]))
            delete_list[j] = true;
        }
        else
        {
          if (0 == temp[j].CompareNoCase(temp[i]))
            delete_list[j] = true;
        }
      }
    }
  }

  free((void*)delete_list);
}

/////////////////////////////////////////////////////////////////////////////

static int __cdecl CompareDoubleIncreasing(const void* pa, const void* pb)
{
  double* a = (double*)pa;
  double* b = (double*)pb;
  ASSERT(a);
  ASSERT(b);
  if (*a < *b) return -1;
  if (*b < *a) return  1;
  return 0;
}

static int __cdecl CompareDoubleDecreasing(const void* pa, const void* pb)
{
  double* a = (double*)pa;
  double* b = (double*)pb;
  ASSERT(a);
  ASSERT(b);
  if (*b < *a) return -1;
  if (*a < *b) return  1;
  return 0;
}

void CDosNumberArray::Sort(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(double),
    bIncreasing
    ? (CompareDoubleIncreasing)
    : (CompareDoubleDecreasing)
    );
}

void CDosNumberArray::Cull(double tolerance)
{
  if (GetSize() <= 1)
    return;

  if (tolerance < DOS_ZERO_TOLERANCE)
    tolerance = DOS_ZERO_TOLERANCE;

  const int count = (int)GetCount();
  CDosNumberArray temp;
  temp.Copy(*this);
  RemoveAll();

  int* delete_list = (int*)malloc(count * sizeof(int));
  memset(delete_list, 0, count * sizeof(int));

  int i, j;
  for (i = 0; i < count; i++)
  {
    if (!delete_list[i])
    {
      Add(temp[i]);
      for (j = i + 1; j < count; j++)
      {
        if (delete_list[j])
          continue;
        if (fabs(temp[j] - temp[i]) <= tolerance)
          delete_list[j] = true;
      }
    }
  }

  free((void*)delete_list);
}

/////////////////////////////////////////////////////////////////////////////

static int __cdecl CompareSortXYIncreasing(const void* pa, const void* pb)
{
  AcGePoint2d* a = (AcGePoint2d*)pa;
  AcGePoint2d* b = (AcGePoint2d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->y < b->y) ? true : false) : false)) return -1;
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->y < a->y) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortXYDecreasing(const void* pa, const void* pb)
{
  AcGePoint2d* a = (AcGePoint2d*)pa;
  AcGePoint2d* b = (AcGePoint2d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->y < a->y) ? true : false) : false)) return -1;
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->y < b->y) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortYXIncreasing(const void* pa, const void* pb)
{
  AcGePoint2d* a = (AcGePoint2d*)pa;
  AcGePoint2d* b = (AcGePoint2d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->x < b->x) ? true : false) : false)) return -1;
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->x < a->x) ? true : false) : false)) return 1;
  return 0;
}

void CDos2dPointArray::SortXY(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint2d),
    bIncreasing
    ? (CompareSortXYIncreasing)
    : (CompareSortXYDecreasing)
    );
}

static int __cdecl CompareSortYXDecreasing(const void* pa, const void* pb)
{
  AcGePoint2d* a = (AcGePoint2d*)pa;
  AcGePoint2d* b = (AcGePoint2d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->x < a->x) ? true : false) : false)) return -1;
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->x < b->x) ? true : false) : false)) return 1;
  return 0;
}

void CDos2dPointArray::SortYX(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint2d),
    bIncreasing
    ? (CompareSortYXIncreasing)
    : (CompareSortYXDecreasing)
    );
}

void CDos2dPointArray::Cull(double tolerance)
{
  if (GetSize() <= 1)
    return;

  if (tolerance < DOS_ZERO_TOLERANCE)
    tolerance = DOS_ZERO_TOLERANCE;

  const int count = (int)GetCount();
  CDos2dPointArray temp;
  temp.Copy(*this);
  RemoveAll();

  int* delete_list = (int*)malloc(count * sizeof(int));
  memset(delete_list, 0, count * sizeof(int));

  int i, j;
  for (i = 0; i < count; i++)
  {
    if (!delete_list[i])
    {
      Add(temp[i]);
      for (j = i + 1; j < count; j++)
      {
        if (delete_list[j])
          continue;
        if (fabs(temp[j].distanceTo(temp[i])) <= tolerance)
          delete_list[j] = true;
      }
    }
  }

  free((void*)delete_list);
}

/////////////////////////////////////////////////////////////////////////////

static int __cdecl CompareSortXYZIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->y < b->y) ? true : (a->y == b->y&&a->z < b->z) ? true : false) : false)) return -1;
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->y < a->y) ? true : (b->y == a->y&&b->z < a->z) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortXYZDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->y < a->y) ? true : (b->y == a->y&&b->z < a->z) ? true : false) : false)) return -1;
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->y < b->y) ? true : (a->y == b->y&&a->z < b->z) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortXYZ(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortXYZIncreasing)
    : (CompareSortXYZDecreasing)
    );
}

static int __cdecl CompareSortXZYIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->z < b->z) ? true : (a->z == b->z&&a->y < b->y) ? true : false) : false)) return -1;
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->z < a->z) ? true : (b->z == a->z&&b->y < a->y) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortXZYDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->x < a->x) ? true : ((b->x == a->x) ? ((b->z < a->z) ? true : (b->z == a->z&&b->y < a->y) ? true : false) : false)) return -1;
  if ((a->x < b->x) ? true : ((a->x == b->x) ? ((a->z < b->z) ? true : (a->z == b->z&&a->y < b->y) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortXZY(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortXZYIncreasing)
    : (CompareSortXZYDecreasing)
    );
}

static int __cdecl CompareSortYXZIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->x < b->x) ? true : (a->x == b->x&&a->z < b->z) ? true : false) : false)) return -1;
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->x < a->x) ? true : (b->x == a->x&&b->z < a->z) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortYXZDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->x < a->x) ? true : (b->x == a->x&&b->z < a->z) ? true : false) : false)) return -1;
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->x < b->x) ? true : (a->x == b->x&&a->z < b->z) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortYXZ(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortYXZIncreasing)
    : (CompareSortYXZDecreasing)
    );
}

static int __cdecl CompareSortYZXIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->z < b->z) ? true : (a->z == b->z&&a->x < b->x) ? true : false) : false)) return -1;
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->z < a->z) ? true : (b->z == a->z&&b->x < a->x) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortYZXDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->y < a->y) ? true : ((b->y == a->y) ? ((b->z < a->z) ? true : (b->z == a->z&&b->x < a->x) ? true : false) : false)) return -1;
  if ((a->y < b->y) ? true : ((a->y == b->y) ? ((a->z < b->z) ? true : (a->z == b->z&&a->x < b->x) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortYZX(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortYZXIncreasing)
    : (CompareSortYZXDecreasing)
    );
}

static int __cdecl CompareSortZXYIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->z < b->z) ? true : ((a->z == b->z) ? ((a->x < b->x) ? true : (a->x == b->x&&a->y < b->y) ? true : false) : false)) return -1;
  if ((b->z < a->z) ? true : ((b->z == a->z) ? ((b->x < a->x) ? true : (b->x == a->x&&b->y < a->y) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortZXYDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->z < a->z) ? true : ((b->z == a->z) ? ((b->x < a->x) ? true : (b->x == a->x&&b->y < a->y) ? true : false) : false)) return -1;
  if ((a->z < b->z) ? true : ((a->z == b->z) ? ((a->x < b->x) ? true : (a->x == b->x&&a->y < b->y) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortZXY(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortZXYIncreasing)
    : (CompareSortZXYDecreasing)
    );
}

static int __cdecl CompareSortZYXIncreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((a->z < b->z) ? true : ((a->z == b->z) ? ((a->y < b->y) ? true : (a->y == b->y&&a->x < b->x) ? true : false) : false)) return -1;
  if ((b->z < a->z) ? true : ((b->z == a->z) ? ((b->y < a->y) ? true : (b->y == a->y&&b->x < a->x) ? true : false) : false)) return 1;
  return 0;
}

static int __cdecl CompareSortZYXDecreasing(const void* pa, const void* pb)
{
  AcGePoint3d* a = (AcGePoint3d*)pa;
  AcGePoint3d* b = (AcGePoint3d*)pb;
  ASSERT(a);
  ASSERT(b);
  if ((b->z < a->z) ? true : ((b->z == a->z) ? ((b->y < a->y) ? true : (b->y == a->y&&b->x < a->x) ? true : false) : false)) return -1;
  if ((a->z < b->z) ? true : ((a->z == b->z) ? ((a->y < b->y) ? true : (a->y == b->y&&a->x < b->x) ? true : false) : false)) return 1;
  return 0;
}

void CDos3dPointArray::SortZYX(bool bIncreasing)
{
  if (GetSize() <= 1)
    return;

  qsort(
    GetData(),
    GetSize(),
    sizeof(AcGePoint3d),
    bIncreasing
    ? (CompareSortZYXIncreasing)
    : (CompareSortZYXDecreasing)
    );
}

void CDos3dPointArray::Cull(double tolerance)
{
  if (GetSize() <= 1)
    return;

  if (tolerance < DOS_ZERO_TOLERANCE)
    tolerance = DOS_ZERO_TOLERANCE;

  const int count = (int)GetCount();
  CDos3dPointArray temp;
  temp.Copy(*this);
  RemoveAll();

  int* delete_list = (int*)malloc(count * sizeof(int));
  memset(delete_list, 0, count * sizeof(int));

  int i, j;
  for (i = 0; i < count; i++)
  {
    if (!delete_list[i])
    {
      Add(temp[i]);
      for (j = i + 1; j < count; j++)
      {
        if (delete_list[j])
          continue;
        if (fabs(temp[j].distanceTo(temp[i])) <= tolerance)
          delete_list[j] = true;
      }
    }
  }

  free((void*)delete_list);
}