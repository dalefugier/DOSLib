/////////////////////////////////////////////////////////////////////////////
// DosFileDialog.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosFileDialog.h"

IMPLEMENT_DYNAMIC(CDosFileDialog, CFileDialog)

CDosFileDialog::CDosFileDialog(LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize)
  : CFileDialog(TRUE, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize, FALSE) // <- bVistaStyle = FALSE
  , m_files(0)
  , m_folder(0)
  , m_bParsed(false)
{
}

CDosFileDialog::~CDosFileDialog()
{
  if (m_files)
  {
    delete[] m_files;
    m_files = 0;
  }
  if (m_folder)
  {
    delete[] m_folder;
    m_folder = 0;
  }
}

BEGIN_MESSAGE_MAP(CDosFileDialog, CFileDialog)
END_MESSAGE_MAP()

INT_PTR CDosFileDialog::DoModal()
{
  if (m_files)
  {
    delete[] m_files;
    m_files = 0;
  }
  if (m_folder)
  {
    delete[] m_folder;
    m_folder = 0;
  }

  // Make sure explorer style flag is set
  m_ofn.Flags |= OFN_EXPLORER;

  INT_PTR rc = CFileDialog::DoModal();
  if (rc == IDCANCEL)
  {
    /*
    When OK is clicked, the file dialog will return IDOK if there were no errors.
    However, in our case, there will be an error as the default buffer was too small,
    so the file dialog will return IDCANCEL. So check for FNERR_BUFFERTOOSMALL using
    CommDlgExtendedError(). If that was the error, and we have used our own buffer,
    then return IDOK.
    */
    DWORD err = CommDlgExtendedError();
    if (err == FNERR_BUFFERTOOSMALL && m_files)
      rc = IDOK;
  }
  return rc;
}

POSITION CDosFileDialog::GetStartPosition() const
{
  if (0 == m_files)
    return CFileDialog::GetStartPosition();

  if (false == m_bParsed)
  {
    // Parse m_files so we end up with a string that contains
    // a number of null-terminated strings.
    CString str = m_files;
    str.Replace(L"\" \"", L"\"");
    str.Delete(0, 1);                     // remove leading quote mark
    str.Delete(str.GetLength() - 1, 1);   // remove trailing space

    wcscpy(m_files, str);

    wchar_t* ptr = m_files;
    while (*ptr)
    {
      if ('\"' == *ptr)
        *ptr = '\0';
      ++ptr;
    }
    const_cast<CDosFileDialog*>(this)->m_bParsed = true;
  }

  return (POSITION)m_files;
}

CString CDosFileDialog::GetNextPathName(POSITION& pos) const
{
  if (0 == m_files)
    return CFileDialog::GetNextPathName(pos);

  ASSERT(pos);
  wchar_t* ptr = (wchar_t*)pos;

  // Append the file to the folder
  CString str = m_folder;
  str += L"\\";
  str += ptr;

  ptr += wcslen(ptr) + 1;
  if (*ptr)
    pos = (POSITION)ptr;
  else
    pos = 0;

  return str;
}

void CDosFileDialog::OnFileNameChange()
{
  CWnd* pWnd = GetParent();
  ASSERT(pWnd);

  // Get the required size for the 'files' buffer
  UINT nFiles = CommDlg_OpenSave_GetSpec(pWnd->m_hWnd, 0, 0);

  // Get the required size for the 'folder' buffer
  UINT nFolder = CommDlg_OpenSave_GetFolderPath(pWnd->m_hWnd, 0, 0);

  // Check if m_ofn.nMaxFile are large enough
  if (nFiles + nFolder > m_ofn.nMaxFile)
  {
    /*
    See MSDN Q131462 - How To Handle FNERR_BUFFERTOOSMALL in Windows.
    Basically, when an application uses the Open File common dialog with
    the OFN_ALLOWMULTISELECT flag, there is a danger that the buffer passed
    to the common dialog in the OPENFILENAME.lpstrFile field will be too
    small. In this situation, an error will occur and CommDlgExtendedError()
    will return FNERR_BUFFERTOOSMALL.

    So, we will check to see if the buffer originally allocated is large enough
    to handle all the files selected. If not, we will allocate our own buffer
    and maintain the files list ourselves.
    */

    if (m_files)
    {
      delete[] m_files;
      m_files = 0;
    }
    m_files = new wchar_t[nFiles + 1];
    wmemset(m_files, 0, nFiles + 1);
    CommDlg_OpenSave_GetSpec(pWnd->m_hWnd, m_files, nFiles);

    if (m_folder)
    {
      delete[] m_folder;
      m_folder = 0;
    }
    m_folder = new wchar_t[nFolder + 1];
    wmemset(m_folder, 0, nFolder + 1);
    CommDlg_OpenSave_GetFolderPath(pWnd->m_hWnd, m_folder, nFolder);
  }
  else
  {
    if (m_files)
    {
      delete[] m_files;
      m_files = 0;
    }
    if (m_folder)
    {
      delete[] m_folder;
      m_folder = 0;
    }
  }

  CFileDialog::OnFileNameChange();
}