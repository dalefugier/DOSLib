/////////////////////////////////////////////////////////////////////////////
// DosCheckListDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosCheckListDialog.h"
#include "DosGetStringDialog.h"

CDosCheckListDialog::CDosCheckListDialog(CWnd* pParent)
  : CAcUiDialog(CDosCheckListDialog::IDD, pParent)
{
  m_Val = L"";
  m_bShowAddButton = false;
}

void CDosCheckListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_LBString(pDX, IDC_LIST, m_Val);
}

BEGIN_MESSAGE_MAP(CDosCheckListDialog, CAcUiDialog)
  ON_BN_CLICKED(IDC_SELECT, OnSelect)
  ON_BN_CLICKED(IDC_CLEAR, OnClear)
  ON_WM_NCHITTEST()
  ON_BN_CLICKED(IDC_ADD, &CDosCheckListDialog::OnBnClickedAdd)
END_MESSAGE_MAP()

BOOL CDosCheckListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:CheckList");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);
  MoveControlX(IDC_SELECT, 100);
  MoveControlX(IDC_CLEAR, 100);
  MoveControlX(IDC_ADD, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  if (m_bShowAddButton)
    GetDlgItem(IDC_ADD)->ShowWindow(SW_SHOW);

  int i;
  for (i = 0; i < m_StrArray.GetCount(); i++)
  {
    m_List.AddString(m_StrArray[i]);
    m_List.SetCheck(m_List.GetCount() - 1, m_nArray[i]);
  }
  m_List.SetCurSel(0);

  return TRUE;
}

void CDosCheckListDialog::OnOK()
{
  int i;
  for (i = 0; i < m_List.GetCount(); i++)
  {
    if (m_List.GetCheck(i) == TRUE)
      m_nArray.SetAt(i, 1);
    else
      m_nArray.SetAt(i, 0);
  }
  CAcUiDialog::OnOK();
}

void CDosCheckListDialog::OnSelect()
{
  int i;
  for (i = 0; i < m_List.GetCount(); i++)
    m_List.SetCheck(i, 1);
}

void CDosCheckListDialog::OnClear()
{
  int i;
  for (i = 0; i < m_List.GetCount(); i++)
    m_List.SetCheck(i, 0);
}

LRESULT CDosCheckListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}

void CDosCheckListDialog::OnBnClickedAdd()
{
  CDosGetStringDialog dlg(acedGetAcadFrame());
  dlg.m_Title = m_Title;
  dlg.m_Prompt = L"Item to add to the list:";

  if (dlg.DoModal() == IDOK)
  {
    CString str = dlg.m_String;
    str.TrimLeft();
    str.TrimRight();

    int i;
    for (i = 0; i < m_List.GetCount(); i++)
    {
      if (str.CompareNoCase(m_StrArray[i]) == 0)
      {
        MessageBox(L"The specified item already exists.", DOS_PlatformName(), MB_ICONEXCLAMATION | MB_OK);
        return;
      }
    }

    m_StrArray.Add(dlg.m_String);
    m_List.AddString(dlg.m_String);
    m_nArray.Add(1);
    m_List.SetCheck(m_List.GetCount() - 1, 1);
  }
}
