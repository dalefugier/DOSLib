/////////////////////////////////////////////////////////////////////////////
// DosMemDC.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosMemDC.h"

CDosMemDC::CDosMemDC(CDC* pDC, const CRect* pRect)
  : m_pDC(pDC)
  , m_bMemDC(false)
  , m_oldBitmap(0)
  , m_crMask(DOS_UNSET_COLOR)
{
  ASSERT(pDC != 0);

  // Some initialization
  if (pDC)
    m_bMemDC = !pDC->IsPrinting();

  // Get the rectangle to draw
  if (0 == pRect)
    pDC->GetClipBox(&m_rect);
  else
    m_rect = *pRect;

  if (m_bMemDC)
  {
    CreateCompatibleDC(pDC);
    pDC->LPtoDP(&m_rect);
    m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
    m_oldBitmap = SelectObject(&m_bitmap);
    SetMapMode(pDC->GetMapMode());
    SetWindowExt(pDC->GetWindowExt());
    SetViewportExt(pDC->GetViewportExt());
    pDC->DPtoLP(&m_rect);
    SetWindowOrg(m_rect.left, m_rect.top);
  }
  else
  {
    m_bPrinting = pDC->m_bPrinting;
    m_hDC = pDC->m_hDC;
    m_hAttribDC = pDC->m_hAttribDC;
  }

  FillSolidRect(m_rect, pDC->GetBkColor());
}

CDosMemDC::~CDosMemDC()
{
  if (m_bMemDC)
  {
    if (DOS_UNSET_COLOR == m_crMask)
      m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
        this, m_rect.left, m_rect.top, SRCCOPY);
    else
      m_pDC->TransparentBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
        this, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
        m_crMask);
    SelectObject(m_oldBitmap);
  }
  else
  {
    m_hDC = m_hAttribDC = 0;
  }
}

COLORREF CDosMemDC::MaskingColor() const
{
  return m_crMask;
}

void CDosMemDC::SetMaskingColor(COLORREF cr)
{
  m_crMask = cr;
}

CDosMemDC* CDosMemDC::operator->()
{
  return this;
}

CDosMemDC::operator CDosMemDC*()
{
  return this;
}