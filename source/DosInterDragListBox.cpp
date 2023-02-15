//////////////////////////////////////////////////////////////////////////////
// DosInterDragListBox.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosInterDragListBox.h"

IMPLEMENT_DYNAMIC(CDosInterDragListBox, CDosDragListBox)

CDosInterDragListBox::CDosInterDragListBox()
{
  m_hTargetList = 0;
}

CDosInterDragListBox::~CDosInterDragListBox()
{
}

BEGIN_MESSAGE_MAP(CDosInterDragListBox, CDosDragListBox)
END_MESSAGE_MAP()

CDosInterDragListBox* CDosInterDragListBox::ListFromPoint(CPoint point)
{
  CWnd* pWnd = WindowFromPoint(point);
  return DYNAMIC_DOWNCAST(CDosInterDragListBox, pWnd);
}

void CDosInterDragListBox::EraseTarget()
{
  if (m_hTargetList)
  {
    CWnd* pTarget = FromHandle(m_hTargetList);
    if (pTarget)
    {
      CDosInterDragListBox* pList = DYNAMIC_DOWNCAST(CDosInterDragListBox, pTarget);
      if (pList)
      {
        pList->EraseInsert();
        pList->EndScroll();
      }
    }
    m_hTargetList = 0;
  }
}

UINT CDosInterDragListBox::Dragging(CPoint point)
{
  UINT retc = 0;
  HWND hTarget = 0;
  CDosInterDragListBox* pList = ListFromPoint(point);
  if (pList == this)
  {
    hTarget = m_hWnd;
    retc = CDosDragListBox::Dragging(point);
  }
  else
  {
    if (pList)
    {
      hTarget = pList->m_hWnd;
      pList->Dragging(point);
    }
    else
    {
      hTarget = 0;
      SetCursor(LoadCursor(0, IDC_NO));
    }
    retc = DL_CURSORSET;
  }

  if (hTarget != m_hTargetList)
  {
    EraseTarget();
    m_hTargetList = hTarget;
  }

  return retc;
}

void CDosInterDragListBox::Dropped(CPoint point)
{
  CDosInterDragListBox *pList = ListFromPoint(point);
  if (pList == this)
  {
    CDosDragListBox::Dropped(point);
  }
  else
  {
    if (pList)
    {
      int	nInsertPos = pList->GetInsertPos(point);
      MoveSelectedItems(pList, nInsertPos);
    }
  }
}

void CDosInterDragListBox::EndDrag()
{
  CDosDragListBox::EndDrag();
  EraseTarget();
}

void CDosInterDragListBox::MoveSelectedItems(CDosInterDragListBox* pToList, int nInsertPos)
{
  if (pToList)
  {
    int	nTop = GetTopIndex();
    CStringArray ItemText;
    CDWordArray ItemData;
    int	nPos = 0;
    GetSelectedItems(ItemText, ItemData, nPos);
    pToList->PasteItems(ItemText, ItemData, nInsertPos);
    SetTopIndex(nTop);
  }
}
