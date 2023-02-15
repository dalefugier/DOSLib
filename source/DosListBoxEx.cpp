/////////////////////////////////////////////////////////////////////////////
// DosListBoxEx.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosInPlaceCtrls.h"
#include "DosListBoxEx.h"

UINT const CDosListBoxEx::m_DragListMsg = ::RegisterWindowMessage(DRAGLISTMSGSTRING);
UINT const CDosListBoxEx::m_IPCEndEditMsg = ::RegisterWindowMessage(CDosInPlace::MsgString());

IMPLEMENT_DYNCREATE(CDosListBoxEx, CDragListBox)

BEGIN_MESSAGE_MAP(CDosListBoxEx, CDragListBox)
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONUP()
  ON_WM_SYSKEYDOWN()
  ON_WM_KEYDOWN()
  ON_REGISTERED_MESSAGE(m_IPCEndEditMsg, OnEndEditMessage)
END_MESSAGE_MAP()

CDosListBoxEx::CDosListBoxEx()
{
  m_pBuddy = NULL;
  m_pBrowseButton = NULL;
  m_dwEditStyle = LBEX_EDITBUTTON;
  m_pEdit = NULL;
  m_iSelected = -1;
  m_iEdited = -1;
  m_bAllowEditing = TRUE;
  m_bAllowDrag = TRUE;
}

CDosListBoxEx::~CDosListBoxEx()
{
  if (m_pEdit)
    delete m_pEdit;
  if (m_pBrowseButton)
    delete m_pBrowseButton;
}

UINT CDosListBoxEx::ListBoxExStyleFlags() const
{
  return WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_EXTENDEDSEL
    | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_NOTIFY
    | LBS_WANTKEYBOARDINPUT | LBS_DISABLENOSCROLL | LBS_OWNERDRAWFIXED;
}

UINT CDosListBoxEx::ListBoxExStyleFlagsEX() const
{
  return WS_EX_CLIENTEDGE;
}

BOOL CDosListBoxEx::AltKeyPressed(UINT uFlag) const
{
  return (uFlag & KF_ALTDOWN ? true : false);
}

BOOL CDosListBoxEx::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style |= ListBoxExStyleFlags();
  cs.dwExStyle |= ListBoxExStyleFlagsEX();
  return CWnd::PreCreateWindow(cs);
}

void CDosListBoxEx::PreSubclassWindow()
{
  // Make sure it has the correct styles
  ModifyStyle(0, ListBoxExStyleFlags(), SWP_SHOWWINDOW);
  ModifyStyleEx(0, ListBoxExStyleFlagsEX(), SWP_SHOWWINDOW);
  CDragListBox::PreSubclassWindow();
  CreateEdit();
}

int CDosListBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDragListBox::OnCreate(lpCreateStruct) == -1)
    return -1;

  CreateEdit();
  return 0;
}

void CDosListBoxEx::CreateEdit()
{
  // Create an in-place edit box
  if (m_pEdit == NULL)
  {
    m_pEdit = new CDosInPlaceEdit;
    m_pEdit->Create(WS_CHILD | WS_BORDER | m_dwEditStyle, CRect(0, 0, 0, 0), this, LBEX_ID_EDITCONTROL);
    m_pEdit->SetFont(GetFont());

    // Create the browse button
    m_pBrowseButton = new CDosInPlaceButton;
    m_pBrowseButton->Create(L"...", WS_CHILD | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, LBEX_ID_BUTTONCONTROL);
  }
}

void CDosListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
  TEXTMETRIC metrics;
  HDC dc = ::GetDC(m_hWnd);
  GetTextMetrics(dc, &metrics);
  ::ReleaseDC(m_hWnd, dc);

  lpMeasureItemStruct->itemHeight = metrics.tmHeight + metrics.tmExternalLeading;
}

void CDosListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  if (lpDrawItemStruct->itemID == -1)
    return;

  CString strItemText;
  CRect rcText(lpDrawItemStruct->rcItem);
  COLORREF clrItemText, clrOldTextColor;
  rcText.left += 8;

  // Act upon the item state
  switch (lpDrawItemStruct->itemAction)
  {
  case ODA_SELECT:
  case ODA_DRAWENTIRE:
    // Is the item selected? 
    if (lpDrawItemStruct->itemState & ODS_SELECTED)
    {
      clrItemText = GetSysColor(COLOR_HIGHLIGHTTEXT);
      // Clear the rectangle
      FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)(COLOR_ACTIVECAPTION + 1));
    }
    else
    {
      clrItemText = GetSysColor(COLOR_WINDOWTEXT);
      // Clear the rectangle
      FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)(COLOR_WINDOW + 1));
    }

    clrOldTextColor = SetTextColor(lpDrawItemStruct->hDC, clrItemText);
    SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);

    // Display the text associated with the item. 
    if (lpDrawItemStruct->itemData != LBEX_LASTITEM_MAGIC)
    {
      GetText(lpDrawItemStruct->itemID, strItemText);
      DrawText(lpDrawItemStruct->hDC, LPCTSTR(strItemText), strItemText.GetLength(), &rcText, DT_SINGLELINE | DT_VCENTER);
    }
    else
    {
      DrawText(lpDrawItemStruct->hDC, L"--- Last Item", strItemText.GetLength(), &rcText, DT_SINGLELINE | DT_VCENTER);
    }

    // Is the item selected? 
    if (lpDrawItemStruct->itemState & ODS_SELECTED)
    {
      SetTextColor(lpDrawItemStruct->hDC, clrOldTextColor);
      DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
    }
    break;
  }
}

void CDosListBoxEx::DrawSeparator(int nIndex)
{
  if (nIndex == -1)
  {
    return;
  }

  CBrush* pBrush = CDC::GetHalftoneBrush();
  CRect rect;
  GetClientRect(&rect);
  CRgn rgn;
  rgn.CreateRectRgnIndirect(&rect);
  CDC* pDC = GetDC();

  // Prevent drawing outside of listbox
  // This can happen at the top of the listbox since the listbox's DC is the
  // parent's DC
  pDC->SelectClipRgn(&rgn);

  GetItemRect(nIndex, &rect);
  rect.bottom = rect.top + 2;
  rect.top -= 2;
  rect.left += 5;
  rect.right -= 5;
  CBrush* pBrushOld = pDC->SelectObject(pBrush);

  // Draw main line
  pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);

  // Draw vertical lines
  pDC->PatBlt(rect.left - 3, rect.top - 4, 3, rect.Height() + 8, PATINVERT);
  pDC->PatBlt(rect.right, rect.top - 4, 3, rect.Height() + 8, PATINVERT);

  pDC->SelectObject(pBrushOld);
  ReleaseDC(pDC);
}

void CDosListBoxEx::DrawInsert(int nIndex)
{
  if (m_nLast != nIndex)
  {
    DrawSeparator(m_nLast);
    DrawSeparator(nIndex);
  }
  // Set last selected
  m_nLast = nIndex;
}

BOOL CDosListBoxEx::OnChildNotify(UINT nMessage, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
  if (nMessage == m_DragListMsg)
  {
    ASSERT(pLResult != NULL);
    if (m_iEdited == -1 && m_bAllowDrag)
    {
      LPDRAGLISTINFO pInfo = (LPDRAGLISTINFO)lParam;
      ASSERT(pInfo != NULL);
      switch (pInfo->uNotification)
      {
      case DL_BEGINDRAG:
        // Removed from the MFC implementation
        //*pLResult = BeginDrag(pInfo->ptCursor);
        *pLResult = TRUE;
        break;

      case DL_CANCELDRAG:
        CancelDrag(pInfo->ptCursor);
        break;

      case DL_DRAGGING:
        *pLResult = Dragging(pInfo->ptCursor);
        break;

      case DL_DROPPED:
        Dropped(GetCurSel(), pInfo->ptCursor);
        break;
      }
    }
    else
    {
      *pLResult = FALSE;
    }

    return TRUE;  // Message handled
  }

  return CListBox::OnChildNotify(nMessage, wParam, lParam, pLResult);
}

void CDosListBoxEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  CDragListBox::OnKeyDown(nChar, nRepCnt, nFlags);

  if ((nChar == VK_F2) && (m_iSelected != -1) && m_bAllowEditing && (OnBeginEditing(m_iSelected) != FALSE))
  {
    BeginEditing(m_iSelected);
    return;
  }

  if ((nChar == VK_DELETE) && (m_iSelected != -1))
  {
    DeleteString(m_iSelected);
    return;
  }

  if (nChar == VK_INSERT)
  {
    EditNew();
    return;
  }
}

void CDosListBoxEx::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (AltKeyPressed(nFlags) && m_bAllowDrag)
  {
    switch (nChar)
    {
    case VK_UP:
    case VK_LEFT:
      MoveItemUp(GetCurSel());
      break;

    case VK_DOWN:
    case VK_RIGHT:
      MoveItemDown(GetCurSel());
      break;
    }
  }

  CDragListBox::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CDosListBoxEx::OnLButtonDown(UINT nFlags, CPoint point)
{
  ClientToScreen(&point);
  int iItem = ItemFromPt(point, FALSE);
  if (iItem != -1)
  {
    if (m_iSelected != iItem)
      m_iSelected = iItem;
  }

  CDragListBox::OnLButtonDown(nFlags, point);
}

void CDosListBoxEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  CDragListBox::OnLButtonDblClk(nFlags, point);

  if (m_bAllowEditing)
  {
    ClientToScreen(&point);
    int iItem = ItemFromPt(point, FALSE);
    TRACE1("LButtonDblClk: %d\n", iItem);

    if ((iItem != -1) && m_bAllowEditing && (OnBeginEditing(iItem) != FALSE))
    {
      // Begin Editing
      BeginEditing(iItem);
    }
  }
}

void CDosListBoxEx::OnLButtonUp(UINT nFlags, CPoint point)
{
  CDragListBox::OnLButtonUp(nFlags, point);
}

BOOL CDosListBoxEx::OnBeginEditing(int iItem)
{
  UNUSED_ALWAYS(iItem);
  return TRUE;
}

BOOL CDosListBoxEx::OnEndEditing(int iItem, BOOL fCanceled)
{
  UNUSED_ALWAYS(iItem);
  UNUSED_ALWAYS(fCanceled);
  return TRUE;
}

void CDosListBoxEx::OnBrowseButton(int iItem)
{
  UNUSED_ALWAYS(iItem);
}

void CDosListBoxEx::BeginEditing(int iItem)
{
  if (m_bAllowEditing && (m_iEdited == -1))
  {
    if (iItem == -1)
    {
      iItem = m_iSelected;
    }
    else
      if (iItem > GetCount() - 1)
      {
        iItem = GetCount();
        AddString(L"");
      }

    // Make it the current selection
    SetCurSel(iItem);
    m_iSelected = iItem;

    // Save item index
    m_iEdited = iItem;

    // Retrieve item text
    CString strItemText;
    GetText(iItem, strItemText);

    // Cache edit/button rectangles
    GetItemRect(iItem, m_rcEdit);
    m_rcButton.CopyRect(m_rcEdit);

    // Adjust for the edit and the button
    m_rcEdit.left += 5;
    if (m_dwEditStyle & LBEX_EDITBUTTON)
    {
      m_rcEdit.right -= 30;
    }
    else
    {
      m_rcEdit.right -= 5;
    }
    m_rcEdit.InflateRect(0, 2);

    // Initialize the edit control with the item text
    m_pEdit->SetWindowText(strItemText);

    // Show the edit control
    m_pEdit->Show(m_rcEdit);

    if (m_dwEditStyle & LBEX_EDITBUTTON)
    {
      m_rcButton.left = m_rcEdit.right;
      m_rcButton.right -= 5;
      m_rcButton.InflateRect(0, 2);
      m_pBrowseButton->Show(m_rcButton);
    }
  }
}

