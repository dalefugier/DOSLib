/////////////////////////////////////////////////////////////////////////////
// DosDualListDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosDualListDialog.h"

IMPLEMENT_DYNAMIC(CDosDualListDialog, CAcUiDialog)

CDosDualListDialog::CDosDualListDialog(CWnd* pParent)
  : CAcUiDialog(CDosDualListDialog::IDD, pParent)
{
}

CDosDualListDialog::~CDosDualListDialog()
{
}

void CDosDualListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST1, m_listbox1);
  DDX_Control(pDX, IDC_LIST2, m_listbox2);
  DDX_Control(pDX, IDC_MOVELEFT, m_left);
  DDX_Control(pDX, IDC_MOVERIGHT, m_right);
}

BEGIN_MESSAGE_MAP(CDosDualListDialog, CAcUiDialog)
  ON_WM_NCHITTEST()
  ON_BN_CLICKED(IDC_MOVELEFT, &CDosDualListDialog::OnMoveLeft)
  ON_BN_CLICKED(IDC_MOVERIGHT, &CDosDualListDialog::OnMoveRight)
END_MESSAGE_MAP()

BOOL CDosDualListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:DualListBox");

  CAcUiDialog::OnInitDialog();

  DLGCTLINFO dcDlgCtls[] = {
    {IDC_PROMPT, ELASTICX, 100},
    {IDC_S_LEFT, ELASTICX, 50},
    {IDC_S_RIGHT, ELASTICX | MOVEX, 50 },
    {IDC_LIST1, ELASTICX, 50},
    {IDC_LIST1, ELASTICY, 100},
    {IDC_LIST2, ELASTICX | MOVEX, 50},
    {IDC_LIST2, ELASTICY, 100},
    {IDC_MOVELEFT, MOVEXY, 50},
    {IDC_MOVERIGHT, MOVEXY, 50},
    {IDOK, MOVEX, 50},
    {IDOK, MOVEY, 100},
    {IDCANCEL, MOVEX, 50},
    {IDCANCEL, MOVEY, 100},
  };

  SetControlProperty(dcDlgCtls, sizeof(dcDlgCtls) / sizeof(DLGCTLINFO));

  SetWindowText(m_title);
  SetDlgItemText(IDC_PROMPT, m_prompt);
  SetDlgItemText(IDC_S_LEFT, m_prompt_list1);
  SetDlgItemText(IDC_S_RIGHT, m_prompt_list2);

  m_left.SetWingDingButton(CDosGlyphButton::BTN_LEFTARROW);
  m_right.SetWingDingButton(CDosGlyphButton::BTN_RIGHTARROW);

  int i, count = (int)m_list1.GetCount();
  for (i = 0; i < count; i++)
  {
    int index = m_listbox1.AddString(m_list1[i]);
    m_listbox1.SetItemData(index, i);
  }

  count = (int)m_list2.GetCount();
  for (i = 0; i < count; i++)
  {
    int index = m_listbox2.AddString(m_list2[i]);
    m_listbox2.SetItemData(index, i);
  }

  return TRUE;
}

void CDosDualListDialog::OnOK()
{
  m_list1.RemoveAll();
  m_list2.RemoveAll();

  int i;
  CString s;

  for (i = 0; i < (int)m_listbox1.GetCount(); i++)
  {
    m_listbox1.GetText(i, s);
    m_list1.Add(s);
  }

  for (i = 0; i < (int)m_listbox2.GetCount(); i++)
  {
    m_listbox2.GetText(i, s);
    m_list2.Add(s);
  }

  CAcUiDialog::OnOK();
}

LRESULT CDosDualListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
void CDosDualListDialog::OnMoveLeft()
{
  if (0 == m_listbox2.GetSelCount())
    return;

  int nInsertPos = m_listbox1.GetAnchorIndex();
  if (nInsertPos == LB_ERR)
    nInsertPos = m_listbox1.GetCount();

  m_listbox2.MoveSelectedItems(&m_listbox1, nInsertPos);
}

void CDosDualListDialog::OnMoveRight()
{
  if (0 == m_listbox1.GetSelCount())
    return;

  int nInsertPos = m_listbox2.GetAnchorIndex();
  if (nInsertPos == LB_ERR)
    nInsertPos = m_listbox2.GetCount();

  m_listbox1.MoveSelectedItems(&m_listbox2, nInsertPos);
}
