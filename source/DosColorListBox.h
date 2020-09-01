/////////////////////////////////////////////////////////////////////////////
// DosColorListBox.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosColorListBox : public CListBox
{
public:
  CDosColorListBox() {}
  virtual ~CDosColorListBox() {}
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  int AddString(LPCTSTR lpszItem, COLORREF itemColor);

protected:
  DECLARE_MESSAGE_MAP()
};
