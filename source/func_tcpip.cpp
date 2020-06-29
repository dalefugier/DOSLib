/////////////////////////////////////////////////////////////////////////////
// func_tcpip.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

////////////////////////////////////////////////////////////////
// dos_hostname
int CDOSLibApp::ads_dos_hostname()
{
  WSAData wsaData;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    acedRetNil();
    return RSRSLT;
  }

  char szName[_MAX_PATH];
  if (gethostname(szName, sizeof(szName)) == SOCKET_ERROR)
  {
    acutPrintf(L"DOSLib error: WinSock error getting local host name.\n ");
    WSACleanup();
    return RSERR;
  }

  struct hostent* pHostEnt = gethostbyname(szName);
  if (pHostEnt == 0)
  {
    acutPrintf(L"DOSLib error: WinSock error looking up host name.\n");
    WSACleanup();
    return RSERR;
  }

  CString str(pHostEnt->h_name);
  acedRetStr(str);
  WSACleanup();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_ipaddress
int CDOSLibApp::ads_dos_ipaddress()
{
  CAdsRetList result;

  PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
  if (0 == pAdapterInfo)
  {
    acutPrintf(L"DOSLib error: out of memory.\n");
    acedRetNil();
    return RSRSLT;
  }

  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
  {
    FREE(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
    if (0 == pAdapterInfo)
    {
      acutPrintf(L"DOSLib error: out of memory.\n");
      acedRetNil();
      return RSRSLT;
    }
  }

  DWORD dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (dwRetVal == NO_ERROR)
  {
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter)
    {
      PIP_ADDR_STRING pIpStr = &pAdapter->IpAddressList;
      while (pIpStr)
      {
        result.Add(CString(pIpStr->IpAddress.String));
        result.Add(CString(pIpStr->IpMask.String));
        pIpStr = pIpStr->Next;
      }
      pAdapter = pAdapter->Next;
    }
  }

  if (pAdapterInfo)
    FREE(pAdapterInfo);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_macaddress
int CDOSLibApp::ads_dos_macaddress()
{
  CAdsRetList result;

  PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
  if (0 == pAdapterInfo)
  {
    acutPrintf(L"DOSLib error: out of memory.\n");
    acedRetNil();
    return RSRSLT;
  }

  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
  {
    FREE(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
    if (0 == pAdapterInfo)
    {
      acutPrintf(L"DOSLib error: out of memory.\n");
      acedRetNil();
      return RSRSLT;
    }
  }

  DWORD dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (dwRetVal == NO_ERROR)
  {
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter)
    {
      CString s, t;
      for (UINT i = 0; i < pAdapter->AddressLength; i++)
      {
        if (i == (pAdapter->AddressLength - 1))
          t.Format(L"%.2X", (int)pAdapter->Address[i]);
        else
          t.Format(L"%.2X:", (int)pAdapter->Address[i]);
        s += t;
      }
      result.Add(s);
      pAdapter = pAdapter->Next;
    }
  }

  if (pAdapterInfo)
    FREE(pAdapterInfo);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_nicinfo

static int GetFriendlyAdapterNames(CMapStringToString& map)
{
  const int WORKING_BUFFER_SIZE = 15000;
  const int MAX_TRIES = 3;

  PIP_ADAPTER_ADDRESSES pAddresses = 0;
  ULONG outBufLen = WORKING_BUFFER_SIZE;
  ULONG nIterations = 0;

  PIP_ADAPTER_ADDRESSES pCurrAddresses = 0;
  DWORD dwRetVal = 0;

  do
  {
    pAddresses = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
    if (0 == pAddresses)
      return -1;

    dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, 0, pAddresses, &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
      FREE(pAddresses);
      pAddresses = 0;
    }
    else
    {
      break;
    }

    nIterations++;

  } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (nIterations < MAX_TRIES));

  if (dwRetVal == NO_ERROR)
  {
    pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {
      map.SetAt(CString(pCurrAddresses->AdapterName), CString(pCurrAddresses->FriendlyName));
      pCurrAddresses = pCurrAddresses->Next;
    }
  }

  if (pAddresses)
    FREE(pAddresses);

  return (int)map.GetCount();
}

int CDOSLibApp::ads_dos_nicinfo()
{
  CMapStringToString map;
  if (-1 == GetFriendlyAdapterNames(map))
  {
    acutPrintf(L"DOSLib error: out of memory.\n");
    acedRetNil();
    return RSRSLT;
  }

  PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
  if (0 == pAdapterInfo)
  {
    acutPrintf(L"DOSLib error: out of memory.\n");
    acedRetNil();
    return RSRSLT;
  }

  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
  {
    FREE(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
    if (0 == pAdapterInfo)
    {
      acutPrintf(L"DOSLib error: out of memory.\n");
      acedRetNil();
      return RSRSLT;
    }
  }

  CAdsRetList result;
  DWORD dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (dwRetVal == NO_ERROR)
  {
    result.AddLB();

    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter)
    {
      result.AddLB();

      // Name or uuid
      result.Add(CString(pAdapter->AdapterName), true);

      // Friendly name
      CString strFriendly;
      map.Lookup(CString(pAdapter->AdapterName), strFriendly);
      result.Add(strFriendly, true);

      // Description
      result.Add(CString(pAdapter->Description), true);

      // Address
      CString s, t;
      for (UINT i = 0; i < pAdapter->AddressLength; i++)
      {
        if (i == (pAdapter->AddressLength - 1))
          t.Format(L"%.2X", (int)pAdapter->Address[i]);
        else
          t.Format(L"%.2X:", (int)pAdapter->Address[i]);
        s += t;
      }
      result.Add(s);

      // Type
      switch (pAdapter->Type)
      {
      case MIB_IF_TYPE_OTHER:
        result.Add(L"OTHER");
        break;
      case MIB_IF_TYPE_ETHERNET:
        result.Add(L"ETHERNET");
        break;
      case MIB_IF_TYPE_TOKENRING:
        result.Add(L"TOKENRING");
        break;
      case MIB_IF_TYPE_FDDI:
        result.Add(L"FDDI");
        break;
      case MIB_IF_TYPE_PPP:
        result.Add(L"PPP");
        break;
      case MIB_IF_TYPE_LOOPBACK:
        result.Add(L"LOOPBACK");
        break;
      case MIB_IF_TYPE_SLIP:
        result.Add(L"SLIP");
        break;
      default:
        result.Add(L"UNKNOWN");
        break;
      }

      // IP address
      result.Add(CString(pAdapter->IpAddressList.IpAddress.String), true);
      // Subnet mask
      result.Add(CString(pAdapter->IpAddressList.IpMask.String), true);
      // Default gateway
      result.Add(CString(pAdapter->GatewayList.IpAddress.String), true);
      // DHCP or fixed ip
      result.Add(pAdapter->DhcpEnabled ? L"DHCP" : L"FIXED");

      result.AddLE();

      pAdapter = pAdapter->Next;
    }

    result.AddLE();
  }

  if (pAdapterInfo)
    FREE(pAdapterInfo);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}
