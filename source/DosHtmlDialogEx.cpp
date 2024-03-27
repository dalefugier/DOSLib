/////////////////////////////////////////////////////////////////////////////
// DosHtmlDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosHtmlDialogEx.h"

CDosHtmlDialogEx::CDosHtmlDialogEx(const wchar_t* pszFileName)
{
  m_strFileName = pszFileName;
  m_strOptions = L"";
}

CDosHtmlDialogEx::~CDosHtmlDialogEx()
{
}

bool CDosHtmlDialogEx::DoModal()
{
  return DisplayDialog(true);
}

bool CDosHtmlDialogEx::DoModeless()
{
  return DisplayDialog(false);
}

bool CDosHtmlDialogEx::DisplayDialog(bool bModal)
{
  bool rc = false;

  HMODULE hModule = LoadLibrary(L"MSHTML.DLL");
  if (0 == hModule)
    return rc;

  SHOWHTMLDIALOGEXFN* pfn = (SHOWHTMLDIALOGEXFN*)GetProcAddress(hModule, "ShowHTMLDialogEx");
  if (pfn)
  {
    BSTR bstrFileName = m_strFileName.AllocSysString();

    IMoniker* pMoniker = 0;
    CreateURLMoniker(0, bstrFileName, &pMoniker);
    if (pMoniker)
    {
      HWND hWnd = adsw_acadMainWnd();
      DWORD dwFlags = (bModal) ? HTMLDLG_MODAL | HTMLDLG_VERIFY : HTMLDLG_MODELESS | HTMLDLG_VERIFY;

      HRESULT hr = S_OK;
      if (m_strOptions.IsEmpty())
      {
        hr = pfn(hWnd, pMoniker, dwFlags, &m_vaArgs, 0, &m_vaResult);
      }
      else
      {
        BSTR bstrOptions = m_strOptions.AllocSysString();
        hr = pfn(hWnd, pMoniker, dwFlags, &m_vaArgs, bstrOptions, &m_vaResult);
        SysFreeString(bstrOptions);
      }

      pMoniker->Release();

      rc = (SUCCEEDED(hr)) ? true : false;
    }

    SysFreeString(bstrFileName);
  }

  FreeLibrary(hModule);

  return rc;
}

void CDosHtmlDialogEx::SetArguments(const VARIANT& vaArgs)
{
  m_vaArgs = vaArgs;
}

void CDosHtmlDialogEx::SetOptions(const wchar_t* pszOptions)
{
  if (pszOptions)
    m_strOptions = pszOptions;
}

VARIANT CDosHtmlDialogEx::GetResult()
{
  return m_vaResult.Detach();
}
