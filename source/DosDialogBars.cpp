/////////////////////////////////////////////////////////////////////////////
// DosDialogBars.cpp
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h" 
#include "DosDialogBars.h"

/////////////////////////////////////////////////////////////////////////////
// CDosToolBar

BEGIN_MESSAGE_MAP(CDosToolBar, CToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
                 
CDosToolBar::CDosToolBar()
: m_pToolTip(0)
{
}

CDosToolBar::~CDosToolBar()
{
  if( m_pToolTip )
  {
    m_pToolTip->DestroyWindow();
    delete m_pToolTip;
  }
} 

LRESULT CDosToolBar::OnIdleUpdateCmdUI( WPARAM wParam, LPARAM ) 
{
	if( !m_ButtonStates.GetCount() )
  {
		if( IsWindowVisible() )
    {
			CFrameWnd *pParent = (CFrameWnd*)GetParent();
			if( pParent )
				OnUpdateCmdUI( pParent, (BOOL)wParam );
		}
	}

	if( IsWindowVisible() )
  {
		POSITION pos = m_ButtonStates.GetStartPosition();
		int id;
		BOOL val;

		while( pos != NULL )
    {
			m_ButtonStates.GetNextAssoc( pos, id, val );
			GetToolBarCtrl().EnableButton( id, val );
		}
	}
		
	return 0L;
}

void CDosToolBar::EnableButton( int nID, BOOL bEnable )
{
	GetToolBarCtrl().EnableButton( nID, bEnable );

	if( bEnable )
    m_ButtonStates.RemoveKey( nID );
	else
    m_ButtonStates.SetAt( nID, bEnable );
}
  
void CDosToolBar::AddToolTip()
{
  m_pToolTip = new CToolTipCtrl();
  if( 0 == m_pToolTip )
    return;

  if( !m_pToolTip->Create(this, TTS_ALWAYSTIP | WS_VISIBLE) )
  {
    delete m_pToolTip;
    m_pToolTip = 0;
  }

  SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY );

  RECT rect;
  int num = 0;

  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"New", &rect, ID_FILE_NEW );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Open", &rect, ID_FILE_NEW );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Save", &rect, ID_FILE_SAVE );
  num++; // spacer
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Undo", &rect, ID_EDIT_UNDO );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Redo", &rect, ID_EDIT_REDO );
  num++; // spacer
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Cut", &rect, ID_EDIT_CUT );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Copy", &rect, ID_EDIT_COPY );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Paste", &rect, ID_EDIT_PASTE );
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Clear", &rect, ID_EDIT_CLEAR );
  num++; // spacer
  GetItemRect( num++, &rect );
  m_pToolTip->AddTool( this, L"Find", &rect, ID_EDIT_FIND );

  m_pToolTip->Activate( TRUE );

  EnableToolTips( TRUE );
}

void CDosToolBar::OnMouseMove( UINT nFlags, CPoint point )
{
  if( m_pToolTip && ::IsWindow(m_pToolTip->m_hWnd) )
  {
    const MSG* pMsg = GetCurrentMessage();
    if( pMsg )
    {
      MSG msg;
      ::CopyMemory( &msg, pMsg, sizeof(MSG) );
      m_pToolTip->Activate( TRUE );
      m_pToolTip->RelayEvent( &msg );
    }
  }
  CToolBar::OnMouseMove(nFlags, point);
}

BOOL CDosToolBar::LoadTrueColorToolBar( int nButtonWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled )
{
	if( !SetTrueColorToolBar(TB_SETIMAGELIST, uToolBar, nButtonWidth) )
		return FALSE;
	
	if( uToolBarHot ) 
  {
		if( !SetTrueColorToolBar(TB_SETHOTIMAGELIST, uToolBarHot, nButtonWidth) )
			return FALSE;
	}

	if( uToolBarDisabled ) 
  {
		if( !SetTrueColorToolBar(TB_SETDISABLEDIMAGELIST, uToolBarDisabled, nButtonWidth) )
			return FALSE;
	}

	return TRUE;
}

BOOL CDosToolBar::SetTrueColorToolBar( UINT uToolBarType, UINT uToolBar, int nBtnWidth )
{
	CImageList cImageList;
	CBitmap cBitmap;
	BITMAP bmBitmap;
	
  if( !cBitmap.Attach(LoadImage(AfxFindResourceHandle(MAKEINTRESOURCE(uToolBar), RT_BITMAP), MAKEINTRESOURCE(uToolBar), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_CREATEDIBSECTION)) ||
      !cBitmap.GetBitmap(&bmBitmap)
      )
    return FALSE;

  /*
	if( !cBitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(uToolBar), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_CREATEDIBSECTION)) ||
	    !cBitmap.GetBitmap(&bmBitmap)
      )
		return FALSE;
  */

	CSize cSize( bmBitmap.bmWidth, bmBitmap.bmHeight ); 
	int nNbBtn = cSize.cx/nBtnWidth;
	RGBTRIPLE* rgb = (RGBTRIPLE*)(bmBitmap.bmBits);
	COLORREF rgbMask	= RGB(rgb[0].rgbtRed, rgb[0].rgbtGreen, rgb[0].rgbtBlue);
	
	if( !cImageList.Create(nBtnWidth, cSize.cy, ILC_COLOR24|ILC_MASK, nNbBtn, 0) )
		return FALSE;
	
	if( cImageList.Add(&cBitmap, rgbMask) == -1 )
		return FALSE;

	SendMessage( uToolBarType, 0, (LPARAM)cImageList.m_hImageList );
	cImageList.Detach(); 
	cBitmap.Detach();
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDosStatusBar

BEGIN_MESSAGE_MAP(CDosStatusBar, CStatusBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()
                 
CDosStatusBar::CDosStatusBar()
{
}

CDosStatusBar::~CDosStatusBar()
{
} 

LRESULT CDosStatusBar::OnIdleUpdateCmdUI( WPARAM wParam, LPARAM ) 
{
	if( IsWindowVisible() ) 
	{
		CFrameWnd* pParent = (CFrameWnd*)GetParent();
		if( pParent )
			OnUpdateCmdUI( pParent, (BOOL)wParam );
	}
  return 0L;
}
