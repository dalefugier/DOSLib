/////////////////////////////////////////////////////////////////////////////
// DosMessageBox.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosMessageBox
{
public:
  CDosMessageBox()
  {
    m_bTimeOut = FALSE;
    m_nResult = 0;
    memset(m_szTimeOutCaption, 0, sizeof(m_szTimeOutCaption) / sizeof(TCHAR));
  };

  CDosMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, UINT uTimeOut, BOOL bShowTimeOut = TRUE)
  {
    m_hWnd = hWnd;
    m_lpText = lpText;
    m_lpCaption = lpCaption;
    m_uType = uType;
    m_uTimeOut = uTimeOut;
    m_bTimeOut = FALSE;
    m_nResult = 0;
    m_bShowTimeOut = bShowTimeOut;
    memset(m_szTimeOutCaption, 0, sizeof(m_szTimeOutCaption) / sizeof(TCHAR));
  };

  virtual ~CDosMessageBox() {}

private:
  LPCTSTR	m_lpCaption;	          // Original caption
  LPCTSTR m_lpText;		            // Original text
  UINT	  m_uType;		            // MessageBox type ( MB_OK ..... )
  HWND	  m_hWnd;			            // Descriptor of parent window
  UINT	  m_uTimeOut;		          // Timeout in milliseconds
  BOOL	  m_bTimeOut;		          // Is timeout reached
  BOOL	  m_bShowTimeOut;	        // Flag to show timeout in MessageBox caption
  int		  m_nResult;		          // Result of MessageBox
  TCHAR	  m_szTimeOutCaption[80]; // Real MessageBox caption

private:
  int	TimedOutMessageBox();

public:
  int DoMessageBox();
  int DoMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, UINT uTimeOut, BOOL bShowTimeOut = TRUE);

private:
  static DWORD WINAPI ThreadFunction(LPVOID lpParameter);
};

