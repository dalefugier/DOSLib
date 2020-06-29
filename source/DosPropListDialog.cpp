/////////////////////////////////////////////////////////////////////////////
// DosPropListDialog.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosPropListDialog.h"

CDosPropListDialog::CDosPropListDialog(CWnd* pParent /*=NULL*/)
  : CAcUiDialog(CDosPropListDialog::IDD, pParent)
{
  m_Val = L"";
}

void CDosPropListDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_LBString(pDX, IDC_LIST, m_Val);
}

BEGIN_MESSAGE_MAP(CDosPropListDialog, CAcUiDialog)
  ON_WM_NCHITTEST()
  ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CDosPropListDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:PropList");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_LIST, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  INT_PTR i, count = m_ValArray.GetCount();
  for (i = 0; i < count; i++)
  {
    CDosPropItem* pItem = new CDosPropItem(m_StrArray[i], m_ValArray[i], PIT_EDIT, L"");
    if (pItem)
    {
      array.Add(pItem);
      m_List.AddPropItem((CDosPropItem*)array[i]);
    }
  }

  return TRUE;
}

void CDosPropListDialog::OnOK()
{
  INT_PTR i, count = m_ValArray.GetCount();
  for (i = 0; i < count; i++)
  {
    CDosPropItem* pItem = (CDosPropItem*)array[i];
    if (pItem)
      m_ValArray.SetAt(i, pItem->Get());
  }

  CAcUiDialog::OnOK();
}

LRESULT CDosPropListDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}

void CDosPropListDialog::OnDestroy()
{
  INT_PTR i, count = m_ValArray.GetCount();
  for (i = 0; i < count; i++)
  {
    CDosPropItem* pItem = (CDosPropItem*)array[i];
    if (pItem)
    {
      pItem = 0;
      delete array[i];
      array[i] = 0;
    }
  }

  CAcUiDialog::OnDestroy();
}
