/////////////////////////////////////////////////////////////////////////////
// DosEdit.cpp
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDosPrinter

CDosPrinter::CDosPrinter( CDC* dc, const wchar_t* pszHeader )
: m_dc(dc),
  m_header(pszHeader)
{
  ASSERT(m_dc);
  m_vert_margin = 0;
  m_horz_margin = 0;
  m_page_height = 0;
  m_page_width = 0;
  m_y = 0;
  m_line_height = 0;
  m_page_started = false;
  m_doc_started = false;
  m_page_number = 0;
}

CDosPrinter::~CDosPrinter()
{
  if( m_doc_started )
	  EndPrinting();
}

bool CDosPrinter::StartPrinting()
{
  bool rc = false;
  if( m_dc )
  {
    DOCINFO info;
    memset( &info, 0, sizeof(DOCINFO) );
    info.cbSize = sizeof(DOCINFO);
    info.lpszDocName = AfxGetAppName();

    SetPrinterFont();

    m_dc->StartDoc( &info );
    m_doc_started = true;

    TEXTMETRIC tm;
    memset( &tm, 0, sizeof(TEXTMETRIC) );

    if( m_dc->GetTextMetrics(&tm) )
    {
      m_line_height = tm.tmHeight + tm.tmInternalLeading;
      m_vert_margin = m_dc->GetDeviceCaps( LOGPIXELSY ) / 2;
      m_horz_margin = m_dc->GetDeviceCaps( LOGPIXELSX ) / 2;
      m_page_height = m_dc->GetDeviceCaps( VERTRES );
      m_page_width = m_dc->GetDeviceCaps( HORZRES );
      m_y = m_vert_margin;
      rc = true;
    }
  }
  return rc;
}

void CDosPrinter::PrintLine( const wchar_t* pszText )
{
  CString str( pszText );
  if( !m_page_started || m_y > m_page_height - m_vert_margin )
  {
    if( m_page_started )
      m_dc->EndPage();
    m_dc->StartPage();
    m_page_number++;
    m_y = m_vert_margin;
    m_page_started = true;
    PageHeading();
  }
  m_dc->TextOut( m_horz_margin, m_y, str );
  m_y += m_line_height;
}

void CDosPrinter::EndPrinting()
{
  if( m_dc )
  {
    m_dc->EndPage();
    m_dc->EndDoc();
    m_doc_started = false;
  }
}

void CDosPrinter::SetPrinterFont()
{
  if( m_dc )
  {
    CFont font;
    if( font.CreateStockObject(SYSTEM_FIXED_FONT) )
      m_dc->SelectObject( font );
  }
}

int CDosPrinter::GetPageNumber() const
{ 
  return m_page_number; 
}

void CDosPrinter::PageHeading()
{
  if( m_dc )
  {
    CString str( m_header.IsEmpty() ? AfxGetAppName() : m_header );
    int heading_y = m_vert_margin - (3 * m_line_height) / 2;
    int line_y = m_vert_margin - m_line_height / 2;
    int saved = m_dc->SaveDC();
    m_dc->SetTextColor( RGB(0,0,0) );
    CPen pen( PS_SOLID, m_dc->GetDeviceCaps(LOGPIXELSX) / 100, RGB(255,0,0) );
    m_dc->SelectObject( pen );
    m_dc->TextOut( m_horz_margin, heading_y, str );
    str.Format( L"%d", m_page_number );
    int width = m_dc->GetTextExtent(str).cx;
    m_dc->TextOut( m_page_width - m_horz_margin - width, heading_y, str );
    m_dc->MoveTo( m_horz_margin, line_y );
    m_dc->LineTo( m_page_width - m_horz_margin, line_y );
    m_dc->RestoreDC( saved );
  }
}

/////////////////////////////////////////////////////////////////////////////
// CDosNumberStatic

UINT urm_SELECTLINE = ::RegisterWindowMessage( L"_LINE_NUMBER_EDIT_SELECTLINE_" );

CDosNumberStatic::CDosNumberStatic()
{
	m_bgcol = GetSysColor(COLOR_BTNFACE);
	m_fgcol = GetSysColor(COLOR_BTNTEXT);
  m_format = L"%d";
  m_topmargin = 0;
  m_topline = 0;
  m_bottomline = 0;
}

CDosNumberStatic::~CDosNumberStatic()
{
}

