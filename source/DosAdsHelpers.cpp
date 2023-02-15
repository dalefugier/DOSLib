/////////////////////////////////////////////////////////////////////////////
// DosAdsHelpers.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosAdsHelpers.h"

////////////////////////////////////////////////////////////////
// CAdsResbuf

CAdsResbuf::CAdsResbuf()
{
  m_d = DOS_UNSET_VALUE;
}

CAdsResbuf::CAdsResbuf(double d)
{
  m_d = d;
}

CAdsResbuf::CAdsResbuf(const wchar_t* s)
{
  m_d = DOS_UNSET_VALUE;
  m_s = s;
}

CAdsResbuf::CAdsResbuf(const CAdsResbuf& src)
{
  m_d = src.m_d;
  m_s = src.m_s;
}

CAdsResbuf& CAdsResbuf::operator=(const CAdsResbuf& src)
{
  if (this != &src)
  {
    m_d = src.m_d;
    m_s = src.m_s;
  }
  return *this;
}

////////////////////////////////////////////////////////////////
// CAdsArgs

CAdsArgs::CAdsArgs()
{
  struct resbuf* rb = acedGetArgs();
  m_head = rb;
  m_rb = rb;
}

CAdsArgs::~CAdsArgs()
{
  if (m_head)
  {
    acutRelRb(m_head);
    m_head = NULL;
    m_rb = NULL;
  }
}

void CAdsArgs::Reset()
{
  m_rb = m_head;
}

int CAdsArgs::Count(bool bAll)
{
  int count = 0, lb_count = 0;
  struct resbuf* rb = (bAll) ? m_head : m_rb;
  while (rb)
  {
    if (rb->restype == RTLB)
      lb_count++;
    else if (m_rb->restype == RTLE)
      lb_count--;
    if (0 == lb_count)
      count++;
    rb = rb->rbnext;
  }
  return count;
}

bool CAdsArgs::Next()
{
  bool rc = false;
  if (m_rb && (m_rb = m_rb->rbnext))
    rc = true;
  return rc;
}

bool CAdsArgs::IsEmpty()
{
  bool rc = false;
  if (0 == m_rb)
    rc = true;
  return rc;
}

bool CAdsArgs::IsNumber()
{
  bool rc = false;
  if (m_rb && (m_rb->restype == RTSHORT || m_rb->restype == RTLONG || m_rb->restype == RTREAL))
    rc = true;
  return rc;
}

bool CAdsArgs::IsInteger()
{
  bool rc = false;
  if (m_rb && (m_rb->restype == RTSHORT || m_rb->restype == RTLONG))
    rc = true;
  return rc;
}

bool CAdsArgs::IsDouble()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTREAL)
    rc = true;
  return rc;
}

bool CAdsArgs::IsString()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTSTR)
    rc = true;
  return rc;
}

bool CAdsArgs::IsChar()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTSTR)
  {
    if (wcslen(m_rb->resval.rstring) == 1)
      rc = true;
  }
  return rc;
}

bool CAdsArgs::IsTrue()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTT)
    rc = true;
  return rc;
}

bool CAdsArgs::IsNil()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTNIL)
    rc = true;
  return rc;
}

bool CAdsArgs::IsListBegin()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTLB)
    rc = true;
  return rc;
}

bool CAdsArgs::IsListEnd()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTLE)
    rc = true;
  return rc;
}

bool CAdsArgs::IsDot() const
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTDOTE)
    rc = true;
  return rc;
}

bool CAdsArgs::IsPoint()
{
  return (Is2dPoint() || Is3dPoint());
}

bool CAdsArgs::Is2dPoint()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTPOINT)
    rc = true;
  return rc;
}

bool CAdsArgs::Is3dPoint()
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RT3DPOINT)
      rc = true;
    else if (m_rb->restype >= 10 && m_rb->restype <= 19)
      rc = true;
    else if (m_rb->restype >= 210 && m_rb->restype <= 239)
      rc = true;
  }
  return rc;
}

