/////////////////////////////////////////////////////////////////////////////
// DosPathMonitor.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosPathMonitor
{
public:
  CDosPathMonitor();
  ~CDosPathMonitor();

  CString Id() const;

  BOOL Install(const wchar_t* pszPath, const wchar_t* pszCommand);
  void Uninstall();

  void SetNotifyFilter(DWORD dwFilter);
  void SetSubtreeWatch(BOOL bState);

  static DWORD WINAPI MonitorProc(LPVOID pv);

private:
  CString m_strPath;
  CString m_strCommand;
  BOOL m_bWatchSubtree;
  DWORD m_dwNotifyFilter;
  HANDLE m_hEvent;
  HANDLE m_hThread;
  CString m_strGuid;

private:
  // no copy constructor
  CDosPathMonitor(const CDosPathMonitor&);
  // no operator=
  CDosPathMonitor& operator=(const CDosPathMonitor&);
};

