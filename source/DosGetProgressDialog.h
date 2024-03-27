/////////////////////////////////////////////////////////////////////////////
// DosGetProgressDialog.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosProgressCtrl.h"

class CDosGetProgressDialog : public CAcUiDialog
{
public:
  CDosGetProgressDialog(CWnd* pParent);

  enum { IDD = IDD_DOSPROGRESS };
  CDosProgressCtrl m_Progress;
  CButton m_Cancel;
  CString m_Title;
  CString m_Prompt;
  int m_nUpper;
  BOOL m_bCancel;

  void KillDialog();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void PostNcDestroy();

protected:
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  afx_msg LRESULT OnAcadKeepFocus(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()
};
