/////////////////////////////////////////////////////////////////////////////
// DosGetIntDialog.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetIntDialog.h"

CDosGetIntDialog::CDosGetIntDialog(CWnd* pParent)
  : CAcUiDialog(CDosGetIntDialog::IDD, pParent)
{
  m_sTitle = L"";
  m_sPrompt = L"";
  m_nValue = 0;
  m_bIsEmpty = TRUE;
  m_bAllowZero = TRUE;
  m_bAllowEmpty = TRUE;
  m_bAllowNeg = TRUE;
  m_bMin = FALSE;
  m_bMax = FALSE;
  m_nMin = 0;
  m_nMax = 0;
}

void CDosGetIntDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_EDIT, m_Edit);
}

BEGIN_MESSAGE_MAP(CDosGetIntDialog, CAcUiDialog)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetIntDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:GetInt");

  CAcUiDialog::OnInitDialog();

  LockDialogHeight();
  StretchControlX(IDC_EDIT, 100);

  SetWindowText(m_sTitle);
  SetDlgItemText(IDC_PROMPT, m_sPrompt);
  if (!m_bAllowEmpty || !m_bIsEmpty)
  {
    CString s;
    s.Format(L"%d", m_nValue);
    m_Edit.SetWindowText(s);
  }

  m_Edit.SetFocus();
  OnChangeEdit();

  return TRUE;
}

void CDosGetIntDialog::OnOK()
{
  CString s, str;
  BOOL bError = FALSE;

  m_Edit.GetWindowText(s);

  if (s.IsEmpty())
  {
    if (m_bAllowEmpty)
    {
      m_bIsEmpty = TRUE;
      CAcUiDialog::OnOK();
      return;
    }
    else
    {
      ErrorMessageBox(L"Please enter an integer number.");
      return;
    }
  }

  int n = 0;
  if (!DOS_IntegerParse(s, n))
  {
    ErrorMessageBox(L"Please enter an integer number.");
    return;
  }

  if (!m_bAllowZero && n == 0)
  {
    ErrorMessageBox(L"Please enter a non-zero integer number.");
    return;
  }

  if (!m_bAllowNeg && n < 0)
  {
    ErrorMessageBox(L"Please enter a positive integer number.");
    return;
  }

  if (m_bMin && n < m_nMin)
  {
    str.Format(L"Please enter an integer number greater than or equal to %d", m_nMin);
    ErrorMessageBox(str);
    return;
  }

  if (m_bMax && n > m_nMax)
  {
    str.Format(L"Please enter an integer number less than or equal to %d", m_nMax);
    ErrorMessageBox(str);
    return;
  }

  m_nValue = n;
  m_bIsEmpty = FALSE;

  CAcUiDialog::OnOK();
}

void CDosGetIntDialog::ErrorMessageBox(LPCTSTR lpMessage)
{
  MessageBox(lpMessage, DOS_PlatformName(), MB_ICONEXCLAMATION | MB_OK);
  m_Edit.SetFocus();
  m_Edit.SetSel(0, -1);
}

void CDosGetIntDialog::OnChangeEdit()
{
  if (m_bAllowEmpty)
    return;

  CString str;
  m_Edit.GetWindowText(str);
  if (str.GetLength() == 0)
    m_OK.EnableWindow(FALSE);
  else
    m_OK.EnableWindow(TRUE);
}

BOOL CDosGetIntDialog::ParseIntValue(LPCTSTR lpString, int* pValue)
{
  CString str1(lpString);
  str1.TrimLeft();
  str1.TrimRight();

  if (str1.Find('.') != -1)
  {
    ErrorMessageBox(L"Please enter an integer number.");
    return FALSE;
  }

  double d = _wtof(lpString);
  if (d < -32768.0 || d > 32767.0)
  {
    ErrorMessageBox(L"Please enter an integer number between -32768 and 32767.");
    return FALSE;
  }

  int n = (int)_wtof(lpString);

  CString str2;
  str2.Format(L"%d", n);

  if (str1.CompareNoCase(str2))
  {
    ErrorMessageBox(L"Please enter an integer number.");
    return FALSE;
  }

  *pValue = n;
  return TRUE;
}

LRESULT CDosGetIntDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
