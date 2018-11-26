/////////////////////////////////////////////////////////////////////////////
// DosReadDir.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosReadDir.h"

#include <io.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>

bool asc_alpha_dir_sort(const CDosReadDir::CDirEntry& a, const CDosReadDir::CDirEntry& b);
bool asc_date_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);
bool asc_size_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);
bool asc_alpha_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);
bool dsc_date_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);
bool dsc_size_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);
bool dsc_alpha_file_sort(const CDosReadDir::CFileEntry& a, const CDosReadDir::CFileEntry& b);


CDosReadDir::CDosReadDir()
{
  m_bRecurse = false;
}

CDosReadDir::~CDosReadDir()
{
}

bool CDosReadDir::GetDirs(const wchar_t* pDirPath)
{
  m_sSourceDir = pDirPath;

  FormatPath(m_sSourceDir);

  // find tree starting from pDirPath
  if (m_bRecurse)
  {
    GetSubDirs(m_dirs, m_sSourceDir);
  }

  // add the main directory. 
  m_dirs.push_back(CDirEntry(m_sSourceDir));

  return true;
}


bool CDosReadDir::GetSubDirs(vector<CDosReadDir::CDirEntry> &dir_array, const CString &path)
{
  CString newPath;

  CString searchString;
  searchString = path;
  searchString += L"\\*.*";

  try
  {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    if ((hFind = FindFirstFile(searchString, &FindFileData)) != INVALID_HANDLE_VALUE)
    {
      // we only care about files, not subdirs
      if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
      {
        // we'll handle parents on our own
        if ((wcscmp(FindFileData.cFileName, L".") != 0) && (wcscmp(FindFileData.cFileName, L"..") != 0))
        {
          newPath = path;
          newPath += L"\\";
          newPath += FindFileData.cFileName;
          GetSubDirs(dir_array, newPath);
          dir_array.push_back(newPath);
        }
      }

      // find the rest of them	
      while (FindNextFile(hFind, &FindFileData))
      {
        // we only care about files, not subdirs
        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          // we'll handle parents on our own
          if ((wcscmp(FindFileData.cFileName, L".") != 0) && (wcscmp(FindFileData.cFileName, L"..") != 0))
          {
            newPath = path;
            newPath += L"\\";
            newPath += FindFileData.cFileName;
            GetSubDirs(dir_array, newPath);
            dir_array.push_back(newPath);
          }

        }
      }
    }
    FindClose(hFind);
  }
  catch (...)
  {
    return false;
  }

  return true;
}


bool CDosReadDir::ClearFiles()
{
  m_files.clear();
  return true;
}


bool CDosReadDir::ClearDirs()
{
  m_dirs.clear();
  return true;
}


bool CDosReadDir::GetFiles(const wchar_t* pFilemask)
{
  // get the files in each of our directories
  for (vector<CDirEntry>::iterator it = m_dirs.begin(); it != m_dirs.end(); it++)
  {
    CString curDir = (*it).m_sName;

    // nity check
    if (curDir.IsEmpty())
    {
      continue;
    }

    if (!FindFiles(curDir, pFilemask))
    {
      return false;
    }
  }

  return true;
}


bool CDosReadDir::SortFiles(int iSortStyle, bool bReverse)
{
  switch (iSortStyle)
  {
  default:
    break;
  case eSortAlpha:
    std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_alpha_file_sort : asc_alpha_file_sort));
    break;
  case eSortWriteDate:
    std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_date_file_sort : asc_date_file_sort));
    break;
  case eSortSize:
    std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_size_file_sort : asc_size_file_sort));
    break;
  }

  return true;
}


bool CDosReadDir::SortDirs()
{
  std::sort(m_dirs.begin(), m_dirs.end(), (asc_alpha_dir_sort));

  return true;
}


// chop off trailing "\"
void CDosReadDir::FormatPath(CString &path)
{
  CString inPath = path;
  inPath.TrimRight();
  CString tmp;

  int iLastSlashPos = inPath.ReverseFind('\\');
  if (iLastSlashPos == -1)
  {
    iLastSlashPos = inPath.ReverseFind('/');
  }

  if (iLastSlashPos != -1)
  {
    if (iLastSlashPos == inPath.GetLength() - 1)
    {
      path = inPath.Left(iLastSlashPos);

      FormatPath(path); // in case the incoming path is "C:\temp\\\..."
    }
  }
}


UINT CDosReadDir::FindFiles(const CString & dir, const CString & filter)
{
  // make sure the path ends in a single "\"
  CString baseName = dir;
  FormatPath(baseName);
  baseName += '\\';

  CString fullPath = baseName;
  fullPath += filter;

  CString fileName;

  try
  {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    if ((hFind = FindFirstFile(fullPath, &FindFileData)) != INVALID_HANDLE_VALUE)
    {
      // we only care about files, not subdirs
      if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
      {
        if (m_bRecurse)
        {
          fileName = baseName;
          fileName += FindFileData.cFileName;
        }
        else
        {
          fileName = FindFileData.cFileName;
        }

        CFileEntry t;
        t.m_sName = fileName;

        ASSERT(sizeof(FindFileData.ftLastWriteTime) == sizeof(_FILETIME));
        memcpy(&t.time_write, &FindFileData.ftLastWriteTime, sizeof(_FILETIME));

        ASSERT(sizeof(FindFileData.ftLastWriteTime) == sizeof(_FILETIME));
        memcpy(&t.time_create, &FindFileData.ftCreationTime, sizeof(_FILETIME));

        t.size = ((unsigned __int64)FindFileData.nFileSizeHigh * ((unsigned __int64)MAXDWORD + 1)) + (unsigned __int64)FindFileData.nFileSizeLow;
        m_files.push_back(t);
      }

      // find the rest of them	
      while (FindNextFile(hFind, &FindFileData))
      {
        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
        {
          if (m_bRecurse)
          {
            fileName = baseName;
            fileName += FindFileData.cFileName;
          }
          else
          {
            fileName = FindFileData.cFileName;
          }

          CFileEntry t;
          t.m_sName = fileName;

          ASSERT(sizeof(FindFileData.ftLastWriteTime) == sizeof(_FILETIME));
          memcpy(&t.time_write, &FindFileData.ftLastWriteTime, sizeof(_FILETIME));

          ASSERT(sizeof(FindFileData.ftLastWriteTime) == sizeof(_FILETIME));
          memcpy(&t.time_create, &FindFileData.ftCreationTime, sizeof(_FILETIME));

          t.size = ((unsigned __int64)FindFileData.nFileSizeHigh * ((unsigned __int64)MAXDWORD + 1)) + (unsigned __int64)FindFileData.nFileSizeLow;
          m_files.push_back(t);
        }
      }
      FindClose(hFind);
    }
  }
  catch (...)
  {
    return false;
  }

  return true;
}


bool asc_alpha_dir_sort(const CDosReadDir::CDirEntry &a, const CDosReadDir::CDirEntry &b)
{
  return (_wcsicmp(a.m_sName, b.m_sName) < 0);
}


bool asc_alpha_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (_wcsicmp(a.m_sName, b.m_sName) < 0);
}


bool asc_date_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (a.time_write < b.time_write);
}


bool asc_size_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (a.size < b.size);
}


bool dsc_alpha_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (!asc_alpha_file_sort(a, b));
}


bool dsc_date_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (!asc_date_file_sort(a, b));
}


bool dsc_size_file_sort(const CDosReadDir::CFileEntry &a, const CDosReadDir::CFileEntry &b)
{
  return (!asc_size_file_sort(a, b));
}
