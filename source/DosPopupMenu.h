/////////////////////////////////////////////////////////////////////////////
// DosPopupMenu.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosPopupMenu
{
public:
  // If you pass a valid resource ID and hInst and HWND then your menu is loaded and a WM_COMMAND
  // message is sent the the HWND with the item ID when done.  If any of these fields are non 0 
  // and invalid then ASSERT(...) is called.  If you use the defult constructor you must calll AddItem 
  // one or more times to add strings to a temp menu.
  CDosPopupMenu(UINT nResourceID = 0, HINSTANCE hInst = 0);
  CDosPopupMenu(LPCTSTR lpsResourceID, HINSTANCE hInst);

  virtual ~CDosPopupMenu();

  // Add items in order you want them to display
  int AddItem(LPCTSTR lpszItem);

  // Add sepertaor (this will not have an index)
  void AddSeperator();

  // Call using index returned by AddItem to enable/dissable a
  // menu item.  The default behaivor is to enable everything.
  void EnableItem(int nItem, BOOL bEnable = TRUE);

  // Called when the menu selection changes, returns index of selected item or -1 if nothing is selected
  virtual void OnMenuSelect(HWND hWnd, HMENU hMenu, int index) {};

  // Called when WM_INITMENU message received for context menu.
  virtual void OnInitMenu(HWND hWnd, HMENU hMenu);

  // Display popup menu, returns -1 on error or if escape pressed otherwise returns index of added item.
  virtual int PopUp(CWnd*,   // Window whos coord system the points are in
    CPoint,  // Bottom point.  Top left corner of the popup.  This is used by default if there is enough
             // screen space to display the popup menu on the bottom
    CPoint); // Top point.  Bottom left corner of the poupup.  This is used if there is not enough space to display
             // the menu below the window.

// Display popup menu, returns -1 on error or if escape pressed otherwise returns index of added item.
  int PopUpEx(CWnd*,   // Window whos coord system the points are in
    CPoint,  // Bottom point.  Top left corner of the popup.  This is used by default if there is enough
             // screen space to display the popup menu on the bottom
    CPoint,  // Top point.  Bottom left corner of the poupup.  This is used if there is not enough space to display
             // the menu below the window.
    int);    // 0 = Not transparent, 1 to 100 = percentage of tranparency.

  bool LeftButtonPicked(); // returns true if menu dismissed with left button
  bool RightButtonPicked(); // returns true if menu dismissed with right button
  LPPOINT PointPicked(); // return NULL if LeftButtonPicked() and RightButtonPicked() are false otherwise returns point where mouse went down. 

  // For internal use only
  void PopupWndProcOnInitMenu(HWND hWnd, HMENU hMenu);
  LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
  int m_menu_select;
  bool m_menu_char;

protected:
  bool CheckDefaults(LPCTSTR nResourceID, HINSTANCE hInst);
  void Init();

  CStringArray m_item_array;
  HMENU        m_hMenu;
  bool         m_bLeft;
  bool         m_bRight;
  CUIntArray   m_nEnabledList;
  CPoint       m_point;
};
