/////////////////////////////////////////////////////////////////////////////
// DosGetRealDialog.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetRealDialog.h"

CDosGetRealDialog::CDosGetRealDialog(CWnd* pParent)
  : CAcUiDialog(CDosGetRealDialog::IDD, pParent)
{
  m_sTitle = L"";
  m_sPrompt = L"";
  m_dblValue = 0.0;
  m_bIsEmpty = TRUE;
  m_bAllowZero = TRUE;
  m_bAllowEmpty = TRUE;
  m_bAllowNeg = TRUE;
  m_bMin = FALSE;
  m_bMax = FALSE;
  m_dblMin = 0.0;
  m_dblMax = 0.0;
}

void CDosGetRealDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_EDIT, m_Edit);
}

BEGIN_MESSAGE_MAP(CDosGetRealDialog, CAcUiDialog)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetRealDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:GetReal");

  CAcUiDialog::OnInitDialog();

  LockDialogHeight();
  StretchControlX(IDC_EDIT, 100);

  SetWindowText(m_sTitle);
  SetDlgItemText(IDC_PROMPT, m_sPrompt);
  if (!m_bAllowEmpty || !m_bIsEmpty)
  {
    CString s;
    s.Format(L"%g", m_dblValue);
    m_Edit.SetWindowText(s);
  }

  m_Edit.SetFocus();
  OnChangeEdit();

  return TRUE;
}

void CDosGetRealDialog::OnOK()
{
  CString s, str;
  BOOL bError = FALSE;

  m_Edit.GetWindowText(s);
  s.TrimLeft();
  s.TrimRight();

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
      ErrorMessageBox(L"Please enter a real number.");
      return;
    }
  }

  double d = DOS_UNSET_VALUE;
  if (!DOS_FloatParse(s, d))
  {
    ErrorMessageBox(L"Please enter a real number.");
    return;
  }

  if (!m_bAllowZero && d == 0.0)
  {
    ErrorMessageBox(L"Please enter a non-zero real number.");
    return;
  }

  if (!m_bAllowNeg && d < 0.0)
  {
    ErrorMessageBox(L"Please enter a positive real number.");
    return;
  }

  if (m_bMin && d < m_dblMin)
  {
    str.Format(L"Please enter a real number greater than or equal to %.f", m_dblMin);
    ErrorMessageBox(str);
    return;
  }

  if (m_bMax && d > m_dblMax)
  {
    str.Format(L"Please enter a real number less than or equal to %.f", m_dblMax);
    ErrorMessageBox(str);
    return;
  }

  m_dblValue = d;
  m_bIsEmpty = FALSE;

  CAcUiDialog::OnOK();
}

void CDosGetRealDialog::ErrorMessageBox(LPCTSTR lpMessage)
{
  MessageBox(lpMessage, DOS_PlatformName(), MB_ICONEXCLAMATION | MB_OK);
  m_Edit.SetFocus();
  m_Edit.SetSel(0, -1);
}

void CDosGetRealDialog::OnChangeEdit()
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

BOOL CDosGetRealDialog::ParseRealValue(LPCTSTR lpString, double* pValue)
{
  if (0 == lpString || 0 == lpString[0])
  {
    ErrorMessageBox(L"Please enter a real number.");
    return FALSE;
  }

  while (*lpString == ' ' || *lpString == '\t')
    lpString++;

  errno = 0;
  wchar_t* lpEndPtr = 0;
  double d = wcstod(lpString, &lpEndPtr);
  if (lpString == lpEndPtr || errno == ERANGE)
  {
    ErrorMessageBox(L"Please enter a real number.");
    return FALSE;
  }

  if (lpEndPtr)
  {
    while (*lpEndPtr == ' ' || *lpEndPtr == '\t')
      lpEndPtr++;

    if (*lpEndPtr != '\0')
    {
      ErrorMessageBox(L"Please enter a real number.");
      return FALSE;
    }
  }

  *pValue = d;
  return TRUE;
}

LRESULT CDosGetRealDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
