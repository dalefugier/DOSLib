/////////////////////////////////////////////////////////////////////////////
// DosGetDateDialog.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosGetDateDialog : public CAcUiDialog
{
  DECLARE_DYNAMIC(CDosGetDateDialog)

public:
  CDosGetDateDialog(CWnd* pParent = NULL);
  virtual ~CDosGetDateDialog();

  // Dialog Data
  enum { IDD = IDD_DOSGETDATE };
  CMonthCalCtrl m_cal;
  CString m_title;
  CTime m_time;
  CTime m_start;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
