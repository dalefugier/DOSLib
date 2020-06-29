/////////////////////////////////////////////////////////////////////////////
// DosEditBoxDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosEditBoxDialog : public CAcUiDialog
{
public:
  CDosEditBoxDialog(CWnd* pParent);

  enum { IDD = IDD_DOSEDITBOX };
  CButton	m_OK;
  CEdit	  m_Edit;
  CString m_Title;
  CString	m_Prompt;
  CString m_String;

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnChangeEdit();
  afx_msg void OnImport();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
