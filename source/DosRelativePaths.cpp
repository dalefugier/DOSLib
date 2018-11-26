/////////////////////////////////////////////////////////////////////////////
// DosRelativePaths.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosRelativePaths.h"

CCriticalSection CDosRelativePaths::sm_cs;

CString CDosRelativePaths::GetFolderAncestor(const CString& sDir, int nLevel)
{
  //What will be the return value
  CString sAncestor(sDir);

  //Remove the trailing "\" if any
  int nAncestorLength = sAncestor.GetLength();
  ASSERT(nAncestorLength);
  if ((nAncestorLength > 1) && sAncestor.GetAt(nAncestorLength - 1) == '\\')
    sAncestor = sAncestor.Left(nAncestorLength - 1);

  //Find the specified amount of folders up
  for (int i = 0; i < nLevel && sAncestor.GetLength(); i++)
  {
    int nIndex = sAncestor.ReverseFind('\\');
    if (nIndex != -1)
    {
      sAncestor = sAncestor.Left(nIndex);
      if ((sDir != L"\\") && (i == (nLevel - 1)) && (sAncestor.IsEmpty()))
        sAncestor = L"\\";
    }
  }

  return sAncestor;
}

int CDosRelativePaths::FoldersUpToRootLevel(const CString& sDir)
{
  //What will be the return value
  CString sAncestor(sDir);

  //Remove the trailing "\" if any
  int nAncestorLength = sAncestor.GetLength();
  ASSERT(nAncestorLength);
  if (sAncestor.GetAt(nAncestorLength - 1) == '\\')
    sAncestor = sAncestor.Left(nAncestorLength - 1);

  int nLevels = -1;
  while (sAncestor.GetLength())
  {
    int nIndex = sAncestor.ReverseFind('\\');
    if (nIndex != -1)
    {
      sAncestor = sAncestor.Left(nIndex);
      nLevels++;
    }
    else
      sAncestor.Empty();
  }

  return nLevels;
}

int CDosRelativePaths::GetAncestorLevel(const CString& sDir, const CString& sSubDir)
{
  //By default assume failure
  int nLevel = -1;

  //What will be the return value
  CString sLocalDir(sDir);

  if (sLocalDir.Find(sSubDir) == 0)
    nLevel = FoldersUpToRootLevel(sDir) - FoldersUpToRootLevel(sSubDir);

  return nLevel;
}

CString CDosRelativePaths::AbsoluteToRelative(const CString& sAbsolutePath, const CString& sRelativeTo)
{
  //What will be the return value
  CString sRelative;

  //Break up the absolute path into its constituent parts
  TCHAR pszAbsoluteDrive[_MAX_DRIVE];
  TCHAR pszAbsoluteDir[_MAX_DIR];
  TCHAR pszAbsoluteFname[_MAX_PATH];
  TCHAR pszAbsoluteExt[_MAX_EXT];
  _tsplitpath(sAbsolutePath, pszAbsoluteDrive, pszAbsoluteDir, pszAbsoluteFname, pszAbsoluteExt);
  CString sAbsoluteDir(pszAbsoluteDir);

  //Break up the relativeto path into its constituent parts
  TCHAR pszRelativeToDrive[_MAX_DRIVE];
  TCHAR pszRelativeToDir[_MAX_DIR];
  _tsplitpath(sRelativeTo, pszRelativeToDrive, pszRelativeToDir, NULL, NULL);
  CString sRelativeToDir(pszRelativeToDir);

  //We can only conver an absolute path to a relative path if the paths are on the same drive or UNC machine 
  if (_tcsicmp(pszAbsoluteDrive, pszRelativeToDrive) == 0)
  {
    BOOL bMatch = FALSE;
    int nRelativeUpLevel = 0;
    CString sRelativeAncestor;
    while (!bMatch)
    {
      sRelativeAncestor = GetFolderAncestor(sRelativeToDir, nRelativeUpLevel);
      if (sRelativeAncestor != L"\\")
      {
        ASSERT(sRelativeAncestor.GetLength());
        if (sAbsoluteDir.Find(sRelativeAncestor) == 0)
        {
          bMatch = TRUE;
          nRelativeUpLevel = GetAncestorLevel(sRelativeToDir, sRelativeAncestor);

          for (int i = 0; i < nRelativeUpLevel; i++)
            sRelative += L"..\\";

          sRelative += sAbsoluteDir.Right(sAbsoluteDir.GetLength() - sRelativeAncestor.GetLength() - 1);
          sRelative += pszAbsoluteFname;
          sRelative += pszAbsoluteExt;
        }
        else
          nRelativeUpLevel++;
      }
      else
      {
        //No matches between the absolute and relative path in that case then the number of levels up
        //for the relative path corresponds to the number of directory levels in sRelativeTo
        bMatch = TRUE;
        nRelativeUpLevel = FoldersUpToRootLevel(sRelativeTo);

        for (int i = 0; i < nRelativeUpLevel; i++)
          sRelative += L"..\\";

        sRelative += sAbsoluteDir.Right(sAbsoluteDir.GetLength() - sRelativeAncestor.GetLength());
        sRelative += pszAbsoluteFname;
        sRelative += pszAbsoluteExt;
      }
    }
  }
  else
    sRelative = sAbsolutePath;

  return sRelative;
}

CString CDosRelativePaths::RelativeToAbsolute(const CString& sRelativePath, const CString& sRelativeTo)
{
  //Serialize access to this function
  CSingleLock sl(&sm_cs, TRUE);

  //Break up the relativeto path into its constituent parts
  TCHAR pszRelativeToDrive[_MAX_DRIVE];
  TCHAR pszRelativeToDir[_MAX_DIR];
  _tsplitpath(sRelativeTo, pszRelativeToDrive, pszRelativeToDir, NULL, NULL);
  TCHAR pszRelativeToDirectory[_MAX_PATH];
  _tmakepath(pszRelativeToDirectory, pszRelativeToDrive, pszRelativeToDir, NULL, NULL);

  //What will be the return value
  TCHAR pszAbsolutePath[_MAX_PATH];
  _tcscpy(pszAbsolutePath, L""); //by default set it to an empty string

  //Only makes sense to convert relative to absolute if sRelativeTo contains
  //a drive letter or is a UNC path
  if (_tcslen(pszRelativeToDrive) || (_tcsstr(pszRelativeToDir, L"\\\\") == pszRelativeToDir))
  {
    //Remember the old current directory before we change it
    TCHAR pszOldCurrentDirectory[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, pszOldCurrentDirectory);

    //Temporarily set the current directory to the relativeto directory
    if (SetCurrentDirectory(pszRelativeToDirectory))
    {
      //Convert the relative path to an absolute path
      LPTSTR pszFilePart;
      GetFullPathName(sRelativePath, _MAX_PATH, pszAbsolutePath, &pszFilePart);

      //Restore the current working directory
      SetCurrentDirectory(pszOldCurrentDirectory);
    }
    else
    {
      //Just return the relative path
      _tcscpy(pszAbsolutePath, sRelativePath);
    }
  }
  else
  {
    //Just return the relative path
    _tcscpy(pszAbsolutePath, sRelativePath);
  }

  return pszAbsolutePath;
}