BEGIN_MESSAGE_MAP(CDosNumberStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CDosNumberStatic::OnPaint() 
{
	CPaintDC dcPaint( this );

	CRect rect;
	GetClientRect( &rect );

	// We double buffer the drawing - preparing the memory CDC
	CDC dc;
	dc.CreateCompatibleDC( &dcPaint );
	int saved = dc.SaveDC();

	// Create GDI and select objects
	CBitmap bmp;
	bmp.CreateCompatibleBitmap( &dcPaint, rect.Width(), rect.Height() );

  CPen pen;
	pen.CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW) /*m_fgcol*/ );

	dc.SelectObject( &bmp );
	dc.SelectObject( &pen );

	// Painting the background
	dc.FillSolidRect( &rect, m_bgcol );
	dc.MoveTo( rect.right - 1, 0 );
	dc.LineTo( rect.right - 1, rect.bottom );

	// Setting other attributes
	dc.SetTextColor( m_fgcol );
	dc.SetBkColor( m_bgcol );
	dc.SelectObject( GetParent()->GetFont() );

	// Output the line numbers
	if( m_bottomline )
	{
		int t, lineheight = dc.GetTextExtent(L"0").cy;
		for( t = m_topline; t < m_bottomline; t++ )
		{
			CString str;
			str.Format( m_format, t + 1 );
			int topposition = m_topmargin + lineheight * ( t - m_topline );
			//dc.TextOut( 2, topposition, output );
      CRect rc( 2, topposition, rect.right - 3, topposition + lineheight );
      dc.DrawText( str, rc, DT_RIGHT  );
		}
	}

	dcPaint.BitBlt( 0, 0, rect. right, rect.bottom, &dc, 0, 0, SRCCOPY );
	dc.RestoreDC( saved );
}

BOOL CDosNumberStatic::OnEraseBkgnd( CDC* ) 
{
	return TRUE;
}

void CDosNumberStatic::OnLButtonDown( UINT nFlags, CPoint point )
{
	// Find the line clicked on
	CClientDC	dc( this );
	dc.SelectObject( GetParent()->GetFont() );
	int lineheight = dc.GetTextExtent(L"0").cy;
	int lineno = (int)( (double)point.y/(double)lineheight );

	// Select this line in the edit control
	GetParent()->SendMessage( urm_SELECTLINE, lineno );

	CStatic::OnLButtonDown( nFlags, point );
}

void CDosNumberStatic::SetBgColor( COLORREF col, BOOL bRedraw )
{
	m_bgcol = col;
	if( m_hWnd && bRedraw )
		RedrawWindow();
}

void CDosNumberStatic::SetFgColor( COLORREF col, BOOL bRedraw )
{
	m_fgcol = col;
	if( m_hWnd && bRedraw )
		RedrawWindow();
}

void CDosNumberStatic::SetTopAndBottom( int topline, int bottomline )
{
	m_topline = topline;
	m_bottomline = bottomline;
	if( m_hWnd )
		RedrawWindow();
}

void CDosNumberStatic::SetTopMargin( int topmargin )
{
  m_topmargin = topmargin;
}

