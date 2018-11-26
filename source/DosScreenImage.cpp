/////////////////////////////////////////////////////////////////////////////
// DosScreenImage.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosScreenImage.h"

BOOL CDosScreenImage::CaptureRect(const CRect& rect)
{
  // Detach and destroy the old bitmap if any attached
  CImage::Destroy();

  // Create a screen and a memory device context
  HDC hDCScreen = ::CreateDC(L"DISPLAY", 0, 0, 0);
  HDC hDCMem = ::CreateCompatibleDC(hDCScreen);

  // Create a compatible bitmap and select it in the memory DC
  HBITMAP hBitmap = ::CreateCompatibleBitmap(hDCScreen, rect.Width(), rect.Height());
  HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);

  // Bit-blit from screen to memory device context
  // Note: CAPTUREBLT flag is required to capture layered windows
  DWORD dwRop = SRCCOPY | CAPTUREBLT;
  BOOL rc = ::BitBlt(hDCMem, 0, 0, rect.Width(), rect.Height(), hDCScreen, rect.left, rect.top, dwRop);

  // Attach bitmap handle to this object
  Attach(hBitmap);

  // Restore the memory DC and perform cleanup
  ::SelectObject(hDCMem, hBmpOld);
  ::DeleteDC(hDCMem);
  ::DeleteDC(hDCScreen);

  return rc;
}

BOOL CDosScreenImage::CaptureScreen()
{
  CRect rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
  return CaptureRect(rect);
}

BOOL CDosScreenImage::CaptureWindow(HWND hWnd)
{
  BOOL rc = FALSE;
  if (::IsWindow(hWnd))
  {
    CRect rect;
    ::GetWindowRect(hWnd, rect);
    rc = CaptureRect(rect);
  }
  return rc;
}

