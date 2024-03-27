/////////////////////////////////////////////////////////////////////////////
// DosListBoxDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosListBoxDialog.h"

CDosListBoxDialog::CDosListBoxDialog(CWnd* pParent)
  : CAcUiDialog(CDosListBoxDialog::IDD, pParent)
{
  m_Val = L"";
}

void CDosListBoxDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_LBString(pDX, IDC_LIST, m_Val);
}

BEGIN_MESSAGE_MAP(CDosListBoxDialog, CAcUiDialog)
  ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosListBoxDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:ListBox");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  int i;
  for (i = 0; i < m_StrArray.GetCount(); i++)
    m_List.AddString(m_StrArray[i]);

  int n = m_List.SelectString(-1, m_Default);
  if (n == CB_ERR)
    m_List.SetCurSel(0);

  return TRUE;
}

void CDosListBoxDialog::OnDblclkList()
{
  OnOK();
  return;
}

void CDosListBoxDialog::OnOK()
{
  CAcUiDialog::OnOK();
}

LRESULT CDosListBoxDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
