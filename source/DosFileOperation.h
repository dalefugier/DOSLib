/////////////////////////////////////////////////////////////////////////////
// DosFileOperation.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosFileOperation
{
public:
  CDosFileOperation();
  ~CDosFileOperation();

public:
  void Initialize();

  bool AddSourceFile(const wchar_t* pszPath);
  bool AddDestFile(const wchar_t* pszPath);

  const CStringList& GetSourceFileList();
  const CStringList& GetDestFileList();

  bool SetOperationFlags(
    UINT uType,
    CWnd* pWnd,
    bool bSilent,
    bool bAllowUndo,
    bool bWildcardFilesOnly,
    bool bNoConfirmation,
    bool bNoConfirmationToMakeDir,
    bool bRenameOnCollision,
    bool bSimpleProgressDlg
    );

  bool SetOperationFlags(UINT uType, CWnd* pWnd, FILEOP_FLAGS fFlags);

  void SetProgressDlgTitle(const wchar_t* pszTitle);

  bool Go(bool* bStarted, int* nResult = 0, bool* bAborted = 0);

  bool Aborted() const;

protected:
  bool m_bFlags;
  bool m_bCalled;
  CStringList m_SourceFiles;
  CStringList m_DestFiles;
  CString m_strProgressTitle;
  SHFILEOPSTRUCT m_fileop;

protected:
  DWORD GetBufferSize(const CStringList& src);
  void FillBuffer(const CStringList& src, wchar_t* psz);
};
