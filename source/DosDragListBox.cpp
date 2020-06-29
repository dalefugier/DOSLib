/////////////////////////////////////////////////////////////////////////////
// DosDragListBox.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosDragListBox.h"

IMPLEMENT_DYNAMIC(CDosDragListBox, CListBox)

CDosDragListBox::CDosDragListBox()
{
  m_DragState = DTS_NONE;
  m_DragOrigin = CPoint(0, 0);
  m_ScrollDelta = 0;
  m_ScrollTimer = 0;
  m_DeferSelect = FALSE;
  m_PrevInsPos = -1;
  m_PrevTop = -1;
}

CDosDragListBox::~CDosDragListBox()
{
}

int CDosDragListBox::HitTest(CPoint point) const
{
  ASSERT(IsWindow(m_hWnd));
  return LBItemFromPt(m_hWnd, point, FALSE); // no scrolling
}

BOOL CDosDragListBox::BeginDrag(CPoint point)
{
  UNREFERENCED_PARAMETER(point);
  m_PrevInsPos = -1;
  m_PrevTop = -1;
  m_DragState = DTS_PREDRAG;
  SendMessage(WM_LBUTTONUP, 0, 0);
  if (::GetCapture() != m_hWnd)
    SetCapture();
  return TRUE;
}

void CDosDragListBox::CancelDrag(CPoint point)
{
  UNREFERENCED_PARAMETER(point);
  EndDrag();
}

UINT CDosDragListBox::Dragging(CPoint point)
{
  AutoScroll(point);
  UpdateInsert(point);
  if (::WindowFromPoint(point) == m_hWnd)
    SetCursor(LoadCursor(0, IDC_ARROW));
  else
    SetCursor(LoadCursor(0, IDC_NO));
  return DL_CURSORSET;
}

void CDosDragListBox::Dropped(CPoint point)
{
  if (GetStyle() & LBS_SORT)
    return;

  int	InsPos = GetInsertPos(point);
  int	top = GetTopIndex();
  CStringArray ItemText;
  CDWordArray ItemData;
  GetSelectedItems(ItemText, ItemData, InsPos);
  PasteItems(ItemText, ItemData, InsPos);
  SetTopIndex(top);
}

void CDosDragListBox::EndDrag()
{
  ReleaseCapture();
  EndScroll();
  EraseInsert();
  m_DragState = DTS_NONE;
  m_DeferSelect = FALSE;
}

void CDosDragListBox::AutoScroll(CPoint point)
{
  CRect	cr, ir, hr;
  GetClientRect(cr);
  GetItemRect(0, ir);
  int	margin = ir.Height();

  CPoint cpt(point);
  ScreenToClient(&cpt);

  if (cpt.y < cr.top + margin)
    m_ScrollDelta = -1;
  else if (cpt.y >= cr.bottom - margin)
    m_ScrollDelta = 1;
  else
    m_ScrollDelta = 0;

  if (m_ScrollDelta && !m_ScrollTimer)
    m_ScrollTimer = SetTimer(SCROLL_TIMER, SCROLL_DELAY, 0);
}

void CDosDragListBox::EndScroll()
{
  if (m_ScrollTimer)
  {
    KillTimer(m_ScrollTimer);
    m_ScrollTimer = 0;
  }
}

int CDosDragListBox::GetInsertPos(CPoint point) const
{
  if (!GetCount())
    return 0;

  BOOL outside = FALSE;
  point.y += GetItemHeight(0) / 2;
  CPoint cpt(point);
  ScreenToClient(&cpt);
  int	InsPos = ItemFromPoint(cpt, outside);
  if (InsPos == GetCount() - 1)
  {
    CRect	r;
    if (outside || (GetItemRect(InsPos, r) && cpt.y > r.bottom))
      InsPos++;
  }

  return InsPos;
}

