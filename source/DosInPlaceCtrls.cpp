/////////////////////////////////////////////////////////////////////////////
// DosInPlaceCtrls.h
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

UINT const CDosInPlace::m_IPCMsg = ::RegisterWindowMessage(CDosInPlace::MsgString());

LPCTSTR CDosInPlace::MsgString()
{
  return L"4C7A6D92_31CD_41a2_ADCF_F96617AD7B67";
}

/////////////////////////////////////////////////////////////////////////////
// CDosInPlaceEdit

IMPLEMENT_DYNCREATE(CDosInPlaceEdit, CEdit)

BEGIN_MESSAGE_MAP(CDosInPlaceEdit, CEdit)
  ON_WM_CREATE()
  ON_WM_KILLFOCUS()
  ON_WM_GETDLGCODE()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CDosInPlaceEdit::CDosInPlaceEdit()
{
  m_hParent = NULL;
}

CDosInPlaceEdit::~CDosInPlaceEdit()
{
}

UINT CDosInPlaceEdit::OnGetDlgCode()
{
  return DLGC_WANTALLKEYS | DLGC_WANTCHARS;
}

void CDosInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if ((nChar == VK_ESCAPE) || (nChar == VK_RETURN))
  {
    CDosInPlace::IPCENDEDITINFO ipeEditInfo;
    ZeroMemory(&ipeEditInfo, sizeof(ipeEditInfo));
    ipeEditInfo.hwndSrc = m_hWnd;
    ipeEditInfo.uKind = CDosInPlace::ipek_key;
    ipeEditInfo.nVirtKey = (int)nChar;
    ipeEditInfo.pt.x = 0;
    ipeEditInfo.pt.y = 0;
    if (::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipeEditInfo))
      Hide();

    return;
  }

  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDosInPlaceEdit::PreCreateWindow(CREATESTRUCT & cs)
{
  // Add ES_MULTILINE style so we can intercept the <Return> keys
  cs.style |= (WS_CHILD | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_NOHIDESEL);
  return CEdit::PreCreateWindow(cs);
}

int CDosInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CEdit::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_hParent = lpCreateStruct->hwndParent;
  return 0;
}

BOOL CDosInPlaceEdit::PreTranslateMessage(MSG *pMsg)
{
  switch (pMsg->message)
  {
  case WM_KEYDOWN:
    if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
    {
      CDosInPlace::IPCENDEDITINFO ipeEditInfo;
      ZeroMemory(&ipeEditInfo, sizeof(ipeEditInfo));
      ipeEditInfo.hwndSrc = m_hWnd;
      ipeEditInfo.uKind = CDosInPlace::ipek_key;
      ipeEditInfo.nVirtKey = (int)pMsg->wParam;
      ipeEditInfo.pt = pMsg->pt;
      if (::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipeEditInfo))
        Hide();
      return TRUE;
    }
    break;
  }

  return CEdit::PreTranslateMessage(pMsg);
}

void CDosInPlaceEdit::OnKillFocus(CWnd *pNewWnd)
{
  if (IsWindowVisible())
  {
    CDosInPlace::IPCENDEDITINFO ipeEditInfo;
    const MSG *pMsg = GetCurrentMessage();
    ZeroMemory(&ipeEditInfo, sizeof(ipeEditInfo));
    ipeEditInfo.uKind = CDosInPlace::ipek_focus;
    ipeEditInfo.hwndSrc = m_hWnd;
    ipeEditInfo.hNewWnd = pNewWnd ? pNewWnd->m_hWnd : NULL;
    ipeEditInfo.pt = pMsg->pt;
    if (::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipeEditInfo))
      Hide();
  }
  CEdit::OnKillFocus(pNewWnd);
}

void CDosInPlaceEdit::Hide()
{
  if (IsWindowVisible())
  {
    ::SetWindowPos(
      m_hWnd, 0, 0, 0, 0, 0,
      SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE
      );
    ::SetFocus(m_hParent);
  }
}

