/////////////////////////////////////////////////////////////////////////////
// DosVersionInfo.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosVersionInfo.h"

CDosVersionInfo::CDosVersionInfo()
  : m_pVersionInfo(0)
{
}

CDosVersionInfo::~CDosVersionInfo()
{
  if (m_pVersionInfo)
    delete[] m_pVersionInfo;
}

bool CDosVersionInfo::GetFileVersionInfo(const wchar_t* pszModuleName)
{
  if (0 == pszModuleName)
    return false;

  m_translation.charset = 1252;		// default = ANSI code page
  memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));

  HMODULE hModule = ::GetModuleHandle(pszModuleName);
  if (0 == hModule)
    return false;

  wchar_t szFilename[_MAX_PATH];
  DWORD dwLength = ::GetModuleFileName(hModule, szFilename, DOS_ArrayCount(szFilename));
  if (0 == dwLength)
    return false;

  DWORD dwDummyHandle = 0;
  dwLength = ::GetFileVersionInfoSize(szFilename, &dwDummyHandle);
  if (0 == dwLength)
    return false;

  if (m_pVersionInfo)
  {
    delete[] m_pVersionInfo;
    m_pVersionInfo = 0;
  }

  m_pVersionInfo = new BYTE[dwLength];
  if (!::GetFileVersionInfo(szFilename, 0, dwLength, m_pVersionInfo))
    return false;

  LPVOID lpBuffer = 0;
  UINT uLength = 0;
  if (!::VerQueryValue(m_pVersionInfo, L"\\", &lpBuffer, &uLength))
    return false;

  *(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpBuffer;

  if (VerQueryValue(m_pVersionInfo, L"\\VarFileInfo\\Translation", &lpBuffer, &uLength) && uLength >= 4)
    m_translation = *(TRANSLATION*)lpBuffer;

  return (dwSignature == VS_FFI_SIGNATURE);
}

CString CDosVersionInfo::GetValue(const wchar_t* pszKeyName)
{
  CString str;
  if (m_pVersionInfo && pszKeyName)
  {
    CString query;
    query.Format(L"\\StringFileInfo\\%04x%04x\\%s", m_translation.langID, m_translation.charset, pszKeyName);
    const wchar_t* pszValue = 0;
    UINT uLength = 0;
    if (VerQueryValue(m_pVersionInfo, (wchar_t*)(const wchar_t*)query, (LPVOID*)&pszValue, &uLength))
      str = pszValue;
  }
  return str;
}
