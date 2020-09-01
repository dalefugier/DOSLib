/////////////////////////////////////////////////////////////////////////////
// DosEditListDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosListBoxEx.h"

class CDosEditListDialog : public CAcUiDialog
{
public:
  CDosEditListDialog(CWnd* pParent);

  enum { IDD = IDD_DOSEDITLIST };
  CDosListBoxEx m_List;
  CDosListBoxExBuddy m_ListBoxExBuddy;
  CString m_Val;
  CString m_Title;
  CString m_Prompt;
  CStringArray m_StrArray;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
