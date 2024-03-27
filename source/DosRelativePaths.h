/////////////////////////////////////////////////////////////////////////////
// DosRelativePaths.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosRelativePaths
{
public:
  static CString AbsoluteToRelative(const CString& strAbsolutePath, const CString& strRelativeTo);
  static CString RelativeToAbsolute(const CString& strRelativePath, const CString& strRelativeTo);

  static CString GetFolderAncestor(const CString& strDir, int nLevel);
  static int FoldersUpToRootLevel(const CString& strDir);
  static int GetAncestorLevel(const CString& strDir, const CString& strSubDir);

public:
  static CCriticalSection sm_cs;
};
