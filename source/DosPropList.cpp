/////////////////////////////////////////////////////////////////////////////
// DosPropList.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosPropList.h"

CDosPropList::CDosPropList()
{
}

CDosPropList::~CDosPropList()
{
}

BEGIN_MESSAGE_MAP(CDosPropList, CListBox)
  ON_WM_CREATE()
  ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
  ON_WM_LBUTTONUP()
  ON_WM_KILLFOCUS()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_CBN_KILLFOCUS(IDC_PROPCMBBOX, OnKillfocusCmbBox)
  ON_CBN_SELCHANGE(IDC_PROPCMBBOX, OnSelchangeCmbBox)
  ON_EN_KILLFOCUS(IDC_PROPEDITBOX, OnKillfocusEditBox)
  ON_EN_CHANGE(IDC_PROPEDITBOX, OnChangeEditBox)
  ON_BN_CLICKED(IDC_PROPBTNCTRL, OnButton)
END_MESSAGE_MAP()

BOOL CDosPropList::PreCreateWindow(CREATESTRUCT& cs)
{
  if (!CListBox::PreCreateWindow(cs))
    return FALSE;

  cs.style &= ~(LBS_OWNERDRAWVARIABLE | LBS_SORT);
  cs.style |= LBS_OWNERDRAWFIXED;

  m_bTracking = FALSE;
  m_nDivider = 0;
  m_bDivIsSet = FALSE;

  return TRUE;
}

void CDosPropList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
  lpMeasureItemStruct->itemHeight = 20; //pixels
}

void CDosPropList::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
  CDC dc;
  dc.Attach(lpDIS->hDC);
  CRect rectFull = lpDIS->rcItem;
  CRect rect = rectFull;
  if (m_nDivider == 0)
    m_nDivider = rect.Width() / 2;
  rect.left = m_nDivider;
  CRect rect2 = rectFull;
  rect2.right = rect.left - 1;
  UINT nIndex = lpDIS->itemID;

  if (nIndex != (UINT)-1)
  {
    //draw two rectangles, one for each row column
    dc.FillSolidRect(rect2, RGB(192, 192, 192));
    dc.DrawEdge(rect2, EDGE_SUNKEN, BF_BOTTOMRIGHT);
    dc.DrawEdge(rect, EDGE_SUNKEN, BF_BOTTOM);

    //get the CDosPropItem for the current row
    CDosPropItem* pItem = (CDosPropItem*)GetItemDataPtr(nIndex);

    //write the property name in the first rectangle
    dc.SetBkMode(TRANSPARENT);
    dc.DrawText(pItem->m_propName, CRect(rect2.left + 3, rect2.top + 3,
      rect2.right - 3, rect2.bottom + 3),
      DT_LEFT | DT_SINGLELINE);

    //write the initial property value in the second rectangle
    dc.DrawText(pItem->m_curValue, CRect(rect.left + 3, rect.top + 3,
      rect.right + 3, rect.bottom + 3),
      DT_LEFT | DT_SINGLELINE);
  }
  dc.Detach();
}

int CDosPropList::AddItem(CString str)
{
  int nIndex = AddString(str);
  return nIndex;
}

int CDosPropList::AddPropItem(CDosPropItem* pItem)
{
  int nIndex = AddString(L"");
  SetItemDataPtr(nIndex, pItem);
  return nIndex;
}

int CDosPropList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CListBox::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_bDivIsSet = FALSE;
  m_nDivider = 0;
  m_bTracking = FALSE;

  m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
  m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

  m_SSerif8Font.CreatePointFont(80, L"MS Sans Serif");

  return 0;
}

void CDosPropList::OnSelchange()
{
  CRect rect;
  CString lBoxSelText;

  //m_curSel = GetCurSel();

  GetItemRect(m_curSel, rect);
  rect.left = m_nDivider;

  CDosPropItem* pItem = (CDosPropItem*)GetItemDataPtr(m_curSel);

  if (m_btnCtrl)
    m_btnCtrl.ShowWindow(SW_HIDE);

  if (pItem->m_nItemType == PIT_COMBO)
  {
    //display the combo box.  If the combo box has already been
    //created then simply move it to the new location, else create it
    m_nLastBox = 0;
    if (m_cmbBox)
      m_cmbBox.MoveWindow(rect);
    else
    {
      rect.bottom += 100;
      m_cmbBox.Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_BORDER,
        rect, this, IDC_PROPCMBBOX);
      m_cmbBox.SetFont(&m_SSerif8Font);
    }

    //add the choices for this particular property
    CString cmbItems = pItem->m_cmbItems;
    lBoxSelText = pItem->m_curValue;

    m_cmbBox.ResetContent();
    m_cmbBox.AddString(L"");
    int i, i2;
    i = 0;
    while ((i2 = cmbItems.Find('|', i)) != -1)
    {
      m_cmbBox.AddString(cmbItems.Mid(i, i2 - i));
      i = i2 + 1;
    }

    m_cmbBox.ShowWindow(SW_SHOW);
    m_cmbBox.SetFocus();

    //jump to the property's current value in the combo box
    int j = m_cmbBox.FindStringExact(0, lBoxSelText);
    if (j != CB_ERR)
      m_cmbBox.SetCurSel(j);
    else
      m_cmbBox.SetCurSel(0);
  }
  else if (pItem->m_nItemType == PIT_EDIT)
  {
    //display edit box
    m_nLastBox = 1;
    m_prevSel = m_curSel;
    rect.bottom -= 3;
    if (m_editBox)
      m_editBox.MoveWindow(rect);
    else
    {
      m_editBox.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | WS_BORDER,
        rect, this, IDC_PROPEDITBOX);
      m_editBox.SetFont(&m_SSerif8Font);
    }

    lBoxSelText = pItem->m_curValue;

    m_editBox.ShowWindow(SW_SHOW);
    m_editBox.SetFocus();
    //set the text in the edit box to the property's current value
    m_editBox.SetWindowText(lBoxSelText);
    m_editBox.SetSel(0, -1);
  }
  else
    DisplayButton(rect);
}

