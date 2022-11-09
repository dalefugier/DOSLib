/////////////////////////////////////////////////////////////////////////////
// DOSLibApp.cpp
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
#include "DosAboutDialog.h"

#define szRDS _RXST("RMA_")

IMPLEMENT_ARX_ENTRYPOINT(CDOSLibApp)

CDOSLibApp& DOSLibApp()
{
  AcRxDbxApp* pDbx = acrxGetApp();
  ASSERT(pDbx != NULL);
  CDOSLibApp* pDosLib = (CDOSLibApp*)pDbx;
  ASSERT(pDosLib != NULL);
  return *pDosLib;
}

CDOSLibApp::CDOSLibApp()
  : AcRxArxApp()
{
  m_strHelpPath = L"";
  m_nMajorVersion = 9;
  m_nMinorVersion = 1;
  m_nServiceRelease = 1;
  m_strVersion.Format(L"%d.%d.%d", m_nMajorVersion, m_nMinorVersion, m_nServiceRelease);
  srand((unsigned)time(nullptr));
}

CDOSLibApp::~CDOSLibApp()
{
}

AcRx::AppRetCode CDOSLibApp::On_kInitAppMsg(void* pkt)
{
  AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

  // Demandloading
  if (false == IsDemandLoadRegistered())
    SetDemandLoadOnRequest();

  // Help file
  CRegKey reg;
  long lResult = reg.Open(HKEY_CURRENT_USER, AppRegKey());
  if (lResult == ERROR_SUCCESS)
  {
    CString strValue;
    ULONG nChars = _MAX_PATH;
    lResult = reg.QueryStringValue(L"Help", strValue.GetBuffer(nChars), &nChars);
    strValue.ReleaseBuffer();
    reg.Close();
    if (lResult == ERROR_SUCCESS && strValue.GetLength() > 0)
      m_strHelpPath = strValue;
  }

  acutPrintf(L"\n%s %s\n%s\n", AppName(), AppVersion(), AppCopyright());

  return retCode;
}

AcRx::AppRetCode CDOSLibApp::On_kUnloadAppMsg(void* pkt)
{
  AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

  // Help file
  if (!m_strHelpPath.IsEmpty())
  {
    CRegKey reg;
    long lResult = reg.Open(HKEY_CURRENT_USER, AppRegKey());
    if (lResult == ERROR_SUCCESS)
    {
      reg.SetStringValue(L"Help", m_strHelpPath);
      reg.Close();
    }
  }

  return retCode;
}

void CDOSLibApp::RegisterServerComponents()
{
}

const wchar_t* CDOSLibApp::AppName() const
{
  return L"DOSLib";
}

const wchar_t* CDOSLibApp::AppDescription() const
{
#if defined(_BRX)
  return L"LISP Library for BricsCAD";
#elif defined(_ZRX)
  return L"LISP Library for ZWCAD";
#else
  return L"LISP Library for AutoCAD";
#endif
}

const wchar_t* CDOSLibApp::AppVersion() const
{
  return m_strVersion;
}

const wchar_t* CDOSLibApp::AppCopyright() const
{
  return L"Copyright (c) 1992-2022 Robert McNeel & Associates.";
}

int CDOSLibApp::MajorVersion() const
{
  return m_nMajorVersion;
}

int CDOSLibApp::MinorVersion() const
{
  return m_nMinorVersion;
}

int CDOSLibApp::ServiceRelease() const
{
  return m_nServiceRelease;
}

const wchar_t* CDOSLibApp::AppInternet() const
{
  return L"https://github.com/dalefugier/DOSLib";
}

CString CDOSLibApp::AppRegKey()
{
  CString str;
  str.Format(L"SOFTWARE\\McNeel\\%s\\%d.%d", AppName(), MajorVersion(), MinorVersion());
  return str;
}

bool CDOSLibApp::IsDemandLoadRegistered() const
{
  bool rc = false;
  AcadAppInfo appInfo;
  appInfo.setAppName(AppName());
  appInfo.setAppDesc(AppDescription());
  if (AcadApp::eOk == appInfo.readFromRegistry())
  {
    CString strPath(appInfo.moduleName());
    if (strPath.CompareNoCase(acedGetAppName()) == 0)
      rc = true;
  }
  return rc;
}