bool CAdsArgs::GetPoint(ads_point& pt)
{
  bool rc = false;
  memset(&pt, 0, sizeof(ads_point));
  if (m_rb)
  {
    if (m_rb->restype == RTPOINT)
    {
      pt[0] = m_rb->resval.rpoint[0];
      pt[1] = m_rb->resval.rpoint[1];
      pt[2] = 0.0;
      rc = true;
    }
    else if (m_rb->restype == RT3DPOINT)
    {
      pt[0] = m_rb->resval.rpoint[0];
      pt[1] = m_rb->resval.rpoint[1];
      pt[2] = m_rb->resval.rpoint[2];
      rc = true;
    }
    else if (m_rb->restype >= 10 && m_rb->restype <= 19)
    {
      pt[0] = m_rb->restype;
      pt[1] = m_rb->resval.rpoint[0];
      pt[2] = m_rb->resval.rpoint[1];
      rc = true;
    }
    else if (m_rb->restype >= 210 && m_rb->restype <= 239)
    {
      pt[0] = m_rb->restype;
      pt[1] = m_rb->resval.rpoint[0];
      pt[2] = m_rb->resval.rpoint[1];
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::Get2dPointList(CDos2dPointArray& arr)
{

  return false;
}

bool CAdsArgs::Get3dPointList(CDos3dPointArray& arr)
{

  return false;
}

bool CAdsArgs::GetBoolean(bool& b)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTT)
    {
      b = true;
      rc = true;
    }
    else if (m_rb->restype == RTNIL)
    {
      b = false;
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      b = (0 == m_rb->resval.rint) ? false : true;
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      b = (0 == m_rb->resval.rlong) ? false : true;
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      b = (0.0 == m_rb->resval.rreal) ? false : true;
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetBoolean(BOOL& b)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTT)
    {
      b = TRUE;
      rc = true;
    }
    else if (m_rb->restype == RTNIL)
    {
      b = FALSE;
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      b = (0 == m_rb->resval.rint) ? FALSE : TRUE;
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      b = (0 == m_rb->resval.rlong) ? FALSE : TRUE;
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      b = (0.0 == m_rb->resval.rreal) ? FALSE : TRUE;
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetInteger(int& n)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTSHORT)
    {
      n = (int)m_rb->resval.rint;
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      n = (int)m_rb->resval.rlong;
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      n = (int)m_rb->resval.rreal;
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetIntegerList(CArray<int, int>& a)
{
  bool rc = false;
  a.RemoveAll();
  if (IsListBegin())
  {
    while (Next() && !IsListEnd())
    {
      int n = 0;
      if (!GetInteger(n))
      {
        a.RemoveAll();
        return false;
      }
      a.Add(n);
    }
  }
  else if (Is3dPoint() || Is2dPoint())
  {
    ads_point pt;
    if (GetPoint(pt))
    {
      a.Add((int)pt[0]);
      a.Add((int)pt[1]);
      if (Is3dPoint())
        a.Add((int)pt[2]);
    }
  }
  return (a.GetCount() > 0 ? true : false);
}

bool CAdsArgs::GetDouble(double& d)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTREAL)
    {
      d = (double)m_rb->resval.rreal;
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      d = (double)m_rb->resval.rint;
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      d = (double)m_rb->resval.rlong;
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetDoubleList(CArray<double, double>& a)
{
  bool rc = false;
  a.RemoveAll();
  if (IsListBegin())
  {
    while (Next() && !IsListEnd())
    {
      double d = 0.0;
      if (!GetDouble(d))
      {
        a.RemoveAll();
        return false;
      }
      a.Add(d);
    }
  }
  else if (Is3dPoint() || Is2dPoint())
  {
    ads_point pt;
    if (GetPoint(pt))
    {
      a.Add(pt[0]);
      a.Add(pt[1]);
      if (Is3dPoint())
        a.Add(pt[2]);
    }
  }
  return (a.GetCount() > 0 ? true : false);
}

bool CAdsArgs::GetResbuf(CAdsResbuf& res)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTSTR)
    {
      res = CAdsResbuf(m_rb->resval.rstring);
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      res = CAdsResbuf((double)m_rb->resval.rint);
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      res = CAdsResbuf((double)m_rb->resval.rlong);
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      res = CAdsResbuf(m_rb->resval.rreal);
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetResbufList(CArray<CAdsResbuf, CAdsResbuf>& a)
{
  bool rc = false;
  a.RemoveAll();
  if (IsListBegin())
  {
    while (Next() && !IsListBegin() && !IsListEnd())
    {
      double d = 0.0;
      CString s;
      if (GetDouble(d))
        a.Add(CAdsResbuf(d));
      else if (GetString(s))
        a.Add(CAdsResbuf(s));
      else
      {
        a.RemoveAll();
        return false;
      }
    }
  }
  else if (Is3dPoint() || Is2dPoint())
  {
    ads_point pt;
    if (GetPoint(pt))
    {
      a.Add(CAdsResbuf(pt[0]));
      a.Add(CAdsResbuf(pt[1]));
      if (Is3dPoint())
        a.Add(CAdsResbuf(pt[2]));
    }
  }
  return (a.GetCount() > 0 ? true : false);
}

bool CAdsArgs::GetString(CString& s, bool bAllowNil)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTSTR)
    {
      s = m_rb->resval.rstring;
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      s.Format(L"%d", m_rb->resval.rint);
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      s.Format(L"%d", m_rb->resval.rlong);
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      s.Format(L"%g", m_rb->resval.rreal);
      rc = true;
    }
    else if (bAllowNil && m_rb->restype == RTNIL)
    {
      s.Empty();
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetPathString(CString& s, bool bAddBackslash)
{
  bool rc = GetString(s);
  if (rc)
  {
    s.Replace('/', '\\');
    if (bAddBackslash)
    {
      PathAddBackslash(s.GetBuffer(_MAX_PATH));
      s.ReleaseBuffer();
    }
  }
  return rc;
}

bool CAdsArgs::GetDriveString(CString& s, bool bUseCurrentDrive)
{
  TCHAR tch;
  if (GetChar(tch) && _istalpha(tch))
  {
    s.Format(L"%c:", tch);
    s.MakeUpper();
    return true;
  }
  else if (bUseCurrentDrive)
  {
    int d = _getdrive();
    if (d >= 1 && d <= 26)
    {
      s.Format(L"%c:", d + 'A' - 1);
      s.MakeUpper();
      return true;
    }
  }
  return false;
}

bool CAdsArgs::GetDriveNumber(int& n, bool bUseCurrentDrive)
{
  TCHAR tch;
  if (GetChar(tch) && _istalpha(tch))
  {
    n = tch - 'A' + 1;
    return true;
  }
  else if (bUseCurrentDrive)
  {
    int d = _getdrive();
    if (d >= 1 && d <= 26)
    {
      n = d;
      return true;
    }
  }
  return false;
}

bool CAdsArgs::GetChar(TCHAR& c, bool bMakeUpper)
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTSTR)
  {
    CString s = m_rb->resval.rstring;
    if (bMakeUpper)
      s.MakeUpper();
    c = s[0];
    rc = true;
  }
  return rc;
}

bool CAdsArgs::GetStringList(CStringArray& a, bool bAllowNil)
{
  a.RemoveAll();
  if (IsListBegin())
  {
    while (Next() && !IsListEnd())
    {
      CString s;
      if (GetString(s, bAllowNil))
        a.Add(s);
      else
      {
        a.RemoveAll();
        return false;
      }
    }
  }
  return (a.GetCount() > 0) ? true : false;
}

bool CAdsArgs::IsEntity()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTENAME)
    rc = true;
  return rc;
}

