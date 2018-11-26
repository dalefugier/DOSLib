/////////////////////////////////////////////////////////////////////////////
// DosPathMonitor.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosPathMonitor.h"

CDosPathMonitor::CDosPathMonitor()
  : m_hThread(0)
  , m_hEvent(0)
  , m_dwNotifyFilter(FILE_NOTIFY_CHANGE_LAST_WRITE)
  , m_bWatchSubtree(FALSE)
{
  GUID guid = GUID_NULL;
  ::UuidCreate(&guid);

  m_strGuid.Format(
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
}

CDosPathMonitor::~CDosPathMonitor()
{
  Uninstall();
}

CString CDosPathMonitor::Id() const
{
  return m_strGuid;
}

void CDosPathMonitor::SetNotifyFilter(DWORD dwFilter)
{
  if (0 == m_hThread) // not effective on active thread
    m_dwNotifyFilter = dwFilter;
}

void CDosPathMonitor::SetSubtreeWatch(BOOL bState)
{
  if (0 == m_hThread) // not effective on active thread
    m_bWatchSubtree = bState;
}

BOOL CDosPathMonitor::Install(const wchar_t* pszPath, const wchar_t* pszCommand)
{
  Uninstall();

  if (0 == pszPath || 0 == pszPath[0])
    return FALSE;

  if (0 == pszCommand || 0 == pszCommand[0])
    return FALSE;

  m_strPath = pszPath;
  m_strCommand = pszCommand;

  m_hEvent = ::CreateEvent(0, FALSE, FALSE, 0);
  if (0 == m_hEvent)
    return FALSE;

  DWORD dwThreadID = 0;
  m_hThread = ::CreateThread(0, 0, CDosPathMonitor::MonitorProc, this, 0, &dwThreadID);
  if (!m_hThread)
  {
    ::CloseHandle(m_hEvent);
    m_hEvent = 0;
    ::CloseHandle(m_hThread);
    m_hThread = 0;
    return FALSE;
  }

  return TRUE;
}

void CDosPathMonitor::Uninstall()
{
  if (0 == m_hThread)
    return;

  ::SetEvent(m_hEvent);
  ::WaitForMultipleObjects(1, &m_hThread, TRUE, INFINITE);

  if (m_hEvent)
    ::CloseHandle(m_hEvent);
  m_hEvent = 0;

  if (m_hThread)
    ::CloseHandle(m_hThread);
  m_hThread = 0;
}

DWORD WINAPI CDosPathMonitor::MonitorProc(LPVOID pv)
{
  CDosPathMonitor* p = (CDosPathMonitor*)pv;
  if (0 == p)
    return 1;

  HANDLE hChange = FindFirstChangeNotification(p->m_strPath, p->m_bWatchSubtree, p->m_dwNotifyFilter);
  if (hChange == INVALID_HANDLE_VALUE)
    return 1;

  HANDLE aHandles[2];
  aHandles[0] = hChange;
  aHandles[1] = p->m_hEvent;

  while (true)
  {
    DWORD dwEvent = WaitForMultipleObjects(2, aHandles, FALSE, INFINITE);
    if (WAIT_OBJECT_0 == dwEvent)
    {
      CString strBuffer;
      wchar_t* lpBuffer = strBuffer.GetBuffer(_MAX_PATH + 1);
      DWORD dwBytesReturned = 0;

      BOOL rc = ReadDirectoryChangesW(hChange, lpBuffer, _MAX_PATH, p->m_bWatchSubtree, p->m_dwNotifyFilter, &dwBytesReturned, 0, 0);

      strBuffer.ReleaseBuffer();
      if (rc)
        ShellExecute(0, L"open", p->m_strCommand, strBuffer, 0, SW_SHOWNORMAL);

      FindNextChangeNotification(hChange);
      continue;
    }
    break;
  }

  FindCloseChangeNotification(hChange);

  return 0;
}


