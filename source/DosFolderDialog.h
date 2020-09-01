/////////////////////////////////////////////////////////////////////////////
// DosFolderDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosFolderDialog
{
public:
  CDosFolderDialog(HWND hWnd = NULL);
  virtual ~CDosFolderDialog();

  void SetDialogCaption(const wchar_t* pszCaption);
  void SetDialogTitle(const wchar_t* pszTitle);
  void SetDialogFolder(const wchar_t* pszFolder);
  void SetDialogFlags(UINT ulFlags);

  virtual int DoModal();

  CString GetFolderName() const;

protected:
  virtual void OnInitDialog();
  virtual void OnSelChanged(LPITEMIDLIST lpItemIDList);
  virtual void CallbackFunction(HWND hWnd, UINT uMsg, LPARAM lParam);

  void EnableOK(BOOL bEnable = TRUE);
  void SetTitle(const wchar_t* pszTitle);
  void SetStatusText(const wchar_t* pszStatusText);
  void SetSelection(const wchar_t* pszSelection);
  void SetSelection(LPITEMIDLIST lpItemIDList);
  void SetExpanded(LPITEMIDLIST lpItemIDList);
  void SetExpanded(const wchar_t* pszFolder);

  friend static int CALLBACK BrowseDirectoryCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

protected:
  UINT m_ulFlags;
  HWND m_hWnd;
  CString m_strCaption;
  CString m_strTitle;
  CString m_strFolder;
  CString m_strDisplayName;
};