bool CAdsArgs::GetEntity(AcDbObjectId& e)
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTENAME)
  {
    if (Acad::eOk == acdbGetObjectId(e, m_rb->resval.rlname))
      rc = true;
  }
  return rc;
}

bool CAdsArgs::IsSelectionSet()
{
  bool rc = false;
  if (m_rb && m_rb->restype == RTPICKS)
    rc = true;
  return rc;
}

bool CAdsArgs::GetEntityList(AcDbObjectIdArray& arr)
{
  arr.removeAll();
  if (m_rb && m_rb->restype == RTPICKS)
  {
#if _ARX_VER > 20 || _ZRX_VER > 20
    Adesk::Int32 sslen = 0;
#else
    long sslen = 0;
#endif
    if (RTNORM == acedSSLength(m_rb->resval.rlname, &sslen))
    {
      arr.setPhysicalLength(sslen);
      long i;
      for (i = 0; i < sslen; i++)
      {
        ads_name ename;
        if (RTNORM == acedSSName(m_rb->resval.rlname, i, ename))
        {
          AcDbObjectId e;
          if (Acad::eOk == acdbGetObjectId(e, ename))
            arr.append(e);
        }
      }
    }
  }
  return (arr.length() > 0 ? true : false);
}

bool CAdsArgs::GetVariant(COleVariant& va)
{
  bool rc = false;
  if (m_rb)
  {
    if (m_rb->restype == RTLB)
    {
      COleSafeArray sa;
      if (GetSafeArray(sa))
      {
        va.Attach(sa.Detach());
        rc = true;
      }
      else
      {
        va.Clear();
        return false;
      }
    }
    else if (m_rb->restype == RTSTR)
    {
      va = m_rb->resval.rstring;
      rc = true;
    }
    else if (m_rb->restype == RTSHORT)
    {
      va = m_rb->resval.rint;
      rc = true;
    }
    else if (m_rb->restype == RTLONG)
    {
      va = (long)m_rb->resval.rlong;
      rc = true;
    }
    else if (m_rb->restype == RTREAL)
    {
      va = m_rb->resval.rreal;
      rc = true;
    }
    else if (m_rb->restype == RTT)
    {
      va.vt = VT_BOOL;
      va.boolVal = VARIANT_TRUE;
      rc = true;
    }
    else if (m_rb->restype == RTNIL)
    {
      va.vt = VT_BOOL;
      va.boolVal = VARIANT_FALSE;
      rc = true;
    }
    else if (m_rb->restype == RTPOINT)
    {
      COleVariant var[2];
      var[0] = m_rb->resval.rpoint[0];
      var[1] = m_rb->resval.rpoint[1];
      COleSafeArray sa;
      sa.CreateOneDim(VT_VARIANT, 2, var);
      va = sa.Detach();
      rc = true;
    }
    else if (m_rb->restype == RT3DPOINT)
    {
      COleVariant var[3];
      var[0] = m_rb->resval.rpoint[0];
      var[1] = m_rb->resval.rpoint[1];
      var[2] = m_rb->resval.rpoint[2];
      COleSafeArray sa;
      sa.CreateOneDim(VT_VARIANT, 3, var);
      va = sa.Detach();
      rc = true;
    }
  }
  return rc;
}

