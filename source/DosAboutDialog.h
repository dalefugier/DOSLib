/////////////////////////////////////////////////////////////////////////////
// DosAboutDialog.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosStaticLink.h"

class CDosAboutDialog : public CAcUiDialog
{
public:
  CDosAboutDialog(CWnd* pParent);

  enum { IDD = IDD_ABOUTBOX };
  CDosStaticLink m_icon;
  CStatic	m_copyright;
  CStatic	m_product;
  CDosStaticLink m_web;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  virtual BOOL OnInitDialog();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  DECLARE_MESSAGE_MAP()
};
