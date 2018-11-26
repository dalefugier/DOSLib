/////////////////////////////////////////////////////////////////////////////
// DosGetPwdDialog.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosGetPwdDialog : public CAcUiDialog
{
public:
  CDosGetPwdDialog(CWnd* pParent);

  enum { IDD = IDD_GETPASSWORD };
  CAcUiStringEdit m_Edit;
  CButton m_OK;

  CString m_Title;
  CString m_Prompt;
  CString m_String;
  int m_Length;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeEdit();
  virtual void OnOK();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
