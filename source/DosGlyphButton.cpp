/////////////////////////////////////////////////////////////////////////////
// DosGlyphButton.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosGlyphButton.h"

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY  5		
#endif

BEGIN_MESSAGE_MAP(CDosGlyphButton, CButton)
END_MESSAGE_MAP()

CDosGlyphButton::CDosGlyphButton()
{
  memset(&m_lf, 0, sizeof(LOGFONT));
  m_cGlyph = 0xB4;		// ? in a diamond
  m_lf.lfCharSet = DEFAULT_CHARSET;
  m_lf.lfHeight = -30;
  m_lf.lfWeight = FW_NORMAL;
  m_lf.lfQuality = CLEARTYPE_QUALITY;
  lstrcpy(m_lf.lfFaceName, _T("WingDings"));
}

CDosGlyphButton::~CDosGlyphButton()
{
  if (m_font.GetSafeHandle())
    m_font.DeleteObject();
}

void CDosGlyphButton::PreSubclassWindow()
{
  CButton::PreSubclassWindow();

  ReconstructFont();
  TCHAR ch[3] = { 0 };
  ch[0] = (TCHAR)m_cGlyph;
  SetWindowText(ch);
}

void CDosGlyphButton::ReconstructFont()
{
  if (m_font.GetSafeHandle())
    m_font.DeleteObject();
  m_lf.lfQuality = CLEARTYPE_QUALITY;
  VERIFY(m_font.CreateFontIndirect(&m_lf));
  CButton::SetFont(&m_font, TRUE);
}

void CDosGlyphButton::SetFont(LOGFONT* plf)
{
  memcpy(&m_lf, plf, sizeof(LOGFONT));
  ReconstructFont();
}

void CDosGlyphButton::SetFont(CFont* pFont)
{
  pFont->GetLogFont(&m_lf);
  ReconstructFont();
}

void CDosGlyphButton::SetGlyph(LOGFONT* plf, UINT cGlyph)
{
  SetFont(plf);
  SetGlyph(cGlyph);
}

void CDosGlyphButton::SetGlyph(CFont* pFont, UINT cGlyph)
{
  pFont->GetLogFont(&m_lf);
  ReconstructFont();
  SetGlyph(cGlyph);
}

void CDosGlyphButton::SetGlyph(LONG lHeight,
  LONG lPointSize,
  LONG lWeight,
  LPCTSTR lpszFaceName,
  UINT cGlyph)
{
  ASSERT(lpszFaceName && lpszFaceName[0] != _T('\0'));
  m_lf.lfFaceName[0] = _T('\0');
  if (lpszFaceName)
    lstrcpy(m_lf.lfFaceName, lpszFaceName);
  m_lf.lfHeight = lHeight;
  if (lPointSize)		// use point size if not 0
    m_lf.lfHeight = GetFontHeight(lPointSize);
  m_lf.lfWeight = lWeight;
  ReconstructFont();
  SetGlyph(cGlyph);
}

void CDosGlyphButton::SetGlyph(LONG lPointSize, LPCTSTR lpszFaceName, UINT cGlyph)
{
  ASSERT(lpszFaceName && lpszFaceName[0] != _T('\0'));
  m_lf.lfFaceName[0] = _T('\0');
  if (lpszFaceName)
    lstrcpy(m_lf.lfFaceName, lpszFaceName);
  m_lf.lfHeight = GetFontHeight(lPointSize);
  ReconstructFont();
  SetGlyph(cGlyph);
}

void CDosGlyphButton::SetGlyph(UINT cGlyph)
{
  m_cGlyph = cGlyph;
  TCHAR ch[3] = { 0 };
  ch[0] = (TCHAR)m_cGlyph;
  SetWindowText(ch);
}

void CDosGlyphButton::SetHeight(LONG lHeight)
{
  m_lf.lfHeight = lHeight;
  ReconstructFont();
}

void CDosGlyphButton::SetWeight(LONG lWeight)
{
  m_lf.lfWeight = lWeight;
  ReconstructFont();
}

void CDosGlyphButton::SetCharSet(BYTE bCharSet)
{
  m_lf.lfCharSet = bCharSet;
  ReconstructFont();
}

void CDosGlyphButton::SetFaceName(LPCTSTR lpszFaceName)
{
  ASSERT(lpszFaceName && lpszFaceName[0] != _T('\0'));
  m_lf.lfFaceName[0] = _T('\0');
  if (lpszFaceName)
    lstrcpy(m_lf.lfFaceName, lpszFaceName);
  ReconstructFont();
}

void CDosGlyphButton::SetPointSize(LONG lPointSize)
{
  m_lf.lfHeight = GetFontHeight(lPointSize);
  ReconstructFont();
}

void CDosGlyphButton::SetWingDingButton(UINT nButton)
{
  m_lf.lfCharSet = SYMBOL_CHARSET;
  m_lf.lfHeight = GetFontHeight(14);
  m_lf.lfWeight = FW_NORMAL;
  lstrcpy(m_lf.lfFaceName, _T("WingDings"));

  SetGlyph(nButton);

  ReconstructFont();
}

LONG CDosGlyphButton::GetFontPointSize(LONG nHeight)
{
  HDC hdc = ::CreateDC(L"DISPLAY", NULL, NULL, NULL);
  ASSERT(hdc);
  LONG cyPixelsPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
  ::DeleteDC(hdc);

  LONG nPointSize = MulDiv(nHeight, 72, cyPixelsPerInch);
  if (nPointSize < 0)
    nPointSize = -nPointSize;

  return nPointSize;
}

LONG CDosGlyphButton::GetFontHeight(LONG nPointSize)
{
  HDC hdc = ::CreateDC(L"DISPLAY", NULL, NULL, NULL);
  ASSERT(hdc);
  LONG cyPixelsPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
  ::DeleteDC(hdc);

  LONG nHeight = -MulDiv(nPointSize, cyPixelsPerInch, 72);

  //TRACE(_T("nPointSize=%d  nHeight=%d\n"), nPointSize, nHeight);

  return nHeight;
}