void CDosNumberStatic::SetLineNumberFormat( CString format )
{
	m_format = format;
	if( m_hWnd )
		RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CDosEdit

UINT _MsgFindReplace = ::RegisterWindowMessage( FINDMSGSTRING );

/////////////////////////////////////////////////////////////////////////////
// CDosEdit::CInsertTextCommand

CDosEdit::CInsertTextCommand::CInsertTextCommand(CDosEdit* const pEditControl, const CString& text) 
	: CDosEditCommand(pEditControl)
	, m_text(text)
{
	int nEnd = 0;
	pEditControl->GetSel( m_nStart, nEnd );
}

void CDosEdit::CInsertTextCommand::DoUndo()
{
	m_pEditControl->DeleteText(m_nStart, m_text.GetLength());
	m_pEditControl->SetSel(m_nStart, m_nStart);
}

void CDosEdit::CInsertTextCommand::DoExecute()
{
	m_pEditControl->InsertText(m_text, m_nStart);
	int nEnd = m_nStart + m_text.GetLength();
	m_pEditControl->SetSel(nEnd, nEnd);
}

//////////////////////////////////////////////////////////////////////////
// CDosEdit::CDeleteSelectionCommand

CDosEdit::CDeleteSelectionCommand::CDeleteSelectionCommand(CDosEdit* const pEditControl, CursorOnUndo cursorOnUndo) 
	: CDosEditCommand(pEditControl)
	, m_textDeleted(pEditControl->GetSelectionText())
{
	m_pEditControl->GetSel(m_nStart, m_nEnd);
	m_cursorOnUndo = cursorOnUndo;
}

void CDosEdit::CDeleteSelectionCommand::DoUndo()
{
	m_pEditControl->InsertText(m_textDeleted, m_nStart);
	switch (m_cursorOnUndo)
	{
	case Selection:
		m_pEditControl->SetSel(m_nStart, m_nEnd);
		break;
	case End:
		m_pEditControl->SetSel(m_nEnd, m_nEnd);
		break;
	case Start:
		m_pEditControl->SetSel(m_nStart, m_nStart);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CDosEdit::CDeleteSelectionCommand::DoExecute()
{
	m_pEditControl->DeleteText(m_nStart, m_nEnd - m_nStart);
	m_pEditControl->SetSel(m_nStart, m_nStart);
}

//////////////////////////////////////////////////////////////////////////
// CDosEdit::CDeleteCharacterCommand

CDosEdit::CDeleteCharacterCommand::CDeleteCharacterCommand(CDosEdit* pEditControl, bool isBackspace) 
	: CDosEditCommand(pEditControl)
	, m_isBackspace(isBackspace)
{
	ASSERT(m_pEditControl->IsSelectionEmpty());
	m_pEditControl->GetSel(m_nStart, m_nStart);
	CString text;
	m_pEditControl->GetWindowText(text);
	if (m_isBackspace)
		--m_nStart;
	m_charDeleted = text[m_nStart];
}

void CDosEdit::CDeleteCharacterCommand::DoUndo()
{
	m_pEditControl->InsertChar(m_charDeleted, m_nStart);
	if (m_isBackspace)
		m_pEditControl->SetSel(m_nStart + 1, m_nStart + 1);
	else
		m_pEditControl->SetSel(m_nStart, m_nStart);
}

void CDosEdit::CDeleteCharacterCommand::DoExecute()
{
	m_pEditControl->DeleteText(m_nStart, 1);
	m_pEditControl->SetSel(m_nStart, m_nStart);
}

//////////////////////////////////////////////////////////////////////////
// CDosEdit::CReplaceSelectionCommand

CDosEdit::CReplaceSelectionCommand::CReplaceSelectionCommand(CDosEdit* const pEditControl, const CString& text) 
	: CDosEditCommand(pEditControl)
{
	ASSERT(pEditControl->IsSelectionEmpty() == false);
	m_pDeleteCommand = new CDeleteSelectionCommand(pEditControl, CDeleteSelectionCommand::Selection);
	m_pInsertCommand = new CInsertTextCommand(pEditControl, text);
	ASSERT(m_pDeleteCommand != NULL);
	ASSERT(m_pInsertCommand != NULL);
}

CDosEdit::CReplaceSelectionCommand::~CReplaceSelectionCommand()
{
	delete m_pDeleteCommand;
	delete m_pInsertCommand;
}

void CDosEdit::CReplaceSelectionCommand::DoUndo()
{
	m_pInsertCommand->Undo();
	m_pDeleteCommand->Undo();
}

void CDosEdit::CReplaceSelectionCommand::DoExecute()
{
	m_pDeleteCommand->Execute();
	m_pInsertCommand->Execute();
}

//////////////////////////////////////////////////////////////////////////
// CDosEdit::CCommandHistory

CDosEdit::CCommandHistory::CCommandHistory()
{
}

CDosEdit::CCommandHistory::~CCommandHistory()
{
	DestroyUndoCommands();
	DestroyRedoCommands();
}

void CDosEdit::CCommandHistory::AddCommand(CDosEditCommand* pCommand)
{
	m_undoCommands.push(pCommand);
	DestroyRedoCommands();
}

bool CDosEdit::CCommandHistory::Undo()
{
	if (!CanUndo())
		return false;
	CDosEditCommand* pCommand = m_undoCommands.top();
	pCommand->Undo();
	m_undoCommands.pop();
	m_redoCommands.push(pCommand);
	return true;
}

bool CDosEdit::CCommandHistory::Redo()
{
	if (!CanRedo())
		return false;
	CDosEditCommand* pCommand = m_redoCommands.top();
	pCommand->Execute();
	m_redoCommands.pop();
	m_undoCommands.push(pCommand);
	return true;
}

bool CDosEdit::CCommandHistory::CanUndo() const
{
	return m_undoCommands.empty() == false;
}

bool CDosEdit::CCommandHistory::CanRedo() const
{
	return m_redoCommands.empty() == false;
}

void CDosEdit::CCommandHistory::DestroyCommands(CommandStack& stack)
{
	while (!stack.empty())
	{
		CDosEditCommand* pCommand = stack.top();
		delete pCommand;
		stack.pop();
	}
}

void CDosEdit::CCommandHistory::DestroyUndoCommands()
{
	DestroyCommands(m_undoCommands);
}

void CDosEdit::CCommandHistory::DestroyRedoCommands()
{
	DestroyCommands(m_redoCommands);
}

//////////////////////////////////////////////////////////////////////////
// CDosEdit implementation

IMPLEMENT_DYNAMIC( CDosEdit, CEdit )

CDosEdit::CDosEdit()
{
	m_contextMenuShownFirstTime = false;
}

CDosEdit::~CDosEdit()
{
}

void CDosEdit::SetDialog( CDosNotepad* pDialog )
{
  m_pDialog = pDialog;
}

bool CDosEdit::IsMultiLine() const
{
	return ( GetStyle() & ES_MULTILINE ) == ES_MULTILINE;
}

bool CDosEdit::IsSelectionEmpty() const
{
	int nStart = 0, nEnd = 0;
	GetSel( nStart, nEnd );
	return ( nEnd == nStart );
}

int CDosEdit::GetCursorPosition() const
{
	ASSERT( IsSelectionEmpty() );
	return GetSel() & 0xFFFF;
}

CString CDosEdit::GetSelectionText() const 
{
	CString text;
	GetWindowText( text );
	int nStart = 0, nEnd = 0;
	GetSel( nStart, nEnd );
	return text.Mid( nStart, nEnd - nStart );
}

void CDosEdit::ReplaceSel( LPCTSTR lpszNewText, BOOL bCanUndo /*= FALSE*/ )
{
	if( bCanUndo )
		CreateInsertTextCommand( lpszNewText );
	CEdit::ReplaceSel( lpszNewText, bCanUndo );
}

BOOL CDosEdit::Undo()
{
	return m_commandHistory.Undo();
}

BOOL CDosEdit::Redo()
{
	return m_commandHistory.Redo();
}

BOOL CDosEdit::CanUndo() const
{
	return m_commandHistory.CanUndo();
}

BOOL CDosEdit::CanRedo() const
{
	return m_commandHistory.CanRedo();
}

void CDosEdit::InsertText( const CString& textToInsert, int nStart )
{
	ASSERT( nStart <= GetWindowTextLength() );
	CString text;
	GetWindowText( text );
	text.Insert( nStart, textToInsert );
	SetWindowText( text );
}

void CDosEdit::InsertChar( TCHAR charToInsert, int nStart )
{
	ASSERT( nStart <= GetWindowTextLength() );
	CString text;
	GetWindowText( text );
	text.Insert( nStart, charToInsert );
	SetWindowText( text );
}

void CDosEdit::DeleteText( int nStart, int nLen )
{
	ASSERT( nLen > 0 );
	ASSERT( GetWindowTextLength() > 0 );
	CString text;
	GetWindowText( text );
	text.Delete( nStart, nLen );
	SetWindowText( text );
}

BOOL CDosEdit::PreTranslateMessage( MSG* pMsg )
{
  if( pMsg )
  {
	  switch( pMsg->message )
	  {
	  case WM_KEYDOWN:
		  if( PreTranslateKeyDownMessage(pMsg->wParam) == TRUE )
			  return TRUE;
		  break;
	  case WM_SYSCHAR:
		  // Alt + Back
		  if( pMsg->wParam == VK_BACK )
		  {
			  // for single-line Alt + Back generates WM_UNDO message but not for multiline!
			  // Therefore we need to capture this keyboard shortcut for multiline control
			  if( IsMultiLine() )
				  Undo();
			  return TRUE;
		  }
		  break;
    }
	}
	return CEdit::PreTranslateMessage( pMsg );
}

LRESULT CDosEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CHAR:
		{
			wchar_t wChar = static_cast<wchar_t>(wParam);
			int nCount = lParam & 0xFF;
			if( iswprint(wChar) )
			{
				CString newText( wChar, nCount );
				CreateInsertTextCommand( newText );
			}
			// special case for Unicode control characters inserted from the context menu
			else if( nCount == 0 )
			{
				CString newText( wChar );
				CreateInsertTextCommand( newText );
			}
		}
		break;

	case WM_PASTE:
		CreatePasteCommand();
		break;

	case WM_CUT:
	case WM_CLEAR: // delete command from context menu
		if( !IsSelectionEmpty() )
			m_commandHistory.AddCommand( new CDeleteSelectionCommand(this, CDeleteSelectionCommand::Selection) );
		break;

	case WM_UNDO:
		Undo();
		return TRUE; // we did the undo and shouldn't pass the message to base class
	}

	return CEdit::WindowProc( message, wParam, lParam );
}

BEGIN_MESSAGE_MAP(CDosEdit, CEdit)
	ON_WM_CONTEXTMENU()
	ON_WM_ENTERIDLE()
END_MESSAGE_MAP()

void CDosEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_contextMenuShownFirstTime = true;
	CEdit::OnContextMenu( pWnd, point );
}

