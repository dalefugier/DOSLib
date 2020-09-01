///////////////////////////////////////////////////////////////////////////////
// func_config.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"

static int EnumerateProfileSectionKeys(const wchar_t* pszFileName, const wchar_t* pszSection)
{
  CAdsRetList result;

  wchar_t szBuffer[32767];
  if (GetPrivateProfileString(pszSection, 0, L"\0", szBuffer, sizeof(szBuffer) / sizeof(wchar_t), pszFileName))
  {
    wchar_t* psz = szBuffer;
    while (*psz)
    {
      result.Add(psz);
      psz += (wcslen(psz) + 1);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

static int EnumerateProfileSectionNames(const wchar_t* pszFileName)
{
  CAdsRetList result;

  wchar_t szBuffer[32767];
  if (GetPrivateProfileSectionNames(szBuffer, sizeof(szBuffer) / sizeof(wchar_t), pszFileName))
  {
    wchar_t* psz = szBuffer;
    while (*psz)
    {
      result.Add(psz);
      psz += (wcslen(psz) + 1);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

///////////////////////////////////////////////////////////////////////////////
// dos_getini
int CDOSLibApp::ads_dos_getini()
{
  CAdsArgs args;

  CString strSection, strKey;
  CDosPathString strFile;
  CString strDefault, strString;

  BOOL bSection = TRUE;
  BOOL bKey = TRUE;
  BOOL bDefault = FALSE;

  // Get section
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strSection))
      return RSERR;
    bSection = TRUE;
  }
  else if (args.IsNil())
  {
    bSection = FALSE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid section argument.\n");
    return RSERR;
  }

  args.Next();

  // Get key
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strKey))
      return RSERR;
    bKey = TRUE;
  }
  else if (args.IsNil())
  {
    bKey = FALSE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid key argument.\n");
    return RSERR;
  }

  args.Next();

  // Filename
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetPathString(strFile))
      return RSERR;
    strFile.FullPath();
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid file argument.\n");
    return RSERR;
  }

  args.Next();

  // Default
  if (args.IsEmpty())
  {
    strDefault = L"NIL";
    bDefault = FALSE;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strDefault))
      return RSERR;
    bDefault = TRUE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid default argument.\n");
    return RSERR;
  }

  // Do it...
  if (bSection == TRUE)
  {
    if (bKey == TRUE)
    {
      DWORD dwResult = GetPrivateProfileString(strSection, strKey, strDefault, strString.GetBuffer(4096), 4096, strFile);
      strString.ReleaseBuffer();

      if (dwResult > 0)
      {
        if (strString.CompareNoCase(L"NIL") == 0 && bDefault == FALSE)
          acedRetNil();
        else
          acedRetStr(strString);
      }
      else
      {
        acedRetNil();
      }
    }
    else
    {
      return EnumerateProfileSectionKeys(strFile, strSection);
    }
  }
  else
  {
    return EnumerateProfileSectionNames(strFile);
  }

  return RSRSLT;
}

///////////////////////////////////////////////////////////////////////////////
// dos_setini

// According to MSDN, if the file was created using Unicode characters, 
// the WritePrivateProfileString function writes Unicode characters to the
// file. Otherwise, the function writes ANSI characters. Thus, if the file
// does not exist, then create a two-byte Unicode file so Unicode strings
// are written properly.
static bool CreateUnicodeIniFile(const wchar_t* pszFileName)
{
  if (0 == pszFileName || 0 == pszFileName[0])
    return false; // validate filename

  if (PathFileExists(pszFileName))
    return true;  // if already exists, just return true

  bool rc = false;
  FILE* fp = 0;
  errno_t err = _wfopen_s(&fp, pszFileName, L"wb");
  if (0 == err || 0 != fp)
  {
    unsigned char bom[] = { 0xFF, 0xFE }; // UTF-16 BOM
    if (2 == fwrite(bom, 1, 2, fp))
    {
      fflush(fp);
      rc = true;
    }
    fclose(fp);
  }
  return rc;
}

