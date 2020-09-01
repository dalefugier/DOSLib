/////////////////////////////////////////////////////////////////////////////
// DosComboListDialog.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosComboListDialog.h"

CDosComboBox::CDosComboBox()
  : CComboBox()
  , m_pEdit(0)
{
}

CDosComboBox::~CDosComboBox()
{
}

BEGIN_MESSAGE_MAP(CDosComboBox, CComboBox)
END_MESSAGE_MAP()

void CDosComboBox::HandleCompletion()
{
  if (m_pEdit == NULL)
  {
    m_pEdit = new CEdit();
    m_pEdit->SubclassWindow(GetDlgItem(1001)->GetSafeHwnd());
  }

  CString windowtext;
  m_pEdit->GetWindowText(windowtext);

  int start, end;
  m_pEdit->GetSel(start, end);

  int bestindex = -1;
  int bestfrom = INT_MAX;
  for (int x = 0; x < GetCount(); x++)
  {
    CString s;
    GetLBText(x, s);

    int from = s.Find(windowtext);
    if (from != -1 && from < bestfrom)
    {
      bestindex = x;
      bestfrom = from;
    }
  }

  if (bestindex != -1 && GetCurSel() != bestindex)
  {
    ShowDropDown(TRUE);
    SetCurSel(bestindex);

    m_pEdit->SetWindowText(windowtext);
    m_pEdit->SetSel(start, end);
  }
}

BOOL CDosComboBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
  if (HIWORD(wParam) == EN_CHANGE)
  {
    HandleCompletion();
    return true;
  }
  return CComboBox::OnCommand(wParam, lParam);
}

CDosComboListDialog::CDosComboListDialog(CWnd* pParent /*=NULL*/)
  : CAcUiDialog(CDosComboListDialog::IDD, pParent)
{
  m_Val = L"";
}

void CDosComboListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_CBString(pDX, IDC_LIST, m_Val);
}

BEGIN_MESSAGE_MAP(CDosComboListDialog, CAcUiDialog)
  ON_CBN_DBLCLK(IDC_LIST, OnDblClkCombo)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosComboListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:ComboList");

  CAcUiDialog::OnInitDialog();

  LockDialogHeight();
  StretchControlX(IDC_PROMPT, 100);
  StretchControlX(IDC_LIST, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  int i;
  for (i = 0; i < m_StrArray.GetCount(); i++)
    m_List.AddString(m_StrArray[i]);

  int n = m_List.SelectString(-1, m_Default);
  if (n == CB_ERR)
    SetDlgItemText(IDC_LIST, m_Default);

  return TRUE;
}

void CDosComboListDialog::OnDblClkCombo()
{
  OnOK();
}

void CDosComboListDialog::OnOK()
{
  CAcUiDialog::OnOK();
}

LRESULT CDosComboListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
