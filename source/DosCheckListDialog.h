/////////////////////////////////////////////////////////////////////////////
// DosCheckListDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosCheckListDialog : public CAcUiDialog
{
public:
  CDosCheckListDialog(CWnd* pParent);

  enum { IDD = IDD_DOSCHECKLIST };
  CCheckListBox m_List;
  CString m_Val;
  CString m_Title;
  CString m_Prompt;
  CStringArray m_StrArray;
  CWordArray m_nArray;
  bool m_bShowAddButton;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSelect();
  afx_msg void OnClear();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedAdd();
};
