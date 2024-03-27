/////////////////////////////////////////////////////////////////////////////
// DosHtmlDialogEx.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosHtmlDialogEx
{
public:
  CDosHtmlDialogEx(const wchar_t* pszFileName);
  virtual ~CDosHtmlDialogEx();

  void SetArguments(const VARIANT& vaArgs);
  void SetOptions(const wchar_t* pszOptions);

  bool DoModal();
  bool DoModeless();

  VARIANT GetResult();

protected:
  bool DisplayDialog(bool bModal);

protected:
  COleVariant m_vaArgs;
  COleVariant m_vaResult;
  CString m_strFileName;
  CString m_strOptions;
};
