/////////////////////////////////////////////////////////////////////////////
// DosFileDialog.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosFileDialog : public CFileDialog
{
  DECLARE_DYNAMIC(CDosFileDialog)

public:
  CDosFileDialog(
    LPCTSTR lpszDefExt = NULL,
    LPCTSTR lpszFileName = NULL,
    DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    LPCTSTR lpszFilter = NULL,
    CWnd* pParentWnd = NULL,
    DWORD dwSize = 0
    );
  virtual ~CDosFileDialog();

  virtual INT_PTR DoModal();

  POSITION GetStartPosition() const;
  CString GetNextPathName(POSITION& pos) const;

protected:
  virtual void OnFileNameChange();
  DECLARE_MESSAGE_MAP()

protected:
  wchar_t* m_files;
  wchar_t* m_folder;
  bool m_bParsed;
};