/////////////////////////////////////////////////////////////////////////////
// DosGlyphButton.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

class CDosGlyphButton : public CButton
{
public:
  CDosGlyphButton();
  virtual ~CDosGlyphButton();

  // Some predefined glyphs from WingDings
  enum
  {
    BTN_DELETE = 0xFB,
    BTN_CHECK = 0xFC,
    BTN_LEFTARROW = 0xDF,
    BTN_RIGHTARROW = 0xE0,
    BTN_UPARROW = 0xE1,
    BTN_DOWNARROW = 0xE2,
    BTN_HOLLOW_LEFTARROW = 0xEF,
    BTN_HOLLOW_RIGHTARROW = 0xF0,
    BTN_HOLLOW_UPARROW = 0xF1,
    BTN_HOLLOW_DOWNARROW = 0xF2
  };

  void SetCharSet(BYTE bCharSet);
  void SetFaceName(LPCTSTR lpszFaceName);
  void SetFont(LOGFONT* plf);
  void SetFont(CFont* pFont);
  void SetGlyph(UINT cGlyph);
  void SetGlyph(LOGFONT* plf, UINT cGlyph);
  void SetGlyph(CFont* pFont, UINT cGlyph);
  void SetGlyph(LONG lHeight, LONG lPointSize, LONG lWeight, LPCTSTR lpszFaceName, UINT cGlyph);
  void SetGlyph(LONG lPointSize, LPCTSTR lpszFaceName, UINT cGlyph);
  void SetHeight(LONG lHeight);
  void SetPointSize(LONG lPointSize);
  void SetWeight(LONG lWeight);
  void SetWingDingButton(UINT nButton);

protected:
  virtual void PreSubclassWindow();
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

  LONG GetFontHeight(LONG nPointSize);
  LONG GetFontPointSize(LONG nHeight);
  void ReconstructFont();

  LOGFONT m_lf;
  UINT m_cGlyph;
  CFont m_font;
};
