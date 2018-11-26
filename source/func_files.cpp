///////////////////////////////////////////////////////////////////////////////
// func_files.cpp
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
#include "DosFileOperation.h"
#include "DosReadDir.h"
#include "DosFileMerge.h"
#include "DosVersionInfo.h"
#include "DosUnicodeTextFile.h"

////////////////////////////////////////////////////////////////

double DOS_AcadTime(const CTime& time)
{
  // Modifed date in AutoCAD-type Julian date
  double julian = 10000 * time.GetYear();
  julian += 100 * time.GetMonth();
  julian += time.GetDay();
  julian += 0.01 * time.GetHour();
  julian += 0.0001 * time.GetMinute();
  julian += 0.000001 * time.GetSecond();
  return julian;
}

double DOS_AcadTime(const FILETIME& time)
{
  CTime ctime(time);
  return DOS_AcadTime(ctime);
}

unsigned int DOS_FileOwner(const wchar_t* szFileOrFolderPathName, wchar_t* pUserNameBuffer, unsigned long uSizeInBytes)
{
  if (0 == szFileOrFolderPathName || 0 == szFileOrFolderPathName[0] || !PathFileExists(szFileOrFolderPathName))
    return ERROR_INVALID_PARAMETER;

  if (uSizeInBytes <= 0 || 0 == pUserNameBuffer)
    return ERROR_INVALID_PARAMETER;

  wchar_t szUNCPathName[32767] = { 0 };
  if (!PathIsUNC(szFileOrFolderPathName))
  {
    DWORD dwUniSize = 32767 * sizeof(wchar_t);
    UNIVERSAL_NAME_INFO* pUNI = (UNIVERSAL_NAME_INFO*)szUNCPathName;
    if (!WNetGetUniversalName(szFileOrFolderPathName, UNIVERSAL_NAME_INFO_LEVEL, pUNI, &dwUniSize))
      lstrcpy(szUNCPathName, pUNI->lpUniversalName);
    else
      lstrcpy(szUNCPathName, szFileOrFolderPathName);
  }
  else
    lstrcpy(szUNCPathName, szFileOrFolderPathName);

  wchar_t szMachineName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
  if (PathIsUNC(szUNCPathName))
  {
    LPWSTR lpMachineName = PathFindNextComponent(szUNCPathName);
    int nPos = 0;
    LPWSTR lpNextSlash = lpMachineName;
    while ((lpNextSlash[0] != L'\\') && (lpNextSlash[0] != L'\0'))
    {
      nPos++;
      lpNextSlash++;
    }
    lstrcpyn(szMachineName, lpMachineName, nPos + 1);
  }

  unsigned long uSizeNeeded = 0;
  GetFileSecurity(szUNCPathName, OWNER_SECURITY_INFORMATION, 0, 0, &uSizeNeeded);
  UINT uRet = GetLastError();
  if (uRet == ERROR_INSUFFICIENT_BUFFER && uSizeNeeded)
  {
    uRet = S_OK;

    LPBYTE lpSecurityBuffer = (LPBYTE)malloc(uSizeNeeded * sizeof(BYTE));
    if (0 == lpSecurityBuffer)
      return ERROR_NOT_ENOUGH_MEMORY;

    if (!GetFileSecurity(szUNCPathName, OWNER_SECURITY_INFORMATION, lpSecurityBuffer, uSizeNeeded, &uSizeNeeded))
    {
      free(lpSecurityBuffer);
      return GetLastError();
    }

    PSID pSID = 0;
    BOOL bOwnerDefaulted = FALSE;
    if (!GetSecurityDescriptorOwner(lpSecurityBuffer, &pSID, &bOwnerDefaulted))
    {
      free(lpSecurityBuffer);
      return GetLastError();
    }

    LPWSTR pName = 0;
    LPWSTR pDomain = 0;
    unsigned long uNameLen = 0;
    unsigned long uDomainLen = 0;
    SID_NAME_USE sidNameUse = SidTypeUser;
    LookupAccountSid(szMachineName, pSID, pName, &uNameLen, pDomain, &uDomainLen, &sidNameUse);
    uRet = GetLastError();
    if ((uRet == ERROR_INSUFFICIENT_BUFFER) && uNameLen && uDomainLen)
    {
      uRet = S_OK;

      pName = (LPWSTR)malloc(uNameLen * sizeof(wchar_t));
      pDomain = (LPWSTR)malloc(uDomainLen * sizeof(wchar_t));
      if (0 == pName || 0 == pDomain)
      {
        free(lpSecurityBuffer);
        return ERROR_NOT_ENOUGH_MEMORY;
      }

      if (!LookupAccountSid(szMachineName, pSID, pName, &uNameLen, pDomain, &uDomainLen, &sidNameUse))
      {
        free(pName);
        free(pDomain);
        free(lpSecurityBuffer);
        return GetLastError();
      }

      if (uSizeInBytes > ((uNameLen + uDomainLen + 2)*sizeof(wchar_t)))
      {
        lstrcpy(pUserNameBuffer, pDomain);
        lstrcat(pUserNameBuffer, L"\\");
        lstrcat(pUserNameBuffer, pName);
      }
      else
        uRet = ERROR_INSUFFICIENT_BUFFER;

      free(pName);
      free(pDomain);
    }

    free(lpSecurityBuffer);
  }

  return uRet;
}