bool CDOSLibApp::IsDemandLoadOnStartup() const
{
  bool rc = false;
  AcadAppInfo appInfo;
  appInfo.setAppName(AppName());
  appInfo.setModuleName(acedGetAppName());
  appInfo.setAppDesc(AppDescription());
  if (AcadApp::eOk == appInfo.readFromRegistry())
  {
    if (AcadApp::kOnAutoCADStartup & appInfo.loadReason())
      rc = true;
  }
  return rc;
}

void CDOSLibApp::SetDemandLoadOnStartup() const
{
  AcadAppInfo appInfo;
  appInfo.setAppName(AppName());
  appInfo.setModuleName(acedGetAppName());
  appInfo.setAppDesc(AppDescription());
  appInfo.setLoadReason(AcadApp::LoadReasons(AcadApp::kOnAutoCADStartup | AcadApp::kOnCommandInvocation | AcadApp::kOnLoadRequest));
  appInfo.writeToRegistry();
  appInfo.writeGroupNameToRegistry(L"RMA_DOSLIB");
  appInfo.writeCommandNameToRegistry(L"_DOSLIB", L"DOSLIB");
  appInfo.writeCommandNameToRegistry(L"_DOSLIBHELP", L"DOSLIBHELP");
}

void CDOSLibApp::SetDemandLoadOnRequest() const
{
  AcadAppInfo appInfo;
  appInfo.setAppName(AppName());
  appInfo.setModuleName(acedGetAppName());
  appInfo.setAppDesc(AppDescription());
  appInfo.setLoadReason(AcadApp::LoadReasons(AcadApp::kOnCommandInvocation | AcadApp::kOnLoadRequest));
  appInfo.writeToRegistry();
  appInfo.writeGroupNameToRegistry(L"RMA_DOSLIB");
  appInfo.writeCommandNameToRegistry(L"_DOSLIB", L"DOSLIB");
  appInfo.writeCommandNameToRegistry(L"_DOSLIBHELP", L"DOSLIBHELP");
}

bool CDOSLibApp::GetHelpPath(CString& strHelpPath)
{
  if (PathFileExists(m_strHelpPath))
  {
    strHelpPath = m_strHelpPath;
    return true;
  }

  CString strPath = acedGetAppName();
  PathRenameExtension(strPath.GetBuffer(_MAX_PATH), L".chm");
  strPath.ReleaseBuffer();
  if (PathFileExists(strPath))
  {
    strHelpPath = strPath;
    m_strHelpPath = strHelpPath;
    return true;
  }

  CString strFile = PathFindFileName(strPath);
  strPath.Empty();
  int rc = DOS_FindFile(strFile, strPath.GetBuffer(_MAX_PATH), _MAX_PATH);

  strPath.ReleaseBuffer();
  if (rc == RSRSLT)
  {
    strHelpPath = strPath;
    m_strHelpPath = strHelpPath;
    return true;
  }

  MessageBox(
    adsw_acadMainWnd(),
    L"Unable to locate the DOSLib help file.\nPlease locate the help file.",
    AppName(),
    MB_ICONEXCLAMATION | MB_OK
  );

  CFileDialog dlg(
    TRUE,
    L"chm",
    strFile,
    OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
    L"HTML Help Files|*.chm||"
  );

  dlg.m_ofn.lpstrTitle = L"Open";
  dlg.m_ofn.nFilterIndex = 1;
  if (dlg.DoModal() == IDOK)
  {
    strHelpPath = dlg.GetPathName();
    m_strHelpPath = strHelpPath;
    return true;
  }

  return false;
}

void CDOSLibApp::RMA_DOSLIB_DOSLIB()
{
  acutPrintf(L"\n%s Loaded.\n", DOSLibApp().AppName());
}

void CDOSLibApp::RMA_DOSLIB_DOSLIBHELP()
{
  CString strPath;
  if (DOSLibApp().GetHelpPath(strPath))
    ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_DISPLAY_TOC, 0);
}

// Commands
ACED_ARXCOMMAND_ENTRY_AUTO(CDOSLibApp, RMA_DOSLIB, _DOSLIB, DOSLIB, ACRX_CMD_SESSION | ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDOSLibApp, RMA_DOSLIB, _DOSLIBHELP, DOSLIBHELP, ACRX_CMD_SESSION | ACRX_CMD_TRANSPARENT, NULL)

