//////////////////////////////////////////////////////////////////////
// PathString.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosPathString.h"

void CDosPathString::RemoveRoot()
{
  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(*this, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
  CString str(dir);
  str.TrimLeft('\\');
  str += fname;
  str += ext;
  *this = str;
}

CString CDosPathString::GetDrive(bool bAddBackslash)
{
  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(*this, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
  CDosPathString str(drive);
  if (bAddBackslash)
    str.AddBackslash();
  return str;
}

CString CDosPathString::GetPath(bool bAddBackslash)
{
  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(*this, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
  CDosPathString str = drive;
  str += dir;
  str.RemoveBackslash();
  if (bAddBackslash)
    str.AddBackslash();
  return str;
}

CString CDosPathString::GetFileName()
{
  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(*this, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
  CString str = fname;
  str += ext;
  return str;
}

CString CDosPathString::GetExtension()
{
  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(*this, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
  CString str = ext;
  return str;
}

void CDosPathString::AddBackslash()
{
  PathAddBackslash(GetBuffer(_MAX_PATH));
  ReleaseBuffer();
}

BOOL CDosPathString::AddExtension(LPCTSTR pszExt)
{
  LPTSTR pszBuffer = GetBuffer(_MAX_PATH);
  BOOL b = PathAddExtension(pszBuffer, pszExt);
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::Append(LPCTSTR pszMore)
{
  BOOL b = PathAppend(GetBuffer(_MAX_PATH), pszMore);
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::BuildRoot(int nDrive)
{
  LPTSTR lp = PathBuildRoot(GetBuffer(_MAX_DRIVE), nDrive);
  ReleaseBuffer();
  return lp ? TRUE : FALSE;
}

BOOL CDosPathString::Canonicalize()
{
  BOOL b = PathCanonicalize(GetBuffer(_MAX_PATH), *this);
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::Combine(LPCTSTR pszDir, LPCTSTR pszFile)
{
  LPTSTR lp = PathCombine(GetBuffer(_MAX_PATH), pszDir, pszFile);
  ReleaseBuffer();
  return lp ? TRUE : FALSE;
}

BOOL CDosPathString::CompactPath(HDC hDC, UINT dx)
{
  BOOL b = PathCompactPath(hDC, GetBuffer(GetLength()), dx);
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::CompactPathEx(UINT cchMax)
{
  CString str;
  BOOL b = PathCompactPathEx(str.GetBuffer(_MAX_PATH), *this, cchMax, 0);
  str.ReleaseBuffer();
  if (b)
    *this = str;
  return b;
}

BOOL CDosPathString::CreateFromUrl(LPCTSTR pszUrl)
{
  DWORD dwLength = _MAX_PATH;
  HRESULT hr = PathCreateFromUrl(pszUrl, GetBuffer(dwLength), &dwLength, 0);
  ReleaseBuffer();
  return (hr == S_OK) ? TRUE : FALSE;
};

BOOL CDosPathString::FileExists()
{
  return PathFileExists(*this);
}

BOOL CDosPathString::FullPath()
{
  CString str;
  LPTSTR psz = str.GetBuffer(_MAX_PATH);
  BOOL b = (_wfullpath(psz, *this, _MAX_PATH)) ? TRUE : FALSE;
  str.ReleaseBuffer();
  if (b)
    *this = str;
  return b;
}

BOOL CDosPathString::LongPath()
{
  CString str;
  BOOL b = GetLongPathName(*this, str.GetBuffer(_MAX_PATH), _MAX_PATH) ? TRUE : FALSE;
  str.ReleaseBuffer();
  if (b)
    *this = str;
  return b;
}

BOOL CDosPathString::ShortPath()
{
  CString str;
  BOOL b = GetShortPathName(*this, str.GetBuffer(_MAX_PATH), _MAX_PATH) ? TRUE : FALSE;
  str.ReleaseBuffer();
  if (b)
    *this = str;
  return b;
}

BOOL CDosPathString::IsDirectory()
{
  int rc = PathIsDirectory(*this);
  return (rc == FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
};

BOOL CDosPathString::IsDirectoryEmpty()
{
  return PathIsDirectoryEmpty(*this);
};

BOOL CDosPathString::IsFileSpec()
{
  return PathIsFileSpec(*this);
}

BOOL CDosPathString::IsHTMLFile()
{
  return PathIsHTMLFile(*this);
}

BOOL CDosPathString::IsLFNFileSpec()
{
  return PathIsLFNFileSpec(*this);
}

BOOL CDosPathString::IsNetworkPath()
{
  return PathIsNetworkPath(*this);
}

BOOL CDosPathString::IsPrefix(LPCTSTR pszPrefix)
{
  return PathIsPrefix(pszPrefix, *this);
}

BOOL CDosPathString::IsRelative()
{
  return PathIsRelative(*this);
}

BOOL CDosPathString::IsRoot()
{
  return PathIsRoot(*this);
}

BOOL CDosPathString::IsSameRoot(LPCTSTR pszPath)
{
  return PathIsSameRoot(*this, pszPath);
}

BOOL CDosPathString::IsSystemFolder()
{
  return PathIsSystemFolder(*this, FILE_ATTRIBUTE_SYSTEM);
}

BOOL CDosPathString::IsUNC()
{
  return PathIsUNC(*this);
}

BOOL CDosPathString::IsUNCServer()
{
  return PathIsUNCServer(*this);
}

BOOL CDosPathString::IsUNCServerShare()
{
  return PathIsUNCServerShare(*this);
}

BOOL CDosPathString::IsURL()
{
  return PathIsURL(*this);
}

BOOL CDosPathString::MakePretty()
{
  BOOL b = PathMakePretty(GetBuffer(_MAX_PATH));
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::MatchSpec(LPCTSTR pszSpec)
{
  return PathMatchSpec(*this, pszSpec);
}

int CDosPathString::ParseIconLocation()
{
  int n = PathParseIconLocation(GetBuffer(GetLength()));
  ReleaseBuffer();
  return n;
}

void CDosPathString::QuoteSpaces()
{
  PathQuoteSpaces(GetBuffer(_MAX_PATH));
  ReleaseBuffer();
}

BOOL CDosPathString::RelativePathTo(LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo)
{
  BOOL b = PathRelativePathTo(GetBuffer(_MAX_PATH), pszFrom, dwAttrFrom, pszTo, dwAttrTo);
  ReleaseBuffer();
  return TRUE;
}

void CDosPathString::RemoveArgs()
{
  PathRemoveArgs(GetBuffer(GetLength()));
  ReleaseBuffer();
}

void CDosPathString::RemoveBackslash()
{
  if (PathIsRoot(*this))
    Remove('\\');
  else
    PathRemoveBackslash(GetBuffer(GetLength()));
  ReleaseBuffer();
}

void CDosPathString::RemoveBlanks()
{
  PathRemoveBlanks(GetBuffer(GetLength()));
  ReleaseBuffer();
}

void CDosPathString::RemoveExtension()
{
  PathRemoveExtension(GetBuffer(GetLength()));
  ReleaseBuffer();
}

BOOL CDosPathString::RemoveFileSpec()
{
  BOOL b = PathRemoveFileSpec(GetBuffer(GetLength()));
  ReleaseBuffer();
  return b;
}

BOOL CDosPathString::RenameExtension(LPCTSTR pszExt)
{
  BOOL b = PathRenameExtension(GetBuffer(_MAX_PATH), pszExt);
  ReleaseBuffer();
  return b;
}

void CDosPathString::SkipRoot()
{
  *this = PathSkipRoot(*this);
}

BOOL CDosPathString::StripToRoot()
{
  BOOL b = PathStripToRoot(GetBuffer(GetLength()));
  ReleaseBuffer();
  return b;
}

void CDosPathString::UnquoteSpaces()
{
  PathUnquoteSpaces(GetBuffer(GetLength()));
  ReleaseBuffer();
}


