/////////////////////////////////////////////////////////////////////////////
// DosEdit.h
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosNotepad;

/////////////////////////////////////////////////////////////////////////////
// CDosPrinter

class CDosPrinter
{
public:
  CDosPrinter( CDC* dc, const wchar_t* pszHeader = 0 );
  virtual ~CDosPrinter();
  bool StartPrinting();
  void EndPrinting();
  void PrintLine( const wchar_t* pszText = 0 );
  virtual void PageHeading();
  virtual void SetPrinterFont();
  int GetPageNumber() const;

protected:
  CDC* m_dc;
  CString m_header;
  int m_vert_margin;
  int m_horz_margin;
  int m_page_height;
  int m_page_width;
  int m_y;
  int m_line_height;
  bool m_page_started;
  bool m_doc_started;
  int m_page_number;
};

/////////////////////////////////////////////////////////////////////////////
// CDosNumberStatic window

class CDosNumberStatic : public CStatic
{
public:
	CDosNumberStatic();
	virtual ~CDosNumberStatic();

public:
	void SetFgColor( COLORREF color, BOOL bbRedraw );
	void SetBgColor( COLORREF color, BOOL bRedraw );
	void SetTopAndBottom( int topline, int bottomline );
	void SetTopMargin( int topmargin );
	void SetLineNumberFormat( CString format );

protected:
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_fgcol;
	COLORREF m_bgcol;
	CString m_format;
	int m_topmargin;	// Current top margin
	int m_topline;		// Current top line number
	int m_bottomline;	// Current bottom line number
};

/////////////////////////////////////////////////////////////////////////////
// CDosEdit

class CDosEdit : public CEdit
{
	// abstract base class
	class CDosEditCommand
	{
	public:
		CDosEditCommand( CDosEdit* const pEditControl ) : m_pEditControl(pEditControl) {}
		virtual ~CDosEditCommand() {}
		void Undo() { DoUndo(); }
		void Execute() { DoExecute(); }
	protected:
		virtual void DoUndo() = 0;
		virtual void DoExecute() = 0;
		CDosEdit* const m_pEditControl;
	};

	class CInsertTextCommand : public CDosEditCommand
	{
	public:
		CInsertTextCommand( CDosEdit* const pEditControl, const CString& text );
	protected:
		virtual void DoUndo();
		virtual void DoExecute();
	private:
		CString m_text;
		int m_nStart;
	};

	class CDeleteSelectionCommand : public CDosEditCommand
	{
	public:
		enum CursorOnUndo { Start, End, Selection };
		CDeleteSelectionCommand( CDosEdit* const pEditControl, CursorOnUndo cursorOnUndo );
	protected:
		virtual void DoUndo();
		virtual void DoExecute();
	private:
		int m_nStart;
		int m_nEnd;
		CString m_textDeleted;
		CursorOnUndo m_cursorOnUndo;
	};

	class CDeleteCharacterCommand : public CDosEditCommand
	{
	public:
		CDeleteCharacterCommand( CDosEdit* pEditControl, bool isBackspace );
	protected:
		virtual void DoUndo();
		virtual void DoExecute();
	private:
		TCHAR m_charDeleted;
		int m_nStart;
		bool m_isBackspace;
	};

	class CReplaceSelectionCommand : public CDosEditCommand
	{
	public:
		CReplaceSelectionCommand( CDosEdit* const pEditControl, const CString& text );
		virtual ~CReplaceSelectionCommand();
	protected:
		virtual void DoUndo();
		virtual void DoExecute();
	private:
		CDeleteSelectionCommand* m_pDeleteCommand;
		CInsertTextCommand* m_pInsertCommand;
	};

	// command history
	class CCommandHistory
	{
	public:
		CCommandHistory();
		~CCommandHistory();

		void AddCommand( CDosEditCommand* pCommand );
		bool Undo();
		bool Redo();
		bool CanUndo() const;
		bool CanRedo() const;

	private:
		typedef std::stack<CDosEditCommand*> CommandStack;

		CommandStack m_undoCommands;
		CommandStack m_redoCommands;

		void DestroyCommands( CommandStack& stack );
		void DestroyUndoCommands();
		void DestroyRedoCommands();
	};

	// CDosEdit definition

	DECLARE_DYNAMIC( CDosEdit )

public:
	CDosEdit();
	virtual ~CDosEdit();

public:
  void SetDialog( CDosNotepad* pDialog );

	bool IsMultiLine() const;
	bool IsSelectionEmpty() const;
	int GetCursorPosition() const;
	CString GetSelectionText() const;

	void ReplaceSel( LPCTSTR lpszNewText, BOOL bCanUndo = FALSE );

	BOOL Undo();
	BOOL Redo();
	BOOL CanUndo() const;
	BOOL CanRedo() const;

	void InsertText( const CString& textToInsert, int nStart );
	void InsertChar( TCHAR charToInsert, int nStart );
	void DeleteText( int nStart, int nLen );

public:
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);

private:
	void CreateInsertTextCommand(const CString& newText);
	BOOL PreTranslateKeyDownMessage(WPARAM wParam);

	BOOL CreatePasteCommand();
	BOOL DoDelete();
	BOOL DoBackspace();

	enum CharCategory
	{
		enumSpace,
		enumPunctuation,
		enumAlphaNum
	};

	void DeleteFromTheBeginning();
	void DeleteToTheEnd();

	CharCategory GetCharCategory(TCHAR ch);
	void ExtendSelectionToStartOfCharacterBlock(int nStart, int nEnd);
	void ExtendSelectionToEndOfCharacterBlock(int nStart, int nEnd);
	void DeleteFromTheBeginningOfWord();
	void DeleteToTheBeginningOfNextWord();

	void UpdateContextMenuItems(CWnd* pWnd);

	CCommandHistory m_commandHistory;
	bool m_contextMenuShownFirstTime;

private:
  CDosNotepad* m_pDialog;

};