void CDosEdit::OnEnterIdle( UINT nWhy, CWnd* pWho )
{
	CEdit::OnEnterIdle( nWhy, pWho );
	if( nWhy == MSGF_MENU )
	{
		if( m_contextMenuShownFirstTime )
		{
			m_contextMenuShownFirstTime = false;
			UpdateContextMenuItems( pWho );
		}
	}
}

BOOL CDosEdit::PreTranslateKeyDownMessage( WPARAM wParam )
{
	switch( wParam ) 
	{
	case _T('A'):
		// Ctrl + 'A'
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			SetSel(0, -1);
			return TRUE;
		}
		break;
	case _T('Z'):
		// Ctrl + 'Z'
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			Undo();
			return TRUE;
		}
		break;
	case _T('Y'):
		// Ctrl + 'Y'
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			Redo();
			return TRUE;
		}
		break;
	case VK_BACK:
		return DoBackspace();
	case VK_DELETE:
		return DoDelete();
	}
	return FALSE;
}

void CDosEdit::CreateInsertTextCommand(const CString& newText)
{
	if (IsSelectionEmpty())
		m_commandHistory.AddCommand(new CInsertTextCommand(this, newText));
	else
		m_commandHistory.AddCommand(new CReplaceSelectionCommand(this, newText));
}