void CDosListBoxEx::EditNew()
{
  BeginEditing(GetCount());
}

void CDosListBoxEx::EndEditing(BOOL fCancel)
{
  // Hide the edit box and browse button
  m_pEdit->Hide();
  m_pBrowseButton->Hide();

  // Update item text
  CString strNewItemText;
  m_pEdit->GetWindowText(strNewItemText);
  if (strNewItemText.IsEmpty())
  {
    DeleteString(m_iEdited);
  }
  else
    if (!fCancel)
    {
      // Replace the text
      SetItemText(m_iEdited, LPCTSTR(strNewItemText));
      // Select the edited item
      SetCurSel(m_iEdited);
    }

  m_iEdited = -1;
  Invalidate();
}

void CDosListBoxEx::SetEditStyle(DWORD dwEditStyle)
{
  if (m_dwEditStyle != dwEditStyle)
  {
    m_dwEditStyle = dwEditStyle;
    delete m_pEdit;
    delete m_pBrowseButton;
    m_pEdit = NULL;
    m_pBrowseButton = NULL;
    CreateEdit();
  }
}

void CDosListBoxEx::SetEditText(const CString& strNewText) const
{
  if (m_pEdit && m_pEdit->IsWindowVisible())
  {
    m_pEdit->SetWindowText(LPCTSTR(strNewText));
    m_pEdit->SetFocus();
  }
}

HWND CDosListBoxEx::GetEditHandle() const
{
  return m_pEdit ? m_pEdit->m_hWnd : NULL;
}

void CDosListBoxEx::SetItem(int iItem, LPCTSTR szItemText, DWORD_PTR dwItemData)
{
  ASSERT(iItem < GetCount());

  SendMessage(WM_SETREDRAW, FALSE, 0);
  DeleteString(iItem);
  InsertString(iItem, szItemText);
  SetItemData(iItem, dwItemData);
  SendMessage(WM_SETREDRAW, TRUE, 0);
}

void CDosListBoxEx::SetItemText(int iItem, LPCTSTR szItemText)
{
  ASSERT(iItem < GetCount());

  DWORD_PTR dwItemData;
  dwItemData = GetItemData(iItem);
  SetItem(iItem, szItemText, dwItemData);
}

void CDosListBoxEx::SwapItems(int iFirstItem, int iSecondItem)
{
  ASSERT(iFirstItem < GetCount());
  ASSERT(iSecondItem < GetCount());

  if (iFirstItem != iSecondItem)
  {
    // Cache the first item data
    CString strFirstItem;
    DWORD_PTR dwFirstItemData;

    GetText(iFirstItem, strFirstItem);
    dwFirstItemData = GetItemData(iFirstItem);

    // Cache the second item data
    CString strSecondItem;
    DWORD_PTR dwSecondItemData;

    GetText(iSecondItem, strSecondItem);
    dwSecondItemData = GetItemData(iSecondItem);

    // Insert the items in reverse order
    if (iFirstItem < iSecondItem)
    {
      SetItem(iFirstItem, strSecondItem, dwSecondItemData);
      SetItem(iSecondItem, strFirstItem, dwFirstItemData);
    }
    else
    {
      SetItem(iSecondItem, strFirstItem, dwFirstItemData);
      SetItem(iFirstItem, strSecondItem, dwSecondItemData);
    }
  }
}

int CDosListBoxEx::MoveItemUp(int iItem)
{
  //ASSERT( iItem > 0 ); 
  if (!(iItem > 0))
    return iItem;

  if (iItem > 0)
  {
    SwapItems(iItem, iItem - 1);
    SetCurSel(iItem - 1);
  }
  return iItem;
}

int CDosListBoxEx::MoveItemDown(int iItem)
{
  ASSERT(iItem >= 0);

  if (iItem != GetCount() - 1)
  {
    SwapItems(iItem, iItem + 1);
    SetCurSel(iItem + 1);
  }
  return iItem;
}

