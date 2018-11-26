/////////////////////////////////////////////////////////////////////////////
// DosPropList.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#define PIT_COMBO	0  //PIT = property item type
#define PIT_EDIT	1
#define PIT_COLOR	2
#define PIT_FONT	3
#define PIT_FILE	4

#define IDC_PROPCMBBOX   712
#define IDC_PROPEDITBOX  713
#define IDC_PROPBTNCTRL  714

class CDosPropItem : public CObject
{
public:
  CDosPropItem() {}

  CDosPropItem(CString propName, CString curValue, int nItemType, CString cmbItems)
  {
    m_propName = propName;
    m_curValue = curValue;
    m_nItemType = nItemType;
    m_cmbItems = cmbItems;
  }

  void Set(CString propName, CString curValue, int nItemType, CString cmbItems)
  {
    m_propName = propName;
    m_curValue = curValue;
    m_nItemType = nItemType;
    m_cmbItems = cmbItems;
  }

  CString Get() { return m_curValue; }

public:
  CString m_propName;
  CString m_curValue;
  int     m_nItemType;
  CString m_cmbItems;
};

class CDosPropList : public CListBox
{
public:
  CDosPropList();

public:
  int AddItem(CString txt);
  int AddPropItem(CDosPropItem* pItem);

  //{{AFX_VIRTUAL(CDosPropList)
public:
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

public:
  virtual ~CDosPropList();

protected:
  //{{AFX_MSG(CDosPropList)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSelchange();
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  //}}AFX_MSG
  afx_msg void OnKillfocusCmbBox();
  afx_msg void OnSelchangeCmbBox();
  afx_msg void OnKillfocusEditBox();
  afx_msg void OnChangeEditBox();
  afx_msg void OnButton();

  DECLARE_MESSAGE_MAP()

  void InvertLine(CDC* pDC, CPoint ptFrom, CPoint ptTo);
  void DisplayButton(CRect region);

  CComboBox m_cmbBox;
  CEdit m_editBox;
  CButton m_btnCtrl;
  CFont m_SSerif8Font;

  int m_curSel, m_prevSel;
  int m_nDivider;
  int m_nDivTop;
  int m_nDivBtm;
  int m_nOldDivX;
  int m_nLastBox;
  BOOL m_bTracking;
  BOOL m_bDivIsSet;
  HCURSOR m_hCursorArrow;
  HCURSOR m_hCursorSize;
};

//
// END CDosPropList class
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
