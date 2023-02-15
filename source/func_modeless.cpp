/////////////////////////////////////////////////////////////////////////////
// func_modeless.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosGetCancelDialog.h"
#include "DosGetProgressDialog.h"
#include "DosHtmlDialog.h"
#include "DosProcessHelpers.h"
#include "ToasterWnd.h"

class CDosWaitCursor
{
public:
  CDosWaitCursor();
  ~CDosWaitCursor();

  BOOL StartWaitCursor();
  void EndWaitCursor();

public:
  HHOOK m_hHook;
  HCURSOR m_hCursor;
  HCURSOR m_hOldCursor;
  BOOL m_bStarted;
  int m_nCursorSize;
  int m_nPickBox;
};

// The one and only CDosWaitCursor object
static class CDosWaitCursor theWaitCursor;

LRESULT CALLBACK WaitCursorProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode < 0)
    return CallNextHookEx(theWaitCursor.m_hHook, nCode, wParam, lParam);

  LRESULT lResult = CallNextHookEx(theWaitCursor.m_hHook, nCode, wParam, lParam);
  ::SetCursor(theWaitCursor.m_hCursor);

  return lResult;
}

CDosWaitCursor::CDosWaitCursor()
{
  m_hHook = 0;
  m_hCursor = 0;
  m_hOldCursor = 0;
  m_bStarted = FALSE;
  m_nCursorSize = 0;
  m_nPickBox = 0;
}

CDosWaitCursor::~CDosWaitCursor()
{
  EndWaitCursor();
}

BOOL CDosWaitCursor::StartWaitCursor()
{
  BOOL rc = FALSE;

  if (!m_bStarted)
  {
    m_hCursor = ::LoadCursor(0, IDC_WAIT);
    m_hOldCursor = ::SetCursor(m_hCursor);
    m_hHook = ::SetWindowsHookEx(
      WH_CALLWNDPROCRET,
      (HOOKPROC)WaitCursorProc,
      (HINSTANCE)NULL,
      GetCurrentThreadId()
      );

    if (m_hHook != NULL)
    {
      m_bStarted = TRUE;

      // save old cursorsize
      struct resbuf rbOldCursorSize, rbNewCursorSize;
      acedGetVar(L"CURSORSIZE", &rbOldCursorSize);
      m_nCursorSize = rbOldCursorSize.resval.rint;

      // set new cursorsize
      rbNewCursorSize.restype = RTSHORT;
      rbNewCursorSize.resval.rint = 1;
      acedSetVar(L"CURSORSIZE", &rbNewCursorSize);

      // save old pickbox
      struct resbuf rbOldPickBox, rbNewPickBox;
      acedGetVar(L"PICKBOX", &rbOldPickBox);
      m_nPickBox = rbOldPickBox.resval.rint;

      // set new pickbox
      rbNewPickBox.restype = RTSHORT;
      rbNewPickBox.resval.rint = 1;
      acedSetVar(L"PICKBOX", &rbNewPickBox);

      rc = TRUE;
    }
    else
    {
      ads_printf(L"DOSLib error: cannot hook window.\n");
    }
  }

  return rc;
}

void CDosWaitCursor::EndWaitCursor()
{
  if (m_bStarted)
  {
    // Unhook windows and restore cursor
    ::UnhookWindowsHookEx(m_hHook);
    ::SetCursor(m_hOldCursor);

    // Restore cursorsize
    struct resbuf rbCursorSize;
    rbCursorSize.restype = RTSHORT;
    rbCursorSize.resval.rint = m_nCursorSize;
    acedSetVar(L"CURSORSIZE", &rbCursorSize);

    // Restore pickbox
    struct resbuf rbPickBox;
    rbPickBox.restype = RTSHORT;
    rbPickBox.resval.rint = m_nPickBox;
    acedSetVar(L"PICKBOX", &rbPickBox);

    m_hHook = NULL;
    m_hCursor = NULL;
    m_hOldCursor = NULL;
    m_bStarted = FALSE;
    m_nCursorSize = 0;
    m_nPickBox = 0;
  }
}

