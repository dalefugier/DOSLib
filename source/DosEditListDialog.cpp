/////////////////////////////////////////////////////////////////////////////
// DosEditListDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosEditListDialog.h"

CDosEditListDialog::CDosEditListDialog(CWnd* pParent)
  : CAcUiDialog(CDosEditListDialog::IDD, pParent)
  , m_ListBoxExBuddy(IDB_BUDDY)
{
}

void CDosEditListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
}

BEGIN_MESSAGE_MAP(CDosEditListDialog, CAcUiDialog)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosEditListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:EditList");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_BUDDYS, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_BUDDYS, m_Prompt);

  m_ListBoxExBuddy.SubclassDlgItem(IDC_BUDDYS, this);
  m_ListBoxExBuddy.SetListBox(&m_List);

  m_List.SetEditStyle(0);
  m_List.AllowDrag(FALSE);

  int i;
  for (i = 0; i < m_StrArray.GetCount(); i++)
    m_List.AddString(m_StrArray[i]);
  m_List.SetCurSel(0);

  return TRUE;
}

void CDosEditListDialog::OnOK()
{
  m_StrArray.RemoveAll();

  int i;
  for (i = 0; i < m_List.GetCount(); i++)
  {
    CString str;
    m_List.GetText(i, str);
    m_StrArray.Add(str);
  }

  CAcUiDialog::OnOK();
}

LRESULT CDosEditListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