// 10-Feb-2016 - This fixes the warning C4838 when using ACED_ADSSYMBOL_ENTRY_AUTO in ARX21
#if defined(_BRX)
#define DOS_ADSSYMBOL_ENTRY_AUTO(T_CLASS,T_NAME,T_REGISTERFUNCTION) \
    __declspec(selectany) _AdsRegisteredSymbol __adsRegisteredSymbol_##T_NAME(T_CLASS::ads_ ##T_NAME,ACRX_T(#T_NAME),T_REGISTERFUNCTION); \
    const bool __adsRegisteredFunction_##T_CLASS##T_NAME = __adsRegisteredSymbol_##T_NAME.registerFunction();
#elif defined(_ZRX)
#define DOS_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
    __declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_##name = { _RXST(#name), classname::ads_ ##name, regFunc, -1 } ; \
    extern "C" __declspec(allocate("ZDSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_##name = &__ZdsSymbolMap_##name ; \
    ZCED_ZDSSYMBOL_ENTRY_PRAGMA(name)
#else
#define DOS_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
    __declspec(selectany) _ADSSYMBOL_ENTRY __AdsSymbolMap_##name = { _RXST(#name), classname::ads_ ##name, regFunc, (UINT)-1 } ; \
    extern "C" __declspec(allocate("ADSSYMBOL$__m")) __declspec(selectany) _ADSSYMBOL_ENTRY* const __pAdsSymbolMap_##name = &__AdsSymbolMap_##name ; \
    ACED_ADSSYMBOL_ENTRY_PRAGMA(name)
#endif

// Drive functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_chkdsk, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_drive, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_drivep, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_drives, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_drivetype, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_filesys, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_format, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_label, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_serialno, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hdserialno, false)

// Path functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_absolutepath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_compactpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fullpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathrelative, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathroot, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathsameroot, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathunc, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathurl, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_longpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_makepath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_path, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pathbackslash, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pathextension, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pathquotes, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_relativepath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_shortpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_splitpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_uncpath, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathnetwork, false) // new
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispathslow, false)    // new
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_localpath, false)     // new
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_expandenv, false)     // new
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_unexpandenv, false)   // new

// Folder functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_chdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_deltree, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dirattrib, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dirp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dirtree, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_mkdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pwdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rendir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rmdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_specialdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_subdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sysdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tempdir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_windir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dirsize, false)

// File functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_attrib, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_copy, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_delete, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dir, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dos2unix, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_encrypt, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_file, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fileex, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_filedate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_filep, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_filesize, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_find, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getfiled, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getfilem, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getfilenav, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_mergefiles, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_move, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_openp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_recent, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_recycle, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rename, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_search, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tempfile, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_touch, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_unix2dos, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fileinfo, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_emptyrecycle, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_filecrc, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fileowner, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_readtextfile, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_writetextfile, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_readdelimitedfile, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isfilename, false)

// Print functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getprn, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_printers, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_setprn, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_spool, false)

// Configuration functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getini, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regaddkey, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regdelkey, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regdelval, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regenumkeys, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regenumnames, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_reggetint, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_reggetstr, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_reggettype, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regsetint, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regsetstr, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_setini, false)

// DEPRECATED REGISTRY FUNCTIONS
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regdel, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regget, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regkey, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_regset, false)

// Process functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_command, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_execute, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_exewait, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_processes, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_shellexe, false)

// Dialog functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_checklist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_combolist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_editbox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_editlist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getcancel, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getcolor, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getint, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getpassword, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getprogress, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getreal, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getstring, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_htmlbox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_htmlboxex, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_listbox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_msgbox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_msgboxex, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_multilist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_popupmenu, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_proplist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_splash, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_waitcursor, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_linetypebox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_traywnd, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_msgboxchk, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getdate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_orderlist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_duallist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_htmldialog, false)
//DOS_ADSSYMBOL_ENTRY_AUTO( CDOSLibApp, dos_notepad, false )

