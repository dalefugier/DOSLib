/////////////////////////////////////////////////////////////////////////////
// DosPopupMenu.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosPopupMenu.h"

#define DOS_POPUP_SEP L"$$seperator$$"

enum constants
{
  start_at = 100,
};

CDosPopupMenu::CDosPopupMenu(UINT nResourceID, HINSTANCE hInst)
  : m_hMenu(0)
{
  Init();
  CheckDefaults(MAKEINTRESOURCE(nResourceID), hInst);
}

CDosPopupMenu::CDosPopupMenu(LPCTSTR lpsResourceID, HINSTANCE hInst)
  : m_hMenu(0)
{
  Init();
  CheckDefaults(lpsResourceID, hInst);
}

void CDosPopupMenu::Init()
{
  m_menu_select = -1;
  m_menu_char = false;
  m_point.x = m_point.y = 0;
  m_bLeft = m_bRight = false;
}

bool CDosPopupMenu::CheckDefaults(LPCTSTR nResourceID, HINSTANCE hInst)
{
  if (nResourceID)
    ASSERT(hInst);
  else
    ASSERT(!nResourceID && !hInst);

  // Nothing to do so return okay
  if (!nResourceID)
    return true;

  m_hMenu = ::LoadMenu(hInst, nResourceID);

  ASSERT(m_hMenu);

  if (!m_hMenu)
    return false;

  return true;
}

CDosPopupMenu::~CDosPopupMenu()
{
}

int CDosPopupMenu::AddItem(LPCTSTR lpszItem)
{
  m_item_array.Add(CString(lpszItem));
  m_nEnabledList.Add(1);
  return (int)m_nEnabledList.GetUpperBound();
}

void CDosPopupMenu::EnableItem(int nItem, BOOL bEnable)
{
  if (nItem < 0 || nItem > m_nEnabledList.GetUpperBound())
    return;

  m_nEnabledList[nItem] = bEnable ? 1 : 0;
}

void CDosPopupMenu::AddSeperator()
{
  m_item_array.Add(CString(DOS_POPUP_SEP));
}

class CDosMenuFont
{
public:
  CDosMenuFont()
  {
    ZeroMemory((PVOID)&m_lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);

    //Get the system metrics for the Captionfromhere
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &nm, 0));
    m_lf = nm.lfMenuFont;
    m_iMenuHeight = nm.iMenuHeight;
    m_fontMenu.CreateFontIndirect(&m_lf);
  }
  LOGFONT m_lf;
  CFont m_fontMenu;
  UINT m_iMenuHeight;
};

static LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  CDosPopupMenu* this_popup = (CDosPopupMenu*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
  if (NULL == this_popup)
    return DefWindowProc(hWnd, message, wParam, lParam);
  return this_popup->WindowProc(hWnd, message, wParam, lParam);
}

static int g_nTransparencty = 0;
static CMenu* g_pMenu = 0;

static bool SetMenuTransparency(HWND hWnd, int nPercent)
{
  if (!::IsWindow(hWnd))
    return false;

  CWnd* pWnd = CWnd::FromHandle(hWnd);
  if (NULL == pWnd)
    return false;

#define LWA_ALPHA 0x00000002
#define WS_EX_LAYERED 0x00080000

  if (WS_EX_LAYERED & pWnd->GetExStyle())
    return false; // Already done....

  typedef BOOL(WINAPI *fpSetLayeredWindowAttributesFunc)(
    HWND hwnd,
    COLORREF crKey,
    BYTE bAlpha,
    DWORD dwFlags);

  static fpSetLayeredWindowAttributesFunc fpSetLayeredWindowAttributes = NULL;
  if (NULL == fpSetLayeredWindowAttributes)
  {
    HINSTANCE hModule = ::GetModuleHandle(L"User32.dll");
    if (NULL == hModule)
      return false;

    fpSetLayeredWindowAttributes = (fpSetLayeredWindowAttributesFunc)::GetProcAddress(hModule, "SetLayeredWindowAttributes");
  }

  if (NULL == fpSetLayeredWindowAttributes)
    return false;

  if (pWnd->ModifyStyleEx(0, WS_EX_LAYERED) && fpSetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), (255 * nPercent) / 100, LWA_ALPHA))
    return true;

  return false;
}