LRESULT CDosListBoxEx::OnEndEditMessage(WPARAM wParam, LPARAM lParam)
{
  UNUSED_ALWAYS(wParam);
  BOOL bEndEdit;

  CDosInPlace::LPIPCENDEDITINFO lpEndEditInfo = (CDosInPlace::LPIPCENDEDITINFO)lParam;
  switch (lpEndEditInfo->uKind)
  {
  case CDosInPlace::ipek_key:
    if (lpEndEditInfo->nVirtKey == VK_ESCAPE)
    {
      bEndEdit = TRUE;
      OnEndEditing(m_iEdited, TRUE);
      EndEditing(TRUE);
    }
    else
    {
      bEndEdit = TRUE;
      EndEditing(!OnEndEditing(m_iEdited, FALSE));
    }
    break;

  case CDosInPlace::ipek_action:
    bEndEdit = FALSE; // Superfluous
    OnBrowseButton(m_iEdited);
    break;

  case CDosInPlace::ipek_focus:
    if ((lpEndEditInfo->hNewWnd == m_pEdit->m_hWnd) ||
      (lpEndEditInfo->hNewWnd == m_pBrowseButton->m_hWnd))
    {
      bEndEdit = FALSE;
    }
    else
    {
      bEndEdit = TRUE;
      EndEditing(!OnEndEditing(m_iEdited, FALSE));
    }
    break;

  default:
    bEndEdit = TRUE;
  }

  return bEndEdit;
}

BEGIN_MESSAGE_MAP(CDosListBoxExBuddy, CWnd)
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_SIZE()
  ON_WM_NCMOUSEMOVE()
  ON_WM_CREATE()
END_MESSAGE_MAP()

CDosListBoxExBuddy::CDosListBoxExBuddy(UINT nBitmapID)
{
  // Load the bitmap by using a temp file
  m_hBitmap = (HBITMAP)::LoadImage(AfxGetResourceHandle(),
    MAKEINTRESOURCE(nBitmapID),
    IMAGE_BITMAP,
    0,
    0,
    LR_LOADMAP3DCOLORS);
  ASSERT(m_hBitmap);
  m_ButtonBitmap.Attach(m_hBitmap);

  // Init other data
  m_bButtonPressed = FALSE;
  m_iButton = bmp_numbtn;
  m_pListBoxEx = NULL;
#ifdef _DEBUG
  // Verify the dimensions
  BITMAP BmpInfo;
  m_ButtonBitmap.GetBitmap(&BmpInfo);
  ASSERT(BmpInfo.bmWidth == bmp_width);
  ASSERT(BmpInfo.bmHeight == bmp_height);
#endif
}

CDosListBoxExBuddy::~CDosListBoxExBuddy()
{
  if (m_hBitmap && (HBITMAP)m_ButtonBitmap)
    m_ButtonBitmap.Detach();
  if (m_hBitmap)
    ::DeleteObject(m_hBitmap);
  m_hBitmap = NULL;
}

int CDosListBoxExBuddy::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }

  // Enable Tooltips
  CreateTooltips();

  return 0;
}

void CDosListBoxExBuddy::PreSubclassWindow()
{
  // Enable Tooltips
  CreateTooltips();

  // Send a WM_SIZE message, as WM_CREATE would do
  CRect rcClient;
  GetClientRect(&rcClient);
  OnSize(0, rcClient.Width(), rcClient.Height());

  CWnd::PreSubclassWindow();
}

static void DrawBitmap(CDC& dc, CBitmap& bmp, int x, int y, int cx, int cy, int src_x = 0, int src_y = 0)
{
  CDC memDC;
  memDC.CreateCompatibleDC(&dc);

  CBitmap *poldbmp = memDC.SelectObject(&bmp);
  dc.BitBlt(x, y, cx, cy, &memDC, src_x, src_y, SRCCOPY);

  memDC.SelectObject(poldbmp);
  memDC.DeleteDC();
}