BOOL CDosEdit::CreatePasteCommand()
{
	if (!OpenClipboard())
		return FALSE;

#ifdef UNICODE
	HANDLE hglb = ::GetClipboardData(CF_UNICODETEXT);
#else
	HANDLE hglb = ::GetClipboardData(CF_TEXT);
#endif
	if (hglb == NULL)
	{
		CloseClipboard(); 
		return FALSE;
	}
	LPTSTR pText = reinterpret_cast<LPTSTR>(::GlobalLock(hglb));
	if (pText != NULL)
	{
		CreateInsertTextCommand(pText);
		::GlobalUnlock(hglb);
	}
	CloseClipboard(); 
	return TRUE;
}

BOOL CDosEdit::DoDelete()
{
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
	{
		// Ctrl + Delete + Shift
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			DeleteToTheEnd();
		// Ctrl + Delete
		else
			DeleteToTheBeginningOfNextWord();
		return TRUE;
	}
	// simple delete
	if (IsSelectionEmpty() == false)
		m_commandHistory.AddCommand(new CDeleteSelectionCommand(this, CDeleteSelectionCommand::Selection));
	else
	{
		if (GetCursorPosition() < GetWindowTextLength())
			m_commandHistory.AddCommand(new CDeleteCharacterCommand(this, false));
	}
	return FALSE;
}

BOOL CDosEdit::DoBackspace()
{
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) 
	{
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			// Ctrl + Shift + Back
			DeleteFromTheBeginning();
		else
			// Ctrl + Back
			DeleteFromTheBeginningOfWord();
		return TRUE;
	}
	// plain Back
	if (IsSelectionEmpty() == false)
		m_commandHistory.AddCommand(new CDeleteSelectionCommand(this, CDeleteSelectionCommand::Selection));
	else
	{
		if (GetCursorPosition() > 0)
			m_commandHistory.AddCommand(new CDeleteCharacterCommand(this, true));
	}
	return FALSE;
}