void CDosDragListBox::UpdateInsert(CPoint point)
{
  if (GetStyle() & LBS_SORT)
    return;

  int	InsPos = GetInsertPos(point);
  if (InsPos != m_PrevInsPos)
  {
    EraseInsert();
    DrawInsert(InsPos, TRUE);
    m_PrevInsPos = InsPos;
  }
}

void CDosDragListBox::EraseInsert()
{
  if (m_PrevInsPos >= 0)
  {
    DrawInsert(m_PrevInsPos, FALSE);
    m_PrevInsPos = -1;
  }
}

void CDosDragListBox::DrawInsert(int Item, bool Enable)
{
  ASSERT(Item >= 0 && Item <= GetCount());

  CDC	*pDC = GetDC();

  CRect	cr;
  GetClientRect(&cr);

  int	items = GetCount();

  int	y = 0;
  CRect	r;
  if (Item < items)
  {
    GetItemRect(Item, &r);
    y = r.top;
  }
  else
  {
    GetItemRect(items - 1, &r);
    y = r.bottom;
  }

  if (y >= cr.bottom)
    y = cr.bottom - 1;

  static const int ARROWS = 2;
  CRgn arrow[ARROWS];
  MakeArrow(CPoint(cr.left, y), TRUE, arrow[0]);
  MakeArrow(CPoint(cr.right, y), FALSE, arrow[1]);

  if (Enable)
  {
    COLORREF InsColor = RGB(255, 0, 0);
    CPen pen(PS_DOT, 1, InsColor);
    CBrush brush(InsColor);
    CPen* pPrevPen = pDC->SelectObject(&pen);
    pDC->SetBkMode(TRANSPARENT);
    pDC->MoveTo(cr.left, y);
    pDC->LineTo(cr.right, y);
    for (int i = 0; i < ARROWS; i++)
      pDC->FillRgn(&arrow[i], &brush);
    pDC->SelectObject(pPrevPen);
  }
  else
  {
    CRect	r(cr.left, y, cr.right, y + 1);
    RedrawWindow(&r, NULL);
    CWnd* pParent = GetParent();
    for (int i = 0; i < ARROWS; i++)
    {
      arrow[i].GetRgnBox(r);
      ClientToScreen(r);
      pParent->ScreenToClient(r);
      pParent->RedrawWindow(&r, NULL);
    }
  }

  ReleaseDC(pDC);
}

void CDosDragListBox::MakeArrow(CPoint point, bool left, CRgn& rgn)
{
  static const POINT ArrowPt[] = { {0, 0}, {5, 5}, {5, 2}, {9, 2}, {9, -1}, {5, -1}, {5, -5} };
  static const int	pts = sizeof(ArrowPt) / sizeof(POINT);
  POINT	pta[pts];
  int	dir = left ? -1 : 1;
  for (int i = 0; i < pts; i++)
  {
    pta[i].x = point.x + ArrowPt[i].x * dir;
    pta[i].y = point.y + ArrowPt[i].y;
  }
  rgn.CreatePolygonRgn(pta, pts, ALTERNATE);
}

