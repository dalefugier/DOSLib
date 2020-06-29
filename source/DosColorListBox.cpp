/////////////////////////////////////////////////////////////////////////////
// DosColorListBox.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosColorListBox.h"

BEGIN_MESSAGE_MAP(CDosColorListBox, CListBox)
END_MESSAGE_MAP()

void CDosColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  // losing focus ?
  if (lpDrawItemStruct->itemID == -1)
  {
    DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);
    return;
  }

  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

  // Get color info from item data
  COLORREF clrNew = (COLORREF)(lpDrawItemStruct->itemData);
  COLORREF clrOld;

  // Is the item selected?
  if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {
    clrOld = pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
    pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_HIGHLIGHT));
  }

  // The item is not selected
  else
  {
    clrOld = pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
    pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_WINDOW));
  }

  // Does the item has focus?
  if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))
  {
    pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
  }

  // Has the item lost focus?
  if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && !(lpDrawItemStruct->itemState & ODS_FOCUS))
  {
    pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
  }

  // Is the item disabled?
  if (lpDrawItemStruct->itemState & ODS_DISABLED)
  {
    clrNew = GetSysColor(COLOR_GRAYTEXT);
    pDC->SetTextColor(clrNew);
  }

  int nBkMode = pDC->SetBkMode(TRANSPARENT);

  CRect rcBlock(lpDrawItemStruct->rcItem);
  CRect rcText(lpDrawItemStruct->rcItem);

  rcBlock.DeflateRect(2, 2);
  rcBlock.right = rcBlock.left + 25;

  rcText.left = rcBlock.right + 8;

  CString strText;
  GetText(lpDrawItemStruct->itemID, strText);
  pDC->DrawText(strText, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

  CBrush brush;
  brush.CreateSolidBrush(clrNew);

  CBrush *pOldBrush = (CBrush*)pDC->SelectObject(&brush);
  CPen* pOldPen = NULL;

  if (lpDrawItemStruct->itemState & ODS_SELECTED)
  {
    pOldPen = (CPen*)pDC->SelectStockObject(WHITE_PEN);
    pDC->Rectangle(&rcBlock);
    rcBlock.DeflateRect(1, 1);
    pDC->SelectStockObject(BLACK_PEN);
    pDC->Rectangle(&rcBlock);
  }
  else
  {
    pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
    rcBlock.DeflateRect(1, 1);
    pDC->Rectangle(&rcBlock);
  }

  pDC->SelectObject(pOldPen);
  pDC->SelectObject(pOldBrush);
  pDC->SetTextColor(clrOld);
  pDC->SetBkMode(nBkMode);
}

int CDosColorListBox::AddString(LPCTSTR lpszItem, COLORREF itemColor)
{
  int nIndex = CListBox::AddString(lpszItem);
  if (nIndex >= 0)
    SetItemData(nIndex, itemColor);
  return nIndex;
}
