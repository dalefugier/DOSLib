/////////////////////////////////////////////////////////////////////////////
// DosColorDialog.cpp
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosColorDialog.h"

IMPLEMENT_DYNAMIC(CDosColorDialog, CColorDialog)

CDosColorDialog::CDosColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd)
  : CColorDialog(clrInit, dwFlags, pParentWnd)
{
  m_Title = L"Color";
}

BEGIN_MESSAGE_MAP(CDosColorDialog, CColorDialog)
END_MESSAGE_MAP()

BOOL CDosColorDialog::OnInitDialog()
{
  CColorDialog::OnInitDialog();
  SetWindowText(m_Title);
  return TRUE;
}