int CDosDragListBox::GetSelectedItems(CStringArray& ItemText, CDWordArray& ItemData, int& InsertPos, bool Delete)
{
  CArray<int, int> sel;
  int	sels = 0;
  if (GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
  {
    sels = GetSelCount();
    sel.SetSize(sels);
    GetSelItems(sels, sel.GetData());
  }
  else
  {
    sels = 1;
    sel.Add(GetCurSel());
  }

  ItemText.SetSize(sels);
  ItemData.SetSize(sels);

  for (int i = sels - 1; i >= 0; i--)
  {
    int	pos = sel[i];
    CString	s;
    GetText(pos, s);
    ItemText[i] = s;
    ItemData[i] = (DWORD)GetItemData(pos);
    if (Delete)
    {
      DeleteString(pos);
      if (pos < InsertPos)
        InsertPos--;
    }
  }

  return sels;
}

void CDosDragListBox::PasteItems(const CStringArray& ItemText, const CDWordArray &ItemData, int InsertPos)
{
  DWORD	style = GetStyle();
  if (style & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
    SetSel(-1, FALSE);

  int	sels = (int)ItemText.GetSize();
  for (int i = 0; i < sels; i++)
  {
    int	pos = 0;
    if (style & LBS_SORT)
      pos = AddString(ItemText[i]);
    else
      pos = InsertString(InsertPos + i, ItemText[i]);

    SetItemData(pos, ItemData[i]);

    if (style & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
      SetSel(pos, TRUE);
    else
      SetCurSel(pos);
  }
}

BEGIN_MESSAGE_MAP(CDosDragListBox, CListBox)
  //{{AFX_MSG_MAP(CDosDragListBox)
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_TIMER()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDosDragListBox message handlers

void CDosDragListBox::OnDestroy()
{
  EndDrag();
  CListBox::OnDestroy();
}

void CDosDragListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
  if (m_DragState == DTS_NONE)
  {
    m_DragState = DTS_TRACK;
    m_DragOrigin = point;
    SetCapture();
  }

  // If extended selection mode and multiple items are selected, don't alter
  // selection on button down without modifier keys; could be start of drag
  m_DeferSelect = (GetStyle() & LBS_EXTENDEDSEL) &&
    GetSelCount() > 1 &&
    !(nFlags & (MK_SHIFT | MK_CONTROL));

  if (!m_DeferSelect)
    CListBox::OnLButtonDown(nFlags, point);
}

void CDosDragListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
  CPoint spt(point);
  ClientToScreen(&spt);

  switch (m_DragState)
  {
  case DTS_NONE:
  case DTS_PREDRAG:
    CListBox::OnLButtonUp(nFlags, point);
    break;

  case DTS_TRACK:
    if (m_DeferSelect) // if selection deferred in button down
    {
      SetSel(-1, FALSE);
      int	pos = HitTest(spt);
      if (pos >= 0)
        SetSel(pos, TRUE);
    }
    EndDrag();
    break;

  case DTS_DRAG:
    EndDrag();
    Dropped(spt);
    break;
  }
}

void CDosDragListBox::OnMouseMove(UINT nFlags, CPoint point)
{
  CPoint spt(point);
  ClientToScreen(&spt);

  if (m_DragState == DTS_TRACK)
  {
    if (nFlags & (MK_SHIFT | MK_CONTROL))
      CListBox::OnMouseMove(nFlags, point);
    else
    {
      if (abs(m_DragOrigin.x - point.x) > GetSystemMetrics(SM_CXDRAG) ||
        abs(m_DragOrigin.y - point.y) > GetSystemMetrics(SM_CYDRAG)
        )
      {
        if (BeginDrag(spt))
          m_DragState = DTS_DRAG;
      }
    }
  }

  if (m_DragState == DTS_DRAG)
    Dragging(spt);
}

void CDosDragListBox::OnTimer(UINT_PTR nIDEvent)
{
  if (nIDEvent == SCROLL_TIMER)
  {
    if (m_ScrollDelta) // if scrolling
    {
      int	NewTop = GetTopIndex() + m_ScrollDelta;
      if (NewTop != m_PrevTop) // if scroll position changed
      {
        EraseInsert();         // erase previous insert before scrolling
        SetTopIndex(NewTop); // scroll to new position
        CPoint pt;
        GetCursorPos(&pt);
        UpdateInsert(pt);    // draw new insert position
        m_PrevTop = NewTop;    // update scroll position
      }
    }
  }
  else
    CListBox::OnTimer(nIDEvent);
}

BOOL CDosDragListBox::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN)
  {
    switch (pMsg->wParam)
    {
    case VK_ESCAPE:
      if (m_DragState != DTS_NONE)
      {
        CPoint pt;
        GetCursorPos(&pt);
        CancelDrag(pt);
        return TRUE;
      }
      break;
    }
  }
  return CListBox::PreTranslateMessage(pMsg);
}
