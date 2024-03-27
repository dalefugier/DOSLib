/////////////////////////////////////////////////////////////////////////////
// DosInterDragListBox.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosDragListBox.h"

class CDosInterDragListBox : public CDosDragListBox
{
  DECLARE_DYNAMIC(CDosInterDragListBox)

public:
  CDosInterDragListBox();
  virtual ~CDosInterDragListBox();

  virtual	UINT Dragging(CPoint point);
  virtual	void Dropped(CPoint point);
  virtual	void EndDrag();

  void MoveSelectedItems(CDosInterDragListBox* pToList, int nInsertPos);

protected:
  DECLARE_MESSAGE_MAP()

  HWND m_hTargetList; // window handle of target foreign list
  CDosInterDragListBox* ListFromPoint(CPoint point);
  void EraseTarget();
};
