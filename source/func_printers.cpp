/////////////////////////////////////////////////////////////////////////////
// func_printers.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"

////////////////////////////////////////////////////////////////
// dos_getprn
int CDOSLibApp::ads_dos_getprn()
{
  CAdsArgs args;

  wchar_t wszPrinter[_MAX_PATH];
  DWORD dw = _MAX_PATH;
  if (GetDefaultPrinter(wszPrinter, &dw))
    acedRetStr(wszPrinter);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_printers
int CDOSLibApp::ads_dos_printers()
{
  HANDLE hHeap = ::GetProcessHeap();
  if (0 == hHeap)
    return RSERR;

  DWORD dwFlags = PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS;
  DWORD dwSizeNeeded = 0;
  DWORD dwNumItems = 0;
  ::EnumPrinters(dwFlags, 0, 2, 0, 0, &dwSizeNeeded, &dwNumItems);

  LPPRINTER_INFO_2 pInfo = (LPPRINTER_INFO_2)::HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSizeNeeded);
  if (0 == pInfo)
    return RSERR;

  CAdsRetList result;
  if (::EnumPrinters(dwFlags, 0, 2, (LPBYTE)pInfo, dwSizeNeeded, &dwSizeNeeded, &dwNumItems))
  {
    DWORD i;
    for (i = 0; i < dwNumItems; i++)
      result.Add(pInfo[i].pPrinterName);
  }

  ::HeapFree(hHeap, 0, pInfo);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_setprn
int CDOSLibApp::ads_dos_setprn()
{
  CAdsArgs args;

  CString strPrinter;
  if (!args.GetString(strPrinter))
    return RSERR;

  if (SetDefaultPrinter(strPrinter))
    acedRetStr(strPrinter);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_spool
int CDOSLibApp::ads_dos_spool()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  args.Next();

  CString strPrinter;
  if (!args.GetString(strPrinter))
    return RSERR;

  TCHAR tchDevice[_MAX_PATH];
  DWORD dwResult = GetProfileString(L"devices", strPrinter, L"", tchDevice, _MAX_PATH);
  if (dwResult < 2)
  {
    acedRetNil();
    return RSRSLT;
  }

  HANDLE hFile = CreateFile(strPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    acedRetNil();
    return RSRSLT;
  }

  DWORD dwFileSize = GetFileSize(hFile, 0);

  HANDLE hFileMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, dwFileSize, 0);
  if (!hFileMap)
  {
    CloseHandle(hFile);
    acedRetNil();
    return RSRSLT;
  }

  LPBYTE lpData = (LPBYTE)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
  if (!lpData)
  {
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    acedRetNil();
    return RSRSLT;
  }

  HANDLE hPrinter = 0;
  BOOL rc = OpenPrinter(strPrinter.GetBuffer(_MAX_PATH), &hPrinter, 0);
  strPrinter.ReleaseBuffer();
  if (!rc)
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    acedRetNil();
    return RSRSLT;
  }

  DOC_INFO_1 DocInfo;
  memset(&DocInfo, 0, sizeof(DocInfo));
  DocInfo.pDocName = strPath.GetBuffer(strPath.GetLength());
  DocInfo.pOutputFile = 0;
  DocInfo.pDatatype = L"RAW";

  DWORD dwJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&DocInfo);
  strPath.ReleaseBuffer();
  if (dwJob == 0)
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    ClosePrinter(hPrinter);
    acedRetNil();
    return RSRSLT;
  }

  if (!StartPagePrinter(hPrinter))
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    EndDocPrinter(hPrinter);
    ClosePrinter(hPrinter);
    acedRetNil();
    return RSRSLT;
  }

  DWORD dwBytesWritten = 0;
  if (!WritePrinter(hPrinter, lpData, dwFileSize, &dwBytesWritten))
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    EndPagePrinter(hPrinter);
    EndDocPrinter(hPrinter);
    ClosePrinter(hPrinter);
    acedRetNil();
    return RSRSLT;
  }

  if (!EndPagePrinter(hPrinter))
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    EndDocPrinter(hPrinter);
    ClosePrinter(hPrinter);
    acedRetNil();
    return RSRSLT;
  }

  if (!EndDocPrinter(hPrinter))
  {
    UnmapViewOfFile(lpData);
    CloseHandle(hFileMap);
    CloseHandle(hFile);
    ClosePrinter(hPrinter);
    acedRetNil();
    return RSRSLT;
  }

  UnmapViewOfFile(lpData);
  CloseHandle(hFileMap);
  CloseHandle(hFile);
  ClosePrinter(hPrinter);

  if (dwBytesWritten != dwFileSize)
  {
    acedRetNil();
    return RSRSLT;
  }

  acedRetStr(strPath);
  return RSRSLT;
}
