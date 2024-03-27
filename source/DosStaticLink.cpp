/////////////////////////////////////////////////////////////////////////////
// DosStaticLink.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosStaticLink.h"

IMPLEMENT_DYNAMIC(CDosStaticLink, CStatic)

BEGIN_MESSAGE_MAP(CDosStaticLink, CStatic)
  ON_WM_SETCURSOR()
  ON_WM_CTLCOLOR_REFLECT()
  ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
END_MESSAGE_MAP()

CDosStaticLink::CDosStaticLink()
{
  m_colorUnvisited = RGB(0, 0, 255); // blue
  m_colorVisited = RGB(128, 0, 128); // purple
  m_bVisited = FALSE;
}

HBRUSH CDosStaticLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
  ASSERT(nCtlColor == CTLCOLOR_STATIC);
  DWORD dwStyle = GetStyle();
  if (!(dwStyle & SS_NOTIFY))
  {
    ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
  }

  HBRUSH hbr = NULL;
  if ((dwStyle & 0xFF) <= SS_RIGHT)
  {
    if (!(HFONT)m_font)
    {
      LOGFONT lf;
      GetFont()->GetObject(sizeof(lf), &lf);
      lf.lfUnderline = TRUE;
      m_font.CreateFontIndirect(&lf);
    }

    pDC->SelectObject(&m_font);
    pDC->SetTextColor(m_bVisited ? m_colorVisited : m_colorUnvisited);
    pDC->SetBkMode(TRANSPARENT);

    hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
  }

  return hbr;
}

void CDosStaticLink::OnClicked()
{
  if (m_link.IsEmpty())
    GetWindowText(m_link);

  ShellExecute(0, L"open", m_link, NULL, NULL, SW_SHOWNORMAL);
  m_bVisited = TRUE;
  Invalidate();
}

BOOL CDosStaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
  HCURSOR hCursor = (HCURSOR)LoadImage(NULL, IDC_HAND, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
  SetCursor(hCursor);
  return TRUE;
}
