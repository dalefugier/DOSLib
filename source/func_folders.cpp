///////////////////////////////////////////////////////////////////////////////
// func_folders.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosReadDir.h"

////////////////////////////////////////////////////////////////
// dos_chdir
int CDOSLibApp::ads_dos_chdir()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSRSLT;

  if (!PathIsRoot(strPath))
    strPath.RemoveBackslash();

  if (strPath.IsEmpty())
    strPath = L"\\";

  //int nSavedDrive = _getdrive();
  if (SetCurrentDirectory(strPath))
  {
    TCHAR tchPath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, tchPath);
    strPath = tchPath;
    strPath.AddBackslash();
    acedRetStr(strPath);
    //_chdrive( nSavedDrive );
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_deltree
static BOOL EmptyDirectory(LPCTSTR lpPath)
{
  CFileFind finder;
  CString strWildCard = lpPath;
  strWildCard += L"\\*.*";

  BOOL bWorking = finder.FindFile(strWildCard);
  BOOL bFound = bWorking;
  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
      continue;

    if (finder.IsDirectory())
    {
      CString strPath = finder.GetFilePath();
      EmptyDirectory(strPath);
      RemoveDirectory(finder.GetFilePath());
      continue;
    }

    DeleteFile(finder.GetFilePath());
  }

  return bFound;
}

