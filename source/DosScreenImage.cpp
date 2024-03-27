/////////////////////////////////////////////////////////////////////////////
// DosScreenImage.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
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
  // detach and destroy the old bitmap if any attached
  CImage::Destroy();

  // get the desktop device context
  HDC hDC = ::GetDC(NULL);

  // create a device context
  HDC hDest = ::CreateCompatibleDC(hDC);

  // create a bitmap
  HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rect.Width(), rect.Height());

  // select the bitmap
  HBITMAP hOld = (HBITMAP)::SelectObject(hDest, hBitmap);

  // copy from the desktop device context to the bitmap device context
  BOOL rc = ::BitBlt(hDest, 0, 0, rect.Width(), rect.Height(), hDC, rect.left, rect.top, SRCCOPY | CAPTUREBLT);

  // attach bitmap handle to this object
  Attach(hBitmap);

  // restore the old object
  ::SelectObject(hDest, hOld);

  // delete the device context
  ::DeleteDC(hDest);

  // release the desktop device context
  ::ReleaseDC(NULL, hDC);
 
  return rc;
}

BOOL CDosScreenImage::CaptureScreen()
{
  CRect rect(
    0, 
    0, 
    ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
    ::GetSystemMetrics(SM_CYVIRTUALSCREEN)
  );
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

BOOL CDosScreenImage::CaptureWindow(CWnd* pWnd)
{
  BOOL rc = FALSE;
  if (pWnd)
    rc = CaptureWindow(pWnd->GetSafeHwnd());
  return rc;
}
