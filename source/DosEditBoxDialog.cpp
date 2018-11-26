/////////////////////////////////////////////////////////////////////////////
// DosEditBoxDialog.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosEditBoxDialog.h"

CDosEditBoxDialog::CDosEditBoxDialog(CWnd* pParent)
  : CAcUiDialog(CDosEditBoxDialog::IDD, pParent)
{
}

void CDosEditBoxDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_OK);
  DDX_Control(pDX, IDC_EDIT, m_Edit);
}

BEGIN_MESSAGE_MAP(CDosEditBoxDialog, CAcUiDialog)
  ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
  ON_BN_CLICKED(IDC_IMPORT, OnImport)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosEditBoxDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:EditBox");

  CAcUiDialog::OnInitDialog();

  StretchControlX(IDC_PROMPT, 100);
  StretchControlXY(IDC_EDIT, 100, 100);
  MoveControlX(IDOK, 100);
  MoveControlX(IDCANCEL, 100);
  MoveControlX(IDC_IMPORT, 100);

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  m_Edit.SetWindowText(m_String);
  m_Edit.SetSel(m_String.GetLength(), m_String.GetLength());

  OnChangeEdit();
  m_OK.SetFocus();

  return TRUE;
}

void CDosEditBoxDialog::OnOK()
{
  m_Edit.GetWindowText(m_String);
  CAcUiDialog::OnOK();
}

void CDosEditBoxDialog::OnChangeEdit()
{
  CString str;
  m_Edit.GetWindowText(str);
  if (str.GetLength() == 0)
    m_OK.EnableWindow(FALSE);
  else
    m_OK.EnableWindow(TRUE);
}

void CDosEditBoxDialog::OnImport()
{
  UpdateData();

  int nStartChar = 0, nEndChar = 0;
  m_Edit.GetSel(nStartChar, nEndChar);

  TCHAR tchCurrentDirectory[_MAX_PATH];
  DWORD dwChars = GetCurrentDirectory(_MAX_PATH, tchCurrentDirectory);

  CFileDialog dlg(
    TRUE,
    L"txt",
    0,
    OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
    L"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"
    );

  dlg.m_ofn.nFilterIndex = 1;
  dlg.m_ofn.lpstrInitialDir = tchCurrentDirectory;

  if (dlg.DoModal() == IDCANCEL)
    return;

  // Read the text from file
  CString str;
  CStdioFile* pFile = 0;

  TRY
  {
    pFile = new CStdioFile(dlg.GetPathName(), CFile::modeRead | CFile::shareDenyNone | CFile::typeText);

    CString strLine;
    while (pFile->ReadString(strLine))
      str += strLine + L"\r\n";

    pFile->Close();

    delete pFile;
    pFile = 0;
  }

    CATCH(CFileException, e)
  {
    e->ReportError();
    if (pFile != NULL)
    {
      pFile->Close();

      delete pFile;
      pFile = 0;
    }
    return;
  }

  AND_CATCH(CMemoryException, pEx)
    AfxAbort();

  END_CATCH

    // Update edit control
    m_Edit.SetFocus();
  m_Edit.SetSel(nStartChar, nEndChar);
  m_Edit.ReplaceSel(str, TRUE);

  UpdateData();
}

BOOL CDosEditBoxDialog::PreTranslateMessage(MSG* pMsg)
{
  CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT);
  if ((CEdit*)this->GetFocus() == pEdit)
  {
    if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB))
    {
      int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
      m_Edit.SetSel(nPos, nPos);
      m_Edit.ReplaceSel(L"\t", TRUE);
      return TRUE;
    }
  }

  return CAcUiDialog::PreTranslateMessage(pMsg);
}

LRESULT CDosEditBoxDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
