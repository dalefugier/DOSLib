/////////////////////////////////////////////////////////////////////////////
// DosProgressCtrl.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosProgressCtrl : public CProgressCtrl
{
public:
  CDosProgressCtrl();
  virtual ~CDosProgressCtrl();

  int SetPos(int nPos);
  int StepIt();
  void SetRange(int nLower, int nUpper);
  void GetRange(int& nLower, int& nUpper) const;
  int OffsetPos(int nPos);
  int SetStep(int nStep);

  void SetShowText(BOOL bShow);
  COLORREF SetBarColour(COLORREF crBarClr = CLR_DEFAULT);
  COLORREF GetBarColour() const;
  COLORREF SetBgColour(COLORREF crBgClr = CLR_DEFAULT);
  COLORREF GetBgColour() const;

protected:
  int      m_nPos;
  int      m_nStepSize;
  int      m_nMax;
  int      m_nMin;
  CString  m_strText;
  BOOL     m_bShowText;
  int      m_nBarWidth;
  COLORREF m_crBarClr;
  COLORREF m_crBgClr;

  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT OnSetText(WPARAM, LPARAM);
  afx_msg LRESULT OnGetText(WPARAM, LPARAM);
  DECLARE_MESSAGE_MAP()
};
