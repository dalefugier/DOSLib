/////////////////////////////////////////////////////////////////////////////
// DosDualListDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosInterDragListBox.h"
#include "DosGlyphButton.h"

class CDosDualListDialog : public CAcUiDialog
{
  DECLARE_DYNAMIC(CDosDualListDialog)

public:
  CDosDualListDialog(CWnd* pParent);
  virtual ~CDosDualListDialog();

  CString m_title;
  CString m_prompt;
  CString m_prompt_list1;
  CString m_prompt_list2;
  CStringArray m_list1;
  CStringArray m_list2;
  CDosGlyphButton	m_left;
  CDosGlyphButton	m_right;

  // Dialog Data
  enum { IDD = IDD_DOSDUALLISTBOX };
  CDosInterDragListBox m_listbox1;
  CDosInterDragListBox m_listbox2;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnMoveLeft();
  afx_msg void OnMoveRight();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
