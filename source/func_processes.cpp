/////////////////////////////////////////////////////////////////////////////
// func_processes.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosProcessHelpers.h"

////////////////////////////////////////////////////////////////
// dos_command
int CDOSLibApp::ads_dos_command()
{
  CAdsArgs args;

  CString strCommand;
  if (!args.GetString(strCommand))
    return RSERR;

  args.Next();

  int nCmdShow = 0;
  if (args.IsEmpty())
    nCmdShow = 0;
  else if (!args.GetInteger(nCmdShow))
    return RSERR;

  UINT uCmdShow = SW_SHOWNORMAL;
  switch (nCmdShow)
  {
  case 0:
    uCmdShow = SW_SHOWNORMAL;
    break;
  case 1:
    uCmdShow = SW_SHOWNOACTIVATE;
    break;
  case 2:
    uCmdShow = SW_SHOWMINIMIZED;
    break;
  case 3:
    uCmdShow = SW_SHOWMINNOACTIVE;
    break;
  case 4:
    uCmdShow = SW_SHOWMAXIMIZED;
    break;
  default:
    uCmdShow = SW_SHOWNORMAL;
    break;
  }

  CString str;
  str.Format(L"CMD.EXE /C %s", strCommand);

  if (WinExec(CStringA(str), uCmdShow) > 31)
    acedRetStr(str);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_execute
int CDOSLibApp::ads_dos_execute()
{
  CAdsArgs args;

  CString strCommand;
  if (!args.GetString(strCommand))
    return RSERR;

  args.Next();

  int nCmdShow = 0;
  if (args.IsEmpty())
    nCmdShow = 0;
  else if (!args.GetInteger(nCmdShow))
    return RSERR;

  UINT uCmdShow = SW_SHOWNORMAL;
  switch (nCmdShow)
  {
  case 0:
    uCmdShow = SW_SHOWNORMAL;
    break;
  case 1:
    uCmdShow = SW_SHOWNOACTIVATE;
    break;
  case 2:
    uCmdShow = SW_SHOWMINIMIZED;
    break;
  case 3:
    uCmdShow = SW_SHOWMINNOACTIVE;
    break;
  case 4:
    uCmdShow = SW_SHOWMAXIMIZED;
    break;
  default:
    uCmdShow = SW_SHOWNORMAL;
    break;
  }

  if (WinExec(CStringA(strCommand), uCmdShow) > 31)
    acedRetStr(strCommand);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_exewait
int CDOSLibApp::ads_dos_exewait()
{
  CAdsArgs args;

  CString strCommand;
  if (!args.GetString(strCommand))
    return RSERR;

  args.Next();

  int nCmdShow = 0;
  if (args.IsEmpty())
    nCmdShow = 0;
  else if (!args.GetInteger(nCmdShow))
    return RSERR;

  WORD uCmdShow = SW_SHOWNORMAL;
  switch (nCmdShow)
  {
  case 0:
    uCmdShow = SW_SHOWNORMAL;
    break;
  case 1:
    uCmdShow = SW_SHOWNOACTIVATE;
    break;
  case 2:
    uCmdShow = SW_SHOWMINIMIZED;
    break;
  case 3:
    uCmdShow = SW_SHOWMINNOACTIVE;
    break;
  case 4:
    uCmdShow = SW_SHOWMAXIMIZED;
    break;
  default:
    uCmdShow = SW_SHOWNORMAL;
    break;
  }

  STARTUPINFO si;
  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = uCmdShow;

  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));

  LPTSTR lpCommand = strCommand.GetBuffer();
  BOOL bResult = CreateProcess(
    0,          // No module name (use command line). 
    lpCommand,  // Command line. 
    0,          // Process handle not inheritable. 
    0,          // Thread handle not inheritable. 
    FALSE,      // Set handle inheritance to FALSE. 
    0,          // No creation flags. 
    0,          // Use parent's environment block. 
    0,          // Use parent's starting directory. 
    &si,        // Pointer to STARTUPINFO structure.
    &pi         // Pointer to PROCESS_INFORMATION structure.
    );
  strCommand.ReleaseBuffer();

  if (bResult == TRUE)
  {
    acutPrintf(L"Running external process, please wait...\n");
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    // 13-Jun-2007
    SetWindowPos(adsw_acadMainWnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetFocus(adsw_acadMainWnd());
    acedRetT();
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_shellexe
int CDOSLibApp::ads_dos_shellexe()
{
  CAdsArgs args;

  CString strPath;
  if (!args.GetString(strPath))
    return RSERR;

  args.Next();

  CDosPathString strParam;
  bool bParam = false;

  if (args.IsEmpty())
  {
    acutPrintf(L"DOSLib error: too few arguments.\n");
    return RSERR;
  }
  else if (args.IsString())
  {
    if (!args.GetString(strParam))
      return RSERR;
    bParam = true;
  }
  else if (args.IsNil())
  {
    bParam = false;
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  args.Next();

  int nOperation = 0;
  int nCmdShow = 0;

  if (args.IsEmpty())
  {
    nOperation = 0;
  }
  else
  {
    if (!args.GetInteger(nOperation))
      return RSERR;

    args.Next();

    if (args.IsEmpty())
    {
      nCmdShow = 0;
    }
    else
    {
      if (!args.GetInteger(nCmdShow))
        return RSERR;
    }
  }

  CString strOperation;
  switch (nOperation)
  {
  case 0:
    strOperation = L"open";
    break;
  case 1:
    strOperation = L"print";
    break;
  case 2:
    strOperation = L"explore";
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  UINT uCmdShow = (UINT)nCmdShow;
  switch (uCmdShow)
  {
  case 0:
    uCmdShow = SW_SHOWNORMAL;
    break;
  case 1:
    uCmdShow = SW_SHOWNOACTIVATE;
    break;
  case 2:
    uCmdShow = SW_SHOWMINIMIZED;
    break;
  case 3:
    uCmdShow = SW_SHOWMINNOACTIVE;
    break;
  case 4:
    uCmdShow = SW_SHOWMAXIMIZED;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  HINSTANCE hReturn = 0;
  if (bParam)
    hReturn = ShellExecute(0, strOperation, strPath, strParam, 0, uCmdShow);
  else
    hReturn = ShellExecute(0, strOperation, strPath, 0, 0, uCmdShow);

  //if( (int)hReturn > 32 )
  acedRetT();
  //else
  //  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_processes

static bool GetProcessName(DWORD processID, CString& strName, bool bFullPath = false)
{
  BOOL rc = FALSE;
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
  if (NULL != hProcess)
  {
    HMODULE hModule = 0;
    DWORD cbNeeded = 0;
    if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
    {
      wchar_t szName[_MAX_PATH] = L"<unknown>";
      if (bFullPath)
        rc = GetModuleFileNameEx(hProcess, hModule, szName, sizeof(szName) / sizeof(szName[0]));
      else
        rc = GetModuleBaseName(hProcess, hModule, szName, sizeof(szName) / sizeof(szName[0]));
      if (rc)
        strName = szName;
    }
  }
  CloseHandle(hProcess);
  return (rc) ? true : false;
}

int CDOSLibApp::ads_dos_processes()
{
  CAdsArgs args;

  bool bFullPath = false;
  if (!args.IsEmpty())
    bFullPath = true;

  DWORD aProcesses[1024], cbNeeded = 0;
  memset(&aProcesses, 0, sizeof(aProcesses));
  if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
  {
    acedRetNil();
    return RSRSLT;
  }

  DWORD cProcesses = cbNeeded / sizeof(DWORD);
  CAdsRetList result;

  unsigned int i;
  for (i = 0; i < cProcesses; i++)
  {
    CString str;
    if (GetProcessName(aProcesses[i], str, bFullPath))
      result.Add(str);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}
