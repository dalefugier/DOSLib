/////////////////////////////////////////////////////////////////////////////
// DosMultiListDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosMultiListDialog : public CAcUiDialog
{
public:
  CDosMultiListDialog(CWnd* pParent);
  ~CDosMultiListDialog();

  enum { IDD = IDD_DOSMULTILIST };
  CListBox m_listbox;
  CString m_title;
  CString m_prompt;
  CStringArray m_string_array;
  CArray<int, int> m_selected_array;
  CStringArray m_result_array;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSelect();
  afx_msg void OnClear();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
