/////////////////////////////////////////////////////////////////////////////
// DosColorDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosColorDialog : public CColorDialog
{
  DECLARE_DYNAMIC(CDosColorDialog)

public:
  CDosColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
  CString m_Title;

protected:
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};
