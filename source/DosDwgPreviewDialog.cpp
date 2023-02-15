/////////////////////////////////////////////////////////////////////////////
// DosDwgPreviewDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosDwgPreviewDialog.h"
#include "DosFolderDialog.h"

BEGIN_MESSAGE_MAP(CDosDwgPreviewCtrl, CStatic)
  ON_WM_PAINT()
END_MESSAGE_MAP()

void CDosDwgPreviewCtrl::OnPaint()
{
  CRect rc, rcframe;
  GetWindowRect(rcframe);
  GetClientRect(rc);
  ClientToScreen(rc);
  int cx = rcframe.Width() - rc.Width();
  int cy = rcframe.Height() - rc.Height();
  ScreenToClient(rc);
  rc.DeflateRect(cx / 2, cy / 2);
  GetDC()->FillRect(rc, &CBrush(GetSysColor(COLOR_BTNFACE)));
  CStatic::OnPaint();
  acdbDisplayPreviewFromDwg(m_strFilename, m_hWnd, (const Adesk::UInt32*)&m_nColor);
}


CDosDwgPreviewDialog::CDosDwgPreviewDialog(CWnd* pParent)
  : CAcUiDialog(CDosDwgPreviewDialog::IDD, pParent)
{
  m_Title = L"";
  m_Prompt = L"";
}

void CDosDwgPreviewDialog::DoDataExchange(CDataExchange* pDX)
{
  CAcUiDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST, m_List);
  DDX_Control(pDX, IDC_STATIC_FRAME, m_DwgPreview);
}

BEGIN_MESSAGE_MAP(CDosDwgPreviewDialog, CAcUiDialog)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
  ON_WM_PAINT()
  ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
  ON_WM_SIZE()
  ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CDosDwgPreviewDialog::OnBrowse()
{
  CDosFolderDialog dlg(adsw_acadMainWnd());
  dlg.SetDialogTitle(L"Select a folder containing drawing files");
  dlg.SetDialogFolder(m_Prompt);
  if (dlg.DoModal() == IDOK)
  {
    m_Prompt = dlg.GetFolderName();
    SetDlgItemText(IDC_PROMPT, m_Prompt);
    FillListBox();
  }
}

void CDosDwgPreviewDialog::OnPaint()
{
  CPaintDC dc(this);
}

BOOL CDosDwgPreviewDialog::OnInitDialog()
{
  SetDialogName(L"DOSLib:DwgPreview");

  CAcUiDialog::OnInitDialog();

  DLGCTLINFO dcDlgCtls[] = {
    {IDC_PROMPT, ELASTICX, 100},
    {IDC_LIST, ELASTICX, 50},
    {IDC_LIST, ELASTICY, 100},
    {IDC_STATIC_FRAME, ELASTICX | MOVEX, 50},
    {IDC_STATIC_FRAME, ELASTICY, 100},
    {IDOK, MOVEX, 50},
    {IDOK, MOVEY, 100},
    {IDCANCEL, MOVEX, 50},
    {IDCANCEL, MOVEY, 100},
    {IDC_BROWSE, MOVEX, 50},
    {IDC_BROWSE, MOVEY, 100},
  };

  SetControlProperty(dcDlgCtls, sizeof(dcDlgCtls) / sizeof(DLGCTLINFO));

  SetWindowText(m_Title);
  SetDlgItemText(IDC_PROMPT, m_Prompt);

  FillListBox();

  return TRUE;
}

void CDosDwgPreviewDialog::FillListBox()
{
  m_List.ResetContent();

  if (m_StrArray.GetCount() > 0)
  {
    int i;
    for (i = 0; i < m_StrArray.GetCount(); i++)
      m_List.AddString(m_StrArray[i]);
    m_List.SetCurSel(0);
    OnSelchangeList();
    GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
    return;
  }

  CString str = m_Prompt;
  str += L"\\*.dwg";

  CFileFind finder;
  BOOL bWorking = finder.FindFile(str);
  if (!bWorking)
    return;

  while (bWorking)
  {
    bWorking = finder.FindNextFile();
    if (finder.IsDots())
      continue;
    if (finder.IsDirectory())
      continue;
    m_List.AddString(finder.GetFileName());
  }

  m_List.SetCurSel(0);
  OnSelchangeList();
}

void CDosDwgPreviewDialog::OnOK()
{
  m_Prompt = m_DwgPreview.m_strFilename;
  CAcUiDialog::OnOK();
}

void CDosDwgPreviewDialog::OnSelchangeList()
{
  CString cFile;
  int nSel = m_List.GetCurSel();
  m_List.GetText(nSel, cFile);

  if (m_StrArray.GetCount() > 0)
    m_DwgPreview.m_strFilename = cFile;
  else
    m_DwgPreview.m_strFilename.Format(L"%s\\%s", m_Prompt, cFile);

  m_DwgPreview.m_nColor = m_Color;
  m_DwgPreview.Invalidate();
}

void CDosDwgPreviewDialog::OnSize(UINT nType, int cx, int cy)
{
  CAcUiDialog::OnSize(nType, cx, cy);
  m_DwgPreview.Invalidate();
}

void CDosDwgPreviewDialog::OnDblclkList()
{
  OnOK();
}

LRESULT CDosDwgPreviewDialog::OnNcHitTest(CPoint point)
{
  LRESULT hit = CAcUiDialog::OnNcHitTest(point);
  if (hit == HTCLIENT)
    return HTCAPTION;
  return hit;
}
