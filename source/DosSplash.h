/////////////////////////////////////////////////////////////////////////////
// DosSplash.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosSplash : public CWinThread
{
public:
  CDosSplash(UINT nIDRes, UINT duration, WORD flags = 0, CDosSplash** ppBackPtr = 0);
  CDosSplash(LPCTSTR szFileName, UINT duration, WORD flags = 0, CDosSplash** ppBackPtr = 0);
  ~CDosSplash();

  enum
  {
    KillOnClick = 0x0001,
    IgnoreCmdLine = 0x0002,
    NoWaitForMainWnd = 0x0004,
  };

  virtual CWnd* OnCreateSplashWnd(UINT nIDRes, UINT duration, WORD flags);
  virtual CWnd* OnCreateSplashWnd(LPCTSTR szFileName, UINT duration, WORD flags);

  static void Kill(CDosSplash* pSplash);

protected:
  virtual BOOL InitInstance();
  DECLARE_DYNAMIC(CDosSplash)

protected:
  CDosSplash** m_ppBackPtr; // Caller's back pointer to me
  UINT m_nIDRes;            // Bitmap resource ID
  UINT m_duration;          // How long to display
  WORD m_flags;             // CDosSplashWnd creation flags
  CString m_strFileName;    // Filename of bitmap
};

class CDosSplashWnd : public CWnd
{
protected:
  friend CDosSplash;

  CDosSplashWnd(CDosSplash* pThread);
  ~CDosSplashWnd();

  CDosSplash* m_pSplash;
  CImage m_image;
  UINT m_duration;
  WORD m_flags;

  virtual BOOL Create(UINT nIDRes, UINT duration, WORD flags);
  virtual BOOL Create(LPCTSTR szFileName, UINT duration, WORD flags);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void PostNcDestroy();

  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnClose();
  afx_msg void OnPaint();
  afx_msg void OnTimer(UINT_PTR nIDEvent);

  DECLARE_MESSAGE_MAP()
  DECLARE_DYNAMIC(CDosSplashWnd)
};


