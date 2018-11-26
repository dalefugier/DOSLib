/////////////////////////////////////////////////////////////////////////////
// DosStaticLink.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosStaticLink : public CStatic
{
public:
  CDosStaticLink();

  COLORREF m_colorUnvisited;
  COLORREF m_colorVisited;
  BOOL m_bVisited;
  CString m_link;

protected:
  DECLARE_DYNAMIC(CDosStaticLink)
  CFont m_font;

protected:
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  afx_msg void OnClicked();
  DECLARE_MESSAGE_MAP()
};
