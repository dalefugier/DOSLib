/////////////////////////////////////////////////////////////////////////////
// DosGetIntDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosGetIntDialog : public CAcUiDialog
{
public:
  CDosGetIntDialog(CWnd* pParent);

  enum { IDD = IDD_GETINT };
  CButton	m_OK;
  CEdit m_Edit;

  void SetTitle(LPCTSTR lp) { m_sTitle = lp; };
  void SetPrompt(LPCTSTR lp) { m_sPrompt = lp; };
  void SetValue(int n = 0) { m_nValue = n; m_bIsEmpty = FALSE; };
  void SetAllowEmpty(BOOL b = TRUE) { m_bAllowEmpty = b; };
  void SetAllowZero(BOOL b = TRUE) { m_bAllowZero = b; };
  void SetAllowNegative(BOOL b = TRUE) { m_bAllowNeg = b; };
  void SetMin(int n = 0) { m_nMin = n; m_bMin = TRUE; };
  void SetMax(int n = 0) { m_nMax = n; m_bMax = TRUE; };

  int  Value() { return m_nValue; };
  BOOL IsEmpty() { return m_bIsEmpty; };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnChangeEdit();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()

  BOOL ParseIntValue(LPCTSTR lpString, int* pValue);
  void ErrorMessageBox(LPCTSTR lpMessage);

  CString m_sTitle;
  CString m_sPrompt;
  int m_nValue;
  BOOL m_bIsEmpty;
  BOOL m_bAllowEmpty;
  BOOL m_bAllowZero;
  BOOL m_bAllowNeg;
  BOOL m_bMin;
  BOOL m_bMax;
  int m_nMin;
  int m_nMax;
};