void CDosPropList::DisplayButton(CRect region)
{
  //displays a button if the property is a file/color/font chooser
  m_nLastBox = 2;
  m_prevSel = m_curSel;

  if (region.Width() > 25)
    region.left = region.right - 25;
  region.bottom -= 3;

  if (m_btnCtrl)
    m_btnCtrl.MoveWindow(region);
  else
  {
    m_btnCtrl.Create(L"...", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, region, this, IDC_PROPBTNCTRL);
    m_btnCtrl.SetFont(&m_SSerif8Font);
  }

  m_btnCtrl.ShowWindow(SW_SHOW);
  m_btnCtrl.SetFocus();
}

void CDosPropList::OnKillFocus(CWnd* pNewWnd)
{
  //m_btnCtrl.ShowWindow(SW_HIDE);

  CListBox::OnKillFocus(pNewWnd);
}

void CDosPropList::OnKillfocusCmbBox()
{
  m_cmbBox.ShowWindow(SW_HIDE);

  Invalidate();
}

void CDosPropList::OnKillfocusEditBox()
{
  CString newStr;
  m_editBox.ShowWindow(SW_HIDE);

  Invalidate();
}

void CDosPropList::OnSelchangeCmbBox()
{
  CString selStr;
  if (m_cmbBox)
  {
    m_cmbBox.GetLBText(m_cmbBox.GetCurSel(), selStr);
    CDosPropItem* pItem = (CDosPropItem*)GetItemDataPtr(m_curSel);
    pItem->m_curValue = selStr;
  }
}

void CDosPropList::OnChangeEditBox()
{
  CString newStr;
  m_editBox.GetWindowText(newStr);

  CDosPropItem* pItem = (CDosPropItem*)GetItemDataPtr(m_curSel);
  pItem->m_curValue = newStr;
}

void CDosPropList::OnButton()
{
  CDosPropItem* pItem = (CDosPropItem*)GetItemDataPtr(m_curSel);

  //display the appropriate common dialog depending on what type
  //of chooser is associated with the property
  if (pItem->m_nItemType == PIT_COLOR)
  {
    COLORREF initClr;
    CString currClr = pItem->m_curValue;
    //parse the property's current color value
    if (currClr.Find(L"RGB") > -1)
    {
      int j = currClr.Find(',', 3);
      CString bufr = currClr.Mid(4, j - 4);
      int RVal = _ttoi(bufr);
      int j2 = currClr.Find(',', j + 1);
      bufr = currClr.Mid(j + 1, j2 - (j + 1));
      int GVal = _ttoi(bufr);
      int j3 = currClr.Find(')', j2 + 1);
      bufr = currClr.Mid(j2 + 1, j3 - (j2 + 1));
      int BVal = _ttoi(bufr);
      initClr = RGB(RVal, GVal, BVal);
    }
    else
      initClr = 0;

    CColorDialog ClrDlg(initClr);

    if (IDOK == ClrDlg.DoModal())
    {
      COLORREF selClr = ClrDlg.GetColor();
      CString clrStr;
      clrStr.Format(L"RGB(%d,%d,%d)", GetRValue(selClr),
        GetGValue(selClr), GetBValue(selClr));
      m_btnCtrl.ShowWindow(SW_HIDE);

      pItem->m_curValue = clrStr;
      Invalidate();
    }
  }
  else if (pItem->m_nItemType == PIT_FILE)
  {
    CString SelectedFile;
    CString Filter(L"Gif Files (*.gif)|*.gif||");

    CFileDialog FileDlg(TRUE, NULL, NULL, NULL,
      Filter);

    CString currPath = pItem->m_curValue;
    FileDlg.m_ofn.lpstrTitle = L"Select file";
    if (currPath.GetLength() > 0)
      FileDlg.m_ofn.lpstrInitialDir = currPath.Left(
        currPath.GetLength() - currPath.ReverseFind('\\'));

    if (IDOK == FileDlg.DoModal())
    {
      SelectedFile = FileDlg.GetPathName();

      m_btnCtrl.ShowWindow(SW_HIDE);

      pItem->m_curValue = SelectedFile;
      Invalidate();
    }
  }
  else if (pItem->m_nItemType == PIT_FONT)
  {
    CFontDialog FontDlg(NULL, CF_EFFECTS | CF_SCREENFONTS, NULL, this);

    if (IDOK == FontDlg.DoModal())
    {
      CString faceName = FontDlg.GetFaceName();

      m_btnCtrl.ShowWindow(SW_HIDE);

      pItem->m_curValue = faceName;
      Invalidate();
    }
  }
}

