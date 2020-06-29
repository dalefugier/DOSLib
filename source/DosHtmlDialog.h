/////////////////////////////////////////////////////////////////////////////
// DosHtmlDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "webbrowser2.h"

class CDosHtmlDialog : public CAcUiDialog
{
public:
  CDosHtmlDialog(CWnd* pParent);
  void SetWebAddress(LPCTSTR lpAddress);
  void ResizeWindow();

  enum { IDD = IDD_DOSHTMLBOX };
  CWebBrowser2 m_webBrowser;

  void SetTitle(LPCTSTR lp) { m_sTitle = lp; };
  void SetAddress(LPCTSTR lp) { m_sAddress = lp; };
  void SetInitSize(int dx, int dy) { m_dx = dx; m_dy = dy; m_bSize = TRUE; };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnTitleChangeExplorer(LPCTSTR Text);
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_EVENTSINK_MAP()
  DECLARE_MESSAGE_MAP()

  CString m_sTitle;
  CString m_sAddress;
  BOOL m_bSize;
  int m_dx;
  int m_dy;
};

////////////////////////////////////////////////////////////////

class CDosModelessHtmlDialog : public CAcUiDialog
{
public:
  CDosModelessHtmlDialog(CWnd* pParent);
  void SetWebAddress(LPCTSTR lpAddress);
  void ResizeWindow();

  enum { IDD = IDD_DOSHTMLBOX };
  CWebBrowser2 m_webBrowser;

  void KillDialog();
  void SetTitle(LPCTSTR lp) { m_sTitle = lp; };
  void SetAddress(LPCTSTR lp) { m_sAddress = lp; };
  void SetInitSize(int dx, int dy) { m_dx = dx; m_dy = dy; m_bSize = TRUE; };
  void Refresh();

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual void PostNcDestroy();

protected:
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnTitleChangeExplorer(LPCTSTR Text);
  afx_msg LRESULT OnNcHitTest(CPoint point);
  afx_msg LRESULT OnAcadKeepFocus(WPARAM wParam, LPARAM lParam);
  DECLARE_EVENTSINK_MAP()
  DECLARE_MESSAGE_MAP()

  CString m_sTitle;
  CString m_sAddress;
  BOOL m_bSize;
  int m_dx;
  int m_dy;
};
