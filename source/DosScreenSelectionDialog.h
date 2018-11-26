/////////////////////////////////////////////////////////////////////////////
// DosScreenSelectionDialog.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
#pragma once

#include "DosScreenImage.h"

class CDosScreenSelectionDialog : public CDialog
{
  DECLARE_DYNAMIC(CDosScreenSelectionDialog)

public:
  CDosScreenSelectionDialog(CRect& rect);
  virtual ~CDosScreenSelectionDialog();

  // Dialog Data
  enum { IDD = IDD_SCREEN_SELECTION };

public:
  virtual BOOL OnInitDialog();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
  DECLARE_MESSAGE_MAP()

private:
  CRect& m_rect;
  CRectTracker m_tracker;
  CDosScreenImage m_image;
};