int CDOSLibApp::ads_dos_deltree()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.RemoveBackslash();
  strPath.FullPath();

  BOOL rc = EmptyDirectory(strPath);
  if (rc)
  {
    RemoveDirectory(strPath);
    strPath.AddBackslash();
    acedRetStr(strPath);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dirp
int CDOSLibApp::ads_dos_dirp()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  BOOL rc = strPath.IsDirectory();
  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_mkdir
int CDOSLibApp::ads_dos_mkdir()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.AddBackslash(); // Must add

  //int nSavedDrive = _getdrive(); 
  int rc = SHCreateDirectoryEx(0, strPath, 0);
  if (ERROR_SUCCESS == rc || ERROR_ALREADY_EXISTS == rc)
  {
    strPath.FullPath();
    strPath.AddBackslash();
    acedRetStr(strPath);
    //_chdrive( nSavedDrive );
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pwdir
int CDOSLibApp::ads_dos_pwdir()
{
  CAdsArgs args;

  if (args.IsEmpty())
  {
    wchar_t* buffer = _wgetcwd(0, 0);
    if (buffer)
    {
      CDosPathString strPath(buffer);
      strPath.AddBackslash();
      acedRetStr(strPath);
      free(buffer); // don't leak...
      return RSRSLT;
    }
    else
    {
      acedRetNil();
      return RSRSLT;
    }
  }

  int drive = 0;
  if (args.GetDriveNumber(drive))
  {
    // Does logical drive exist?
    if (GetLogicalDrives() & (1 << (drive - 1)))
    {
      wchar_t* buffer = _wgetdcwd(drive, 0, 0);
      if (buffer)
      {
        CDosPathString strPath(buffer);
        strPath.AddBackslash();
        acedRetStr(strPath);
        free(buffer); // don't leak...
        return RSRSLT;
      }
      else
      {
        acedRetNil();
        return RSRSLT;
      }
    }
  }

  acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rendir
int CDOSLibApp::ads_dos_rendir()
{
  CAdsArgs args;

  CDosPathString strSource;
  if (!args.GetPathString(strSource))
    return RSRSLT;

  strSource.FullPath();
  strSource.RemoveBackslash();

  args.Next();

  CDosPathString strDest;
  if (!args.GetPathString(strDest))
    return RSRSLT;

  strDest.FullPath();
  strDest.RemoveBackslash();

  //int nSavedDrive = _getdrive();
  if (MoveFile(strSource, strDest))
  {
    strDest.AddBackslash();
    acedRetStr(strDest);
    //_chdrive( nSavedDrive );
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rmdir
int CDOSLibApp::ads_dos_rmdir()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSRSLT;

  strPath.RemoveBackslash();

  //int nSavedDrive = _getdrive();
  if (RemoveDirectory(strPath))
  {
    strPath.FullPath();
    strPath.AddBackslash();
    acedRetStr(strPath);
    //_chdrive( nSavedDrive );
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_subdir
int CDOSLibApp::ads_dos_subdir()
{
  CAdsArgs args;

  CDosPathString strPath;
  bool bAddMarkers = false;

  if (args.IsEmpty())
  {
    strPath = L"*.*";
  }
  else
  {
    if (!args.GetPathString(strPath))
      return RSERR;

    strPath.AddBackslash();
    strPath += L"*.*";

    args.Next();

    if (!args.IsEmpty())
      bAddMarkers = true;
  }

  strPath.FullPath();

  CAdsRetList result;

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
    {
      if (bAddMarkers)
        result.Add(finder.GetFileName());
      continue;
    }

    if (finder.IsDirectory())
      result.Add(finder.GetFileName());
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_sysdir
int CDOSLibApp::ads_dos_sysdir()
{
  CAdsArgs args;

  TCHAR tchPath[_MAX_PATH];
  if (GetSystemDirectory(tchPath, _MAX_PATH))
  {
    CDosPathString strPath(tchPath);
    strPath.AddBackslash();
    acedRetStr(strPath);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_tempdir
int CDOSLibApp::ads_dos_tempdir()
{
  CAdsArgs args;

  TCHAR tchPath[_MAX_PATH];
  if (GetTempPath(_MAX_PATH, tchPath))
  {
    CDosPathString strPath(tchPath);
    strPath.AddBackslash();
    acedRetStr(strPath);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_windir
int CDOSLibApp::ads_dos_windir()
{
  CAdsArgs args;

  TCHAR tchPath[_MAX_PATH];
  if (GetWindowsDirectory(tchPath, _MAX_PATH))
  {
    CDosPathString strPath(tchPath);
    strPath.AddBackslash();
    acedRetStr(strPath);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_specialdir
int CDOSLibApp::ads_dos_specialdir()
{
  CAdsArgs args;

  int nFolder = 0;
  if (!args.GetInteger(nFolder))
    return RSERR;

  TCHAR tchPath[_MAX_PATH];
  HRESULT hr = SHGetFolderPath(adsw_acadMainWnd(), nFolder, 0, SHGFP_TYPE_CURRENT, tchPath);
  if (SUCCEEDED(hr))
  {
    CDosPathString strPath(tchPath);
    strPath.AddBackslash();
    acedRetStr(strPath);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dirtree
int CDOSLibApp::ads_dos_dirtree()
{
  CAdsArgs args;

  CDosPathString strPath;

  if (args.IsEmpty())
  {
    strPath = L"*.*";
  }
  else
  {
    if (!args.GetPathString(strPath))
      return RSERR;

    strPath.AddBackslash();
    strPath += L"*.*";
  }

  strPath.FullPath();
  strPath.RemoveFileSpec();
  strPath.AddBackslash();

  if (!strPath.IsDirectory() && !strPath.IsRoot())
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  CDosReadDir dr;
  dr.Recurse() = true;
  dr.GetDirs(strPath);
  dr.SortDirs();
  CDosReadDir::DirVector &dirs = dr.Dirs();
  for (CDosReadDir::DirVector::const_iterator dit = dirs.begin(); dit != dirs.end(); dit++)
  {
    CString str = (*dit).m_sName;
    str += "\\";
    result.Add(str);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

/*
Description:
  Get 64-bit file size
Parameters:
  lpszPath - [in] fully qualified path to file
  lpFileSize - [out] 64-bit file size
Returns:
  Returns nonzero if successful or zero otherwise.
*/
BOOL DosGetFileSize64(LPCTSTR lpszPath, PLARGE_INTEGER lpFileSize)
{
  BOOL rc = FALSE;
  if (lpszPath && lpFileSize)
  {
    lpFileSize->QuadPart = 0;
    HANDLE hFile = hFile = CreateFile(lpszPath, READ_CONTROL, 0, 0, OPEN_EXISTING, 0, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
      rc = GetFileSizeEx(hFile, lpFileSize);
      CloseHandle(hFile);
    }
  }

  return rc;
}

/*
Description:
  Get folder size
Parameters:
  lpszStartFolder - [in] fully qualified starting folder path
  bRecurse - [in] recurse into subdirectories
  bQuickSize - [in/out] if lpszStartFolder is a drive, use SHGetDiskFreeSpace()
                        to get size for entire disk, instead of recursing (folder
                        count and file count will be 0).
  lpFolderSize - [out] pointer to 64-bit folder size
  lpFolderCount - [out] pointer to 32-bit folder count
  lpFileCount - [out] pointer to 64-bit file count
Returns:
  Returns nonzero if successful or zero otherwise.
*/
BOOL DosGetFolderSize(
  LPCTSTR lpszStartFolder,
  BOOL bRecurse,
  BOOL bQuickSize,
  PLARGE_INTEGER lpFolderSize,
  LPDWORD lpFolderCount,
  LPDWORD lpFileCount
  )
{
  BOOL rc = FALSE;
  if (lpszStartFolder && lpFolderSize)
  {
    lpFolderSize->QuadPart = 0;
    if (lpFolderCount)
      *lpFolderCount = 0;
    if (lpFileCount)
      *lpFileCount = 0;

    if (bQuickSize && (_tcslen(lpszStartFolder) < 4))
    {
      // get used space for entire volume
      ULARGE_INTEGER ulFreeCaller;
      ulFreeCaller.QuadPart = 0;
      ULARGE_INTEGER ulTotal;
      ulTotal.QuadPart = 0;
      ULARGE_INTEGER ulFree;
      ulFree.QuadPart = 0;
      rc = SHGetDiskFreeSpace(lpszStartFolder, &ulFreeCaller, &ulTotal, &ulFree);
      lpFolderSize->QuadPart = ulTotal.QuadPart - ulFree.QuadPart;
      return rc;
    }

    HANDLE hFF = 0;
    WIN32_FIND_DATA wfd = { 0 };
    TCHAR szPath[2000];
    LARGE_INTEGER li_total;
    li_total.QuadPart = 0;
    DWORD dwTotalFolderCount = 0;
    DWORD dwTotalFileCount = 0;

    _tcsncpy(szPath, lpszStartFolder, sizeof(szPath) / sizeof(TCHAR) - 1);
    _tcscat(szPath, L"\\*");

    hFF = FindFirstFile(szPath, &wfd);
    if (hFF != INVALID_HANDLE_VALUE)
    {
      rc = TRUE;
      do
      {
        _tcsncpy(szPath, lpszStartFolder, sizeof(szPath) / sizeof(TCHAR) - 1);
        _tcscat(szPath, L"\\");
        _tcscat(szPath, wfd.cFileName);

        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          if (_tcscmp(wfd.cFileName, L".") && _tcscmp(wfd.cFileName, L"..") && bRecurse)
          {
            // recurse into the subdirectory
            LARGE_INTEGER li;
            li.QuadPart = 0;
            DWORD dwFolderCount = 0;
            DWORD dwFileCount = 0;
            rc = DosGetFolderSize(szPath, bRecurse, bQuickSize, &li, &dwFolderCount, &dwFileCount);
            li_total.QuadPart += li.QuadPart;
            dwTotalFolderCount += dwFolderCount + 1;
            dwTotalFileCount += dwFileCount;
          }
        }
        else
        {
          LARGE_INTEGER li;
          li.QuadPart = 0;
          rc = DosGetFileSize64(szPath, &li);
          li_total.QuadPart += li.QuadPart;
          dwTotalFileCount += 1;
        }

      } while (FindNextFile(hFF, &wfd));

      FindClose(hFF); // 3-Mar-2011
    }

    *lpFolderSize = li_total;
    if (lpFolderCount)
      *lpFolderCount = dwTotalFolderCount;
    if (lpFileCount)
      *lpFileCount = dwTotalFileCount;
  }

  return rc;
}

////////////////////////////////////////////////////////////////
// dos_dirsize
int CDOSLibApp::ads_dos_dirsize()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSRSLT;

  strPath.FullPath();
  strPath.RemoveBackslash();

  args.Next();

  BOOL bRecurse = FALSE;
  if (!args.IsEmpty())
    bRecurse = TRUE;

  //LARGE_INTEGER li;
  LONGLONG li;
  DWORD dwFolderCount = 0;
  DWORD dwFileCount = 0;

  BOOL rc = DosGetFolderSize(strPath, bRecurse, TRUE, (LARGE_INTEGER*)&li, &dwFolderCount, &dwFileCount);
  if (rc)
  {
    CAdsRetList result;
    result.Add((double)li);
    result.Add((long)dwFolderCount);
    result.Add((long)dwFileCount);
    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}