/////////////////////////////////////////////////////////////////////////////
// DosSplash.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosSplash.h"

IMPLEMENT_DYNAMIC(CDosSplashWnd, CWnd)

BEGIN_MESSAGE_MAP(CDosSplashWnd, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_TIMER()
END_MESSAGE_MAP()

class CDosSplashLockData
{
private:
  CSyncObject& m_syncObj;

public:
  CDosSplashLockData(CSyncObject& so)
    : m_syncObj(so)
  {
    VERIFY(m_syncObj.Lock());
  }

  ~CDosSplashLockData()
  {
    m_syncObj.Unlock();
  }
};

/////////////////////////////////////////////////////////////////////////////
// CDosSplashWnd

CDosSplashWnd::CDosSplashWnd(CDosSplash* pThread)
{
  m_pSplash = pThread;
}

CDosSplashWnd::~CDosSplashWnd()
{
}

BOOL CDosSplashWnd::Create(UINT nIDRes, UINT duration, WORD flags)
{
  return FALSE;
}

BOOL CDosSplashWnd::Create(LPCTSTR szFileName, UINT duration, WORD flags)
{
  HRESULT hr = m_image.Load(szFileName);
  if (FAILED(hr))
    return FALSE;

  m_duration = duration;
  m_flags = flags;

  BOOL rc = CreateEx(
    WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
    AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
    0,
    WS_POPUP | WS_VISIBLE,
    0, 0,
    m_image.GetWidth(), m_image.GetHeight(),
    adsw_acadMainWnd(),
    0
    );

  return rc;
}

int CDosSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  CenterWindow();
  UpdateWindow();
  SetForegroundWindow();

  if (m_duration != -1)
    SetTimer(1, m_duration, 0);

  return 0;
}

void CDosSplashWnd::PostNcDestroy()
{
  if (::IsWindow(adsw_acadMainWnd()))
    ::SetForegroundWindow(adsw_acadMainWnd());
  delete this;
}

void CDosSplashWnd::OnPaint()
{
  CPaintDC dc(this);
  if (!m_image.IsNull())
    m_image.StretchBlt(dc, 0, 0, m_image.GetWidth(), m_image.GetHeight(), SRCCOPY);
}

void CDosSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
  if ((m_flags & CDosSplash::NoWaitForMainWnd) || ::IsWindow(adsw_acadMainWnd()))
    SendMessage(WM_CLOSE);
  else
    SetTimer(1, 100, NULL);
}

BOOL CDosSplashWnd::PreTranslateMessage(MSG* pMsg)
{
  if (m_flags & CDosSplash::KillOnClick)
  {
    UINT msg = pMsg->message;
    if (msg == WM_KEYDOWN ||
      msg == WM_SYSKEYDOWN ||
      msg == WM_LBUTTONDOWN ||
      msg == WM_RBUTTONDOWN ||
      msg == WM_MBUTTONDOWN)
    {
      PostMessage(WM_CLOSE);
      return TRUE;
    }
  }
  return CWnd::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNAMIC(CDosSplash, CWinThread)

CDosSplash::CDosSplash(UINT nIDRes, UINT duration, WORD flags, CDosSplash** ppBackPtr)
{
  m_ppBackPtr = ppBackPtr;
  m_nIDRes = nIDRes;
  m_duration = duration;
  m_flags = flags;
  m_strFileName.Empty();
  CreateThread();
}

CDosSplash::CDosSplash(LPCTSTR szFileName, UINT duration, WORD flags, CDosSplash** ppBackPtr)
{
  m_ppBackPtr = ppBackPtr;
  m_nIDRes = 0;
  m_duration = duration;
  m_flags = flags;
  m_strFileName = szFileName;
  CreateThread();
}

CDosSplash::~CDosSplash()
{
  //if( m_ppBackPtr )
  //	*m_ppBackPtr = NULL;
}

BOOL CDosSplash::InitInstance()
{
  if (m_strFileName.IsEmpty())
    m_pMainWnd = OnCreateSplashWnd(m_nIDRes, m_duration, m_flags);
  else
    m_pMainWnd = OnCreateSplashWnd(m_strFileName, m_duration, m_flags);
  return m_pMainWnd != NULL;
}

CWnd* CDosSplash::OnCreateSplashWnd(UINT nIDRes, UINT duration, WORD flags)
{
  CDosSplashWnd* pSplashWnd = new CDosSplashWnd(this);
  if (pSplashWnd)
    pSplashWnd->Create(nIDRes, duration, flags);
  return pSplashWnd;
}

CWnd* CDosSplash::OnCreateSplashWnd(LPCTSTR szFileName, UINT duration, WORD flags)
{
  CDosSplashWnd *pSplashWnd = new CDosSplashWnd(this);
  if (pSplashWnd)
    pSplashWnd->Create(szFileName, duration, flags);
  return pSplashWnd;
}

void CDosSplash::Kill(CDosSplash* pSplash)
{
  static CCriticalSection here;
  CDosSplashLockData lock(here);
  if (pSplash)
  {
    CDosSplashWnd* pSplashWnd = (CDosSplashWnd*)pSplash->m_pMainWnd;
    ASSERT(pSplashWnd);
    ASSERT_KINDOF(CDosSplashWnd, pSplashWnd);
    pSplashWnd->m_pSplash = NULL;
    if (pSplash->m_ppBackPtr)
    {
      *pSplash->m_ppBackPtr = NULL;
      pSplash->m_ppBackPtr = NULL;
    }
    pSplashWnd->PostMessage(WM_CLOSE);
  }
}
