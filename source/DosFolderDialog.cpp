/////////////////////////////////////////////////////////////////////////////
// DosFolderDialog.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosFolderDialog.h"

static int CALLBACK BrowseDirectoryCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  CDosFolderDialog* pFd = (CDosFolderDialog*)lpData;
  pFd->CallbackFunction(hWnd, uMsg, lParam);
  return 0;
}

CDosFolderDialog::CDosFolderDialog(HWND hWnd)
{
  m_hWnd = hWnd;
  m_ulFlags = BIF_RETURNONLYFSDIRS;
  m_strCaption = L"Browse for folder";
  m_strFolder = L"";
  m_strTitle = L"Please select a folder below. Then click OK.";
  m_strDisplayName = L"";
}

CDosFolderDialog::~CDosFolderDialog()
{
}

void CDosFolderDialog::SetDialogCaption(const wchar_t* pszCaption)
{
  m_strCaption = pszCaption;
}

void CDosFolderDialog::SetDialogTitle(const wchar_t* pszTitle)
{
  m_strTitle = pszTitle;
}

void CDosFolderDialog::SetDialogFolder(const wchar_t* pszFolder)
{
  m_strFolder = pszFolder;
}

void CDosFolderDialog::SetDialogFlags(UINT ulFlags)
{
  m_ulFlags = ulFlags;
}

int CDosFolderDialog::DoModal()
{
  int nReturn = IDCANCEL;

  BROWSEINFO bi = { 0 };
  bi.hwndOwner = m_hWnd;
  bi.pidlRoot = NULL;
  bi.pszDisplayName = m_strDisplayName.GetBufferSetLength(_MAX_PATH);
  bi.lpszTitle = m_strTitle;
  bi.ulFlags = m_ulFlags;
  bi.lpfn = BrowseDirectoryCallback;
  bi.lParam = (LPARAM)this;

  LPITEMIDLIST lpItemIDList = ::SHBrowseForFolder(&bi);

  m_strDisplayName.ReleaseBuffer();

  if (lpItemIDList)
  {
    CString strFolder;
    BOOL bResult = ::SHGetPathFromIDList(lpItemIDList, strFolder.GetBufferSetLength(_MAX_PATH));
    strFolder.ReleaseBuffer();
    if (bResult)
    {
      m_strFolder = strFolder;
      nReturn = IDOK;
    }
    ::CoTaskMemFree(lpItemIDList);
  }

  return nReturn;
}

CString CDosFolderDialog::GetFolderName() const
{
  return m_strFolder;
}

void CDosFolderDialog::CallbackFunction(HWND hWnd, UINT uMsg, LPARAM lParam)
{
  m_hWnd = hWnd;
  switch (uMsg)
  {
  case BFFM_INITIALIZED:
    OnInitDialog();
    break;
  case BFFM_SELCHANGED:
    OnSelChanged((LPITEMIDLIST)lParam);
    break;
  }
}

void CDosFolderDialog::OnInitDialog()
{
  if (!m_strCaption.IsEmpty())
    ::SetWindowText(m_hWnd, m_strCaption);

  if (!m_strFolder.IsEmpty())
  {
    SetSelection(m_strFolder);
    SetExpanded(m_strFolder);
    SetStatusText(m_strFolder);
  }

  // Find and modify all static windows with the SS_LEFTNOWORDWRAP style
  HWND hWnd = NULL;
  while (true)
  {
    hWnd = ::FindWindowEx(m_hWnd, hWnd, L"Static", NULL);
    if (hWnd == NULL)
      break;
    CWnd* pWnd = CWnd::FromHandle(hWnd);
    if (pWnd)
      pWnd->ModifyStyle(0, SS_LEFTNOWORDWRAP);
  }
}

void CDosFolderDialog::OnSelChanged(LPITEMIDLIST lpItemIDList)
{
  CString strFolder;
  BOOL bResult = ::SHGetPathFromIDList(lpItemIDList, strFolder.GetBufferSetLength(_MAX_PATH));
  strFolder.ReleaseBuffer();
  if (bResult)
    SetStatusText(strFolder);
}

void CDosFolderDialog::EnableOK(BOOL bEnable)
{
  ::SendMessage(m_hWnd, BFFM_ENABLEOK, 0, (bEnable ? 1 : 0));
}

void CDosFolderDialog::SetSelection(const wchar_t* pszSelection)
{
  ::SendMessage(m_hWnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pszSelection);
}

void CDosFolderDialog::SetSelection(LPITEMIDLIST lpItemIDList)
{
  ::SendMessage(m_hWnd, BFFM_SETSELECTIONW, FALSE, (LPARAM)lpItemIDList);
}

void CDosFolderDialog::SetStatusText(const wchar_t* pszStatusText)
{
  CString strText = pszStatusText;
  CRect rect;
  ::GetClientRect(m_hWnd, rect);
  HDC hDC = ::GetDC(m_hWnd);
  PathCompactPath(hDC, strText.GetBufferSetLength(_MAX_PATH), rect.Width());
  strText.ReleaseBuffer();
  ::ReleaseDC(m_hWnd, hDC);
  ::SendMessage(m_hWnd, BFFM_SETSTATUSTEXTW, 0, (LPARAM)(const wchar_t*)strText);
}

void CDosFolderDialog::SetTitle(const wchar_t* pszStatusText)
{
  ::SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)(const wchar_t*)m_strTitle);
}

void CDosFolderDialog::SetExpanded(LPITEMIDLIST lpItemIDList)
{
  ::SendMessage(m_hWnd, BFFM_SETEXPANDED, FALSE, (LPARAM)lpItemIDList);
}

void CDosFolderDialog::SetExpanded(const wchar_t* pszFolder)
{
  ::SendMessage(m_hWnd, BFFM_SETEXPANDED, TRUE, (LPARAM)pszFolder);
}