void CDosListBoxExBuddy::OnPaint()
{
  CPaintDC dc(this);

  CDC memDC;
  memDC.CreateCompatibleDC(&dc);

  CRect rcClient;
  GetClientRect(&rcClient);

  // This is used to center the button bitmap
  int nBmpTopY = (rcClient.Height() - bmp_height) / 2;

  // To store old selected objects
  CBitmap* pOldBmp = NULL;
  CFont* pOldFont = NULL;

  // Select the font
  CFont font;
  font.Attach((HFONT)GetStockObject(DEFAULT_GUI_FONT));
  pOldFont = memDC.SelectObject(&font);

  // Select the out-of-screen bitmap
  CBitmap memBmp;
  memBmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
  pOldBmp = memDC.SelectObject(&memBmp);

  // Erase the background
  CBrush brush;
  brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
  memDC.FillRect(&rcClient, &brush);
  brush.DeleteObject();

  // Prepare to draw the text transparently
  memDC.SetBkMode(TRANSPARENT);
  memDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

  // Draw the text
  CString strWindowText;
  GetWindowText(strWindowText);
  memDC.DrawText(strWindowText, rcClient, DT_SINGLELINE | DT_VCENTER);

  // Draw the button bitmap
  DrawBitmap(memDC, m_ButtonBitmap, rcClient.right - bmp_width - 2, nBmpTopY, bmp_width, bmp_height);

  // Draw the button edge
  if (m_iButton != bmp_numbtn)
  {
    CRect rcButtonEdge(
      rcClient.right - (bmp_numbtn - m_iButton) * bmp_btnwidth - 2,
      nBmpTopY,
      rcClient.right - (bmp_numbtn - m_iButton - 1) * bmp_btnwidth - 2,
      bmp_height + nBmpTopY
      );
    memDC.DrawEdge(&rcButtonEdge, m_bButtonPressed ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT);
  }

  // Bit copy
  dc.BitBlt(2, 0, rcClient.Width() - 2, rcClient.Height(), &memDC, 0, 0, SRCCOPY);

  // Select the bitmap out of the device context
  memDC.SelectObject(pOldBmp);

  // Select the font out of the device context
  memDC.SelectObject(pOldFont);
}

int CDosListBoxExBuddy::FindButton(const CPoint& point)
{
  UINT iIndex;
  for (iIndex = 0; iIndex < bmp_numbtn; iIndex++)
  {
    if (m_arcButtons[iIndex].PtInRect(point))
    {
      break;
    }
  }

  return iIndex;
}

void CDosListBoxExBuddy::InvalidateButton(int iIndex, BOOL bUpdateWindow)
{
  if (iIndex < bmp_numbtn)
    InvalidateRect(&m_arcButtons[iIndex], FALSE);

  if (bUpdateWindow)
    UpdateWindow();
}

void CDosListBoxExBuddy::OnMouseMove(UINT nFlags, CPoint point)
{
  if (!m_bButtonPressed)
  {
    UINT iIndex = FindButton(point);
    if (iIndex != m_iButton)
    {
      InvalidateButton(m_iButton, FALSE);
      m_iButton = iIndex;
      InvalidateButton(m_iButton, TRUE);
    }
  }

  CWnd::OnMouseMove(nFlags, point);
}

void CDosListBoxExBuddy::OnLButtonDown(UINT nFlags, CPoint point)
{
  SetCapture();

  m_iButton = FindButton(point);

  if (m_iButton != bmp_numbtn)
  {
    m_bButtonPressed = TRUE;
    InvalidateRect(&m_arcButtons[m_iButton], FALSE);
    UpdateWindow();
  }

  CWnd::OnLButtonDown(nFlags, point);
}

void CDosListBoxExBuddy::OnLButtonUp(UINT nFlags, CPoint point)
{
  UINT iButton = FindButton(point);

  if (iButton == m_iButton)
  {
    if (m_iButton != bmp_numbtn)
    {
      DoClick(m_iButton);
    }
  }

  m_bButtonPressed = FALSE;
  Invalidate(FALSE);
  m_iButton = iButton;
  ReleaseCapture();

  CWnd::OnLButtonUp(nFlags, point);
}

