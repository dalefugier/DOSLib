/////////////////////////////////////////////////////////////////////////////
// DosScreenImage.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
#pragma once

class CDosScreenImage : public CImage
{
public:
  BOOL CaptureRect(const CRect& rect) throw();
  BOOL CaptureScreen() throw();
  BOOL CaptureWindow(HWND hWnd) throw();
};
