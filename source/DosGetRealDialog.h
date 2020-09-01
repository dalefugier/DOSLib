/////////////////////////////////////////////////////////////////////////////
// DosGetRealDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosGetRealDialog : public CAcUiDialog
{
public:
  CDosGetRealDialog(CWnd* pParent);

  enum { IDD = IDD_GETREAL };
  CButton	m_OK;
  CEdit m_Edit;

  CString m_sTitle;
  CString m_sPrompt;

  void SetTitle(LPCTSTR lp) { m_sTitle = lp; };
  void SetPrompt(LPCTSTR lp) { m_sPrompt = lp; };
  void SetValue(double d = 0.0) { m_dblValue = d; m_bIsEmpty = FALSE; };
  void SetAllowEmpty(BOOL b = TRUE) { m_bAllowEmpty = b; };
  void SetAllowZero(BOOL b = TRUE) { m_bAllowZero = b; };
  void SetAllowNegative(BOOL b = TRUE) { m_bAllowNeg = b; };
  void SetMin(double d = 0.0) { m_dblMin = d; m_bMin = TRUE; };
  void SetMax(double d = 0.0) { m_dblMax = d; m_bMax = TRUE; };

  double Value() { return m_dblValue; };
  BOOL IsEmpty() { return m_bIsEmpty; };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnChangeEdit();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()

  BOOL ParseRealValue(LPCTSTR lpString, double* pValue);
  void ErrorMessageBox(LPCTSTR lpMessage);

  double m_dblValue;
  BOOL m_bIsEmpty;
  BOOL m_bAllowEmpty;
  BOOL m_bAllowZero;
  BOOL m_bAllowNeg;
  BOOL m_bMin;
  BOOL m_bMax;
  double m_dblMin;
  double m_dblMax;
};
