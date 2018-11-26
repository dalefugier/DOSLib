/////////////////////////////////////////////////////////////////////////////
// DosGetPwdDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetPwdDialog.h"

CDosGetPwdDialog::CDosGetPwdDialog(CWnd* pParent)
  : CAcUiDialog(CDosGetPwdDialog::IDD, pParent)
{
  m_Title = L"";
  m_Prompt = L"";
  m_String = L"";
  m_Length = 0;
}

void CDosGetPwdDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_EDIT, m_Edit);
}

BEGIN_MESSAGE_MAP(CDosGetPwdDialog, CAcUiDialog)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetPwdDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:GetPassword");

  CAcUiDialog::OnInitDialog();

  LockDialogHeight();
  StretchControlX(IDC_EDIT, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  if (m_Length > 0)
    m_Edit.SetLimitText(m_Length);
  m_Edit.SetWindowText(m_String);
  m_Edit.SetFocus();
  OnChangeEdit();

  return TRUE;
}

void CDosGetPwdDialog::OnChangeEdit()
{
  CString str;
  m_Edit.GetWindowText(str);
  if (str.GetLength() == 0)
    m_OK.EnableWindow(FALSE);
  else
    m_OK.EnableWindow(TRUE);
}

void CDosGetPwdDialog::OnOK()
{
  m_Edit.GetWindowText(m_String);
  CAcUiDialog::OnOK();
}

LRESULT CDosGetPwdDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
