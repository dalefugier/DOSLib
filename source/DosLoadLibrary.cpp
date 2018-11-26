/////////////////////////////////////////////////////////////////////////////
// DosLoadLibrary.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosLoadLibrary.h"

CDosLoadLibrary::CDosLoadLibrary()
  : m_hInst(0)
{
}

CDosLoadLibrary::CDosLoadLibrary(const wchar_t* pszLibName)
  : m_hInst(0)
{
  Load(pszLibName);
}

CDosLoadLibrary::~CDosLoadLibrary()
{
  Free();
}

CDosLoadLibrary::operator HINSTANCE() const
{
  return m_hInst;
}

HINSTANCE CDosLoadLibrary::Detach()
{
  HINSTANCE hInst = m_hInst;
  m_hInst = 0;
  return hInst;
}

bool CDosLoadLibrary::Load(const wchar_t* pszLibName, DWORD dwFlags)
{
  Free();
  if (pszLibName && pszLibName[0])
    m_hInst = ::LoadLibraryEx(pszLibName, 0, dwFlags);
  return IsLoaded();
}

void CDosLoadLibrary::Free()
{
  if (IsLoaded())
  {
    ::FreeLibrary(m_hInst);
    m_hInst = 0;
  }
}

bool CDosLoadLibrary::IsLoaded() const
{
  return (m_hInst != 0);
}

FARPROC CDosLoadLibrary::ProcAddress(const char* pszFuncName) const
{
  if (IsLoaded())
  {
    if (pszFuncName && pszFuncName[0])
      return ::GetProcAddress(m_hInst, pszFuncName);
  }
  return 0;
}

bool CDosLoadLibrary::ModuleFileName(CString& filename) const
{
  bool rc = false;
  if (IsLoaded())
  {
    wchar_t szFileName[MAX_PATH];
    if (::GetModuleFileName(m_hInst, szFileName, MAX_PATH))
    {
      filename = szFileName;
      rc = true;
    }
  }
  return rc;
}

bool CDosLoadLibrary::IsDataFile() const
{
  if (IsLoaded())
    return (((ULONG_PTR)(m_hInst)) & (ULONG_PTR)1) ? true : false;
  return false;
}

bool CDosLoadLibrary::IsImageMapping() const
{
  if (IsLoaded())
    return (((ULONG_PTR)(m_hInst)) & (ULONG_PTR)2) ? true : false;
  return false;
}

bool CDosLoadLibrary::IsResource() const
{
  if (IsLoaded())
    return (IsDataFile() || IsImageMapping()) ? true : false;
  return false;
}
