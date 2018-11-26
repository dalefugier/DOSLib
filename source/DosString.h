/////////////////////////////////////////////////////////////////////////////
// DosString.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDosStringW
// Converts both 8-bit and 16-bit strings to 16-bit wideness

class CDosStringW
{
public:
  CDosStringW(const char*);
  CDosStringW(const wchar_t*);
  ~CDosStringW();

  operator const wchar_t*() const;

private:
  CDosStringW(const CDosStringW&);
  CDosStringW& operator=(const CDosStringW&);

private:
  wchar_t* m_sz;
  bool m_bIsForeign;
};


/////////////////////////////////////////////////////////////////////////////
// CDosStringA
// Converts both 8-bit and 16-bit strings to 8-bit wideness

class CDosStringA
{
public:
  CDosStringA(const char*);
  CDosStringA(const wchar_t*);
  ~CDosStringA();

  operator const char*() const;

private:
  CDosStringA(const CDosStringA&);
  CDosStringA& operator=(const CDosStringA&);

private:
  char* m_sz;
  bool m_bIsForeign;
};

/////////////////////////////////////////////////////////////////////////////
// Conditional Typedefs for Win32 and OLE Text Data Types

// typedef OLESTRCVAR to emulate the OLESTR 
// macro (converts any string at runtime instead 
// of simply changing layout of string literal at
// compile-time).

#ifdef OLE2ANSI
typedef CDosStringA OLESTRCVAR;
#else
typedef CDosStringW OLESTRCVAR;
#endif

// typedef __TEXTCVAR to emulate the __TEXT
// macro (converts any string at runtime instead 
// of simply changing layout of string literal at
// compile-time).

#ifdef UNICODE
typedef CDosStringW __TEXTCVAR;
#else
typedef CDosStringA __TEXTCVAR;
#endif
