/////////////////////////////////////////////////////////////////////////////
// DosFileOperation.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosFileOperation.h"

CDosFileOperation::CDosFileOperation()
{
  Initialize();
}

CDosFileOperation::~CDosFileOperation()
{
}

bool CDosFileOperation::AddSourceFile(const wchar_t* pszPath)
{
  if (0 == pszPath || 0 == pszPath[0])
    return false;

  try
  {
    m_SourceFiles.AddTail(pszPath);
  }
  catch (...)
  {
    return false;
  }

  return true;
}

bool CDosFileOperation::AddDestFile(const wchar_t* pszPath)
{
  if (0 == pszPath || 0 == pszPath[0])
    return false;

  try
  {
    m_DestFiles.AddTail(pszPath);
  }
  catch (...)
  {
    return false;
  }

  return true;
}

const CStringList& CDosFileOperation::GetSourceFileList()
{
  return m_SourceFiles;
}

const CStringList& CDosFileOperation::GetDestFileList()
{
  return m_DestFiles;
}

bool CDosFileOperation::SetOperationFlags(
  UINT uType,
  CWnd* pWnd,
  bool bSilent,
  bool bAllowUndo,
  bool bWildcardFilesOnly,
  bool bNoConfirmation,
  bool bNoConfirmationToMakeDir,
  bool bRenameOnCollision,
  bool bSimpleProgressDlg
  )
{
  if (uType != FO_COPY && uType != FO_DELETE && uType != FO_MOVE && uType != FO_RENAME)
    return false;

  if (0 == pWnd || !::IsWindow(pWnd->GetSafeHwnd()))
    return false;

  m_fileop.wFunc = uType;
  m_fileop.hwnd = pWnd->GetSafeHwnd();

  FILEOP_FLAGS fFlags = 0;
  if (bSilent)                  fFlags |= FOF_SILENT;
  if (bAllowUndo)               fFlags |= FOF_ALLOWUNDO;
  if (bWildcardFilesOnly)       fFlags |= FOF_FILESONLY;
  if (bNoConfirmation)          fFlags |= FOF_NOCONFIRMATION;
  if (bNoConfirmationToMakeDir) fFlags |= FOF_NOCONFIRMMKDIR;
  if (bRenameOnCollision)       fFlags |= FOF_RENAMEONCOLLISION;
  if (bSimpleProgressDlg)       fFlags |= FOF_SIMPLEPROGRESS;
  m_fileop.fFlags = fFlags;

  m_bFlags = true;

  return true;
}

bool CDosFileOperation::SetOperationFlags(UINT uType, CWnd* pWnd, FILEOP_FLAGS fFlags)
{
  if (uType != FO_COPY && uType != FO_DELETE && uType != FO_MOVE && uType != FO_RENAME)
    return false;

  if (0 == pWnd || !::IsWindow(pWnd->GetSafeHwnd()))
    return false;

  m_fileop.wFunc = uType;
  m_fileop.hwnd = pWnd->GetSafeHwnd();
  m_fileop.fFlags = fFlags;

  m_bFlags = true;

  return true;
}

void CDosFileOperation::SetProgressDlgTitle(const wchar_t* pszTitle)
{
  ASSERT(AfxIsValidString(pszTitle));
  try
  {
    m_strProgressTitle = pszTitle;
  }
  catch (...)
  {
  }
}

bool CDosFileOperation::Aborted() const
{
  if (!m_bCalled)
    return false;

  return (m_fileop.fAnyOperationsAborted ? true : false);
}

void CDosFileOperation::Initialize()
{
  m_bFlags = false;
  m_bCalled = false;
  m_SourceFiles.RemoveAll();
  m_DestFiles.RemoveAll();
  m_strProgressTitle.Empty();
  ZeroMemory(&m_fileop, sizeof(m_fileop));
}

bool CDosFileOperation::Go(bool* bStarted, int* nResult, bool* bAborted)
{
  m_bCalled = false;

  if (bStarted)
    *bStarted = false;

  if (!m_bFlags)
    return false;

  if (m_fileop.wFunc != FO_COPY && m_fileop.wFunc != FO_DELETE && m_fileop.wFunc != FO_MOVE && m_fileop.wFunc != FO_RENAME)
    return false;

  if (m_SourceFiles.IsEmpty())
    return false;

  if (m_fileop.wFunc != FO_DELETE && m_DestFiles.IsEmpty())
    return false;

  if (m_fileop.wFunc != FO_DELETE)
  {
    if (m_DestFiles.GetCount() != 1 && m_DestFiles.GetCount() != m_SourceFiles.GetCount())
      return false;
  }

  DWORD dwSourceBufferSize = GetBufferSize(m_SourceFiles);
  DWORD dwDestBufferSize = 0;
  if (m_fileop.wFunc != FO_DELETE)
    dwDestBufferSize = GetBufferSize(m_DestFiles);

  wchar_t* pszSourceFiles = 0;
  wchar_t* pszDestFiles = 0;

  try
  {
    pszSourceFiles = new wchar_t[dwSourceBufferSize];
    if (m_fileop.wFunc != FO_DELETE)
      pszDestFiles = new wchar_t[dwDestBufferSize];
  }
  catch (...)
  {
    return false;
  }

  FillBuffer(m_SourceFiles, pszSourceFiles);

  if (m_fileop.wFunc != FO_DELETE)
    FillBuffer(m_DestFiles, pszDestFiles);

  m_fileop.pFrom = pszSourceFiles;
  m_fileop.pTo = pszDestFiles;
  m_fileop.lpszProgressTitle = (const wchar_t*)m_strProgressTitle;

  if (m_DestFiles.GetCount() > 1)
    m_fileop.fFlags |= FOF_MULTIDESTFILES;

  m_bCalled = true;

  if (bStarted)
    *bStarted = true;

  int rc = SHFileOperation(&m_fileop);

  // Save the return value from the API.    
  if (nResult)
    *nResult = rc;

  // Check if the user cancelled the operation
  if (bAborted)
    *bAborted = (m_fileop.fAnyOperationsAborted ? true : false);

  if (pszSourceFiles)
    delete[] pszSourceFiles;

  if (pszDestFiles)
    delete[] pszDestFiles;

  return (m_bCalled && 0 == rc);
}

DWORD CDosFileOperation::GetBufferSize(const CStringList& list)
{
  if (list.IsEmpty())
    return 0;

  DWORD dwResult = 0;
  POSITION pos = list.GetHeadPosition();
  while (0 != pos)
  {
    CString str = list.GetNext(pos);
    dwResult += sizeof(wchar_t) * (str.GetLength() + 1);
  }
  return (dwResult + sizeof(wchar_t)); // add one more for the final null
}

void CDosFileOperation::FillBuffer(const CStringList& list, wchar_t* pszBuffer)
{
  if (list.IsEmpty() || 0 == pszBuffer)
    return;

  wchar_t* pszCurrent = pszBuffer;
  POSITION pos = list.GetHeadPosition();
  while (0 != pos)
  {
    CString str = list.GetNext(pos);
    wcscpy(pszCurrent, str);
    pszCurrent = _wcsinc(wcschr(pszCurrent, '\0'));
  }

  *pszCurrent = '\0';
}