void CDosListBoxExBuddy::DoClick(int iIndex)
{
  int iSelected = m_pListBoxEx->GetCurSel();
  switch (iIndex)
  {
  case btn_new:
    m_pListBoxEx->EditNew();
    break;

  case btn_del:
    if (iSelected != -1)
      m_pListBoxEx->DeleteString(iSelected);
    m_pListBoxEx->SetFocus();
    break;

  case btn_up:
    if (iSelected != -1)
      m_pListBoxEx->MoveItemUp(iSelected);
    m_pListBoxEx->SetFocus();
    break;

  case btn_down:
    if (iSelected != -1)
      m_pListBoxEx->MoveItemDown(m_pListBoxEx->GetCurSel());
    m_pListBoxEx->SetFocus();
    break;
  }
}

void CDosListBoxExBuddy::OnNcMouseMove(UINT nHitTest, CPoint point)
{
  InvalidateButton(m_iButton, FALSE);
  m_iButton = FindButton(point);
  InvalidateButton(m_iButton, TRUE);

  CWnd::OnNcMouseMove(nHitTest, point);
}

void CDosListBoxExBuddy::CreateTooltips()
{
  m_ToolTip.Create(this);

  TOOLINFO ttInfo;
  ttInfo.cbSize = sizeof(TOOLINFO);
  ttInfo.uFlags = TTF_SUBCLASS;
  ttInfo.hwnd = m_hWnd;
  ttInfo.rect = CRect(0, 0, 0, 0); // OnSize will resize it
  ttInfo.hinst = NULL;
  ttInfo.lpszText = LPSTR_TEXTCALLBACK;
  ttInfo.lParam = 0;

  int i;
  for (i = 0; i < bmp_numbtn; i++)
  {
    ttInfo.uId = i + 1;
    m_ToolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ttInfo);
    m_ToolTip.Activate(TRUE);
  }
}

void CDosListBoxExBuddy::SetTipText(UINT nID, LPTSTR szTipText)
{
  wchar_t* aTips[] =
  {
    L"New",
    L"Delete",
    L"Move Up",
    L"Move Down"
  };

  if (nID < 0 || nID >= _countof(aTips))
    return;

  CString str = aTips[nID];
  if (str.IsEmpty())
    return;

  if (nID < bmp_numbtn)
    wcscpy(szTipText, str);
}

void CDosListBoxExBuddy::OnSize(UINT nType, int cx, int cy)
{
  CRect rcClient;
  GetClientRect(&rcClient);

  int nBmpTopY = (rcClient.Height() - bmp_height) / 2;

  TOOLINFO ttInfo;
  for (int iIndex = 0; iIndex < bmp_numbtn; iIndex++)
  {
    m_arcButtons[iIndex].top = nBmpTopY;
    m_arcButtons[iIndex].left = cx - (bmp_numbtn - iIndex)*bmp_btnwidth;
    m_arcButtons[iIndex].bottom = bmp_height + nBmpTopY;
    m_arcButtons[iIndex].right = cx - (bmp_numbtn - iIndex - 1)*bmp_btnwidth;

    ttInfo.cbSize = sizeof(TOOLINFO);
    ttInfo.hwnd = m_hWnd;
    ttInfo.uId = iIndex + 1;
    ttInfo.rect = m_arcButtons[iIndex];
    m_ToolTip.SendMessage(TTM_NEWTOOLRECT, 0, (LPARAM)&ttInfo);
  }

  CWnd::OnSize(nType, cx, cy);
}

BOOL CDosListBoxExBuddy::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  UINT nCode = ((NMHDR*)lParam)->code;
  if (nCode == TTN_GETDISPINFO)
  {
    UINT nID = (UINT)(((NMHDR *)lParam)->idFrom - 1);
    SetTipText(nID, ((NMTTDISPINFO*)lParam)->szText);
    return TRUE;
  }

  return CWnd::OnNotify(wParam, lParam, pResult);
}
