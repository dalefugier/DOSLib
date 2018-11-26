/////////////////////////////////////////////////////////////////////////////
// DosGetCancelDialog.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosGetCancelDialog : public CAcUiDialog
{
public:
  CDosGetCancelDialog(CWnd* pParent);

  enum { IDD = IDD_DOSCANCEL };
  CString m_Title;
  CString	m_Prompt;

  void KillDialog();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void PostNcDestroy();

protected:
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  afx_msg LRESULT OnAcadKeepFocus(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()
};
