/////////////////////////////////////////////////////////////////////////////
// DosProgressCtrl.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosProgressCtrl.h"
#include "DosMemDC.h"

CDosProgressCtrl::CDosProgressCtrl()
{
  m_nPos = 0;
  m_nStepSize = 1;
  m_nMax = 100;
  m_nMin = 0;
  m_bShowText = TRUE;
  m_strText.Empty();
  m_crBarClr = CLR_DEFAULT;
  m_crBgClr = CLR_DEFAULT;
  m_nBarWidth = -1;
}

CDosProgressCtrl::~CDosProgressCtrl()
{
}

BEGIN_MESSAGE_MAP(CDosProgressCtrl, CProgressCtrl)
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_MESSAGE(WM_SETTEXT, OnSetText)
  ON_MESSAGE(WM_GETTEXT, OnGetText)
END_MESSAGE_MAP()

LRESULT CDosProgressCtrl::OnSetText(WPARAM wParam, LPARAM lParam)
{
  LRESULT result = Default();
  LPCTSTR szText = (LPCTSTR)lParam;
  if ((!szText && m_strText.GetLength()) || (szText && (m_strText != szText)))
  {
    m_strText = szText;
    Invalidate();
  }
  return result;
}

LRESULT CDosProgressCtrl::OnGetText(WPARAM wParam, LPARAM lParam)
{
  UINT cchTextMax = (UINT)wParam;
  LPTSTR szText = (LPTSTR)lParam;
  if (!wcsncpy_s(szText, cchTextMax, m_strText, m_strText.GetLength()))
    return 0;
  else
    return min(cchTextMax, (UINT)m_strText.GetLength());
}

BOOL CDosProgressCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
  return TRUE;
}

void CDosProgressCtrl::OnSize(UINT nType, int cx, int cy)
{
  CProgressCtrl::OnSize(nType, cx, cy);
  m_nBarWidth = -1;
}

void CDosProgressCtrl::OnPaint()
{
  if (m_nMin >= m_nMax)
    return;

  COLORREF crBarColour, crBgColour;
  crBarColour = (m_crBarClr == CLR_DEFAULT) ? ::GetSysColor(COLOR_HIGHLIGHT) : m_crBarClr;
  crBgColour = (m_crBgClr == CLR_DEFAULT) ? ::GetSysColor(COLOR_WINDOW) : m_crBgClr;

  CRect LeftRect, RightRect, ClientRect;
  GetClientRect(ClientRect);

  double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));

  CPaintDC PaintDC(this);
  CDosMemDC dc(&PaintDC);

  LeftRect = RightRect = ClientRect;

#ifdef PBS_VERTICAL
  DWORD dwStyle = GetStyle();
  if (dwStyle & PBS_VERTICAL)
  {
    LeftRect.top = LeftRect.bottom - (int)((LeftRect.bottom - LeftRect.top)*Fraction);
    RightRect.bottom = LeftRect.top;
  }
  else
#endif
  {
    LeftRect.right = LeftRect.left + (int)((LeftRect.right - LeftRect.left)*Fraction);
    RightRect.left = LeftRect.right;
  }
  dc.FillSolidRect(LeftRect, crBarColour);
  dc.FillSolidRect(RightRect, crBgColour);

  if (m_bShowText
#ifdef PBS_VERTICAL
    && (dwStyle & PBS_VERTICAL) == 0
#endif
    )
  {
    CString str;
    if (m_strText.GetLength())
      str = m_strText;
    else
      str.Format(L"%d%%", (int)(Fraction*100.0));

    dc.SetBkMode(TRANSPARENT);

    DWORD dwTextStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

    // If we are drawing vertical, then create a new verticla font
    // based on the current font (only works with TrueType fonts)
    CFont font, *pOldFont = NULL;
#ifdef PBS_VERTICAL
    if (dwStyle & PBS_VERTICAL)
    {
      LOGFONT lf;
      GetFont()->GetLogFont(&lf);
      lf.lfEscapement = lf.lfOrientation = 900;
      font.CreateFontIndirect(&lf);
      pOldFont = (CFont*)dc.SelectObject(&font);

      dwTextStyle = DT_VCENTER | DT_CENTER | DT_SINGLELINE;
    }
#endif

    CRgn rgn;
    rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
    dc.SelectClipRgn(&rgn);
    dc.SetTextColor(crBgColour);
    dc.DrawText(str, ClientRect, dwTextStyle);

    rgn.DeleteObject();
    rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
    dc.SelectClipRgn(&rgn);
    dc.SetTextColor(crBarColour);
    dc.DrawText(str, ClientRect, dwTextStyle);

    if (pOldFont)
    {
      dc.SelectObject(pOldFont);
      font.DeleteObject();
    }
  }
}

void CDosProgressCtrl::SetShowText(BOOL bShow)
{
  if (::IsWindow(m_hWnd) && m_bShowText != bShow)
    Invalidate();

  m_bShowText = bShow;
}

void CDosProgressCtrl::SetRange(int nLower, int nUpper)
{
  m_nMax = nUpper;
  m_nMin = nLower;
}

void CDosProgressCtrl::GetRange(int& nLower, int& nUpper) const
{
  nUpper = m_nMax;
  nLower = m_nMin;
}

int CDosProgressCtrl::SetPos(int nPos)
{
  if (!::IsWindow(m_hWnd))
    return -1;

  int nOldPos = m_nPos;
  m_nPos = nPos;

  CRect rect;
  GetClientRect(rect);

  double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));
  int nBarWidth = (int)(Fraction * rect.Width());

  if (nBarWidth != m_nBarWidth)
  {
    m_nBarWidth = nBarWidth;
    RedrawWindow();
  }

  return nOldPos;
}

int CDosProgressCtrl::StepIt()
{
  return SetPos(m_nPos + m_nStepSize);
}

int CDosProgressCtrl::OffsetPos(int nPos)
{
  return SetPos(m_nPos + nPos);
}

int CDosProgressCtrl::SetStep(int nStep)
{
  int nOldStep = m_nStepSize;
  m_nStepSize = nStep;
  return nOldStep;
}

COLORREF CDosProgressCtrl::SetBarColour(COLORREF crBarClr /*= CLR_DEFAULT*/)
{
  if (::IsWindow(m_hWnd))
    Invalidate();

  COLORREF crOldBarClr = m_crBarClr;
  m_crBarClr = crBarClr;
  return crOldBarClr;
}

COLORREF CDosProgressCtrl::GetBarColour() const
{
  return m_crBarClr;
}

COLORREF CDosProgressCtrl::SetBgColour(COLORREF crBgClr /*= CLR_DEFAULT*/)
{
  if (::IsWindow(m_hWnd))
    Invalidate();

  COLORREF crOldBgClr = m_crBgClr;
  m_crBgClr = crBgClr;
  return crOldBgClr;
}

COLORREF CDosProgressCtrl::GetBgColour() const
{
  return m_crBgClr;
}
