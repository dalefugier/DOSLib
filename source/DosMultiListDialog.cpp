/////////////////////////////////////////////////////////////////////////////
// DosMultiListDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosMultiListDialog.h"

CDosMultiListDialog::CDosMultiListDialog(CWnd* pParent)
  : CAcUiDialog(CDosMultiListDialog::IDD, pParent)
{
}

CDosMultiListDialog::~CDosMultiListDialog()
{
}

void CDosMultiListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_listbox);
}

BEGIN_MESSAGE_MAP(CDosMultiListDialog, CAcUiDialog)
  ON_BN_CLICKED(IDC_SELECT, OnSelect)
  ON_BN_CLICKED(IDC_CLEAR, OnClear)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosMultiListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:MultiList");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);
  MoveControlX(IDC_SELECT, 100);
  MoveControlX(IDC_CLEAR, 100);

  SetWindowText(m_title);
  SetDlgItemText(IDC_PROMPT, m_prompt);

  int i = 0;
  for (i = 0; i < m_string_array.GetCount(); i++)
    m_listbox.AddString(m_string_array[i]);

  for (i = 0; i < m_selected_array.GetCount(); i++)
  {
    int item = m_selected_array[i];
    if (item >= 0 && item < m_listbox.GetCount())
      m_listbox.SetSel(item);
  }

  return TRUE;
}

void CDosMultiListDialog::OnOK()
{
  m_result_array.RemoveAll();

  int count = m_listbox.GetSelCount();
  if (count < 1)
    EndDialog(IDCANCEL);

  CArray<int, int> selitems;
  selitems.SetSize(count);

  m_listbox.GetSelItems(count, selitems.GetData());

  int i;
  for (i = 0; i < selitems.GetCount(); i++)
    m_result_array.Add(m_string_array[selitems[i]]);

  CAcUiDialog::OnOK();
}

void CDosMultiListDialog::OnSelect()
{
  int i;
  for (i = 0; i < m_listbox.GetCount(); i++)
    m_listbox.SetSel(i, TRUE);
}

void CDosMultiListDialog::OnClear()
{
  int i;
  for (i = 0; i < m_listbox.GetCount(); i++)
    m_listbox.SetSel(i, FALSE);
}

LRESULT CDosMultiListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
