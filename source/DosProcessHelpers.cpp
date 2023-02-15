/////////////////////////////////////////////////////////////////////////////
// DosProcessHelpers.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosProcessHelpers.h"

bool IsPowerUser()
{
  HANDLE AccessToken = 0;
  UCHAR InfoBuffer[1024];
  PTOKEN_GROUPS Groups = (PTOKEN_GROUPS)InfoBuffer;
  DWORD InfoBufferSize = 0;
  PSID AdministratorsSid = 0;
  PSID PowerUsersSid = 0;
  SID_IDENTIFIER_AUTHORITY NtAuthority = { SECURITY_NT_AUTHORITY };

  bool bSuccess = false;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &AccessToken))
    return bSuccess;

  if (!GetTokenInformation(AccessToken, TokenGroups, InfoBuffer, 1024, &InfoBufferSize))
    return bSuccess;

  CloseHandle(AccessToken);

  if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsSid))
    return bSuccess;

  if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_POWER_USERS, 0, 0, 0, 0, 0, 0, &PowerUsersSid))
    return bSuccess;

  UINT x;
  for (x = 0; x < Groups->GroupCount; x++)
  {
    if (EqualSid(AdministratorsSid, Groups->Groups[x].Sid) || EqualSid(PowerUsersSid, Groups->Groups[x].Sid))
    {
      bSuccess = true;
      break;
    }
  }

  FreeSid(AdministratorsSid);
  FreeSid(PowerUsersSid);

  return bSuccess;
}

bool IsAdministrator()
{
  HANDLE AccessToken = 0;
  UCHAR InfoBuffer[1024];
  PTOKEN_GROUPS Groups = (PTOKEN_GROUPS)InfoBuffer;
  DWORD InfoBufferSize = 0;
  PSID AdministratorsSid = 0;
  SID_IDENTIFIER_AUTHORITY NtAuthority = { SECURITY_NT_AUTHORITY };

  bool bSuccess = false;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &AccessToken))
    return bSuccess;

  if (!GetTokenInformation(AccessToken, TokenGroups, InfoBuffer, 1024, &InfoBufferSize))
    return bSuccess;

  CloseHandle(AccessToken);

  if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsSid))
    return bSuccess;

  UINT x;
  for (x = 0; x < Groups->GroupCount; x++)
  {
    if (EqualSid(AdministratorsSid, Groups->Groups[x].Sid))
    {
      bSuccess = true;
      break;
    }
  }

  FreeSid(AdministratorsSid);

  return bSuccess;
}

#define BUFSIZE                                 256
#define PRODUCT_UNDEFINED                       0x00000000
#define PRODUCT_ULTIMATE                        0x00000001
#define PRODUCT_HOME_BASIC                      0x00000002
#define PRODUCT_HOME_PREMIUM                    0x00000003
#define PRODUCT_ENTERPRISE                      0x00000004
#define PRODUCT_HOME_BASIC_N                    0x00000005
#define PRODUCT_BUSINESS                        0x00000006
#define PRODUCT_STANDARD_SERVER                 0x00000007
#define PRODUCT_DATACENTER_SERVER               0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
#define PRODUCT_STARTER                         0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
#define PRODUCT_BUSINESS_N                      0x00000010
#define PRODUCT_WEB_SERVER                      0x00000011
#define PRODUCT_CLUSTER_SERVER                  0x00000012
#define PRODUCT_HOME_SERVER                     0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019
#define PRODUCT_UNLICENSED                      0xABCDABCD
#define PROCESSOR_ARCHITECTURE_AMD64            9
#define SM_SERVERR2                             89
#define VER_SUITE_COMPUTE_SERVER                0x00004000
#define VER_SUITE_STORAGE_SERVER                0x00002000
#define VER_SUITE_BLADE                         0x00000400
#define VER_SUITE_PERSONAL                      0x00000200

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

