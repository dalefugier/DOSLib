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

#include "StdAfx.h"
#include "DosAboutDialog.h"
#include "DOSLibApp.h"

CDosAboutDialog::CDosAboutDialog(CWnd* pParent)
  : CAcUiDialog(CDosAboutDialog::IDD, pParent)
{
}

void CDosAboutDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_DOSLIBICON, m_icon);
  DDX_Control(pDX, IDC_COPYRIGHT, m_copyright);
  DDX_Control(pDX, IDC_PRODUCT, m_product);
  DDX_Control(pDX, IDC_WEB, m_web);
}

BEGIN_MESSAGE_MAP(CDosAboutDialog, CAcUiDialog)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosAboutDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:About");
  CAcUiDialog::OnInitDialog();
  LockDialogWidth();
  LockDialogHeight();

  CString str;
  str.Format(L"%s Version %s", DOSLibApp().AppName(), DOSLibApp().AppVersion());
  m_product.SetWindowText(str);

  m_copyright.SetWindowText(DOSLibApp().AppCopyright());

  m_web.SetWindowText(DOSLibApp().AppInternet());
  m_icon.m_link = DOSLibApp().AppInternet();
  m_web.m_link = DOSLibApp().AppInternet();

  return TRUE;
}

LRESULT CDosAboutDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
