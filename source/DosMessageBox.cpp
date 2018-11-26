/////////////////////////////////////////////////////////////////////////////
// DosMessageBox.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosMessageBox.h"

int CDosMessageBox::DoMessageBox()
{
  if (!m_uTimeOut)
    return MessageBox(m_hWnd, m_lpText, m_lpCaption, m_uType);
  return TimedOutMessageBox();
}

int CDosMessageBox::DoMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, UINT uTimeOut, BOOL bShowTimeOut)
{
  if (!uTimeOut)
    return MessageBox(hWnd, lpText, lpCaption, uType);

  m_hWnd = hWnd;
  m_lpText = lpText;
  m_lpCaption = lpCaption;
  m_uType = uType;
  m_uTimeOut = uTimeOut;
  m_bShowTimeOut = bShowTimeOut;

  return TimedOutMessageBox();
}

int CDosMessageBox::TimedOutMessageBox()
{
  DWORD dwThreadId = 0;
  HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, (LPVOID)this, 0, &dwThreadId);
  if (0 == hThread)
    return -1;

  if (m_bShowTimeOut)
    swprintf(m_szTimeOutCaption, L"%s, time remaining: %d", m_lpCaption, (int)(m_uTimeOut / 1000));
  else
    wcscpy(m_szTimeOutCaption, m_lpCaption);

  m_nResult = MessageBox(m_hWnd, m_lpText, m_szTimeOutCaption, m_uType);

  WaitForSingleObject(hThread, INFINITE);
  CloseHandle(hThread);

  if (m_bTimeOut)
    return 0;

  return m_nResult;
}


DWORD WINAPI CDosMessageBox::ThreadFunction(LPVOID lpParameter)
{
  TCHAR szBuffer[_MAX_PATH];

  if (lpParameter)
  {
    CDosMessageBox* pObject = static_cast<CDosMessageBox*>(lpParameter);
    if (pObject)
    {
      DWORD dwInitTime = GetTickCount();
      DWORD dwStartTime = dwInitTime;
      while (!pObject->m_nResult)
      {
        DWORD dwCurrentTime = GetTickCount();
        DWORD dwDeltaTime = dwCurrentTime - dwStartTime;
        if (dwDeltaTime >= 1000)
        {
          if ((dwCurrentTime - dwInitTime) >= pObject->m_uTimeOut)
          {
            pObject->m_bTimeOut = TRUE;
            PostMessage(FindWindow(0, pObject->m_szTimeOutCaption), WM_CLOSE, 0, 0);
            return WAIT_OBJECT_0;
          }

          if (pObject->m_bShowTimeOut)
          {
            HWND hWnd = FindWindow(0, pObject->m_szTimeOutCaption);
            if (hWnd)
            {
              int nCaptionLen = (int)wcslen(pObject->m_szTimeOutCaption);
              int nMarker = nCaptionLen;

              while (pObject->m_szTimeOutCaption[nMarker] != 32)
                nMarker--;

              int val = (int)ceil((double)((pObject->m_uTimeOut - (dwCurrentTime - dwInitTime))) / 1000);
              _itow(val, szBuffer, 10);

              memcpy(&pObject->m_szTimeOutCaption[nMarker + 1], szBuffer, wcslen(szBuffer) + 1);
              SetWindowText(hWnd, pObject->m_szTimeOutCaption);
            }
          }

          dwStartTime = GetTickCount();
        }

        Sleep(5);
      }
    }
  }

  return WAIT_OBJECT_0;
}


