/////////////////////////////////////////////////////////////////////////////
// DosGetDateDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetDateDialog.h"

IMPLEMENT_DYNAMIC(CDosGetDateDialog, CAcUiDialog)

CDosGetDateDialog::CDosGetDateDialog(CWnd* pParent /*=NULL*/)
  : CAcUiDialog(CDosGetDateDialog::IDD, pParent)
{
  m_time = CTime::GetCurrentTime();
  m_start = CTime::GetCurrentTime();
}

CDosGetDateDialog::~CDosGetDateDialog()
{
}

void CDosGetDateDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MONTHCALENDAR1, m_cal);
}

BEGIN_MESSAGE_MAP(CDosGetDateDialog, CAcUiDialog)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetDateDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:GetDate");

  CAcUiDialog::OnInitDialog();

  StretchControlXY(IDC_MONTHCALENDAR1, 100, 100);
  MoveControlXY(IDOK, 50, 100);
  MoveControlXY(IDCANCEL, 50, 100);

  SetWindowText(m_title);

  m_cal.SetCurSel(m_start);

  return TRUE;
}

void CDosGetDateDialog::OnOK()
{
  if (m_cal.GetCurSel(m_time))
    CAcUiDialog::OnOK();
  else
    CAcUiDialog::OnCancel();
}

LRESULT CDosGetDateDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}