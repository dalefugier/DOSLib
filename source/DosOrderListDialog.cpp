/////////////////////////////////////////////////////////////////////////////
// DosOrderListDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosOrderListDialog.h"

CDosOrderListDialog::CDosOrderListDialog(CWnd* pParent)
  : CAcUiDialog(CDosOrderListDialog::IDD, pParent)
{
}

void CDosOrderListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
}

BEGIN_MESSAGE_MAP(CDosOrderListDialog, CAcUiDialog)
  ON_BN_CLICKED(IDC_MOVEUP, &CDosOrderListDialog::OnMoveUp)
  ON_BN_CLICKED(IDC_MOVEDOWN, &CDosOrderListDialog::OnMoveDown)
  ON_BN_CLICKED(IDC_MOVETOP, &CDosOrderListDialog::OnMoveTop)
  ON_BN_CLICKED(IDC_MOVEBOTTOM, &CDosOrderListDialog::OnMoveBottom)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosOrderListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:OrderList");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);
  MoveControlX(IDC_MOVETOP, 100);
  MoveControlX(IDC_MOVEUP, 100);
  MoveControlX(IDC_MOVEDOWN, 100);
  MoveControlX(IDC_MOVEBOTTOM, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  int i, index = 0;
  for (i = 0; i < m_StrArray.GetCount(); i++)
  {
    index = m_List.AddString(m_StrArray[i]);
    m_List.SetItemData(index, i);
  }

  return TRUE;
}

void CDosOrderListDialog::OnOK()
{
  m_StrArray.RemoveAll();
  m_IntArray.RemoveAll();

  CString s;
  for (int i = 0; i < m_List.GetCount(); i++)
  {
    m_List.GetText(i, s);
    m_StrArray.Add(s);
    m_IntArray.Add((DWORD)m_List.GetItemData(i));
  }

  CAcUiDialog::OnOK();
}

void CDosOrderListDialog::OnMoveUp()
{
  int sel_count = m_List.GetSelCount();
  if (0 == sel_count)
    return;

  CArray<int, int> sel_items;
  sel_items.SetSize(sel_count);
  m_List.GetSelItems(sel_count, sel_items.GetData());

  m_List.SetRedraw(FALSE);

  for (int i = 0; i < sel_count; i++)
  {
    int index = sel_items[i];
    if (index <= 0)
      break;

    int rc = Move(index, index - 1);
    if (rc < 0)
      break;
  }

  m_List.SetRedraw(TRUE);
  m_List.Invalidate();
}

void CDosOrderListDialog::OnMoveDown()
{
  int sel_count = m_List.GetSelCount();
  if (0 == sel_count)
    return;

  CArray<int, int> sel_items;
  sel_items.SetSize(sel_count);
  m_List.GetSelItems(sel_count, sel_items.GetData());

  m_List.SetRedraw(FALSE);

  const int count = m_List.GetCount();
  int i;
  for (i = sel_count - 1; i >= 0; i--)
  {
    int index = sel_items[i];
    if (index >= count - 1)
      break;

    int rc = Move(index, index + 1);
    if (rc < 0)
      break;
  }

  m_List.SetRedraw(TRUE);
  m_List.Invalidate();
}

void CDosOrderListDialog::OnMoveTop()
{
  int sel_count = m_List.GetSelCount();
  if (0 == sel_count)
    return;

  CArray<int, int> sel_items;
  sel_items.SetSize(sel_count);
  m_List.GetSelItems(sel_count, sel_items.GetData());

  m_List.SetRedraw(FALSE);

  int range = 0;
  for (int i = 0; i < sel_count; i++)
  {
    int index = sel_items[i];
    if (index <= 0)
      break;

    int rc = Move(index, range++);
    if (rc < 0)
      break;
  }

  m_List.SetRedraw(TRUE);
  m_List.Invalidate();
}

void CDosOrderListDialog::OnMoveBottom()
{
  int sel_count = m_List.GetSelCount();
  if (0 == sel_count)
    return;

  CArray<int, int> sel_items;
  sel_items.SetSize(sel_count);
  m_List.GetSelItems(sel_count, sel_items.GetData());

  m_List.SetRedraw(FALSE);

  int range = m_List.GetCount() - 1;
  int i;
  for (i = sel_count - 1; i >= 0; i--)
  {
    int index = sel_items[i];
    if (index >= range)
      break;

    int rc = Move(index, range--);
    if (rc < 0)
      break;
  }

  m_List.SetRedraw(TRUE);
  m_List.Invalidate();
}

int CDosOrderListDialog::Move(int old_index, int new_index)
{
  if (old_index == new_index)
    return old_index;

  const int count = m_List.GetCount();

  if (old_index < 0 ||
    new_index < 0 ||
    old_index > count - 1 ||
    new_index > count - 1
    )
    return LB_ERR;

  CString text;
  m_List.GetText(old_index, text);
  DWORD data = (DWORD)m_List.GetItemData(old_index);

  m_List.DeleteString(old_index);
  int inserted_index = m_List.InsertString(new_index, text);
  if (inserted_index != LB_ERR && inserted_index != LB_ERRSPACE)
  {
    m_List.SetItemData(inserted_index, data);
    m_List.SetSel(inserted_index, TRUE);
  }

  return inserted_index;
}

LRESULT CDosOrderListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}