bool GetOSDisplayString(CString& str)
{
  OSVERSIONINFOEX osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (!GetVersionEx((OSVERSIONINFO*)&osvi))
    return false;

  SYSTEM_INFO si;
  ZeroMemory(&si, sizeof(SYSTEM_INFO));

  PGNSI pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
  if (pGNSI)
    pGNSI(&si);
  else
    GetSystemInfo(&si);

  if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
  {
    str = L"Microsoft ";

    if (osvi.dwMajorVersion == 6)
    {
      if (osvi.dwMinorVersion == 1)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          str += L"Windows 7 ";
        else
          str += L"Windows Server 2008 R2 ";
      }
      else if (osvi.dwMinorVersion == 0)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          str += L"Windows Vista ";
        else
          str += L"Windows Server 2008 ";
      }

      DWORD dwType = 0;
      PGPI pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
      pGPI(6, 0, 0, 0, &dwType);

      switch (dwType)
      {
      case PRODUCT_ULTIMATE:
        str += L"Ultimate Edition";
        break;
      case PRODUCT_HOME_PREMIUM:
        str += L"Home Premium Edition";
        break;
      case PRODUCT_HOME_BASIC:
        str += L"Home Basic Edition";
        break;
      case PRODUCT_ENTERPRISE:
        str += L"Enterprise Edition";
        break;
      case PRODUCT_BUSINESS:
        str += L"Business Edition";
        break;
      case PRODUCT_STARTER:
        str += L"Starter Edition";
        break;
      case PRODUCT_CLUSTER_SERVER:
        str += L"Cluster Server Edition";
        break;
      case PRODUCT_DATACENTER_SERVER:
        str += L"Datacenter Edition";
        break;
      case PRODUCT_DATACENTER_SERVER_CORE:
        str += L"Datacenter Edition (core installation)";
        break;
      case PRODUCT_ENTERPRISE_SERVER:
        str += L"Enterprise Edition";
        break;
      case PRODUCT_ENTERPRISE_SERVER_CORE:
        str += L"Enterprise Edition (core installation)";
        break;
      case PRODUCT_ENTERPRISE_SERVER_IA64:
        str += L"Enterprise Edition for Itanium-based Systems";
        break;
      case PRODUCT_SMALLBUSINESS_SERVER:
        str += L"Small Business Server";
        break;
      case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
        str += L"Small Business Server Premium Edition";
        break;
      case PRODUCT_STANDARD_SERVER:
        str += L"Standard Edition";
        break;
      case PRODUCT_STANDARD_SERVER_CORE:
        str += L"Standard Edition (core installation)";
        break;
      case PRODUCT_WEB_SERVER:
        str += L"Web Server Edition";
        break;
      }

      if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        str += L", 64-bit";
      else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        str += L", 32-bit";
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
    {
      if (GetSystemMetrics(SM_SERVERR2))
        str += L"Windows Server 2003 R2, ";
      else if (osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER)
        str += L"Windows Storage Server 2003";
      else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        str += L"Windows XP Professional x64 Edition";
      else
        str += L"Windows Server 2003, ";

      if (osvi.wProductType != VER_NT_WORKSTATION)
      {
        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            str += L"Datacenter Edition for Itanium-based Systems";
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            str += L"Enterprise Edition for Itanium-based Systems";
        }
        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            str += L"Datacenter x64 Edition";
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            str += L"Enterprise x64 Edition";
          else
            str += L"Standard x64 Edition";
        }
        else
        {
          if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            str += L"Compute Cluster Edition";
          else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            str += L"Datacenter Edition";
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            str += L"Enterprise Edition";
          else if (osvi.wSuiteMask & VER_SUITE_BLADE)
            str += L"Web Edition";
          else
            str += L"Standard Edition";
        }
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
    {
      str += L"Windows XP ";
      if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
        str += L"Home Edition";
      else
        str += L"Professional";
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
    {
      str += L"Windows 2000 ";
      if (osvi.wProductType == VER_NT_WORKSTATION)
        str += L"Professional";
      else
      {
        if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
          str += L"Datacenter Server";
        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
          str += L"Advanced Server";
        else
          str += L"Server";
      }
    }

    if (wcslen(osvi.szCSDVersion) > 0)
    {
      str += L" ";
      str += osvi.szCSDVersion;
    }

    CString tmp;
    tmp.Format(L" (build %d)", osvi.dwBuildNumber);
    str += tmp;

    return true;
  }

  return false;
}

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
bool IsWow64()
{
  BOOL bIsWow64 = TRUE;
  LPFN_ISWOW64PROCESS fnIsWow64Process = 0;
  fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
  if (fnIsWow64Process)
    fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
  return (bIsWow64 == TRUE);
}

bool Is64BitWindows()
{
#if defined(_WIN64)
  return true;       // 64-bit programs run only on Win64
#elif defined(_WIN32)
  return IsWow64();  // 32-bit programs run on both Win32 and Win64
#else
  return false;
#endif
}

bool IsVistaOrHigher()
{
  OSVERSIONINFOEX osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  if (GetVersionEx((OSVERSIONINFO*)&osvi))
  {
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 6)
      return true;
  }
  return false;
}

void DOS_StartProcess(const wchar_t* pszApplicationName)
{
  if (0 == pszApplicationName || 0 == pszApplicationName[0])
    return;

  STARTUPINFO si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  CreateProcess(
    pszApplicationName,
    NULL,           // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
  );

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}