int CDOSLibApp::ads_dos_setini()
{
  CAdsArgs args;

  CString strSection, strKey, strString, strBuffer;
  CDosPathString strFile;
  BOOL bSection = TRUE;
  BOOL bKey = TRUE;
  BOOL bString = TRUE;

  // Get section
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strSection))
      return RSERR;
    bSection = TRUE;
  }
  else if (args.IsNil())
  {
    bSection = FALSE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid section argument.\n");
    return RSERR;
  }

  args.Next();

  // Get key
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strKey))
      return RSERR;
    bKey = TRUE;
  }
  else if (args.IsNil())
  {
    bKey = FALSE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid key argument.\n");
    return RSERR;
  }

  args.Next();

  // Get string
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strString))
      return RSERR;
    bString = TRUE;
  }
  else if (args.IsNil())
  {
    bString = FALSE;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid string argument.\n");
    return RSERR;
  }

  args.Next();

  // Filename
  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetPathString(strFile))
      return RSERR;
    strFile.FullPath();
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid file argument.\n");
    return RSERR;
  }

  if (!CreateUnicodeIniFile(strFile))
  {
    acedRetNil();
    return RTNORM;
  }

  // Do it...
  if (bSection == TRUE)
  {
    if (bKey == TRUE)
    {
      if (bString == TRUE)
      {
        if (WritePrivateProfileString(strSection, strKey, strString, strFile))
          acedRetStr(strFile);
        else
          acedRetNil();
      }
      else
      {
        if (WritePrivateProfileString(strSection, strKey, 0, strFile))
          acedRetStr(strFile);
        else
          acedRetNil();
      }
    }
    else
    {
      if (WritePrivateProfileString(strSection, 0, 0, strFile))
        acedRetStr(strFile);
      else
        acedRetNil();
    }
  }
  else
  {
    WritePrivateProfileString(0, 0, 0, strFile);
    acedRetNil();
  }

  return RSRSLT;
}

static HKEY GetHiveFromString(LPCTSTR lpHive)
{
  CString strHive(lpHive);
  strHive.TrimLeft();
  strHive.TrimRight();

  HKEY hHive = 0;
  if (strHive.CompareNoCase(L"HKEY_CURRENT_USER") == 0)
    hHive = HKEY_CURRENT_USER;
  else if (strHive.CompareNoCase(L"HKEY_LOCAL_MACHINE") == 0)
    hHive = HKEY_LOCAL_MACHINE;
  else if (strHive.CompareNoCase(L"HKEY_USERS") == 0)
    hHive = HKEY_USERS;
  else if (strHive.CompareNoCase(L"HKEY_CURRENT_CONFIG") == 0)
    hHive = HKEY_CURRENT_CONFIG;
  else if (strHive.CompareNoCase(L"HKEY_CLASSES_ROOT") == 0)
    hHive = HKEY_CLASSES_ROOT;
  else if (strHive.CompareNoCase(L"HKEY_PERFORMANCE_DATA") == 0)
    hHive = HKEY_PERFORMANCE_DATA;
  else if (strHive.CompareNoCase(L"HKEY_DYN_DATA") == 0)
    hHive = HKEY_DYN_DATA;
  else
    return 0;

  return hHive;
}

////////////////////////////////////////////////////////////////
// dos_regenumkeys
int CDOSLibApp::ads_dos_regenumkeys()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    DWORD iIndex = 0;
    for (;;)
    {
      CString strName;
      DWORD dwSize = _MAX_PATH;
      lResult = reg.EnumKey(iIndex, strName.GetBuffer(dwSize), &dwSize);
      strName.ReleaseBuffer();
      if (lResult == ERROR_SUCCESS)
      {
        result.Add(strName);
        iIndex++;
        continue;
      }
      break;
    }
    reg.Close();
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regenumnames
int CDOSLibApp::ads_dos_regenumnames()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    DWORD iIndex = 0;
    for (;;)
    {
      CString strName;
      DWORD dwSize = _MAX_PATH;
      lResult = RegEnumValue(reg.m_hKey, iIndex, strName.GetBuffer(dwSize), &dwSize, 0, 0, 0, 0);
      strName.ReleaseBuffer();
      if (lResult == ERROR_SUCCESS)
      {
        result.Add(strName);
        iIndex++;
        continue;
      }
      break;
    }
    reg.Close();
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_reggettype
int CDOSLibApp::ads_dos_reggettype()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  DWORD dwType = 0;

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = RegQueryValueEx(reg.m_hKey, strName, 0, &dwType, 0, 0);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetInt((int)dwType);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_reggetstr
int CDOSLibApp::ads_dos_reggetstr()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strValue;
  ULONG nChars = _MAX_PATH;

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.QueryStringValue(strName, strValue.GetBuffer(nChars), &nChars);
    strValue.ReleaseBuffer();
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetStr(strValue);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_reggetint
int CDOSLibApp::ads_dos_reggetint()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  DWORD dwValue = 0;

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.QueryDWORDValue(strName, dwValue);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetInt((int)dwValue);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regsetstr
int CDOSLibApp::ads_dos_regsetstr()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  args.Next();

  CString strValue;
  if (!args.GetString(strValue))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.SetStringValue(strName, strValue);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regsetint