void CDosInPlaceEdit::Show(CRect rcEdit)
{
  if (!IsWindowVisible() && !rcEdit.IsRectEmpty())
  {
    ::SetWindowPos(
      m_hWnd,
      HWND_TOP,
      rcEdit.left, rcEdit.top, rcEdit.Width(), rcEdit.Height(),
      SWP_SHOWWINDOW | SWP_NOREDRAW
      );

    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN, 2);
    SetSel(0, -1);
    Invalidate();
    ::SetFocus(m_hWnd);
  }
}

/////////////////////////////////////////////////////////////////////////////
// CDosInPlaceButton

IMPLEMENT_DYNCREATE(CDosInPlaceButton, CButton)

BEGIN_MESSAGE_MAP(CDosInPlaceButton, CButton)
  ON_WM_CREATE()
  ON_WM_KILLFOCUS()
  ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
END_MESSAGE_MAP()

CDosInPlaceButton::CDosInPlaceButton()
{
  m_hParent = NULL;
  m_bInAction = FALSE;
}

CDosInPlaceButton::~CDosInPlaceButton()
{
}

BOOL CDosInPlaceButton::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style |= (WS_CHILD | BS_PUSHBUTTON);
  return CButton::PreCreateWindow(cs);
}

int CDosInPlaceButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CButton::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_hParent = lpCreateStruct->hwndParent;
  return 0;
}

BOOL CDosInPlaceButton::PreTranslateMessage(MSG *pMsg)
{
  switch (pMsg->message)
  {
  case WM_KEYDOWN:

    if ((int)pMsg->wParam == VK_ESCAPE)
    {
      CDosInPlace::IPCENDEDITINFO ipcInfo;
      ZeroMemory(&ipcInfo, sizeof(ipcInfo));
      ipcInfo.hwndSrc = m_hWnd;
      ipcInfo.uKind = CDosInPlace::ipek_key;
      ipcInfo.nVirtKey = (int)pMsg->wParam;
      ipcInfo.pt = pMsg->pt;
      ::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipcInfo);
      return TRUE;
    }
    break;
  }

  return CButton::PreTranslateMessage(pMsg);
}

void CDosInPlaceButton::OnKillFocus(CWnd *pNewWnd)
{
  if (IsWindowVisible() && !m_bInAction)
  {
    CDosInPlace::IPCENDEDITINFO ipcEventInfo;
    const MSG *pMsg = GetCurrentMessage();
    ZeroMemory(&ipcEventInfo, sizeof(ipcEventInfo));
    ipcEventInfo.uKind = CDosInPlace::ipek_focus;
    ipcEventInfo.hNewWnd = pNewWnd->m_hWnd;
    ipcEventInfo.pt = pMsg->pt;
    if (::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipcEventInfo))
      Hide();
  }

  CButton::OnKillFocus(pNewWnd);
}

void CDosInPlaceButton::OnClick()
{
  CDosInPlace::IPCENDEDITINFO ipcEventInfo;
  ZeroMemory(&ipcEventInfo, sizeof(ipcEventInfo));
  ipcEventInfo.uKind = CDosInPlace::ipek_action;
  ipcEventInfo.uID = GetWindowLong(m_hWnd, GWL_ID);
  ipcEventInfo.pt.x = LOWORD(GetMessagePos());
  ipcEventInfo.pt.y = HIWORD(GetMessagePos());
  m_bInAction = TRUE;
  ::SendMessage(m_hParent, CDosInPlace::m_IPCMsg, 0, (LPARAM)&ipcEventInfo);
  m_bInAction = FALSE;
}

void CDosInPlaceButton::Hide()
{
  if (IsWindowVisible())
  {
    ::SetWindowPos(
      m_hWnd, 0, 0, 0, 0, 0,
      SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE
      );
    ::SetFocus(m_hParent);
  }
}

void CDosInPlaceButton::Show(CRect rcButton)
{
  if (!IsWindowVisible() && !rcButton.IsRectEmpty())
  {
    ::SetWindowPos(
      m_hWnd,
      HWND_TOP,
      rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(),
      SWP_SHOWWINDOW | SWP_NOREDRAW
      );
    Invalidate();
  }
}
