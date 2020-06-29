/////////////////////////////////////////////////////////////////////////////
// DosProcessHelpers.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

// Determines if teh user is a member of the Power Users group
bool IsPowerUser();
// Determines if teh user is a member of the Administrators group
bool IsAdministrator();

// Gets operating system version display string
bool GetOSDisplayString(CString& str);

// Determines if the current 32-bit process is running under 64-bit Windows 
bool IsWow64();
// Determines if Windows is 64-bit
bool Is64BitWindows();
// Determines if Windows is Vista or higher
bool IsVistaOrHigher();

void DOS_StartProcess(const wchar_t* pszApplicationName);
