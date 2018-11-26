/////////////////////////////////////////////////////////////////////////////
// DosOrderListDialog.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosDragListBox.h"

class CDosOrderListDialog : public CAcUiDialog
{
public:
  CDosOrderListDialog(CWnd* pParent);

  // Dialog Data
  enum { IDD = IDD_DOSORDERLIST };
  CDosDragListBox m_List;
  CString m_Title;
  CString m_Prompt;
  CStringArray m_StrArray;
  CDWordArray m_IntArray;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  int Move(int old_index, int new_index);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnMoveUp();
  afx_msg void OnMoveDown();
  afx_msg void OnMoveTop();
  afx_msg void OnMoveBottom();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
