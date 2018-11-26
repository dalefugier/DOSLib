/////////////////////////////////////////////////////////////////////////////
// DosMemDC.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#define DOS_UNSET_COLOR 0xFFFFFFFF

class CDosMemDC : public CDC
{
public:
  CDosMemDC(CDC* pDC, const CRect* pRect = 0);
  ~CDosMemDC();

  CDosMemDC* operator->();
  operator CDosMemDC*();

  COLORREF MaskingColor() const;
  void SetMaskingColor(COLORREF cr);

private:
  CBitmap m_bitmap;     // Offscreen bitmap
  CBitmap* m_oldBitmap; // Bitmap originally found in CDosMemDC
  CDC* m_pDC;           // Saves CDC passed in constructor
  CRect m_rect;         // Rectangle of drawing area.
  bool m_bMemDC;        // True if CDC really is a memory DC.
  COLORREF m_crMask;    // If set to DOS_UNSET_COLOR the destructor will bit blit the bitmap to
                        // m_pDC. If set to a color other than DOS_UNSET_COLOR then the bitmap
                        // will transparent blit to m_pDC using m_crMask as the masking color.
};
