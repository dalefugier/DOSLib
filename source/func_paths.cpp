///////////////////////////////////////////////////////////////////////////////
// func_paths.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosRelativePaths.h"

////////////////////////////////////////////////////////////////
// dos_absolutepath
int CDOSLibApp::ads_dos_absolutepath()
{
  CAdsArgs args;

  CDosPathString strFrom;
  if (!args.GetPathString(strFrom))
    return RSERR;

  args.Next();

  CDosPathString strTo;
  if (!args.GetPathString(strTo))
    return RSERR;

  CString str = CDosRelativePaths::RelativeToAbsolute(strTo, strFrom);

  if (str.IsEmpty())
    acedRetNil();
  else
    acedRetStr(str);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_compactpath
int CDOSLibApp::ads_dos_compactpath()
{
  CAdsArgs args;

  CDosPathString str;
  if (!args.GetPathString(str))
    return RSERR;

  args.Next();

  int nChars = 0;
  if (!args.GetInteger(nChars))
    return RSERR;

  if (nChars <= 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  if (str.CompactPathEx((UINT)nChars))
    acedRetStr(str);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_fullpath
int CDOSLibApp::ads_dos_fullpath()
{
  CAdsArgs args;

  CDosPathString str;
  if (!args.GetPathString(str))
    return RSERR;

  if (str.FullPath())
    acedRetStr(str);
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_makepath
int CDOSLibApp::ads_dos_makepath()
{
  CAdsArgs args;

  CString strDrive;
  if (!args.GetPathString(strDrive))
    return RSERR;

  args.Next();

  CString strDir;
  if (!args.GetPathString(strDir))
    return RSERR;

  args.Next();

  CString strFname;
  if (!args.GetPathString(strFname))
    return RSERR;

  args.Next();

  CString strExt;
  if (!args.GetPathString(strExt))
    return RSERR;

  TCHAR tchPath[_MAX_PATH];
  _wmakepath_s(tchPath, _MAX_PATH, strDrive, strDir, strFname, strExt);
  acedRetStr(tchPath);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_path
int CDOSLibApp::ads_dos_path()
{
  CAdsArgs args;

  size_t required_size;
  _wgetenv_s(&required_size, 0, 0, L"PATH");

  wchar_t* path_var = (wchar_t*)malloc(required_size * sizeof(wchar_t));
  if (!path_var)
  {
    acutPrintf(L"DOSLib error: failed to allocate memory.\n");
    return RSERR;
  }

  _wgetenv_s(&required_size, path_var, required_size, L"PATH");
  CString str(path_var);
  free(path_var);

  if (str.IsEmpty())
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  int nPos = 0;
  CString strResult = str.Tokenize(L";", nPos);
  while (!strResult.IsEmpty())
  {
    result.Add(strResult);
    strResult = str.Tokenize(L";", nPos);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_splitpath
int CDOSLibApp::ads_dos_splitpath()
{
  CAdsArgs args;

  CString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(strPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  CAdsRetList result;
  result.Add(drive, true);
  result.Add(dir, true);
  result.Add(fname, true);
  result.Add(ext, true);
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_shortpath
int CDOSLibApp::ads_dos_shortpath()
{
  CAdsArgs args;

  CDosPathString str;
  if (!args.GetPathString(str))
    return RSERR;

  str.RemoveBackslash();
  if (str.ShortPath())
  {
    str.AddBackslash();
    acedRetStr(str);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_longpath
int CDOSLibApp::ads_dos_longpath()
{
  CAdsArgs args;

  CDosPathString str;
  if (!args.GetPathString(str))
    return RSERR;

  str.RemoveBackslash();
  if (str.LongPath())
  {
    str.AddBackslash();
    acedRetStr(str);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_uncpath
int CDOSLibApp::ads_dos_uncpath()
{
  CAdsArgs args;

  CDosPathString path;
  if (!args.GetString(path))
    return RSERR;

  if (path.IsUNC())
  {
    acedRetStr(path);
    return RSRSLT;
  }

  if (path.IsNetworkPath())
  {
    DWORD dwBufferLength = 1024;
    wchar_t szBuffer[1024];
    wmemset(szBuffer, 0, 1024);
    UNIVERSAL_NAME_INFO* pNameInfo = (UNIVERSAL_NAME_INFO*)&szBuffer;
    DWORD dwRetVal = WNetGetUniversalName(path, UNIVERSAL_NAME_INFO_LEVEL, (LPVOID)pNameInfo, &dwBufferLength);
    if (dwRetVal == NO_ERROR)
      acedRetStr(pNameInfo->lpUniversalName);
    else
      acedRetNil();
  }
  else
  {
    wchar_t buffer[_MAX_PATH];
    DWORD dwSize = _MAX_PATH;
    GetComputerName(buffer, &dwSize);

    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];
    _wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

    CString str;
    str.Format(L"\\\\%s%s%s%s", buffer, dir, fname, ext);
    acedRetStr(str);
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_localpath
int CDOSLibApp::ads_dos_localpath()
{
  CAdsArgs args;

  CDosPathString strUncPath;
  if (!args.GetString(strUncPath))
    return RSERR;

  if (!strUncPath.IsUNC() || !strUncPath.IsNetworkPath())
  {
    acedRetStr(strUncPath);
    return RSRSLT;
  }

  CDosPathString strTempName, strRemoteName, strLocalName;
  HANDLE hEnum = 0;
  LPNETRESOURCE lpnrLocal = 0;
  DWORD cbBuffer = 16384; // 16K is a good size
  DWORD cEntries = -1;    // enumerate all possible entries
  DWORD dwResultEnum = 0;

  strTempName = strUncPath;
  strTempName.MakeLower();

  DWORD dwResult = WNetOpenEnum(RESOURCE_CONNECTED, RESOURCETYPE_DISK, 0, 0, &hEnum);
  if (dwResult == NO_ERROR)
  {
    lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
    if (lpnrLocal)
    {
      do
      {
        ZeroMemory(lpnrLocal, cbBuffer);
        dwResultEnum = WNetEnumResource(hEnum, &cEntries, lpnrLocal, &cbBuffer);
        if (dwResultEnum == NO_ERROR)
        {
          for (DWORD i = 0; i < cEntries; i++)
          {
            strRemoteName = lpnrLocal[i].lpRemoteName;
            strRemoteName.MakeLower();
            if (strTempName.Find(strRemoteName) >= 0)
            {
              strLocalName = DOS_ReplaceNoCase(strUncPath, lpnrLocal[i].lpRemoteName, lpnrLocal[i].lpLocalName);
              dwResultEnum = (DWORD)ERROR_NO_MORE_ITEMS;
              break;
            }
          }
        }
      } while (dwResultEnum != ERROR_NO_MORE_ITEMS);
      GlobalFree((HGLOBAL)lpnrLocal);
    }
    WNetCloseEnum(hEnum);
  }

  if (strLocalName.IsEmpty())
    acedRetNil();
  else
    acedRetStr(strLocalName);

  return RTNORM;
}

////////////////////////////////////////////////////////////////
// dos_ispathrelative
int CDOSLibApp::ads_dos_ispathrelative()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  if (strPath.IsRelative())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathroot
int CDOSLibApp::ads_dos_ispathroot()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  if (strPath.IsRoot())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathsameroot
int CDOSLibApp::ads_dos_ispathsameroot()
{
  CAdsArgs args;

  CDosPathString strPath1;
  if (!args.GetPathString(strPath1))
    return RSERR;

  args.Next();

  CDosPathString strPath2;
  if (!args.GetPathString(strPath2))
    return RSERR;

  if (strPath1.IsSameRoot(strPath2))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathunc
int CDOSLibApp::ads_dos_ispathunc()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  if (strPath.IsUNC())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathurl
int CDOSLibApp::ads_dos_ispathurl()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  if (strPath.IsURL())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_relativepath

static bool IsPathDirectory(const CDosPathString& path)
{
  CDosPathString p(path);

  // Does the directory exist?
  if (p.IsDirectory())
    return true;

  // Is the last character a '\\'?
  if (p[p.GetLength() - 1] == '\\')
    return true;

  // Anybody's guess...
  return false;
}

static bool IsPathFile(const CDosPathString& path)
{
  CDosPathString p(path);

  // Does the file exist?
  if (p.FileExists())
    return true;

  // Does it have a file extension?
  CString ext = p.GetExtension();
  if (ext.GetLength())
    return true;

  // Anybody's guess...
  return false;
}

int CDOSLibApp::ads_dos_relativepath()
{
  CAdsArgs args;

  CDosPathString strFrom;
  if (!args.GetPathString(strFrom))
    return RSERR;

  args.Next();

  CDosPathString strTo;
  if (!args.GetPathString(strTo))
    return RSERR;

  // Defaults (if we cannot figure it out...)
  DWORD dwAttrFrom = FILE_ATTRIBUTE_DIRECTORY;
  DWORD dwAttrTo = FILE_ATTRIBUTE_DIRECTORY;

  if (IsPathDirectory(strFrom))
    dwAttrFrom = FILE_ATTRIBUTE_DIRECTORY;
  else if (IsPathFile(strFrom))
    dwAttrFrom = FILE_ATTRIBUTE_NORMAL;

  if (IsPathDirectory(strTo))
    dwAttrFrom = FILE_ATTRIBUTE_DIRECTORY;
  else if (IsPathFile(strTo))
    dwAttrFrom = FILE_ATTRIBUTE_NORMAL;

  CDosPathString strPath;
  LPTSTR lpPath = strPath.GetBuffer(_MAX_PATH * 2);
  BOOL rc = PathRelativePathTo(lpPath, strFrom, dwAttrFrom, strTo, dwAttrTo);
  strPath.ReleaseBuffer();
  if (rc)
    acedRetStr(strPath);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pathbackslash
int CDOSLibApp::ads_dos_pathbackslash()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
    strPath.AddBackslash();
  else
    strPath.RemoveBackslash();

  acedRetStr(strPath);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pathextension
int CDOSLibApp::ads_dos_pathextension()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    CString strExt;
    if (!args.GetString(strExt))
      return RSERR;

    if ('.' != strExt[0])
      strExt.Insert(0, '.');

    strPath.RenameExtension(strExt);
  }
  else
    strPath.RemoveExtension();

  acedRetStr(strPath);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pathquotes
int CDOSLibApp::ads_dos_pathquotes()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
    strPath.QuoteSpaces();
  else
    strPath.UnquoteSpaces();

  acedRetStr(strPath);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathnetwork
int CDOSLibApp::ads_dos_ispathnetwork()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  if (strPath.IsNetworkPath())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispathslow
int CDOSLibApp::ads_dos_ispathslow()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  DWORD dwAttr = -1;
  if (PathIsSlow(strPath, dwAttr))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}