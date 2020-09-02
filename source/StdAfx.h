/////////////////////////////////////////////////////////////////////////////
// StdAfx.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)

#define STRICT

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			    //  Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE				      // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0550		  // Change this to the appropriate value to target IE 5.5 or later.
#endif

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include "ArxVer.h"           // Defines ARX and BRX versions

#include <map>                // ObjectARX and OMF headers needs this

#include <afxwin.h>				    // MFC core and standard components
#include <afxext.h>				    // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>				    // MFC OLE classes
#include <afxodlgs.h>			    // MFC OLE dialog classes
#include <afxdisp.h>			    // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>				    // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>				    // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>			    // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>				    // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>          // ATL base classes
#include <atlimage.h>         // CImage support
#include <shlwapi.h>          // Shell Lightweight API
#include <objbase.h>          // Component object model defintions
#include <shellapi.h>         // Shell functions
#include <shlobj.h>           // Shell definitions
#include <direct.h>           // Directory handling/creation
#include <winnetwk.h>         // Standard WINNET Header File for WIN32
#include <psapi.h>            // Include file for APIs provided by PSAPI.DLL
#include <mmsystem.h>         // Include file for Multimedia API's
#include <afxmt.h>            // MFC Multithreaded Extensions
#include <vfw.h>              // Video for windows include file for WIN32
#include <gdiplus.h>          // GDI+ declarations
#include <winspool.h>         // Printing functions
#include <vector>             // Used by CDosReadDir
#include <htmlhelp.h>         // HTML help support
#include <winsock.h>          // Windows sockets
#include <Iphlpapi.h>         // IP Helper API
#include <uxtheme.h>          // CToasterWnd support
#include <cderr.h>            // for FNERR_BUFFERTOSMALL
#include <mshtmhst.h>         // MSTHML host interfaces
#include <math.h>             // Math functions
#include <limits.h>
#include <float.h>
#include <stack>

#include <arxHeaders.h>
#include <dbxHeaders.h>

#if _ARX_VER > 20 || _ZRX_VER > 20
#include <acdbimageutils.h>
#endif

#include "DosMath.h"
#include "DosString.h"
#include "DosPathString.h"
#include "DosStringArray.h"
#include "DosAdsHelpers.h"
#include "Resource.h"

// Linker pragmas
#pragma comment( lib, "Mpr.lib" )
#pragma comment( lib, "Psapi.lib" )
#pragma comment( lib, "Rpcrt4.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "vfw32.lib" )
#pragma comment( lib, "shlwapi.lib" )
#pragma comment( lib, "shell32.lib" )
#pragma comment( lib, "htmlhelp.lib" )
#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "wsock32.lib" )
#pragma comment( lib, "iphlpapi.lib" )
#pragma comment( lib, "Version.lib" )
#pragma comment( lib, "UxTheme.lib" )

#pragma pack (pop)
