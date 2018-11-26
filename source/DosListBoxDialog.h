/////////////////////////////////////////////////////////////////////////////
// DosListBoxDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosListBoxDialog : public CAcUiDialog
{
public:
  CDosListBoxDialog(CWnd* pParent);

  enum { IDD = IDD_DOSLISTBOX };
  CListBox m_List;
  CString m_Val;
  CString m_Title;
  CString m_Prompt;
  CString m_Default;
  CStringArray m_StrArray;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnDblclkList();
  virtual void OnOK();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