// String functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strcase, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strcat, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strchar, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strcompare, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strdelete, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strexcluding, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strfind, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strfindoneof, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strincluding, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strinsert, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strischar, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strleft, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strlength, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strmatch, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strmid, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strremove, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strreplace, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strreverse, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strreversefind, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strright, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strtokens, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strtrim, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strtrimleft, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strtrimright, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strformat, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strformatnumber, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strformatcurrency, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strformatdate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strsort, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strcmplogical, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strcull, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_strregexp, false)

// AutoCAD functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_acadmem, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_acitorgb, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_arxlist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_closeall, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cmdline, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_curvearea, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dwgpreview, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dwgver, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_extractpreview, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isbreak, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_istextscr, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isvlide, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_layerlistbox, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_lisplist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_massprops, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_progbar, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rgbtoaci, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_saveall, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_show, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_xreflist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hideobjects, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_showobjects, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rgbtogray, false) // new
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rgbtohls, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hlstorgb, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isinsidecurve, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_plinecentroid, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hatcharea, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cmdargs, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_getsecret, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cleardbmod, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_summaryinfo, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_custominfo, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_olelist, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_imagelist, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_purgexdata, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_proxycount, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_proxyclean, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_plinewinding, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_acadname, false);
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_slblist, false);

// DOSLib functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_about, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_demandload, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_help, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_version, false)

// System functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_adminp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_beep, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_capslock, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cdate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_clipboard, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_computer, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_date, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_guidgen, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hostname, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_htmlhelp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ispoweruser, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ipaddress, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_macaddress, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_mem, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_numlock, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pause, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_random, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_scrolllock, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sortlist, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_speaker, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_time, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_username, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ver, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_wav, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_winhelp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_iswin64, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isacad64, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tickcount, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_nicinfo, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_printscrn, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_asynckeystate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_systemmetrics, false)

// Math functions
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_abs, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_acos, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_acosh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_asin, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_asinh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_atan, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_atan2, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_atanh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_ceil, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_clamp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cos, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cosh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_div, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dtr, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_e, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_exp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_floor, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fmod, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_hypot, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_interp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_log, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_log10, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_log2, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_max, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_mean, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_median, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_min, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_mode, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_modf, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_moment, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_normalize, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_parameterize, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pi, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_pow, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_range, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_round, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_rtd, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sin, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sinh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sqrt, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sum, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_difference, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_product, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_quotient, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tan, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tanh, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_trunc, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_gcd, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_lcm, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_fact, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_equal, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sign, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_chgsign, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_copysign, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_sortnumbers, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cullnumbers, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_permute, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_vector, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_unitize, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_length, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_negate, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_crossproduct, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_dotproduct, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_wedgeproduct, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tripleproduct, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_perpendicularto, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_zerop, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_tinyp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_unitp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_parallelp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_perpendicularp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_orthogonalp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_orthonormalp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_righthandp, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_anglebetween, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_scale, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_quadratic, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_phi, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_cbrt, false)
DOS_ADSSYMBOL_ENTRY_AUTO(CDOSLibApp, dos_isprime, false)

////////////////////////////////////////////////////////////////
// dos_about
int CDOSLibApp::ads_dos_about()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;
  CDosAboutDialog dlg(acedGetAcadFrame());
  dlg.DoModal();
  acedRetNil();
  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_version
int CDOSLibApp::ads_dos_version()
{
  CAdsRetList result;
  result.Add(DOSLibApp().MajorVersion());
  result.Add(DOSLibApp().MinorVersion());
  result.Add(DOSLibApp().ServiceRelease());
  acedRetList(result);
  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_demandload
int CDOSLibApp::ads_dos_demandload()
{
  CAdsArgs args;
  if (args.IsEmpty())
  {
    if (DOSLibApp().IsDemandLoadOnStartup())
      acedRetT();
    else
      acedRetNil();
  }
  else if (args.IsNil())
  {
    DOSLibApp().SetDemandLoadOnRequest();
    acedRetNil();
  }
  else
  {
    DOSLibApp().SetDemandLoadOnStartup();
    acedRetT();
  }
  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_help
int CDOSLibApp::ads_dos_help()
{
  CString strPath;
  if (!DOSLibApp().GetHelpPath(strPath))
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsArgs args;

  if (args.IsEmpty())
  {
    ::HtmlHelp(adsw_acadMainWnd(), strPath, HH_DISPLAY_TOC, 0);
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