////////////////////////////////////////////////////////////////
// dos_copy
int CDOSLibApp::ads_dos_copy()
{
  CAdsArgs args;

  CDosPathString strSource;
  if (!args.GetPathString(strSource))
    return RSERR;

  strSource.FullPath();

  args.Next();

  CDosPathString strDestination;
  if (!args.GetPathString(strDestination))
    return RSERR;

  strDestination.FullPath();
  strDestination.RemoveBackslash();

  FILEOP_FLAGS flags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NORECURSION | FOF_NOERRORUI;

  CDosFileOperation op;
  op.AddSourceFile(strSource);
  op.AddDestFile(strDestination);
  op.SetOperationFlags(FO_COPY, CWnd::FromHandle(adsw_acadMainWnd()), flags);

  bool bStarted = false;
  bool bAborted = false;
  int nResult = 0;

  if (op.Go(&bStarted, &nResult, &bAborted))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_delete
int CDOSLibApp::ads_dos_delete()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  FILEOP_FLAGS flags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NORECURSION | FOF_NOERRORUI;

  CDosFileOperation op;
  op.AddSourceFile(strPath);
  op.SetOperationFlags(FO_DELETE, CWnd::FromHandle(adsw_acadMainWnd()), flags);

  bool bStarted = false;
  bool bAborted = false;
  int nResult = 0;

  if (op.Go(&bStarted, &nResult, &bAborted))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_recycle
int CDOSLibApp::ads_dos_recycle()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  FILEOP_FLAGS flags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NORECURSION | FOF_NOERRORUI | FOF_ALLOWUNDO;

  CDosFileOperation op;
  op.AddSourceFile(strPath);
  op.SetOperationFlags(FO_DELETE, CWnd::FromHandle(adsw_acadMainWnd()), flags);

  bool bStarted = false;
  bool bAborted = false;
  int nResult = 0;

  if (op.Go(&bStarted, &nResult, &bAborted))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_emptyrecycle
int CDOSLibApp::ads_dos_emptyrecycle()
{
  HRESULT hr = SHEmptyRecycleBin(adsw_acadMainWnd(), 0, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
  if (SUCCEEDED(hr))
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_move
int CDOSLibApp::ads_dos_move()
{
  CAdsArgs args;

  CDosPathString strSource;
  if (!args.GetPathString(strSource))
    return RSERR;

  strSource.FullPath();

  args.Next();

  CDosPathString strDestination;
  if (!args.GetPathString(strDestination))
    return RSERR;

  strDestination.FullPath();
  strDestination.RemoveBackslash();

  FILEOP_FLAGS flags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NORECURSION | FOF_NOERRORUI;

  CDosFileOperation op;
  op.AddSourceFile(strSource);
  op.AddDestFile(strDestination);
  op.SetOperationFlags(FO_MOVE, CWnd::FromHandle(adsw_acadMainWnd()), flags);

  bool bStarted = false;
  bool bAborted = false;
  int nResult = 0;

  if (op.Go(&bStarted, &nResult, &bAborted))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rename
int CDOSLibApp::ads_dos_rename()
{
  CAdsArgs args;

  CDosPathString strSource;
  if (!args.GetPathString(strSource))
    return RSERR;

  strSource.FullPath();

  args.Next();

  CDosPathString strDestination;
  if (!args.GetPathString(strDestination))
    return RSERR;

  strDestination.FullPath();

  FILEOP_FLAGS flags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_NORECURSION | FOF_NOERRORUI;

  CDosFileOperation op;
  op.AddSourceFile(strSource);
  op.AddDestFile(strDestination);
  op.SetOperationFlags(FO_RENAME, CWnd::FromHandle(adsw_acadMainWnd()), flags);

  bool bStarted = false;
  bool bAborted = false;
  int nResult = 0;

  if (op.Go(&bStarted, &nResult, &bAborted))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_file
int CDOSLibApp::ads_dos_file()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  finder.FindNextFile();
  if (!finder.IsDots() && !finder.IsDirectory())
  {
    CString str;

    result.Add(finder.GetFileName());
    result.Add((double)finder.GetLength());

    CTime time;
    finder.GetLastWriteTime(time);
    result.Add(DOS_AcadTime(time));

    // Return attribues like dos_attrib
    int nAttributes = 0;
    if (finder.IsReadOnly())
      nAttributes += 1;
    if (finder.IsHidden())
      nAttributes += 2;
    if (finder.IsSystem())
      nAttributes += 4;
    if (finder.IsArchived())
      nAttributes += 8;
    result.Add(nAttributes);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_fileex
int CDOSLibApp::ads_dos_fileex()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  finder.FindNextFile();
  if (!finder.IsDots() && !finder.IsDirectory())
  {
    result.Add(finder.GetRoot());
    result.Add(finder.GetFileName());
    result.Add((double)finder.GetLength());

    CTime create_time;
    finder.GetCreationTime(create_time);
    result.Add(DOS_AcadTime(create_time));

    CTime write_time;
    finder.GetLastWriteTime(write_time);
    result.Add(DOS_AcadTime(write_time));

    CTime access_time;
    finder.GetLastAccessTime(access_time);
    result.Add(DOS_AcadTime(access_time));

    // Return attribues like dos_attrib
    int nAttributes = 0;
    if (finder.IsNormal())
      nAttributes = 0;
    if (finder.IsReadOnly())
      nAttributes += 1;
    if (finder.IsHidden())
      nAttributes += 2;
    if (finder.IsSystem())
      nAttributes += 4;
    if (finder.IsArchived())
      nAttributes += 8;
    if (finder.IsCompressed())
      nAttributes += 16;
    if (finder.IsTemporary())
      nAttributes += 32;
    result.Add(nAttributes);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_filep
int CDOSLibApp::ads_dos_filep()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  if (strPath.FileExists())
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_tempfile
int CDOSLibApp::ads_dos_tempfile()
{
  CAdsArgs args;

  CDosPathString strPath, strFile;

  LPTSTR lpPath = strPath.GetBuffer(_MAX_PATH);
  DWORD dw = GetTempPath(_MAX_PATH, lpPath);
  strPath.ReleaseBuffer();
  if (dw == 0)
    return RSERR;

  strPath.LongPath();

  for (;;)
  {
    LPTSTR lpFile = strFile.GetBuffer(_MAX_PATH);
    UINT rc = GetTempFileName(strPath, L"~ac", (UINT)GetTickCount(), lpFile);
    strFile.ReleaseBuffer();
    if (rc == 0)
      return RSERR;

    if (!strFile.FileExists())
      break;
  }

  if (strFile.IsEmpty())
    acedRetNil();
  else
    acedRetStr(strFile);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_filedate
int CDOSLibApp::ads_dos_filedate()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (args.IsEmpty())
    strPath = L"*.*";
  else if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  // Start association list
  CAdsRetList result;
  result.AddLB();

  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
      continue;

    CTime time;
    finder.GetLastWriteTime(time);

    // Add dotted par
    result.AddLB();
    result.Add(finder.GetFileName());
    result.AddDot();
    result.Add(DOS_AcadTime(time));
    result.AddLE();
  }

  // End association list
  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_filesize
int CDOSLibApp::ads_dos_filesize()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (args.IsEmpty())
    strPath = L"*.*";
  else if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  // Start association list
  CAdsRetList result;
  result.AddLB();

  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
      continue;

    if (finder.IsDirectory())
      continue;

    result.AddLB();
    result.Add(finder.GetFileName());
    result.AddDot();
    result.Add((double)finder.GetLength());
    result.AddLE();
  }

  // End association list
  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_filesys
int CDOSLibApp::ads_dos_filesys()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;

  strDrive.AddBackslash();

  TCHAR tchVolumeNameBuffer[_MAX_PATH];
  TCHAR tchFileSystemNameBuffer[_MAX_PATH];
  DWORD dwVolumeSerialNumber, dwMaximumComponentLength, dwFileSystemFlags;

  BOOL rc = GetVolumeInformation(
    strDrive,                  // root directory
    tchVolumeNameBuffer,       // volume name buffer
    (DWORD)_MAX_PATH,          // length of name buffer
    &dwVolumeSerialNumber,     // volume serial number
    &dwMaximumComponentLength, // maximum file name length
    &dwFileSystemFlags,        // file system options
    tchFileSystemNameBuffer,   // file system name buffer
    (DWORD)_MAX_PATH           // length of file system name buffer
    );

  if (rc)
    acedRetStr(tchFileSystemNameBuffer);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_touch
int CDOSLibApp::ads_dos_touch()
{
  FILETIME ft;
  memset(&ft, 0, sizeof(FILETIME));

  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);
  }
  else
  {
    // Get local time
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);

    CArray<int, int> date_list;
    if (!args.GetIntegerList(date_list))
      return RSERR;

    int date_count = (int)date_list.GetCount();
    if (date_count < 3)
      return RSERR;

    st.wYear = date_list[0];
    st.wMonth = date_list[1];
    st.wDay = date_list[2];

    args.Next();

    if (!args.IsEmpty())
    {
      CArray<int, int> time_list;
      if (!args.GetIntegerList(time_list))
        return RSERR;

      int time_count = (int)time_list.GetCount();
      if (time_count < 2)
        return RSERR;

      st.wHour = time_list[0];
      st.wMinute = time_list[1];
      st.wSecond = (time_count > 2) ? time_list[2] : 0;
      st.wMilliseconds = (time_count > 3) ? time_list[3] : 0;
    }

    FILETIME lt;
    memset(&lt, 0, sizeof(FILETIME));
    SystemTimeToFileTime(&st, &lt); // local system time to local file time
    LocalFileTimeToFileTime(&lt, &ft); // local file time to file time
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
      continue;

    if (finder.IsDirectory())
      continue;

    HANDLE hFile = ::CreateFile(finder.GetFilePath(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
      SetFileTime(hFile, &ft, &ft, &ft);
      CloseHandle(hFile);
      result.Add(finder.GetFileName());
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_openp
int CDOSLibApp::ads_dos_openp()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  if (!strPath.FileExists())
  {
    acedRetNil();
    return RSRSLT;
  }

  CFile f;
  CFileException e;
  if (!f.Open(strPath, CFile::modeRead | CFile::shareExclusive, &e))
  {
    if (GetLastError() == ERROR_SHARING_VIOLATION)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    f.Close();
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dir
int CDOSLibApp::ads_dos_dir()
{
  CAdsArgs args;

  CDosPathString strBuffer(L"*.*");
  int nFlag = 0;

  if (!args.IsEmpty())
  {
    if (!args.GetPathString(strBuffer))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(nFlag))
        return RSERR;
    }
  }
  strBuffer.FullPath();

  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(strBuffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  CString strPath, strFile;
  strPath.Format(L"%s%s", drive, dir);
  strFile.Format(L"%s%s", fname, ext);

  int nSortStyle = 0;
  bool bReverse = false;

  switch (nFlag)
  {
  case 0:
    nSortStyle = 0;   // eSortNone
    bReverse = false;
    break;
  case 1:
    nSortStyle = 1;   // eSortAlpha
    bReverse = false;
    break;
  case 2:
    nSortStyle = 1;   // eSortAlpha
    bReverse = true;
    break;
  case 3:
    nSortStyle = 2;   // eSortWriteDate
    bReverse = false;
    break;
  case 4:
    nSortStyle = 2;   // eSortWriteDate
    bReverse = true;
    break;
  case 5:
    nSortStyle = 3;   // eSortSize
    bReverse = false;
    break;
  case 6:
    nSortStyle = 3;   // eSortSize
    bReverse = true;
    break;
  default:
    nSortStyle = 0;   // eSortNone
    bReverse = false;
    break;
  }

  CAdsRetList result;

  CDosReadDir dr;
  dr.Recurse() = false;
  dr.GetDirs(strPath);
  dr.GetFiles(strFile);
  CDosReadDir::FileVector &files = dr.Files();
  dr.SortFiles(nSortStyle, bReverse);
  for (CDosReadDir::FileVector::const_iterator fit = files.begin(); fit != files.end(); fit++)
  {
    result.Add((*fit).m_sName);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_find
int CDOSLibApp::ads_dos_find()
{
  CAdsArgs args;

  CDosPathString strBuffer(L"*.*");
  int nFlag = 0;

  if (!args.IsEmpty())
  {
    if (!args.GetPathString(strBuffer))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(nFlag))
        return RSERR;
    }
  }
  strBuffer.FullPath();

  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(strBuffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  CString strPath, strFile;
  strPath.Format(L"%s%s", drive, dir);
  strFile.Format(L"%s%s", fname, ext);

  int nSortStyle = 0;
  bool bReverse = false;

  switch (nFlag)
  {
  case 0:
    nSortStyle = 0;   // eSortNone
    bReverse = false;
    break;
  case 1:
    nSortStyle = 1;   // eSortAlpha
    bReverse = false;
    break;
  case 2:
    nSortStyle = 1;   // eSortAlpha
    bReverse = true;
    break;
  case 3:
    nSortStyle = 2;   // eSortWriteDate
    bReverse = false;
    break;
  case 4:
    nSortStyle = 2;   // eSortWriteDate
    bReverse = true;
    break;
  case 5:
    nSortStyle = 3;   // eSortSize
    bReverse = false;
    break;
  case 6:
    nSortStyle = 3;   // eSortSize
    bReverse = true;
    break;
  default:
    nSortStyle = 0;   // eSortNone
    bReverse = false;
    break;
  }

  CAdsRetList result;

  CDosReadDir dr;
  dr.Recurse() = true;
  dr.GetDirs(strPath);
  dr.GetFiles(strFile);
  CDosReadDir::FileVector &files = dr.Files();
  dr.SortFiles(nSortStyle, bReverse);

  for (CDosReadDir::FileVector::const_iterator fit = files.begin(); fit != files.end(); fit++)
  {
    result.Add((*fit).m_sName);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_search
int CDOSLibApp::ads_dos_search()
{
  CAdsArgs args;

  CDosPathString strFile;
  if (!args.GetPathString(strFile))
    return RSERR;

  args.Next();

  TCHAR tcsBuffer[_MAX_PATH];
  LPTSTR lpFilePart = 0;

  if (args.IsEmpty())
  {
    if (SearchPath(0, strFile, 0, _MAX_PATH, tcsBuffer, &lpFilePart))
      acedRetStr(tcsBuffer);
    else
      acedRetNil();
  }
  else
  {
    CDosPathString strPath;
    if (!args.GetPathString(strPath))
      return RSERR;

    if (SearchPath(strPath, strFile, 0, _MAX_PATH, tcsBuffer, &lpFilePart))
      acedRetStr(tcsBuffer);
    else
      acedRetNil();
  }

  return RSRSLT;
}

int DOS_SetAttributeFlags(int nAttributes)
{
  if (nAttributes == -1)
    return -1;

  if (nAttributes == 0)
    return FILE_ATTRIBUTE_NORMAL;

  int nFlags = 0;

  if (nAttributes & 1)
    nFlags += FILE_ATTRIBUTE_READONLY;
  if (nAttributes & 2)
    nFlags += FILE_ATTRIBUTE_HIDDEN;
  if (nAttributes & 4)
    nFlags += FILE_ATTRIBUTE_SYSTEM;
  if (nAttributes & 8)
    nFlags += FILE_ATTRIBUTE_ARCHIVE;

  return nFlags;
}

int DOS_GetAttributeFlags(int nFlags)
{
  if (nFlags & FILE_ATTRIBUTE_NORMAL)
    return 0;

  int nAttributes = 0;

  if (nFlags & FILE_ATTRIBUTE_READONLY)
    nAttributes += 1;
  if (nFlags & FILE_ATTRIBUTE_HIDDEN)
    nAttributes += 2;
  if (nFlags & FILE_ATTRIBUTE_SYSTEM)
    nAttributes += 4;
  if (nFlags & FILE_ATTRIBUTE_ARCHIVE)
    nAttributes += 8;

  return nAttributes;
}

////////////////////////////////////////////////////////////////
// dos_attrib
int CDOSLibApp::ads_dos_attrib()
{
  CAdsArgs args;

  CDosPathString strPath;
  int nAttributes = -1;

  if (args.IsEmpty())
    strPath = L"*.*";
  else
  {
    if (!args.GetPathString(strPath))
      return RSERR;

    args.Next();

    if (args.IsEmpty())
      nAttributes = -1;
    else
    {
      if (!args.GetInteger(nAttributes))
        return RSERR;
    }
  }

  strPath.FullPath();

  if (nAttributes > 15)
  {
    acutPrintf(L"DOSLib error: invalid bit-code value.\n");
    return RSERR;
  }

  nAttributes = DOS_SetAttributeFlags(nAttributes);

  CAdsRetList result;

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  result.AddLB();

  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
      continue;

    if (finder.IsDirectory())
      continue;

    if (nAttributes > -1)
      BOOL bResult = SetFileAttributes(finder.GetFilePath(), nAttributes);

    int nFlags = GetFileAttributes(finder.GetFilePath());
    nFlags = DOS_GetAttributeFlags(nFlags);

    result.AddLB();
    result.Add(finder.GetFileName());
    result.AddDot();
    result.Add((short)nFlags);
    result.AddLE();
  }

  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dirattrib
int CDOSLibApp::ads_dos_dirattrib()
{
  CAdsArgs args;

  CDosPathString strPath;
  int nAttributes = -1;

  if (args.IsEmpty())
    strPath = L"*.*";
  else
  {
    if (!args.GetPathString(strPath))
      return RSERR;

    strPath.RemoveBackslash();

    args.Next();

    if (args.IsEmpty())
      nAttributes = -1;
    else
    {
      if (!args.GetInteger(nAttributes))
        return RSERR;
    }
  }

  strPath.FullPath();

  if (nAttributes > 15)
  {
    acutPrintf(L"DOSLib error: invalid bit-code value.\n");
    return RSERR;
  }

  nAttributes = DOS_SetAttributeFlags(nAttributes);

  CAdsRetList result;

  CFileFind finder;
  BOOL bWorking = finder.FindFile(strPath);
  if (!bWorking)
  {
    acedRetNil();
    return RSRSLT;
  }

  result.AddLB();

  while (bWorking)
  {
    bWorking = finder.FindNextFile();

    if (finder.IsDots())
      continue;

    if (!finder.IsDirectory())
      continue;

    if (nAttributes > -1)
      BOOL bResult = SetFileAttributes(finder.GetFilePath(), nAttributes);

    int nFlags = GetFileAttributes(finder.GetFilePath());
    nFlags = DOS_GetAttributeFlags(nFlags);

    result.AddLB();
    result.Add(finder.GetFileName());
    result.AddDot();
    result.Add((short)nFlags);
    result.AddLE();
  }

  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_mergefiles

int CDOSLibApp::ads_dos_mergefiles()
{
  CAdsArgs args;

  CStringArray input;
  if (!args.GetStringList(input))
    return RSERR;

  args.Next();

  CDosPathString output;
  if (!args.GetPathString(output))
  {
    if (input.GetCount() < 2)
      return RSERR;

    output = input[0];
    input.RemoveAt(0);
  }

  CDosFileMerge fm(output, input);
  bool rc = fm.Merge();

  if (rc)
    acedRetStr(output);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_fileinfo
int CDOSLibApp::ads_dos_fileinfo()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  CDosVersionInfo ver;
  if (ver.GetFileVersionInfo(strPath))
  {
    CAdsRetList result;

    result.AddLB();
    result.Add((int)HIWORD(ver.dwFileVersionMS));
    result.Add((int)LOWORD(ver.dwFileVersionMS));
    result.Add((int)HIWORD(ver.dwFileVersionLS));
    result.Add((int)LOWORD(ver.dwFileVersionLS));
    result.AddLE();

    result.AddLB();
    result.Add((int)HIWORD(ver.dwProductVersionMS));
    result.Add((int)LOWORD(ver.dwProductVersionMS));
    result.Add((int)HIWORD(ver.dwProductVersionLS));
    result.Add((int)LOWORD(ver.dwProductVersionLS));
    result.AddLE();

    result.Add(ver.GetValue(L"Comments"));
    result.Add(ver.GetValue(L"CompanyName"));
    result.Add(ver.GetValue(L"FileDescription"));
    result.Add(ver.GetValue(L"FileVersion"));
    result.Add(ver.GetValue(L"InternalName"));
    result.Add(ver.GetValue(L"LegalCopyright"));
    result.Add(ver.GetValue(L"LegalTrademarks"));
    result.Add(ver.GetValue(L"OriginalFilename"));
    result.Add(ver.GetValue(L"PrivateBuild"));
    result.Add(ver.GetValue(L"ProductName"));
    result.Add(ver.GetValue(L"ProductVersion"));
    result.Add(ver.GetValue(L"SpecialBuild"));

    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_fileowner
int CDOSLibApp::ads_dos_fileowner()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  wchar_t buffer[512] = L"";
  unsigned int uRes = DOS_FileOwner(strPath, buffer, 512);
  if (S_OK == uRes)
    acedRetStr(buffer);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_readtextfile
int CDOSLibApp::ads_dos_readtextfile()
{
  CAdsArgs args;

  bool bSkipEmptyRows = false;
  bool bTrimSpaces = false;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetBoolean(bSkipEmptyRows))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetBoolean(bTrimSpaces))
        return RSERR;
    }
  }

  CDosUnicodeTextFile f;
  if (!f.OpenToRead(strPath))
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  CString str;
  while (f.ReadString(str))
  {
    str.Remove('\r'); // Remove carridge-returns
    str.Remove('\n'); // Remove line-feeds

    if (bTrimSpaces)
    {
      str.TrimLeft();
      str.TrimRight();
    }

    if (str.GetLength())
      result.Add(str, true);
    else if (!bSkipEmptyRows)
      result.Add(false);
  }
  f.Close();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_writetextfile
int CDOSLibApp::ads_dos_writetextfile()
{
  CAdsArgs args;

  bool bAppend = false;
  int encoding = 1;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  CStringArray strings;
  if (!args.GetStringList(strings, true))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetBoolean(bAppend))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(encoding))
        return RSERR;

      encoding = DOS_CLAMP(encoding, 0, 2);
    }
  }

  CDosUnicodeTextFile::filetype filetype = CDosUnicodeTextFile::utf8_filetype;
  switch (encoding)
  {
  case 0:
    filetype = CDosUnicodeTextFile::ansi_filetype;
    break;
  case 1:
    filetype = CDosUnicodeTextFile::utf8_filetype;
    break;
  case 2:
    filetype = CDosUnicodeTextFile::utf16_filetype;
    break;
  }

  if (!strPath.FileExists())
    bAppend = false;

  CDosUnicodeTextFile f;
  if (bAppend)
  {
    if (!f.OpenToAppend(strPath))
    {
      acedRetNil();
      return RSRSLT;
    }
  }
  else
  {
    if (!f.OpenToWrite(strPath, filetype))
    {
      acedRetNil();
      return RSRSLT;
    }
  }

  CString str;
  bool rc = true;
  int i, count = 0;
  for (i = 0; rc && i < strings.GetCount(); i++)
  {
    CString tmp = strings[i];
    tmp.Remove('\r'); // Remove carridge-returns
    tmp.Remove('\n'); // Remove line-feeds
    str.Format(L"%s\n", tmp);
    rc = f.WriteString(str);
    if (rc)
      count++;
  }
  f.Close();

  acedRetInt(count);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_readdelimitedfile

static int ParseDelimitedString(const wchar_t* string, wchar_t delimiter, CStringArray& tokens)
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

    // eat leading whitespace characters
    if (ch == ' ' && str.IsEmpty())
    {
      line_pos++;
      continue;
    }

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
      return (int)tokens.GetCount();
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

int CDOSLibApp::ads_dos_readdelimitedfile()
{
  CAdsArgs args;

  CString strDelimiter = L",";
  bool bSkipEmptyRows = false;
  bool bTrimSpaces = false;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetString(strDelimiter))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetBoolean(bSkipEmptyRows))
        return RSERR;

      args.Next();

      if (!args.IsEmpty())
      {
        if (!args.GetBoolean(bTrimSpaces))
          return RSERR;
      }
    }
  }

  CDosUnicodeTextFile f;
  if (!f.OpenToRead(strPath))
  {
    acedRetNil();
    return RSRSLT;
  }

  CArray<CDosStringArray, CDosStringArray&> grid;
  int num_cols = 0;

  CString str;
  while (f.ReadString(str))
  {
    str.Remove('\r'); // Remove carridge-returns
    str.Remove('\n'); // Remove line-feeds

    if (bTrimSpaces)
    {
      str.TrimLeft();
      str.TrimRight();
    }

    CDosStringArray tokens;
    int count = ParseDelimitedString(str, strDelimiter[0], tokens);

    if (count > num_cols)
      num_cols = count;

    if (count || !bSkipEmptyRows)
      grid.Add(tokens);
  }

  if (0 == (int)grid.GetCount())
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  int i, j;
  for (i = 0; i < (int)grid.GetCount(); i++)
  {
    result.AddLB();

    CDosStringArray& tokens = grid[i];
    for (j = 0; j < num_cols; j++)
    {
      bool bAdded = false;

      if (j < (int)tokens.GetCount())
      {
        CString tok = tokens[j];
        tok.TrimLeft();
        tok.TrimRight();

        int n = 0;
        double d = 0.0;

        if (tok.IsEmpty())
          bAdded = result.Add(false);
        else if (DOS_SimpleIntegerParse(tok, n))
          bAdded = result.Add(n);
        else if (DOS_SimpleFloatParse(tok, d))
          bAdded = result.Add(d);
        else
        {
          CString str = tokens[j];
          if (bTrimSpaces)
          {
            str.TrimLeft();
            str.TrimRight();
          }
          if (str.IsEmpty())
            bAdded = result.Add(false);
          else
            bAdded = result.Add(str, true);
        }
      }

      if (!bAdded)
        result.Add(false);
    }

    result.AddLE();
    tokens.RemoveAll(); // release some memory...
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

int CDOSLibApp::ads_dos_isfilename()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  CString strFileName = strPath.GetFileName();
  if (strFileName.IsEmpty())
  {
    acedRetNil();
  }
  else
  {
    if (0 == DOS_IsValidFileName(strFileName))
      acedRetT();
    else
      acedRetNil();
  }

  return RSRSLT;
}