////////////////////////////////////////////////////////////////
// dos_waitcursor
int CDOSLibApp::ads_dos_waitcursor()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  if (args.IsEmpty())
  {
    theWaitCursor.EndWaitCursor();
    acedRetNil();
  }
  else
  {
    if (theWaitCursor.StartWaitCursor())
      acedRetT();
    else
      acedRetNil();
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////

class CDosCancel
{
public:
  CDosCancel();
  ~CDosCancel();

  bool IsCancelCreated();
  bool IsCancelVisible();
  bool SetCancelFocus();
  bool SetCancelVisible();
  bool DisplayCancel(LPCTSTR lpTitle, LPCTSTR lpPrompt);
  void DestroyCancel();

  CDosGetCancelDialog* m_pDialog;
};

// The one and only CDosCancel object
static class CDosCancel theCancel;

CDosCancel::CDosCancel()
  : m_pDialog(0)
{
}

CDosCancel::~CDosCancel()
{
  DestroyCancel();
}

bool CDosCancel::IsCancelCreated()
{
  bool bResult = false;
  if (m_pDialog && ::IsWindow(m_pDialog->m_hWnd))
    bResult = true;
  return bResult;
}

bool CDosCancel::IsCancelVisible()
{
  bool bResult = false;
  if (IsCancelCreated() && m_pDialog->IsWindowVisible())
    bResult = true;
  return bResult;
}

bool CDosCancel::SetCancelFocus()
{
  bool bResult = false;
  if (IsCancelCreated() && IsCancelVisible())
  {
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosCancel::SetCancelVisible()
{
  bool bResult = false;
  if (IsCancelCreated() && !IsCancelVisible())
  {
    m_pDialog->ShowWindow(SW_SHOWNORMAL);
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosCancel::DisplayCancel(LPCTSTR lpTitle, LPCTSTR lpPrompt)
{
  if (IsCancelCreated())
  {
    if (!IsCancelVisible())
      SetCancelVisible();
    m_pDialog->SetFocus();
    return true;
  }

  m_pDialog = new CDosGetCancelDialog(CWnd::FromHandle(adsw_acadMainWnd()));
  m_pDialog->m_Title = lpTitle;
  m_pDialog->m_Prompt = lpPrompt;
  BOOL rc = m_pDialog->Create(IDD_DOSCANCEL, CWnd::FromHandle(adsw_acadMainWnd()));
  if (rc)
  {
    m_pDialog->ShowWindow(SW_SHOW);
    m_pDialog->UpdateWindow();
    m_pDialog->SetFocus();
  }
  else
  {
    delete m_pDialog;
    m_pDialog = 0;
  }

  return rc ? true : false;
}

void CDosCancel::DestroyCancel()
{
  if (IsCancelVisible())
    m_pDialog->KillDialog();
  m_pDialog = 0;
}

/////////////////////////////////////////////////////////////////////////////
// dos_getcancel
int CDOSLibApp::ads_dos_getcancel()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  if (args.IsEmpty())
  {
    if (theCancel.IsCancelVisible())
      acedRetT();
    else
      acedRetNil();
    return RSRSLT;
  }

  if (args.IsTrue())
  {
    theCancel.DestroyCancel();
    acedRetNil();
    return RSRSLT;
  }

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  if (theCancel.DisplayCancel(strTitle, strPrompt))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////

class CDosProgress
{
public:
  CDosProgress();
  ~CDosProgress();
  LPCTSTR Name() const { return L"dos_getprogress"; }
  int Run(CAdsArgs& args);

  bool IsProgressCreated();
  bool IsProgressVisible();
  bool SetProgressFocus();
  bool SetProgressVisible();
  bool SetProgressHidden();
  bool DisplayProgress(LPCTSTR lpTitle, LPCTSTR lpPrompt, int nUpper, BOOL bCancel);
  void DestroyProgress();

  CDosGetProgressDialog* m_pDialog;
};

// The one and only CDosProgress object
static class CDosProgress theProgress;

CDosProgress::CDosProgress()
  : m_pDialog(0)
{
}

CDosProgress::~CDosProgress()
{
  DestroyProgress();
}

bool CDosProgress::IsProgressCreated()
{
  bool bResult = false;
  if (m_pDialog && ::IsWindow(m_pDialog->m_hWnd))
    bResult = true;
  return bResult;
}

bool CDosProgress::IsProgressVisible()
{
  bool bResult = false;
  if (IsProgressCreated() && m_pDialog->IsWindowVisible())
    bResult = true;
  return bResult;
}

bool CDosProgress::SetProgressFocus()
{
  bool bResult = false;
  if (IsProgressCreated() && IsProgressVisible())
  {
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosProgress::SetProgressVisible()
{
  bool bResult = false;
  if (IsProgressCreated() && !IsProgressVisible())
  {
    m_pDialog->ShowWindow(SW_SHOWNORMAL);
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosProgress::DisplayProgress(LPCTSTR lpTitle, LPCTSTR lpPrompt, int nUpper, BOOL bCancel)
{
  if (IsProgressCreated())
  {
    if (!IsProgressVisible())
      SetProgressVisible();
    m_pDialog->SetFocus();
    return true;
  }

  m_pDialog = new CDosGetProgressDialog(CWnd::FromHandle(adsw_acadMainWnd()));
  m_pDialog->m_Title = lpTitle;
  m_pDialog->m_Prompt = lpPrompt;
  m_pDialog->m_nUpper = nUpper;
  m_pDialog->m_bCancel = bCancel;

  BOOL rc = m_pDialog->Create(IDD_DOSPROGRESS, CWnd::FromHandle(adsw_acadMainWnd()));
  if (rc)
  {
    m_pDialog->ShowWindow(SW_SHOW);
    m_pDialog->UpdateWindow();
    m_pDialog->SetFocus();
  }
  else
  {
    delete m_pDialog;
    m_pDialog = 0;
  }

  return rc ? true : false;
}

void CDosProgress::DestroyProgress()
{
  if (IsProgressVisible())
    m_pDialog->KillDialog();
  m_pDialog = 0;
}

////////////////////////////////////////////////////////////////
// dos_getprogress
int CDOSLibApp::ads_dos_getprogress()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  // Query for visibility
  if (args.IsEmpty())
  {
    if (theProgress.IsProgressVisible())
      acedRetT();
    else
      acedRetNil();
    return RSRSLT;
  }

  // Kill it
  if (args.IsTrue())
  {
    theProgress.DestroyProgress();
    acedRetNil();
    return RSRSLT;
  }

  // Step  or increment
  if (args.IsInteger())
  {
    if (!theProgress.IsProgressVisible())
      return RSERR;

    int nPos = 0;
    if (!args.GetInteger(nPos))
      return RSERR;

    if (nPos >= 0)
      theProgress.m_pDialog->m_Progress.SetPos(nPos);
    else
      theProgress.m_pDialog->m_Progress.OffsetPos(abs(nPos));

    acedRetInt(nPos);
    return RSRSLT;
  }

  // Create it
  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  int nUpper = 0;
  if (!args.GetInteger(nUpper))
    return RSERR;

  if (nUpper <= 0)
  {
    acutPrintf(L"DOSLib error: range must be a positive integer.\n");
    return RSERR;
  }

  args.Next();

  BOOL bCancel = FALSE;
  if (!args.IsEmpty())
    bCancel = TRUE;

  if (theProgress.DisplayProgress(strTitle, strPrompt, nUpper, bCancel))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////

class CDosModelessHtmlBoxEx
{
public:
  CDosModelessHtmlBoxEx();
  ~CDosModelessHtmlBoxEx();

  bool IsCreated();
  bool IsVisible();
  bool SetFocus();
  bool SetVisible();
  bool Display(LPCTSTR lpTitle, LPCTSTR lpAddress, int dx, int dy);
  void Destroy();

  CDosModelessHtmlDialog* m_pDialog;
};

// The one and only CDosModelessHtmlBoxEx object
static class CDosModelessHtmlBoxEx theModelessHtmlDialog;

CDosModelessHtmlBoxEx::CDosModelessHtmlBoxEx()
  : m_pDialog(0)
{
}

CDosModelessHtmlBoxEx::~CDosModelessHtmlBoxEx()
{
  Destroy();
}

bool CDosModelessHtmlBoxEx::IsCreated()
{
  bool bResult = false;
  if (m_pDialog && ::IsWindow(m_pDialog->m_hWnd))
    bResult = true;
  return bResult;
}

bool CDosModelessHtmlBoxEx::IsVisible()
{
  bool bResult = false;
  if (IsCreated() && m_pDialog->IsWindowVisible())
    bResult = true;
  return bResult;
}

bool CDosModelessHtmlBoxEx::SetFocus()
{
  bool bResult = false;
  if (IsCreated() && IsVisible())
  {
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosModelessHtmlBoxEx::SetVisible()
{
  bool bResult = false;
  if (IsCreated() && !IsVisible())
  {
    m_pDialog->ShowWindow(SW_SHOWNORMAL);
    m_pDialog->SetFocus();
    bResult = true;
  }
  return bResult;
}

bool CDosModelessHtmlBoxEx::Display(LPCTSTR lpTitle, LPCTSTR lpAddress, int dx, int dy)
{
  if (IsCreated())
  {
    if (!IsVisible())
      SetVisible();
    m_pDialog->SetTitle(lpTitle);
    m_pDialog->SetAddress(lpAddress);
    m_pDialog->Refresh();
    m_pDialog->SetFocus();
    return true;
  }

  m_pDialog = new CDosModelessHtmlDialog(CWnd::FromHandle(adsw_acadMainWnd()));
  m_pDialog->SetTitle(lpTitle);
  m_pDialog->SetAddress(lpAddress);
  if (dx > 0 && dy > 0)
    m_pDialog->SetInitSize(dx, dy);

  BOOL rc = m_pDialog->Create(IDD_DOSHTMLBOX, CWnd::FromHandle(adsw_acadMainWnd()));
  if (rc)
  {
    m_pDialog->ShowWindow(SW_SHOW);
    m_pDialog->UpdateWindow();
    m_pDialog->SetFocus();
  }
  else
  {
    delete m_pDialog;
    m_pDialog = 0;
  }

  return rc ? true : false;
}

void CDosModelessHtmlBoxEx::Destroy()
{
  if (IsVisible())
    m_pDialog->KillDialog();
  m_pDialog = 0;
}

////////////////////////////////////////////////////////////////
// dos_htmlboxex
int CDOSLibApp::ads_dos_htmlboxex()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  if (args.IsEmpty())
  {
    if (theModelessHtmlDialog.IsVisible())
      acedRetT();
    else
      acedRetNil();
    return RSRSLT;
  }

  if (args.IsTrue())
  {
    theModelessHtmlDialog.Destroy();
    acedRetNil();
    return RSRSLT;
  }

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strAddress;
  if (!args.GetString(strAddress))
    return RSERR;

  args.Next();

  int dx = 0, dy = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(dx))
      return RSERR;

    args.Next();

    if (!args.GetInteger(dy))
      return RSERR;

    if (dx < 1 || dy < 1)
    {
      acutPrintf(L"DOSLib error: invalid dialog window size.\n");
      return RSERR;
    }
  }

  if (theModelessHtmlDialog.Display(strTitle, strAddress, dx, dy))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////

class CDosToasterWnd : public CToasterWnd
{
public:
  CDosToasterWnd(CString strShell)
    : CToasterWnd()
  {
    m_strShell = strShell;
  }

  virtual void OnBodyTextClicked()
  {
    OnClick();
  }

  virtual void OnTitleTextClicked()
  {
    OnClick();
  }

  void OnClick()
  {
    if (!m_strShell.IsEmpty())
      ShellExecute(0, L"open", m_strShell, 0, 0, SW_SHOWNORMAL);
  }

private:
  CString m_strShell;
};

class CDosToasterWndHandler
{
public:
  CDosToasterWndHandler();
  ~CDosToasterWndHandler();

  void Show(
    const CString& sTitle,
    const CString& sText,
    const CString& sShell,
    int width,
    int height,
    int duration
    );

  CDosToasterWnd* m_pToaster;
};

// The one and only CDosToasterWndHandler object
static class CDosToasterWndHandler theToaster;

CDosToasterWndHandler::CDosToasterWndHandler()
  : m_pToaster(0)
{
}

CDosToasterWndHandler::~CDosToasterWndHandler()
{
  if (m_pToaster)
  {
    m_pToaster->Close();
    m_pToaster = 0;
  }
}

void CDosToasterWndHandler::Show(
  const CString& sTitle,
  const CString& sText,
  const CString& sShell,
  int width,
  int height,
  int duration
  )
{
  if (m_pToaster)
  {
    m_pToaster->Close();
    m_pToaster = 0;
  }

  m_pToaster = new CDosToasterWnd(sShell);
  m_pToaster->m_sTitle = sTitle;
  m_pToaster->m_sText = sText;
  m_pToaster->m_AnimationStyle = static_cast<CToasterWnd::AnimationStyle>(0);
  m_pToaster->m_BackgroundStyle = static_cast<CToasterWnd::BackgroundStyle>(0);
  m_pToaster->m_PopupPosition = static_cast<CToasterWnd::PopupPosition>(0);
  m_pToaster->m_nHeight = height;
  m_pToaster->m_nWidth = width;
  m_pToaster->m_bTransparent = FALSE;
  m_pToaster->m_dwWaitTime = (DWORD)duration;
  m_pToaster->Show();
}

////////////////////////////////////////////////////////////////
// dos_popupwnd
int CDOSLibApp::ads_dos_traywnd()
{
  CAdsArgs args;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strText;
  if (!args.GetString(strText))
    return RSERR;

  args.Next();

  int width = 0;
  if (!args.GetInteger(width))
    return RSERR;

  args.Next();

  int height = 0;
  if (!args.GetInteger(height))
    return RSERR;

  args.Next();

  CString strShell;
  int duration = 9000;

  if (!args.IsEmpty())
  {
    if (!args.GetString(strShell))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(duration))
        return RSERR;
    }
  }

  theToaster.Show(strTitle, strText, strShell, abs(width), abs(height), abs(duration));

  acedRetNil();

  return RSRSLT;
}