static bool IsMenuItemHighlighted(CMenu* pMenu, UINT nID, bool byPosition)
{
  MENUITEMINFO mii;
  memset(&mii, 0, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;
  if (pMenu->GetMenuItemInfo(nID, &mii, byPosition))
    return(MFS_HILITE & mii.fState ? true : false);
  return false;
}

LRESULT CALLBACK CDosPopupMenu::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static bool could_not_get_menu_hwnd = false;
  static bool delete_last_menu_item = false;

  switch (message)
  {
  case WM_MEASUREITEM:
  {
    LPMEASUREITEMSTRUCT lpMIS = (LPMEASUREITEMSTRUCT)lParam;
    CString s;

    g_pMenu->GetMenuString(lpMIS->CtlID, s, MF_BYCOMMAND);
    if (s.IsEmpty())
      s = L"empty_string";

    // Obtain the width of the text:
    CDosMenuFont mf;
    CWnd* pWnd = AfxGetMainWnd(); // Get main window
    CDC*pDC = pWnd->GetDC(); // Get device context
    CFont* pFont = pDC->SelectObject(&mf.m_fontMenu);		// Select menu font in...
    SIZE t;
    GetTextExtentPoint32(pDC->GetSafeHdc(), s, s.GetLength(), &t); // Width of caption
    pDC->SelectObject(pFont);							// Select old font in
    AfxGetApp()->m_pMainWnd->ReleaseDC(pDC);			// Release the DC

    lpMIS->itemWidth = t.cx;
    lpMIS->itemHeight = t.cy;
  }
  return true;

  case WM_DRAWITEM:
  {
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
    HWND hWndMenu = ::WindowFromDC(lpDIS->hDC);

    if (!::IsWindow(hWndMenu))
    {
      could_not_get_menu_hwnd = true;
    }

    else if (::SetMenuTransparency(hWndMenu, g_nTransparencty))
    {
      CMenu cmenu;
      CString s;
      //
      // Remove owner draw flag, our work here is done!
      //
      cmenu.Attach((HMENU)lpDIS->hwndItem);
      cmenu.GetMenuString(lpDIS->itemID, s, MF_BYCOMMAND);
      if (!s.IsEmpty())
      {
        UINT nFlags = MF_BYCOMMAND | MF_STRING;
        if (::IsMenuItemHighlighted(&cmenu, lpDIS->itemID, false))
          nFlags |= MF_HILITE;
        cmenu.ModifyMenu(lpDIS->itemID, nFlags, lpDIS->itemID, s);
      }
      if (delete_last_menu_item)
      {
        cmenu.DeleteMenu(cmenu.GetMenuItemCount() - 1, MF_BYPOSITION);
        delete_last_menu_item = false;
      }
      cmenu.Detach();
    }
  }
  return true;

  case WM_ENTERIDLE:
    if (could_not_get_menu_hwnd)
    {
      // Force menu to redraw
      g_pMenu->AppendMenu(MF_STRING, start_at + g_pMenu->GetMenuItemCount(), L".");
      could_not_get_menu_hwnd = false;
      delete_last_menu_item = true;
    }
    break;

  case WM_INITMENU:
    could_not_get_menu_hwnd = false;
    delete_last_menu_item = false;
    m_menu_select = -1;
    PopupWndProcOnInitMenu(hWnd, (HMENU)wParam);
    break;

  case WM_MENUCHAR:
    m_menu_char = true;
    return MAKELONG(m_menu_select, MNC_CLOSE); // make spacebar (and other keys) work just like right click

  case WM_MENUSELECT:
  {
    int id = LOWORD(wParam);
    if (m_menu_char || lParam == 0)
      break;
    m_menu_select = id;
    id = id > 0 ? id - start_at : -1;
    OnMenuSelect(hWnd, (HMENU)lParam, id);
  }
  break;

  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

void CDosPopupMenu::PopupWndProcOnInitMenu(HWND hWnd, HMENU hMenu)
{
  CString sSeperator(DOS_POPUP_SEP);

  for (int i = 0, j = 0, cnt = (int)m_item_array.GetCount(); i < cnt; i++)
  {
    if (sSeperator.CompareNoCase(m_item_array[i]) == 0)
    {
      j++;
      continue;
    }

    if ((i - j) > m_nEnabledList.GetUpperBound())
      break;

    if (m_nEnabledList[i - j] > 0)
      continue;

    ::EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
  }

  OnInitMenu(hWnd, hMenu);
}

void CDosPopupMenu::OnInitMenu(HWND hWnd, HMENU hMenu)
{
}

bool CDosPopupMenu::LeftButtonPicked()
{
  return m_bLeft;
}

bool CDosPopupMenu::RightButtonPicked()
{
  return m_bRight;
}

LPPOINT CDosPopupMenu::PointPicked()
{
  if (LeftButtonPicked() || RightButtonPicked())
    return &m_point;
  return NULL;
}

int CDosPopupMenu::PopUp(CWnd* parent, CPoint bottom, CPoint top)
{
  return PopUpEx(parent, bottom, top, 0);
}

int CDosPopupMenu::PopUpEx(CWnd* parent, CPoint bottom, CPoint top, int nTransparent)
{
  g_nTransparencty = nTransparent;
  g_pMenu = NULL;

  ASSERT(parent && ::IsWindow(parent->m_hWnd));
  Init();

  CMenu menu, *pmenu = &menu;
  bool bCreateMenu = m_hMenu ? false : true;

  if (!bCreateMenu && !menu.Attach(m_hMenu))
    return -1;

  if (!bCreateMenu)
    pmenu = menu.GetSubMenu(0);

  if (bCreateMenu && !menu.CreatePopupMenu())
    return -1;

  const int cnt = (int)m_item_array.GetCount();

  if (bCreateMenu && cnt < 1)
    return -1;

  HWND hwnd = 0;

  g_pMenu = pmenu; // Make sure that menu is available to message loop

  if (bCreateMenu)
    hwnd = ::CreateWindow(L"STATIC", 0, WS_POPUP, 0, 0, 10, 10, NULL, 0, ::AfxGetInstanceHandle(), 0);
  else
    hwnd = parent->m_hWnd;

  if (!hwnd)
    return -1;

  if (bCreateMenu)
  {
#ifdef _WIN64
    ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)PopupWndProc);
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
#else
    ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, PtrToLong(PopupWndProc));
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToLong(this));
#endif
  }

  CString sSeparator(DOS_POPUP_SEP);

  for (int j = 0, i = 0; i < cnt; i++)
  {
    CString s(m_item_array[i]);

    if (s.Compare(sSeparator))
      pmenu->AppendMenu(MF_STRING, start_at + j++, s);
    else
      pmenu->AppendMenu(MF_SEPARATOR, 0, L"");
  }

  if (nTransparent > 0)
  {
    g_nTransparencty = min(nTransparent, 100);
    for (int i = 0, cnt = pmenu->GetMenuItemCount(); i < cnt; i++)
    {
      CString s;
      pmenu->GetMenuString(i, s, MF_BYPOSITION);

      if (s.IsEmpty())
        continue;

      UINT nFlags = MF_BYPOSITION | MF_STRING | MF_OWNERDRAW;

      if (::IsMenuItemHighlighted(pmenu, i, true))
        nFlags |= MF_HILITE;

      pmenu->ModifyMenu(i, nFlags, pmenu->GetMenuItemID(i), s);
      break;
    }
  }

  ::ClientToScreen(parent->m_hWnd, &bottom);

  UINT nFlags = TPM_LEFTALIGN | TPM_RIGHTBUTTON;

  if (bCreateMenu)
    nFlags |= TPM_RETURNCMD;

  int result = ::TrackPopupMenu(pmenu->m_hMenu, nFlags, bottom.x, bottom.y, 0, hwnd, 0);

  if (result == 0 && m_menu_char)
    result = m_menu_select;

  // check what's left in the queue
  // it gives an indication how the menu got closed
  MSG msg;
  ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

  if (msg.message == WM_RBUTTONDOWN)
  {
    m_bRight = true;
    m_point.x = LOWORD(msg.lParam);
    m_point.y = HIWORD(msg.lParam);
  }
  else if (msg.message == WM_LBUTTONDOWN)
  {
    m_bLeft = true;
    m_point.x = LOWORD(msg.lParam);
    m_point.y = HIWORD(msg.lParam);
  }
  else if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN)
  {
    result = m_menu_select;
  }

  if (bCreateMenu)
    ::DestroyWindow(hwnd);
  else if (m_hMenu)
    ::DestroyMenu(m_hMenu);

  if (result < 1)
    return -1;

  return(bCreateMenu ? result - start_at : result);
}