int CDOSLibApp::ads_dos_regsetint()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  args.Next();

  int nValue = 0;
  if (!args.GetInteger(nValue))
    return RSERR;

  DWORD dwValue = (DWORD)nValue;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.SetDWORDValue(strName, dwValue);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regdelval
int CDOSLibApp::ads_dos_regdelval()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.DeleteValue(strName);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regdelkey
int CDOSLibApp::ads_dos_regdelkey()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  args.Next();

  CString strName;
  if (!args.GetString(strName))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  long lResult = reg.Open(hHive, strKey);
  if (lResult == ERROR_SUCCESS)
  {
    lResult = reg.RecurseDeleteKey(strName);
    reg.Close();
  }

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regaddkey
int CDOSLibApp::ads_dos_regaddkey()
{
  CAdsArgs args;

  CString strHive;
  if (!args.GetString(strHive))
    return RSERR;

  args.Next();

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  long lResult = reg.Create(hHive, strKey);
  if (lResult == ERROR_SUCCESS)
    reg.Close();

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regdel (DEPRECATED)
int CDOSLibApp::ads_dos_regdel()
{
  CAdsArgs args;

  CString strKeyBuffer;
  if (!args.GetString(strKeyBuffer))
    return RSERR;

  CString strResultKey = strKeyBuffer;

  CString strNameBuffer;
  if (args.Next())
  {
    if (!args.GetString(strNameBuffer))
      return RSERR;
  }

  int nPos = strKeyBuffer.Find('\\');
  if (nPos < 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strHive = strKeyBuffer.Left(nPos);
  strHive.TrimLeft();
  strHive.TrimRight();
  if (strHive.IsEmpty())
  {
    acedRetNil();
    return RSRSLT;
  }

  strKeyBuffer.Delete(0, nPos + 1);

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strRegKey, strRegName;
  if (strNameBuffer.IsEmpty())
  {
    int nPos = strKeyBuffer.ReverseFind('\\');
    if (nPos >= 0)
    {
      strRegKey = strKeyBuffer.Left(nPos);
      strRegName = strKeyBuffer.Right(strKeyBuffer.GetLength() - nPos - 1);
    }
  }
  else
  {
    strRegKey = strKeyBuffer;
    strRegName = strNameBuffer;
  }

  LONG lResult = ERROR_SUCCESS;
  if (strNameBuffer.IsEmpty())
  {
    CRegKey reg;
    lResult = reg.Open(hHive, strRegKey);
    if (lResult == ERROR_SUCCESS)
    {
      lResult = reg.RecurseDeleteKey(strRegName);
      reg.Close();
    }
  }
  else
  {
    CRegKey reg;
    lResult = reg.Open(hHive, strRegKey);
    if (lResult == ERROR_SUCCESS)
    {
      lResult = reg.DeleteValue(strRegName);
      reg.Close();
    }
  }

  if (lResult == ERROR_SUCCESS)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regget (DEPRECATED)
int CDOSLibApp::ads_dos_regget()
{
  CAdsArgs args;

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  CString strResult = strKey;

  CString strName;
  if (args.Next())
  {
    if (!args.GetString(strName))
      return RSERR;
  }

  int nPos = strKey.Find('\\');
  if (nPos < 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strHive = strKey.Left(nPos);
  strHive.TrimLeft();
  strHive.TrimRight();
  if (strHive.IsEmpty())
  {
    acedRetNil();
    return RSRSLT;
  }

  strKey.Delete(0, nPos + 1);

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  if (strName.IsEmpty())
  {
    CAdsRetList result;

    CRegKey reg;
    LONG lResult = reg.Open(hHive, strKey, KEY_READ);
    if (lResult == ERROR_SUCCESS)
    {
      DWORD iIndex = 0;
      for (;;)
      {
        CString strName;
        DWORD dwSize = _MAX_PATH;
        lResult = RegEnumValue(reg.m_hKey, iIndex, strName.GetBuffer(dwSize), &dwSize, 0, 0, 0, 0);
        strName.ReleaseBuffer();
        if (lResult == ERROR_SUCCESS)
        {
          result.Add(strName);
          iIndex++;
          continue;
        }
        break;
      }
      reg.Close();
    }

    if (result.IsEmpty())
      acedRetNil();
    else
      acedRetList(result);
  }
  else
  {
    CString strValue;
    ULONG nChars = _MAX_PATH;

    CRegKey reg;
    LONG lResult = reg.Open(hHive, strKey, KEY_READ);
    if (lResult == ERROR_SUCCESS)
    {
      lResult = reg.QueryStringValue(strName, strValue.GetBuffer(nChars), &nChars);
      strValue.ReleaseBuffer();
      reg.Close();
    }

    if (lResult == ERROR_SUCCESS)
      acedRetStr(strValue);
    else
      acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regkey (DEPRECATED)
int CDOSLibApp::ads_dos_regkey()
{
  CAdsArgs args;

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  CString strResult = strKey;

  int nPos = strKey.Find('\\');
  if (nPos < 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strHive = strKey.Left(nPos);
  strHive.TrimLeft();
  strHive.TrimRight();
  if (strHive.IsEmpty())
  {
    acedRetNil();
    return RSRSLT;
  }

  strKey.Delete(0, nPos + 1);

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  CRegKey reg;
  LONG lResult = reg.Open(hHive, strKey, KEY_READ);
  if (lResult == ERROR_SUCCESS)
  {
    DWORD iIndex = 0;
    for (;;)
    {
      CString strName;
      DWORD dwSize = _MAX_PATH;
      lResult = reg.EnumKey(iIndex, strName.GetBuffer(dwSize), &dwSize);
      strName.ReleaseBuffer();
      if (lResult == ERROR_SUCCESS)
      {
        result.Add(strName);
        iIndex++;
        continue;
      }
      break;
    }
    reg.Close();
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_regset (DEPRECATED)
int CDOSLibApp::ads_dos_regset()
{
  CAdsArgs args;

  CString strKey;
  if (!args.GetString(strKey))
    return RSERR;

  CString strResult = strKey;

  CString strName, strValue;
  if (args.Next())
  {
    if (!args.GetString(strName))
      return RSERR;

    if (args.Next())
    {
      if (!args.GetString(strValue))
        return RSERR;
    }
  }

  int nPos = strKey.Find('\\');
  if (nPos < 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString strHive = strKey.Left(nPos);
  strHive.TrimLeft();
  strHive.TrimRight();
  if (strHive.IsEmpty())
  {
    acedRetNil();
    return RSRSLT;
  }

  strKey.Delete(0, nPos + 1);

  HKEY hHive = GetHiveFromString(strHive);
  if (hHive == 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  CRegKey reg;
  LONG lResult = reg.Create(hHive, strKey);
  if (lResult != ERROR_SUCCESS)
  {
    acedRetNil();
    return RSRSLT;
  }

  if (!strName.IsEmpty())
  {
    lResult = reg.SetStringValue(strName, strValue);
    if (lResult == ERROR_SUCCESS)
      acedRetStr(strValue.IsEmpty() ? strName : strValue);
    else
      acedRetNil();
  }
  else
  {
    acedRetStr(strResult);
  }

  reg.Close();

  return RSRSLT;
}
