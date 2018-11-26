/////////////////////////////////////////////////////////////////////////////
// DosReadDir.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

using namespace std;

class CDosReadDir
{
public:
  CDosReadDir();
  virtual ~CDosReadDir();

  bool&	Recurse() { return m_bRecurse; }
  bool GetDirs(const wchar_t * pDirPath);
  bool GetFiles(const wchar_t *pFilemask);
  bool ClearDirs();
  bool ClearFiles();

  enum { eSortNone, eSortAlpha, eSortWriteDate, eSortSize };
  bool SortFiles(int iSortStyle, bool bReverse);
  bool SortDirs();

  struct CDirEntry
  {
    CDirEntry() {}
    CDirEntry(const CString &s)
    {
      m_sName = s;
    }
    CString m_sName;
  };

  typedef vector<CDirEntry> DirVector;

  DirVector &Dirs() { return m_dirs; }

  struct CFileEntry
  {
    CString	m_sName;
    unsigned int attrib;
    unsigned __int64 time_create;
    unsigned __int64 time_access;
    unsigned __int64 time_write;
    unsigned __int64 size;
  };

  typedef vector<CFileEntry> FileVector;

  FileVector &Files() { return m_files; }

protected:
  bool GetSubDirs(DirVector& dir_array, const CString& path);
  UINT FindFiles(const CString& dir, const CString& filter);
  void FormatPath(CString& path);

  bool m_bRecurse;
  CString	m_sSourceDir;

  DirVector m_dirs;
  FileVector m_files;
};