bool CAdsArgs::GetSafeArray(COleSafeArray& sa, bool bAcad)
{
  CArray<COleVariant, COleVariant> arr;

  if (bAcad)
  {
    LPDISPATCH lpApp = acedGetAcadWinApp()->GetIDispatch(TRUE);
    if (0 == lpApp)
      return false;

    COleVariant va;
    va.vt = VT_DISPATCH;
    va.pdispVal = lpApp;
    arr.Add(va);
  }

  bool bSuccess = true;

  if (IsListBegin())
  {
    while (bSuccess && Next() && !IsListEnd())
    {
      COleVariant va;
      if (GetVariant(va))
        arr.Add(va);
      else
        bSuccess = false;
    }
  }
  else
  {
    COleVariant va;
    if (GetVariant(va))
      arr.Add(va);
    else
      bSuccess = false;
  }

  if (bSuccess)
    bSuccess = (arr.GetCount() > 0) ? true : false;

  if (bSuccess)
  {
    DWORD dwCount = (DWORD)arr.GetCount();
    if (dwCount > 0)
    {
      DWORD numElements[1];
      numElements[0] = dwCount;
      sa.Create(VT_VARIANT, 1, numElements);
      long lIndex[1];
      for (DWORD i = 0; i < dwCount; i++)
      {
        lIndex[0] = (long)i;
        sa.PutElement(lIndex, arr[i]);
      }
    }
  }
  else
  {
    int i;
    for (i = 0; i < (int)arr.GetCount(); i++)
      arr[i].Clear();
    arr.RemoveAll();
  }

  return bSuccess;
}

struct resbuf* CAdsArgs::Resbuf()
{
  return m_rb;
}


////////////////////////////////////////////////////////////////
// CAdsRetList

CAdsRetList::CAdsRetList()
{
  m_rbhead = m_rbtail = m_rbnew = NULL;
}

CAdsRetList::~CAdsRetList()
{
  if (m_rbhead)
    acutRelRb(m_rbhead);

  m_rbhead = m_rbtail = m_rbnew = NULL;
}

CAdsRetList::operator const struct resbuf*() const
{
  return m_rbhead;
}

struct resbuf* CAdsRetList::List()
{
  return m_rbhead;
}

bool CAdsRetList::IsEmpty()
{
  if (m_rbhead)
    return false;
  return true;
}

