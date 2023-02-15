///////////////////////////////////////////////////////////////////////////////
// func_systems.cpp
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
#include "DosScreenImage.h"
#include "DosScreenSelectionDialog.h"
#include "DosProcessHelpers.h"

////////////////////////////////////////////////////////////////
// dos_adminp
int CDOSLibApp::ads_dos_adminp()
{
  if (IsAdministrator())
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ispoweruser
int CDOSLibApp::ads_dos_ispoweruser()
{
  if (IsPowerUser())
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_clipboard
int CDOSLibApp::ads_dos_clipboard()
{
  CAdsArgs args;

  CString strResult;
  if (OpenClipboard(adsw_acadMainWnd()))
  {
    BOOL bFormat = IsClipboardFormatAvailable(CF_UNICODETEXT);
    if (bFormat)
    {
      HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
      if (hGlobal)
      {
        LPTSTR lpBuffer = (LPTSTR)GlobalLock(hGlobal);
        strResult = lpBuffer;
        GlobalUnlock(hGlobal);
      }
    }
    CloseClipboard();
  }

  if (!args.IsEmpty())
  {
    CString strText;
    if (!args.GetString(strText))
      return RSERR;

    if (OpenClipboard(adsw_acadMainWnd()))
    {
      EmptyClipboard();
      size_t dwBytes = (strText.GetLength() + 1) * sizeof(wchar_t);
      HGLOBAL hGlobal = GlobalAlloc(GMEM_DDESHARE, dwBytes);
      LPTSTR lpBuffer = (LPTSTR)GlobalLock(hGlobal);
      wcscpy(lpBuffer, strText);
      GlobalUnlock(lpBuffer);
      SetClipboardData(CF_UNICODETEXT, hGlobal);
      CloseClipboard();
    }
  }

  if (strResult.IsEmpty())
    return acedRetNil();
  else
    acedRetStr(strResult);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_computer
int CDOSLibApp::ads_dos_computer()
{
  CAdsArgs args;

  wchar_t szBuffer[_MAX_PATH];
  wmemset(szBuffer, 0, _MAX_PATH);
  DWORD dwSize = _MAX_PATH;

  if (args.IsEmpty())
  {
    if (GetComputerName(szBuffer, &dwSize))
      acedRetStr(szBuffer);
    else
      acedRetNil();
    return RSRSLT;
  }

  if (args.IsInteger())
  {
    int cnf = 0;
    if (!args.GetInteger(cnf) || cnf < 0 || cnf >= ComputerNameMax)
      return RSRSLT;

    //wchar_t szDescription[8][32] = 
    //{
    //  L"NetBIOS", 
    //  L"DNS hostname", 
    //  L"DNS domain", 
    //  L"DNS fully-qualified", 
    //  L"Physical NetBIOS", 
    //  L"Physical DNS hostname", 
    //  L"Physical DNS domain", 
    //  L"Physical DNS fully-qualified"
    //};

    if (GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, szBuffer, &dwSize))
      acedRetStr(szBuffer);
    else
      acedRetNil();
    return RSRSLT;
  }

  CString str;
  if (!args.GetString(str))
    return RSERR;

  if (str.IsEmpty())
  {
    acedRetNil();
  }
  else
  {
    if (str.GetLength() > MAX_COMPUTERNAME_LENGTH)
      str = str.Left(MAX_COMPUTERNAME_LENGTH);

    if (SetComputerName(str))
      acedRetStr(str);
    else
      acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_mem
int CDOSLibApp::ads_dos_mem()
{
  MEMORYSTATUS ms;
  ZeroMemory(&ms, sizeof(MEMORYSTATUS));
  ms.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&ms);

  struct resbuf* retlist = acutBuildList(
    RTLONG, (long)ms.dwMemoryLoad,
    RTREAL, (double)ms.dwTotalPhys,
    RTREAL, (double)ms.dwAvailPhys,
    RTREAL, (double)ms.dwTotalPageFile,
    RTREAL, (double)ms.dwAvailPageFile,
    RTREAL, (double)ms.dwTotalVirtual,
    RTREAL, (double)ms.dwAvailVirtual,
    0);

  acedRetList(retlist);
  acutRelRb(retlist);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_username
int CDOSLibApp::ads_dos_username()
{
  TCHAR tchBuffer[_MAX_PATH];
  DWORD dwSize = _MAX_PATH;

  if (GetUserName(tchBuffer, &dwSize))
    acedRetStr(tchBuffer);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ver


int CDOSLibApp::ads_dos_ver()
{
  CString str;
  if (GetOSDisplayString(str))
    acedRetStr(str);
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_guidgen
int CDOSLibApp::ads_dos_guidgen()
{
  GUID guid = GUID_NULL;
  ::UuidCreate(&guid);

  if (guid == GUID_NULL)
  {
    acutPrintf(L"DOSLib error: unable to create new GUID.\n");
    return RSERR;
  }
  else
  {
    CString str;
    str.Format(
      L"%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
      guid.Data1,
      guid.Data2,
      guid.Data3,
      guid.Data4[0],
      guid.Data4[1],
      guid.Data4[2],
      guid.Data4[3],
      guid.Data4[4],
      guid.Data4[5],
      guid.Data4[6],
      guid.Data4[7]
    );
    acedRetStr(str);
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_beep
int CDOSLibApp::ads_dos_beep()
{
  CAdsArgs args;

  UINT uType = 0;
  if (args.IsEmpty())
  {
    uType = 0;
  }
  else
  {
    int nType = 0;
    if (!args.GetInteger(nType))
      return RSERR;
    uType = (UINT)nType;
  }

  switch (uType)
  {
  case 0: // standard beep
    uType = 0xFFFFFFFF;
    break;
  case 1: // system asterisk
    uType = MB_ICONASTERISK;
    break;
  case 2: // system exclamation
    uType = MB_ICONEXCLAMATION;
    break;
  case 3: // system hand
    uType = MB_ICONHAND;
    break;
  case 4: // system question
    uType = MB_ICONQUESTION;
    break;
  case 5: // system default
    uType = MB_OK;
    break;
  default:
    uType = 0xFFFFFFFF;
    break;
  }

  if (MessageBeep(uType))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_speaker
int CDOSLibApp::ads_dos_speaker()
{
  CAdsArgs args;

  int nFrequency = 0;
  if (!args.GetInteger(nFrequency))
    return RSERR;

  args.Next();

  int nDuration = 0;
  if (!args.GetInteger(nDuration))
    return RSERR;

  nFrequency = DOS_CLAMP(nFrequency, 37, 32767); // 0x25 through 0x7FFF
  nDuration = DOS_CLAMP(nDuration, 1, 10000); // 10 seconds...

  if (Beep(nFrequency, nDuration))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_wav
int CDOSLibApp::ads_dos_wav()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  if (PlaySound(strPath, 0, SND_SYNC | SND_NODEFAULT))
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_random
int CDOSLibApp::ads_dos_random()
{
  acedRetInt(rand());
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_date
int CDOSLibApp::ads_dos_date()
{
  SYSTEMTIME st;
  memset(&st, 0, sizeof(st));
  GetLocalTime(&st);

  CAdsArgs args;

  if (args.IsEmpty())
  {
    CAdsRetList result;
    result.Add(st.wYear);
    result.Add(st.wMonth);
    result.Add(st.wDay);
    acedRetList(result);
    return RSRSLT;
  }

  CArray<int, int> time_list;
  if (!args.GetIntegerList(time_list))
    return RSERR;

  int time_count = (int)time_list.GetCount();
  if (time_count < 3)
    return RSERR;

  st.wYear = time_list[0];
  st.wMonth = time_list[1];
  st.wDay = time_list[2];

  if (SetLocalTime(&st))
  {
    CAdsRetList result;
    result.Add(st.wYear);
    result.Add(st.wMonth);
    result.Add(st.wDay);
    acedRetList(result);
  }
  else
  {
    DOS_PrintLastError();
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_time
int CDOSLibApp::ads_dos_time()
{
  SYSTEMTIME st;
  memset(&st, 0, sizeof(st));
  GetLocalTime(&st);

  CAdsArgs args;

  if (args.IsEmpty())
  {
    CAdsRetList result;
    result.Add(st.wHour);
    result.Add(st.wMinute);
    result.Add(st.wSecond);
    result.Add(st.wMilliseconds);
    acedRetList(result);
    return RSRSLT;
  }

  CArray<int, int> time_list;
  if (!args.GetIntegerList(time_list))
    return RSERR;

  int time_count = (int)time_list.GetCount();
  if (time_count < 2)
    return RSERR;

  st.wHour = time_list[0];
  st.wMinute = time_list[1];
  st.wSecond = (time_count > 2 ? time_list[2] : 0);
  st.wMilliseconds = time_count > 3 ? time_list[3] : 0;

  if (SetLocalTime(&st))
  {
    CAdsRetList result;
    result.Add(st.wHour);
    result.Add(st.wMinute);
    result.Add(st.wSecond);
    result.Add(st.wMilliseconds);
    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cdate
int CDOSLibApp::ads_dos_cdate()
{
  CAdsArgs args;

  CString strFormat;
  if (!args.GetString(strFormat))
    return RSERR;

  // %D is not a valid formatting code, and will crash _tcsftime,
  // used by CTime::Format.
  strFormat.Replace(L"%D", L"%d");

  CTime time = CTime::GetCurrentTime();
  CString strTime = time.Format(strFormat);

  if (strTime.IsEmpty())
    acedRetNil();
  else
    acedRetStr(strTime);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_recent
int CDOSLibApp::ads_dos_recent()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (args.IsEmpty())
  {
    SHAddToRecentDocs(SHARD_PATH, 0);
    acedRetNil();
  }
  else
  {
    if (!args.GetPathString(strPath))
      return RSERR;
    SHAddToRecentDocs(SHARD_PATH, strPath);
    strPath.ReleaseBuffer();
    acedRetT();
  }

  return RSRSLT;
}

static void ToggleKeyLockState(UINT uCode)
{
  UINT uScanCode = ::MapVirtualKey(uCode, 0);
  keybd_event(uCode, uScanCode, KEYEVENTF_EXTENDEDKEY, 0);
  keybd_event(uCode, uScanCode, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

static void SetKeyLockOn(UINT uCode)
{
  BOOL bKeyLock = ((GetKeyState(uCode) & 1) != 0);
  if (!bKeyLock)
    ToggleKeyLockState(uCode);
}

static void SetKeyLockOff(UINT uCode)
{
  BOOL bKeyLock = ((GetKeyState(uCode) & 1) != 0);
  if (bKeyLock)
    ToggleKeyLockState(uCode);
}

////////////////////////////////////////////////////////////////
// dos_capslock
int CDOSLibApp::ads_dos_capslock()
{
  CAdsArgs args;

  if (!args.IsEmpty())
  {
    SetKeyLockOn(VK_CAPITAL);
    acedRetT();
  }
  else
  {
    SetKeyLockOff(VK_CAPITAL);
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_numlock
int CDOSLibApp::ads_dos_numlock()
{
  CAdsArgs args;

  if (!args.IsEmpty())
  {
    SetKeyLockOn(VK_NUMLOCK);
    acedRetT();
  }
  else
  {
    SetKeyLockOff(VK_NUMLOCK);
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_scrolllock
int CDOSLibApp::ads_dos_scrolllock()
{
  CAdsArgs args;

  if (!args.IsEmpty())
  {
    SetKeyLockOn(VK_SCROLL);
    acedRetT();
  }
  else
  {
    SetKeyLockOff(VK_SCROLL);
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_asynckeystate
int CDOSLibApp::ads_dos_asynckeystate()
{
  CAdsArgs args;

  int vKey = 0;
  if (!args.GetInteger(vKey))
    return RSERR;

  short sResult = ::GetAsyncKeyState(vKey);
  acedRetInt((int)sResult);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_systemmetrics
int CDOSLibApp::ads_dos_systemmetrics()
{
  CAdsArgs args;

  int nIndex = 0;
  if (!args.GetInteger(nIndex))
    return RSERR;

  int nResult = ::GetSystemMetrics(nIndex);
  acedRetInt(nResult);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_pause
int CDOSLibApp::ads_dos_pause()
{
  CAdsArgs args;

  int nSeconds = 0;
  if (!args.GetInteger(nSeconds))
    return RSERR;

  if (nSeconds > 0)
  {
    CWinApp* pApp = acedGetAcadWinApp();
    if (pApp)
    {
      CWnd* pWnd = pApp->GetMainWnd();
      if (pWnd)
      {
        DWORD dwMilliseconds = (DWORD)(nSeconds * 1000);
        DWORD dwStart = ::GetTickCount();
        do
        {
          MSG msg;
          while (::PeekMessage(&msg, pWnd->m_hWnd, 0, 0, PM_NOREMOVE))
          {
            pApp->PumpMessage();
          }
        } while ((::GetTickCount() - dwStart) < dwMilliseconds);
      }
    }
  }

  acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_winhelp
int CDOSLibApp::ads_dos_winhelp()
{
  CAdsArgs args;

  HWND hWnd = adsw_acadMainWnd();

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  args.Next();

  if (args.IsEmpty())
  {
    if (WinHelp(hWnd, strPath, HELP_FINDER, (ULONG_PTR)0))
      acedRetT();
    else
      acedRetNil();
  }

  else if (args.IsInteger())
  {
    int nContent = 0;
    if (!args.GetInteger(nContent))
      return RSERR;

    if (WinHelp(hWnd, strPath, HELP_CONTEXT, (ULONG_PTR)nContent))
      acedRetT();
    else
      acedRetNil();
  }

  else if (args.IsString())
  {
    CString strKey;
    if (!args.GetString(strKey))
      return RSERR;

    if (WinHelp(hWnd, strPath, HELP_PARTIALKEY, (ULONG_PTR)(LPCTSTR)strKey))
      acedRetT();
    else
      acedRetNil();
  }

  else
  {
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_htmlhelp
int CDOSLibApp::ads_dos_htmlhelp()
{
  CAdsArgs args;

  CString strPath;
  if (!args.GetString(strPath))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_DISPLAY_TOC, 0);
  }

  else if (args.IsInteger())
  {
    int nTopic = 0;
    if (args.GetInteger(nTopic))
      ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_DISPLAY_INDEX, (DWORD_PTR)nTopic);
  }
  else
  {
    CString strWord;
    if (args.GetString(strWord))
    {
      HH_AKLINK link;
      memset(&link, 0, sizeof(link));
      link.cbStruct = sizeof(link);
      link.fReserved = FALSE;
      link.pszKeywords = strWord;
      link.fIndexOnFail = TRUE;
      ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_DISPLAY_INDEX, (DWORD_PTR)(LPCTSTR)strWord);
      ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_KEYWORD_LOOKUP, (DWORD_PTR)&link);
    }
  }
  acedRetNil();

  return RSRSLT;
}



////////////////////////////////////////////////////////////////
// dos_iswin64
int CDOSLibApp::ads_dos_iswin64()
{
  if (Is64BitWindows())
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isacad64
int CDOSLibApp::ads_dos_isacad64()
{
#if defined(_WIN64)
  acedRetT();
#else
  acedRetNil();
#endif
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_tickcount
int CDOSLibApp::ads_dos_tickcount()
{
  acedRetReal((ads_real)::GetTickCount());
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_printscrn
int CDOSLibApp::ads_dos_printscrn()
{
  CAcModuleResourceOverride myResources;

  CAdsArgs args;

  CDosPathString strBitmap = L"dos_printscrn.clipboard";
  if (!args.IsNil())
  {
    if (!args.GetPathString(strBitmap))
      return RSERR;
  }

  args.Next();

  int type = 0; // full screen
  CString prompt = L"Drag a screen rectangle to capture:\n";

  if (!args.IsEmpty())
  {
    if (!args.GetInteger(type))
      return RSERR;

    type = DOS_CLAMP(type, 0, 4);

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetString(prompt))
        return RSERR;
    }
  }

  strBitmap.FullPath();

  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(strBitmap, 0, 0, 0, 0, 0, 0, ext, _MAX_EXT);

  CString strExt(ext);

  GUID guidFileType;
  if (strExt.CompareNoCase(L".bmp") == 0)
    guidFileType = Gdiplus::ImageFormatBMP;
  else if (strExt.CompareNoCase(L".jpg") == 0)
    guidFileType = Gdiplus::ImageFormatPNG;
  else if (strExt.CompareNoCase(L".jpeg") == 0)
    guidFileType = Gdiplus::ImageFormatPNG;
  else if (strExt.CompareNoCase(L".gif") == 0)
    guidFileType = Gdiplus::ImageFormatGIF;
  else if (strExt.CompareNoCase(L".tiff") == 0)
    guidFileType = Gdiplus::ImageFormatTIFF;
  else if (strExt.CompareNoCase(L".tif") == 0)
    guidFileType = Gdiplus::ImageFormatTIFF;
  else if (strExt.CompareNoCase(L".png") == 0)
    guidFileType = Gdiplus::ImageFormatPNG;
  else if (strExt.CompareNoCase(L".clipboard") == 0)
    guidFileType = Gdiplus::ImageFormatUndefined;
  else
  {
    acutPrintf(L"DOSLib error: unknown bitmap format.\n");
    acedRetNil();
    return RSRSLT;
  }

  CDosHideCrosshairs hide;

  BOOL rc = FALSE;
  CDosScreenImage image;
  if (0 == type)
  {
    // Capture primary screen
    rc = image.CaptureScreen();
  }
  else if (1 == type)
  {
    // Capture AutoCAD main window
    rc = image.CaptureWindow(adsw_acadMainWnd());
  }
  else if (2 == type)
  {
    // Capture AutoCAD document view window
    rc = image.CaptureWindow(adsw_acadDocWnd());
  }
  else if (3 == type)
  {
    // Capture foreground window
    rc = image.CaptureWindow(::GetForegroundWindow());
  }
  else // if( 4 == type )
  {
    acutPrintf(prompt);
    CRect rect;
    CDosScreenSelectionDialog dlg(rect);
    dlg.DoModal();
    rc = image.CaptureRect(rect);
  }

  if (rc)
  {
    if (guidFileType == Gdiplus::ImageFormatUndefined)
    {
      ::OpenClipboard(adsw_acadMainWnd());
      ::EmptyClipboard();
      ::SetClipboardData(CF_BITMAP, image);
      ::CloseClipboard();

      image.Detach();

      acedRetT();
      return RSRSLT;
    }
    else
    {
      HRESULT hr = image.Save(strBitmap, guidFileType);
      if (SUCCEEDED(hr))
      {
        acedRetStr(strBitmap);
        return RSRSLT;
      }
    }
  }

  acedRetNil();

  return RSRSLT;
}
