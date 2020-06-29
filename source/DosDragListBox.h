/////////////////////////////////////////////////////////////////////////////
// DosDragListBox.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosDragListBox : public CListBox
{
  DECLARE_DYNAMIC(CDosDragListBox)

public:
  CDosDragListBox();
  virtual ~CDosDragListBox();

  virtual	BOOL BeginDrag(CPoint point);
  virtual	void CancelDrag(CPoint point);
  virtual	UINT Dragging(CPoint point);
  virtual	void Dropped(CPoint point);
  virtual	void EndDrag();

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

  enum // drag states
  {
    DTS_NONE,    // not dragging
    DTS_TRACK,   // left button down, but motion hasn't reached drag threshold
    DTS_PREDRAG, // motion reached drag threshold, but drag hasn't started yet
    DTS_DRAG,    // drag is in progress
  };

  enum
  {
    SCROLL_TIMER = 150, // scroll timer ID
    SCROLL_DELAY = 50   // scroll delay in milliseconds
  };

  int m_DragState;	// drag state; see enum above
  CPoint m_DragOrigin;	// point where left button was pressed, in client coords
  int m_ScrollDelta;	// scroll by this amount per timer tick
  INT_PTR m_ScrollTimer;	// if non-zero, timer instance for scrolling
  bool m_DeferSelect;	// true if select was deferred to button up
  int m_PrevInsPos;	// previous position of insert marker
  int m_PrevTop;		// previous scroll position

  virtual void DrawInsert(int nItem, bool bEnable);

  int HitTest(CPoint point) const;
  void AutoScroll(CPoint point);
  void EndScroll();
  int GetInsertPos(CPoint point) const;
  void UpdateInsert(CPoint point);
  void EraseInsert();
  static void MakeArrow(CPoint point, bool bLeft, CRgn& rgn);
  int GetSelectedItems(CStringArray& ItemText, CDWordArray& ItemData, int& nInsertPos, bool bDelete = TRUE);
  void PasteItems(const CStringArray& ItemText, const CDWordArray& ItemData, int nInsertPos);
};
