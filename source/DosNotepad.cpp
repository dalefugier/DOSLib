/////////////////////////////////////////////////////////////////////////////
// DosNotepad.cpp : implementation file
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosNotepad.h"

IMPLEMENT_DYNAMIC( CDosNotepad, CAcUiDialog )

CDosNotepad::CDosNotepad( CWnd* pParent )
: CAcUiDialog(CDosNotepad::IDD, pParent)
{
	m_bStatusBar = FALSE;
	m_bToolBar = FALSE;
}

CDosNotepad::~CDosNotepad()
{
}

void CDosNotepad::DoDataExchange( CDataExchange* pDX )
{
	CAcUiDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDosNotepad, CAcUiDialog)
	ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString) 
  ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
END_MESSAGE_MAP()

BOOL CDosNotepad::OnInitDialog() 
{
  SetDialogName( L"DOSLib:Notepad" );

  CWnd* pWnd = GetDlgItem( IDC_EDIT );
  ASSERT( pWnd );
  CRect rect;
  pWnd->GetWindowRect( rect );
  pWnd->DestroyWindow();
  pWnd = 0;

  DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_WANTRETURN | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
  m_EditCtrl.Create( dwStyle, rect, this, IDC_EDIT );
  m_EditCtrl.ModifyStyleEx( 0, WS_EX_STATICEDGE ); 
  m_EditCtrl.SetFocus();
  m_EditCtrl.SetWindowText( L"Dale" );

  // Make sure initial control has fixed font
  CFont* font = CFont::FromHandle( (HFONT)::GetStockObject(SYSTEM_FIXED_FONT) );
  if( font )
    m_EditCtrl.SetFont( font );

  m_EditCtrl.SetTabStops( 2 );
  m_EditCtrl.SetDialog( this );

  ::DragAcceptFiles( m_hWnd, TRUE );

  // Load settings from registry
  // LoadProfile();

  UINT indicators[] = { ID_SEPARATOR, /*ID_INDICATOR_TIME,*/ ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_OVR };    
  if( m_StatusBarCtrl.CreateEx(this) )
  {
    m_StatusBarCtrl.SetIndicators( indicators, 5 );
    OnSetMessageString( AFX_IDS_IDLEMESSAGE );
		m_StatusBarCtrl.SetPaneInfo( 0, m_StatusBarCtrl.GetItemID(0), SBPS_STRETCH|SBPS_NOBORDERS, NULL );
		m_bStatusBar = TRUE;
	}

  //if( m_ToolBarCtrl.CreateEx(this, TBSTYLE_FLAT | TB_AUTOSIZE | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_TOP, CRect(3,3,3,3)) )
  //{
  //  m_ToolBarCtrl.LoadToolBar( IDR_NOTEPAD );
  //  m_ToolBarCtrl.LoadTrueColorToolBar( 16, IDR_NOTEPAD, 0, 0 );
  //  m_ToolBarCtrl.AddToolTip();
  //  m_bToolBar = TRUE;
  //}

  SetStatusBarPosition( 0, 0 );

  m_nTimer = SetTimer( 666, 100, NULL );

  // OnInitDialog should be called here AND RepositionBars following
  // to ensure the dialog appears correct when it is displayed.
  CAcUiDialog::OnInitDialog();

	if( m_bStatusBar || m_bToolBar)
    RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDC_EDIT );

	return TRUE;
}

void CDosNotepad::OnPaint() 
{
	if( IsIconic() )
	{
		CPaintDC dc( this );
		SendMessage( WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0 );

		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );

    CRect rect;
		GetClientRect( &rect );
		
    int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon( x, y, m_hIcon );
	}
	else
	{
		CAcUiDialog::OnPaint();
	}
}

HCURSOR CDosNotepad::OnQueryDragIcon() 
{
  return (HCURSOR)GetDialogIcon();
}

void CDosNotepad::OnOK() 
{
	DestroyWindow();
}

void CDosNotepad::SetStatusBarPosition( int nLine, int nColumn )
{
  CString str;
	if( nLine >= 0 )
  {
    if( nColumn >= 0 )
      str.Format( L"Ln %d, Col %d", nLine, nColumn );
	  else
      str.Format( L"Line %d", nLine );
  }
  m_StatusBarCtrl.SetPaneText( 1, str );
}

LRESULT CDosNotepad::OnSetMessageString( WPARAM wParam, LPARAM lParam )
{
  AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UINT nIDMsg = (UINT)wParam;
	
	if( nIDMsg )
  {
    if( m_StatusBarCtrl.m_hWnd )
      m_StatusBarCtrl.SetWindowText( L"Ready"); //RhinoScriptPlugIn().LookupStringItem(nIDMsg) );
	}
  else
  {
		if( m_StatusBarCtrl.m_hWnd )
      m_StatusBarCtrl.SetWindowText( L"" );
	}
	
	UINT nIDLast = m_nIDLastMessage;
	m_nIDLastMessage = nIDMsg;
	m_nIDTracking = nIDMsg;
	return nIDLast;  
}

void CDosNotepad::OnCancel() 
{
	DestroyWindow();
}

void CDosNotepad::KillDialog()
{
  OnCancel();
}

void CDosNotepad::PostNcDestroy() 
{
	CAcUiDialog::PostNcDestroy();
  delete this;
}

LRESULT CDosNotepad::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}