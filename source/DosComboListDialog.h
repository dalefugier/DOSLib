/////////////////////////////////////////////////////////////////////////////
// DosComboListDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosComboBox : public CComboBox
{
public:
  CDosComboBox();
  virtual ~CDosComboBox();

protected:
  virtual void HandleCompletion();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
  CEdit* m_pEdit;
};

class CDosComboListDialog : public CAcUiDialog
{
public:
  CDosComboListDialog(CWnd* pParent);

  enum { IDD = IDD_DOSCOMBOLIST };
  CDosComboBox m_List;
  CString m_Val;
  CString m_Title;
  CString m_Prompt;
  CString m_Default;
  CStringArray m_StrArray;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  afx_msg void OnDblClkCombo();
  virtual void OnOK();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
