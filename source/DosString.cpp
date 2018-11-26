/////////////////////////////////////////////////////////////////////////////
// DosString.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosString.h"

/////////////////////////////////////////////////////////////////////////////
// CDosStringW

CDosStringW::CDosStringW(const wchar_t* p)
  : m_sz((wchar_t*)p)
  , m_bIsForeign(false)
{
}

CDosStringW::operator const wchar_t*() const
{
  return m_sz;
}

CDosStringW::CDosStringW(const char* p)
  : m_bIsForeign(true)
{
  size_t len = strlen(p);
  size_t size = mbstowcs(0, p, len) + 1;
  if (m_sz = new wchar_t[size])
    mbstowcs(m_sz, p, size);
}

CDosStringW::~CDosStringW()
{
  if (m_bIsForeign)
    delete[] m_sz;
}

/////////////////////////////////////////////////////////////////////////////
// CDosStringA

CDosStringA::CDosStringA(const char* p)
  : m_sz((char*)p)
  , m_bIsForeign(false)
{
}

CDosStringA::operator const char*() const
{
  return m_sz;
}

CDosStringA::CDosStringA(const wchar_t* p)
  : m_bIsForeign(true)
{
  size_t len = wcslen(p);
  size_t size = wcstombs(0, p, len) + 1;
  if (m_sz = new char[size])
    wcstombs(m_sz, p, size);
}

CDosStringA::~CDosStringA()
{
  if (m_bIsForeign)
    delete[] m_sz;
}