bool CAdsRetList::Add(const wchar_t* string, bool bAllowEmpty)
{
  if (0 == string || 0 == string[0] && !bAllowEmpty)
    return false;

  m_rbnew = acutBuildList(RTSTR, string, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

static struct resbuf* DOS_BuildList(double d)
{
  if (_isnan(d) || d == DOS_UNSET_VALUE)
    return 0;

  if (INT_MIN <= d && d <= INT_MAX)
  {
    int n = (int)d;
    if (fabs(d - n) < DOS_ZERO_TOLERANCE)
    {
      if (SHRT_MIN <= n && n <= SHRT_MAX)
        return acutBuildList(RTSHORT, (short)n, 0);
      else
        return acutBuildList(RTLONG, (long)n, 0);
    }
  }

  return acutBuildList(RTREAL, d, 0);
}

bool CAdsRetList::Add(double d)
{
  m_rbnew = DOS_BuildList(d);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::Add(const double* data, int count)
{
  ASSERT(data);
  int num_added = 0;
  for (int i = 0; i < count; i++)
  {
    if (Add(data[i]))
      num_added++;
    else
      break;
  }
  return (num_added == count);
}

bool CAdsRetList::Add(int n)
{
  m_rbnew = acutBuildList(RTLONG, (long)n, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::Add(long l)
{
  m_rbnew = acutBuildList(RTLONG, l, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::Add(short i)
{
  m_rbnew = acutBuildList(RTSHORT, i, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::Add(bool b)
{
  m_rbnew = acutBuildList(b ? RTT : RTNIL, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::AddLB()
{
  m_rbnew = acutBuildList(RTLB, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::AddLE()
{
  m_rbnew = acutBuildList(RTLE, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

bool CAdsRetList::AddDot()
{
  m_rbnew = acutBuildList(RTDOTE, 0);
  if (!m_rbnew)
  {
    DOS_PrintError(L"cannot allocate result buffer.");
    return false;
  }

  AddToTail();
  return true;
}

void CAdsRetList::AddToTail()
{
  if (m_rbhead)
  {
    m_rbtail->rbnext = m_rbnew;
    m_rbtail = m_rbnew;
  }
  else
  {
    m_rbhead = m_rbtail = m_rbnew;
  }
}

bool CAdsRetList::IsArray(const VARIANT& va)
{
  if (va.vt == VT_ERROR && va.scode == DISP_E_PARAMNOTFOUND)
    return false;

  const VARIANT* pva = &va;
  while (pva->vt == (VT_BYREF | VT_VARIANT))
    pva = pva->pvarVal;

  SAFEARRAY* psa = 0;
  if (pva->vt & VT_ARRAY)
  {
    if (pva->vt & VT_BYREF)
      psa = *pva->pparray;
    else
      psa = pva->parray;
  }

  return (psa) ? true : false;
}

bool CAdsRetList::SafeArrayHelper(SAFEARRAY* psa)
{
  if (0 == psa)
    return false;

  long i, lower, upper;
  HRESULT hl = SafeArrayGetLBound(psa, 1, &lower);
  HRESULT hu = SafeArrayGetUBound(psa, 1, &upper);

  bool rc = false;
  if (SUCCEEDED(hl) && SUCCEEDED(hu))
  {
    VARTYPE vt = 0;
    HRESULT hr = SafeArrayGetVartype(psa, &vt);
    if (SUCCEEDED(hr))
    {
      if (vt == VT_BOOL)
      {
        VARIANT_BOOL* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
            Add(pvData[i] == VARIANT_TRUE ? true : false);
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
      else if (vt == VT_I2)
      {
        short* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
            Add(pvData[i]);
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
      else if (vt == VT_I4)
      {
        long* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
            Add(pvData[i]);
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
      else if (vt == VT_R4)
      {
        float* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
            Add((double)pvData[i]);
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
      else if (vt == VT_R8)
      {
        double* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
            Add(pvData[i]);
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
      else if (vt == VT_BSTR)
      {
        BSTR* pvData = 0;
        hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
        if (SUCCEEDED(hr) && pvData)
        {
          AddLB();
          for (i = lower; i <= upper; i++)
          {
            CString str(pvData[i]);
            Add(str, true);
          }
          AddLE();
          SafeArrayUnaccessData(psa);
          rc = true;
        }
      }
    }
  }

  return rc;
}

bool CAdsRetList::VariantArrayHelper(SAFEARRAY* psa)
{
  if (0 == psa)
    return false;

  long i, lower, upper;
  HRESULT hl = SafeArrayGetLBound(psa, 1, &lower);
  HRESULT hu = SafeArrayGetUBound(psa, 1, &upper);
  if (SUCCEEDED(hl) && SUCCEEDED(hu))
  {
    VARIANT* pvData = 0;
    HRESULT hr = SafeArrayAccessData(psa, (void HUGEP**)&pvData);
    if (SUCCEEDED(hr) && pvData)
    {
      AddLB();
      for (i = lower; i <= upper; i++)
        Add(pvData[i]);
      AddLE();
      SafeArrayUnaccessData(psa);
      return true;
    }
  }

  return false;
}

bool CAdsRetList::Add(const VARIANT& va)
{
  if (va.vt == VT_ERROR && va.scode == DISP_E_PARAMNOTFOUND)
    return false;

  const VARIANT* pva = &va;
  while (pva->vt == (VT_BYREF | VT_VARIANT))
    pva = pva->pvarVal;

  SAFEARRAY* psa = 0;
  if (pva->vt & VT_ARRAY)
  {
    if (pva->vt & VT_BYREF)
      psa = *pva->pparray;
    else
      psa = pva->parray;
  }

  if (psa)
  {
    if (1 == SafeArrayGetDim(psa))
    {
      if (psa->fFeatures & FADF_VARIANT)
        return VariantArrayHelper(psa);
      else
        return SafeArrayHelper(psa);
    }
    else
      return false;
  }

  switch (pva->vt)
  {
  case VT_BOOL:
    Add(pva->boolVal == VARIANT_TRUE ? true : false);
    break;
  case VT_I2:
    Add(pva->iVal);
    break;
  case VT_I4:
    Add(pva->lVal);
    break;
  case VT_R4:
    Add((double)pva->fltVal);
    break;
  case VT_R8:
    Add(pva->dblVal);
    break;
  case VT_BSTR:
  {
    CString str(pva->bstrVal);
    Add(str, true);
  }
  break;
  default:
    Add(false);
    break;
  }

  return true;
}

////////////////////////////////////////////////////////////////
// CAdsWorkingFolder
CAdsWorkingFolder::CAdsWorkingFolder(const wchar_t* pszFolder)
{
  m_strSavedFolder.Empty();
  if (pszFolder && pszFolder[0])
  {
    if (!PathIsDirectory(pszFolder))
    {
      CDosPathString strTemp(pszFolder);
      strTemp.AddBackslash();
      SHCreateDirectoryEx(0, strTemp, 0);
    }

    if (PathIsDirectory(pszFolder))
    {
      wchar_t szSavedFolder[_MAX_PATH];
      if (GetCurrentDirectory(_MAX_PATH, szSavedFolder))
      {
        if (SetCurrentDirectory(pszFolder))
          m_strSavedFolder = szSavedFolder;
      }
    }
  }
}

CAdsWorkingFolder::~CAdsWorkingFolder()
{
  if (!m_strSavedFolder.IsEmpty())
    SetCurrentDirectory(m_strSavedFolder);
}

////////////////////////////////////////////////////////////////

const wchar_t* DOS_PlatformName()
{
#if defined(_BRX)
  return L"BricsCAD";
#elif defined(_ZRX)
  return L"ZWCAD";
#else
  return L"AutoCAD";
#endif
}

int DOS_FindFile(const wchar_t* fname, wchar_t* result, size_t nBufLen)
{
#if _ARX_VER > 19 ||_BRX_VER > 16
  return acedFindFile(fname, result, nBufLen);
#else
  return acedFindFile(fname, result);
#endif
}

void DOS_PrintError(const wchar_t* string)
{
  CString str;
  if (string && string[0])
    str.Format(L"DOSLib Error: %s\n", string);
  else
    str = L"DOSLib Error: unknown error.\n";
  acutPrintf(str);
}

void DOS_PrintLastError()
{
  DWORD dwError = ::GetLastError();
  if (ERROR_SUCCESS != dwError)
  {
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
    wchar_t* pszBuffer = 0;
    if (::FormatMessage(dwFlags, 0, dwError, dwLanguageId, (wchar_t*)&pszBuffer, 0, 0))
    {
      DOS_PrintError(pszBuffer);
      ::LocalFree(pszBuffer);
    }
  }
}

bool DOS_IntegerParse(const wchar_t* string, int& rc)
{
  if (0 == string || 0 == string[0])
    return false;

  int n = 0;
  if (1 != _snwscanf_s(string, wcslen(string), L"%ld", &n))
    return false;

  rc = n;
  return true;
}

bool DOS_SimpleIntegerParse(const wchar_t* string, int& rc)
{
  if (0 == string || 0 == string[0])
    return false;

  while (*string == ' ' || *string == '\t')
    string++;

  errno = 0;
  wchar_t* endptr = 0;
  long l = wcstol(string, &endptr, 10);
  if (string == endptr || errno == ERANGE)
    return false;

  if (endptr)
  {
    while (*endptr == ' ' || *endptr == '\t')
      endptr++;

    if (*endptr != '\0')
      return false;   // not terminated properly
  }

  rc = (int)l;
  return true;
}

bool DOS_FloatParse(const wchar_t* string, double& rc)
{
  if (0 == string || 0 == string[0])
    return false;

  double d = DOS_UNSET_VALUE;
  if (1 != _snwscanf_s(string, wcslen(string), L"%lf", &d) || d == DOS_UNSET_VALUE)
    return false;

  rc = d;
  return true;
}

bool DOS_SimpleFloatParse(const wchar_t* string, double& rc)
{
  if (0 == string || 0 == string[0])
    return false;

  while (*string == ' ' || *string == '\t')
    string++;

  errno = 0;
  wchar_t* endptr = 0;
  double d = wcstod(string, &endptr);
  if (string == endptr || errno == ERANGE)
    return false;

  if (endptr)
  {
    while (*endptr == ' ' || *endptr == '\t')
      endptr++;

    if (*endptr != '\0')
      return false;   // not terminated properly
  }

  rc = d;
  return true;
}

int DOS_StringParse(const wchar_t* string, wchar_t delimiter, CStringArray& tokens)
{
  if (0 == string || 0 == string[0])
    return 0;

  CString line(string);
  const int line_length = line.GetLength();

  int line_pos = 0;
  wchar_t ch = 0;
  CString str;
  bool bQuotes = false;

  while (line[line_pos] != 0 && line_pos < line_length)
  {
    ch = line[line_pos];

    if (!bQuotes && str.GetLength() == 0 && ch == '"')
    {
      // begin double quote
      bQuotes = true;
    }

    else if (bQuotes && ch == '"')
    {
      // double quote char
      if ((line_pos + 1 < line_length) && (line[line_pos + 1] == '"'))
      {
        // encountered 2 double quotes in a row (resolves to 1 double quote)
        str += ch;
        line_pos++;
      }
      else
      {
        // end double quote
        bQuotes = false;
      }
    }

    else if (!bQuotes && ch == delimiter)
    {
      // end of field
      tokens.Add(str);
      str.Empty();
    }

    else if (!bQuotes && (ch == '\r' || ch == '\n'))
    {
      // end of line
      tokens.Add(str);
      return (int)tokens.GetCount();;
    }

    else
    {
      str += ch;
    }

    line_pos++;
  }

  tokens.Add(str);

  return (int)tokens.GetCount();
}

int acedRetNum(double d)
{
  if (_isnan(d) || d == DOS_UNSET_VALUE)
  {
    acedRetNil();
    return RTNORM;
  }

  if (SHRT_MIN <= d && d <= SHRT_MAX)
  {
    int n = (int)d;
    if (fabs(d - n) < DOS_ZERO_TOLERANCE)
    {
      acedRetInt(n);
      return RTNORM;
    }
  }

  acedRetReal(d);
  return RTNORM;
}

/////////////////////////////////////////////////////////////////////////////

static DWORD CalculateColor(DWORD backgroundColor, DWORD crosshairColor)
{
  int r = GetRValue(backgroundColor);
  int g = GetGValue(backgroundColor);
  int b = GetBValue(backgroundColor);

  if (r == 0)
    r = 1;
  else if (r == 255)
    r = 254;
  else
    r++;

  return RGB(r, g, b);
}

CDosHideCrosshairs::CDosHideCrosshairs()
{
  AcColorSettings settings;
  memset(&settings, 0, sizeof(AcColorSettings));
  acedGetCurrentColors(&settings);

  // Save the original crosshair colors
  m_dwModelCrossHairColor = settings.dwModelCrossHairColor;
  m_dwLayoutCrossHairColor = settings.dwLayoutCrossHairColor;
  m_dwParallelCrossHairColor = settings.dwParallelCrossHairColor;
  m_dwPerspectiveCrossHairColor = settings.dwPerspectiveCrossHairColor;
  m_dwBEditCrossHairColor = settings.dwBEditCrossHairColor;

  // Cook up some new crosshair colors
  settings.dwModelCrossHairColor = CalculateColor(settings.dwGfxModelBkColor, settings.dwModelCrossHairColor);
  settings.dwLayoutCrossHairColor = CalculateColor(settings.dwGfxLayoutBkColor, settings.dwLayoutCrossHairColor);
  settings.dwParallelCrossHairColor = CalculateColor(settings.dwParallelBkColor, settings.dwParallelCrossHairColor);
  settings.dwPerspectiveCrossHairColor = CalculateColor(settings.dwParallelBkColor, settings.dwPerspectiveCrossHairColor);
  settings.dwBEditCrossHairColor = CalculateColor(settings.dwBEditBkColor, settings.dwBEditCrossHairColor);

  acedSetCurrentColors(&settings);

  acedGetAcadFrame()->SetFocus();
  acedGetAcadDwgView()->SendMessage(WM_MOUSEMOVE, 0, 0);
  DOS_Sleep(100);
}

CDosHideCrosshairs::~CDosHideCrosshairs()
{
  AcColorSettings settings;
  memset(&settings, 0, sizeof(AcColorSettings));
  acedGetCurrentColors(&settings);

  // Reset the original crosshair colors
  settings.dwModelCrossHairColor = m_dwModelCrossHairColor;
  settings.dwLayoutCrossHairColor = m_dwLayoutCrossHairColor;
  settings.dwParallelCrossHairColor = m_dwParallelCrossHairColor;
  settings.dwPerspectiveCrossHairColor = m_dwPerspectiveCrossHairColor;
  settings.dwBEditCrossHairColor = m_dwBEditCrossHairColor;

  acedSetCurrentColors(&settings);

  acedGetAcadFrame()->SetFocus();
  acedGetAcadDwgView()->SendMessage(WM_MOUSEMOVE, 0, 0);
  DOS_Sleep(100);
}

void DOS_Pump()
{
  CWinApp* pApp = acedGetAcadWinApp();
  if (pApp)
  {
    CWnd* pWnd = pApp->GetMainWnd();
    if (pWnd)
    {
      MSG msg;
      while (::PeekMessage(&msg, pWnd->GetSafeHwnd(), 0, 0, PM_NOREMOVE))
      {
        if (!pApp->PumpMessage())
        {
          ::PostQuitMessage(0);
          break;
        }
      }
      LONG lIdle = 0;
      while (pApp->OnIdle(lIdle++));
    }
  }
}

void DOS_Sleep(DWORD dwMilliseconds)
{
  CWinApp* pApp = acedGetAcadWinApp();
  if (pApp)
  {
    CWnd* pWnd = pApp->GetMainWnd();
    if (pWnd)
    {
      DWORD dwStart = ::GetTickCount();
      do
      {
        MSG msg;
        while (::PeekMessage(&msg, pWnd->m_hWnd, 0, 0, PM_NOREMOVE))
        {
          if (!pApp->PumpMessage())
          {
            ::PostQuitMessage(0);
            return;
          }
        }
        LONG lIdle = 0;
        while (pApp->OnIdle(lIdle++));
      } while ((::GetTickCount() - dwStart) < dwMilliseconds);
    }
  }
}

CString DOS_ReplaceNoCase(const wchar_t* instr, const wchar_t* oldstr, const wchar_t* newstr)
{
  CString output(instr);
  CString input_lower(instr);
  CString oldone_lower(oldstr);
  input_lower.MakeLower();
  oldone_lower.MakeLower();

  int pos = 0;
  while ((pos = input_lower.Find(oldone_lower, pos)) != -1)
  {
    input_lower.Delete(pos, lstrlen(oldstr));
    input_lower.Insert(pos, newstr);
    output.Delete(pos, lstrlen(oldstr));
    output.Insert(pos, newstr);
  }

  return output;
}


enum
{
  ISVALID_FILENAME_ERROR = -1,
  INVALID_FILENAME_CLOCK = -2,
  INVALID_FILENAME_AUX = -3,
  INVALID_FILENAME_CON = -4,
  INVALID_FILENAME_NUL = -5,
  INVALID_FILENAME_PRN = -6,
  INVALID_FILENAME_COM1 = -7,
  INVALID_FILENAME_COM2 = -8,
  INVALID_FILENAME_COM3 = -9,
  INVALID_FILENAME_COM4 = -10,
  INVALID_FILENAME_COM5 = -11,
  INVALID_FILENAME_COM6 = -12,
  INVALID_FILENAME_COM7 = -13,
  INVALID_FILENAME_COM8 = -14,
  INVALID_FILENAME_COM9 = -15,
  INVALID_FILENAME_LPT1 = -16,
  INVALID_FILENAME_LPT2 = -17,
  INVALID_FILENAME_LPT3 = -18,
  INVALID_FILENAME_LPT4 = -19,
  INVALID_FILENAME_LPT5 = -20,
  INVALID_FILENAME_LPT6 = -21,
  INVALID_FILENAME_LPT7 = -22,
  INVALID_FILENAME_LPT8 = -23,
  INVALID_FILENAME_LPT9 = -24
};

int DOS_IsValidFileName(const wchar_t* pszFileName)
{
  if (0 == pszFileName || 0 == pszFileName[0])
    return ISVALID_FILENAME_ERROR;

  int nonDot = -1;	// position of the first non dot in the file name
  int dot = -1;		// position of the first dot in the file name
  int len = 0;		// length of the file name
  for (; len < 256 && pszFileName[len]; len++)
  {
    if (pszFileName[len] == '.')
    {
      if (dot < 0)
        dot = len;
      continue;
    }
    else if (nonDot < 0)
      nonDot = len;

    // The upper characters can be passed with a single check and
    // since only the backslash and bar are above the ampersand
    // it saves memory to do the check this way with little performance
    // cost.
    if (pszFileName[len] >= '@')
    {
      if (pszFileName[len] == '\\' || pszFileName[len] == '|')
        return pszFileName[len];

      continue;
    }

    static bool isCharValid[32] =
    {
      //  ' '   !     "      #     $     %     &     '     (     )     *      +     ,      -     .      / 
        true, true, false, true, true, true, true, true, true, true, false, true, true,  true, true,  false,
        //  0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
          true, true, true,  true, true, true, true, true, true, true, false, true, false, true, false, false
      //  0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
    };

    // This is faster, at the expense of memory, than checking each
    // invalid character individually. However, either method is much
    // faster than using strchr().
    if (pszFileName[len] >= 32)
    {
      if (isCharValid[pszFileName[len] - 32])
        continue;
    }
    return pszFileName[len];
  }

  if (len == 256)
    return ISVALID_FILENAME_ERROR;

  // if nonDot is still -1, no non-dots were encountered, return a dot (period)
  if (nonDot < 0)
    return '.';

  // if the first character is a dot, the filename is okay
  if (dot == 0)
    return 0;

  // if the file name has a dot, we only need to check up to the first dot
  if (dot > 0)
    len = dot;

  // Since the device names aren't numerous, this method of checking is the
  // fastest. Note that each character is checked with both cases.
  if (len == 3)
  {
    if (pszFileName[0] == 'a' || pszFileName[0] == 'A')
    {
      if ((pszFileName[1] == 'u' || pszFileName[1] == 'U') &&
        (pszFileName[2] == 'x' || pszFileName[2] == 'X'))
        return INVALID_FILENAME_AUX;
    }
    else if (pszFileName[0] == 'c' || pszFileName[0] == 'C')
    {
      if ((pszFileName[1] == 'o' || pszFileName[1] == 'O') &&
        (pszFileName[2] == 'n' || pszFileName[2] == 'N'))
        return INVALID_FILENAME_CON;
    }
    else if (pszFileName[0] == 'n' || pszFileName[0] == 'N')
    {
      if ((pszFileName[1] == 'u' || pszFileName[1] == 'U') &&
        (pszFileName[2] == 'l' || pszFileName[2] == 'L'))
        return INVALID_FILENAME_NUL;
    }
    else if (pszFileName[0] == 'p' || pszFileName[0] == 'P')
    {
      if ((pszFileName[1] == 'r' || pszFileName[1] == 'R') &&
        (pszFileName[2] == 'n' || pszFileName[2] == 'N'))
        return INVALID_FILENAME_PRN;
    }
  }
  else if (len == 4)
  {
    if (pszFileName[0] == 'c' || pszFileName[0] == 'C')
    {
      if ((pszFileName[1] == 'o' || pszFileName[1] == 'O') &&
        (pszFileName[2] == 'm' || pszFileName[2] == 'M') &&
        (pszFileName[3] >= '1' || pszFileName[3] <= '9'))
        return INVALID_FILENAME_COM1 - (pszFileName[3] - '1');
    }
    else if (pszFileName[0] == 'l' || pszFileName[0] == 'L')
    {
      if ((pszFileName[1] == 'p' || pszFileName[1] == 'P') &&
        (pszFileName[2] == 't' || pszFileName[2] == 'T') &&
        (pszFileName[3] >= '1' || pszFileName[3] <= '9'))
        return INVALID_FILENAME_LPT1 - (pszFileName[3] - '1');
    }
  }
  else if (len == 6)
  {
    if ((pszFileName[0] == 'c' || pszFileName[0] == 'C') &&
      (pszFileName[1] == 'l' || pszFileName[1] == 'L') &&
      (pszFileName[2] == 'o' || pszFileName[2] == 'O') &&
      (pszFileName[3] == 'c' || pszFileName[3] == 'C') &&
      (pszFileName[4] == 'k' || pszFileName[4] == 'K') &&
      (pszFileName[5] == '$' || pszFileName[5] == '$'))
      return INVALID_FILENAME_CLOCK;
  }

  return 0;
}

const wchar_t* g_pInvalidFileNameErrStr[] =
{
  L"Error",
  L"CLOCK$",
  L"AUX",
  L"CON",
  L"NUL",
  L"PRN",
  L"COM1",
  L"COM2",
  L"COM3",
  L"COM4",
  L"COM5",
  L"COM6",
  L"COM7",
  L"COM8",
  L"COM9",
  L"LPT1",
  L"LPT2",
  L"LPT3",
  L"LPT4",
  L"LPT5",
  L"LPT6",
  L"LPT7",
  L"LPT8",
  L"LPT9",
  NULL
};

///////////////////////////////////////////////////////////////////////////
// const TCHAR *GetIsValidFileNameErrStr(int err)
//
//	Return an string associated with the passed error code
//
// Parameters
//
//	err   A negative error number returned by IsValidFileName
//
// Returns
//
//	A pointer to the device string
//
//	NULL if err is not in the range of the INVALID_FILENAME_ enumeration.
//

const wchar_t* DOS_GetIsValidFileNameErrStr(int err)
{
  if (err >= 0 || err < INVALID_FILENAME_LPT9)
    return g_pInvalidFileNameErrStr[0];

  return g_pInvalidFileNameErrStr[(-err) - 1];
}