void CDosPropList::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (m_bTracking)
  {
    //if columns were being resized then this indicates
    //that mouse is up so resizing is done.  Need to redraw
    //columns to reflect their new widths.

    m_bTracking = FALSE;
    //if mouse was captured then release it
    if (GetCapture() == this)
      ::ReleaseCapture();

    ::ClipCursor(NULL);

    CClientDC dc(this);
    InvertLine(&dc, CPoint(point.x, m_nDivTop), CPoint(point.x, m_nDivBtm));
    //set the divider position to the new value
    m_nDivider = point.x;

    //redraw
    Invalidate();
  }
  else
  {
    BOOL loc;
    int i = ItemFromPoint(point, loc);
    m_curSel = i;
    CListBox::OnLButtonUp(nFlags, point);
  }
}

void CDosPropList::OnLButtonDown(UINT nFlags, CPoint point)
{
  if ((point.x >= m_nDivider - 5) && (point.x <= m_nDivider + 5))
  {
    //if mouse clicked on divider line, then start resizing

    ::SetCursor(m_hCursorSize);

    CRect windowRect;
    GetWindowRect(windowRect);
    windowRect.left += 10; windowRect.right -= 10;
    //do not let mouse leave the list box boundary
    ::ClipCursor(windowRect);

    if (m_cmbBox)
      m_cmbBox.ShowWindow(SW_HIDE);
    if (m_editBox)
      m_editBox.ShowWindow(SW_HIDE);

    CRect clientRect;
    GetClientRect(clientRect);

    m_bTracking = TRUE;
    m_nDivTop = clientRect.top;
    m_nDivBtm = clientRect.bottom;
    m_nOldDivX = point.x;

    CClientDC dc(this);
    InvertLine(&dc, CPoint(m_nOldDivX, m_nDivTop), CPoint(m_nOldDivX, m_nDivBtm));

    //capture the mouse
    SetCapture();
  }
  else
  {
    m_bTracking = FALSE;
    CListBox::OnLButtonDown(nFlags, point);
  }
}

void CDosPropList::OnMouseMove(UINT nFlags, CPoint point)
{
  if (m_bTracking)
  {
    //move divider line to the mouse pos. if columns are
    //currently being resized
    CClientDC dc(this);
    //remove old divider line
    InvertLine(&dc, CPoint(m_nOldDivX, m_nDivTop), CPoint(m_nOldDivX, m_nDivBtm));
    //draw new divider line
    InvertLine(&dc, CPoint(point.x, m_nDivTop), CPoint(point.x, m_nDivBtm));
    m_nOldDivX = point.x;
  }
  else if ((point.x >= m_nDivider - 5) && (point.x <= m_nDivider + 5))
    //set the cursor to a sizing cursor if the cursor is over the row divider
    ::SetCursor(m_hCursorSize);
  else
    CListBox::OnMouseMove(nFlags, point);
}

void CDosPropList::InvertLine(CDC* pDC, CPoint ptFrom, CPoint ptTo)
{
  int nOldMode = pDC->SetROP2(R2_NOT);

  pDC->MoveTo(ptFrom);
  pDC->LineTo(ptTo);

  pDC->SetROP2(nOldMode);
}

void CDosPropList::PreSubclassWindow()
{
  m_bDivIsSet = FALSE;
  m_nDivider = 0;
  m_bTracking = FALSE;
  m_curSel = 0;

  m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
  m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

  m_SSerif8Font.CreatePointFont(80, L"MS Sans Serif");
}

BOOL CDosPropList::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
  {
    if (GetKeyState(VK_SHIFT) < 0)
    {
      if (m_curSel > 0)
        m_curSel--;
      else
        m_curSel = GetCount() - 1;
    }
    else
    {
      if (m_curSel < GetCount() - 1)
        m_curSel++;
      else
        m_curSel = 0;
    }
    OnSelchange();
    return TRUE;
  }

  return CListBox::PreTranslateMessage(pMsg);
}
