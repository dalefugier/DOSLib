/////////////////////////////////////////////////////////////////////////////
// DosScreenSelectionDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosScreenSelectionDialog.h"

IMPLEMENT_DYNAMIC(CDosScreenSelectionDialog, CDialog)

CDosScreenSelectionDialog::CDosScreenSelectionDialog(CRect& rect)
  : CDialog(IDD_SCREEN_SELECTION)
  , m_rect(rect)
{
  rect.SetRectEmpty();
}

CDosScreenSelectionDialog::~CDosScreenSelectionDialog()
{
}

BEGIN_MESSAGE_MAP(CDosScreenSelectionDialog, CDialog)
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CDosScreenSelectionDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  //AfxGetMainWnd()->ShowWindow(SW_HIDE);
  //::Sleep(333);

  if (!m_image.CaptureScreen())
    EndDialog(IDCANCEL);

  const int cx = ::GetSystemMetrics(SM_CXSCREEN);
  const int cy = ::GetSystemMetrics(SM_CYSCREEN);
  SetWindowPos(&CWnd::wndTopMost, 0, 0, cx, cy, SWP_SHOWWINDOW);

  return TRUE;
}

BOOL CDosScreenSelectionDialog::OnEraseBkgnd(CDC* pDC)
{
  return TRUE;
}

void CDosScreenSelectionDialog::OnPaint()
{
  CPaintDC dc(this);
  m_image.Draw(dc.GetSafeHdc(), 0, 0);
  m_tracker.Draw(&dc);
}

void CDosScreenSelectionDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
  m_tracker.TrackRubberBand(this, point);
  m_tracker.GetTrueRect(m_rect);
  EndDialog(IDOK);
}
