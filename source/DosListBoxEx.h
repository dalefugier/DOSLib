/////////////////////////////////////////////////////////////////////////////
// DosListBoxEx.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosListBoxExBuddy;
class CDosInPlaceEdit;
class CDosInPlaceButton;

class CDosListBoxEx : public CDragListBox
{
  DECLARE_DYNCREATE(CDosListBoxEx)

public:
  CDosListBoxEx();
  virtual ~CDosListBoxEx();

  enum
  {
    LBEX_EDITBUTTON = 0x4000L
  };

  void AllowEditing(BOOL bAllowEditing = TRUE) { m_bAllowEditing = bAllowEditing; }
  void AllowDrag(BOOL bAllowDrag = TRUE) { m_bAllowDrag = bAllowDrag; }

  void BeginEditing(int iItem);
  void EndEditing(BOOL fCancel);

  void EditNew();
  void SetEditStyle(DWORD dwEditStyle);
  HWND GetEditHandle() const;
  void SetEditText(const CString& strNewText) const;

  int MoveItemUp(int iItem);
  int MoveItemDown(int iItem);
  void SwapItems(int iFirstItem, int iSecondItem);
  void SetItem(int iItem, LPCTSTR szItemText, DWORD_PTR dwItemData);
  void SetItemText(int iItem, LPCTSTR szItemText);

private:
  void CreateEdit();
  UINT ListBoxExStyleFlags() const;
  UINT ListBoxExStyleFlagsEX() const;
  BOOL AltKeyPressed(UINT uFlag) const;

private:
  enum
  {
    LBEX_ID_EDITCONTROL = 1,
    LBEX_ID_BUTTONCONTROL = 2,
    LBEX_LASTITEM_MAGIC = 0x45424558 // 'LBEX'
  };

private:
  static UINT const m_DragListMsg;
  static UINT const m_IPCEndEditMsg;
  CDosListBoxExBuddy* m_pBuddy;
  DWORD m_dwEditStyle;
  CDosInPlaceEdit* m_pEdit;
  CDosInPlaceButton* m_pBrowseButton;
  CRect m_rcEdit;
  CRect m_rcButton;
  int m_iSelected;
  int m_iEdited;
  BOOL m_bAllowEditing;
  BOOL m_bAllowDrag;

public:
  virtual BOOL OnBeginEditing(int iItem);
  virtual BOOL OnEndEditing(int iItem, BOOL fCanceled);
  virtual void OnBrowseButton(int iItem);
  virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void PreSubclassWindow();
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual void DrawInsert(int nIndex);
  void DrawSeparator(int nIndex);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg LRESULT OnEndEditMessage(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()
};

class CDosListBoxExBuddy : public CWnd
{
public:
  CDosListBoxExBuddy(UINT nBitmapID);
  virtual ~CDosListBoxExBuddy();

  enum
  {
    bmp_width = 88,
    bmp_height = 20,
    bmp_btnwidth = 22,
    btn_new = 0,
    btn_del = 1,
    btn_up = 2,
    btn_down = 3,
    bmp_numbtn = 4
  };

  void SetListBox(CDosListBoxEx *pListBox) { m_pListBoxEx = pListBox; }

protected:
  virtual void PreSubclassWindow();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  afx_msg void OnPaint();
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  DECLARE_MESSAGE_MAP()

private:
  void CreateTooltips();
  void SetTipText(UINT nID, LPTSTR szTipText);
  int  FindButton(const CPoint& point);
  void InvalidateButton(int iIndex, BOOL bUpdateWindow = TRUE);
  void DoClick(int iIndex);

private:
  CDosListBoxEx* m_pListBoxEx;
  CBitmap m_ButtonBitmap;
  UINT m_iButton;
  BOOL m_bButtonPressed;
  CRect m_arcButtons[bmp_numbtn];
  CToolTipCtrl m_ToolTip;
  HBITMAP m_hBitmap;
};
