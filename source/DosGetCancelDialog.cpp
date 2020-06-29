/////////////////////////////////////////////////////////////////////////////
// DosGetCancelDialog.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetCancelDialog.h"

CDosGetCancelDialog::CDosGetCancelDialog(CWnd* pParent)
  : CAcUiDialog(CDosGetCancelDialog::IDD, pParent)
{
}

void CDosGetCancelDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDosGetCancelDialog, CAcUiDialog)
  ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetCancelDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:GetCancel");

  CAcUiDialog::OnInitDialog();

  LockDialogHeight();
  StretchControlX(IDC_PROMPT, 100);
  MoveControlX(IDCANCEL, 50);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);
  CenterWindow();

  return TRUE;
}

LRESULT CDosGetCancelDialog::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
  return TRUE;
}

void CDosGetCancelDialog::KillDialog()
{
  OnCancel();
}

void CDosGetCancelDialog::OnCancel()
{
  DestroyWindow();
}

void CDosGetCancelDialog::PostNcDestroy()
{
  CAcUiDialog::PostNcDestroy();
  delete this;
}

LRESULT CDosGetCancelDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}

