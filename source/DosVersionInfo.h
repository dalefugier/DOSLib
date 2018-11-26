/////////////////////////////////////////////////////////////////////////////
// DosVersionInfo.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosVersionInfo : public VS_FIXEDFILEINFO
{
public:
  CDosVersionInfo();
  virtual ~CDosVersionInfo();

  bool GetFileVersionInfo(const wchar_t* pszModuleName);
  CString GetValue(const wchar_t* pszKeyName);

protected:
  BYTE* m_pVersionInfo;	// all version info

  struct TRANSLATION
  {
    WORD langID;			// language ID
    WORD charset;			// character set (code page)
  } m_translation;
};