// Deletes characters from the start of the content up
// to the end of selection
void CDosEdit::DeleteFromTheBeginning()
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	if (nEnd == 0)
		return;
	SetSel(0, nEnd);
	CDosEditCommand* pCommand = new CDeleteSelectionCommand(this, CDeleteSelectionCommand::End);
	m_commandHistory.AddCommand(pCommand);
	pCommand->Execute();
}

// Deletes characters from the start of the selection 
// to the end of content
void CDosEdit::DeleteToTheEnd()
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	nEnd = GetWindowTextLength();
	if (nStart == nEnd)
		return;
	SetSel(nStart, nEnd);
	CDosEditCommand* pCommand = new CDeleteSelectionCommand(this, CDeleteSelectionCommand::Start);
	m_commandHistory.AddCommand(pCommand);
	pCommand->Execute();
}

// Gets character category
CDosEdit::CharCategory CDosEdit::GetCharCategory(TCHAR ch)
{
	if (::_istspace(ch))
		return enumSpace;
	if (::_istpunct(ch))
		return enumPunctuation;
	return enumAlphaNum;
}

// Extends start of selection to include characters of the same category
void CDosEdit::ExtendSelectionToStartOfCharacterBlock(int nStart, int nEnd) 
{
	ASSERT(nStart > 0);
	ASSERT(nEnd >= nEnd);
	CString text;
	GetWindowText(text);
	// skip trailing whitespaces
	while (--nStart > 0)
	{
		if (!_istspace(text[nStart]))
			break;
	}
	// find start of block with the same character category
	CharCategory blockCharCategory = GetCharCategory(text[nStart]);
	while (nStart > 0)
	{
		--nStart;
		if (blockCharCategory != GetCharCategory(text[nStart]))
		{
			++nStart;
			break;
		}
	}
	ASSERT(nStart >= 0 && nEnd >= nEnd);
	SetSel(nStart, nEnd, TRUE);
}

// Extends end of selection to include characters of the same category
void CDosEdit::ExtendSelectionToEndOfCharacterBlock(int nStart, int nEnd) 
{
	CString text;
	GetWindowText(text);
	CharCategory blockCharCategory = GetCharCategory(text[nEnd]);
	int len = text.GetLength();
	while (++nEnd < len) 
	{
		if (blockCharCategory != GetCharCategory(text[nEnd]))
			break;
	}
	// skip trailing whitespaces
	while (nEnd < len) 
	{
		if (!_istspace(text[nEnd]))
			break;
		++nEnd;
	}
	ASSERT(nEnd >= nStart && nEnd <= len);
	SetSel(nStart, nEnd, TRUE);
}

// Deletes characters from the beginning of the block
// consisting of characters of the same category as
// the first character in the selection
void CDosEdit::DeleteFromTheBeginningOfWord()
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	ASSERT(nStart <= nEnd);
	if (nStart == 0)
		return;
	ExtendSelectionToStartOfCharacterBlock(nStart, nEnd);
	CDosEditCommand* pCommand = new CDeleteSelectionCommand(this, CDeleteSelectionCommand::End);
	m_commandHistory.AddCommand(pCommand);
	pCommand->Execute();
}

// Deletes characters to the end of the block
// consisting of characters of the same category as
// the last character in the selection
void CDosEdit::DeleteToTheBeginningOfNextWord()
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	ASSERT(nStart <= nEnd);
	if (GetWindowTextLength() <= nEnd)
		return;
	ExtendSelectionToEndOfCharacterBlock(nStart, nEnd);
	CDosEditCommand* pCommand = new CDeleteSelectionCommand(this, CDeleteSelectionCommand::Start);
	m_commandHistory.AddCommand(pCommand);
	pCommand->Execute();
}

// updates Undo menu item state
void CDosEdit::UpdateContextMenuItems(CWnd* pWnd)
{
	MENUBARINFO mbi = {0};
	mbi.cbSize = sizeof(MENUBARINFO);
	::GetMenuBarInfo(pWnd->m_hWnd, OBJID_CLIENT, 0, &mbi);
	ASSERT(::IsMenu(mbi.hMenu));
	HMENU hMenu = mbi.hMenu;
	if (m_commandHistory.CanUndo()) 
		::EnableMenuItem(hMenu, WM_UNDO, MF_BYCOMMAND | MF_ENABLED);
	else
		::EnableMenuItem(hMenu, WM_UNDO, MF_BYCOMMAND | MF_GRAYED);
}
