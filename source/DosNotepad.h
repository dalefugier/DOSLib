/////////////////////////////////////////////////////////////////////////////
// DosNotepad.h
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#include "DosEdit.h"
#include "DosDialogBars.h"
#include "Resource.h"

#define MAX_MRU_SIZE 10

class CDosNotepad : public CAcUiDialog
{
	DECLARE_DYNAMIC( CDosNotepad )

public:
	CDosNotepad( CWnd* pParent );
	virtual ~CDosNotepad();

  // Dialog Data
	enum { IDD = IDD_DOSNOTEPAD };

public:
  void SetStatusBarPosition( int line = -1, int column = -1 );
  void KillDialog();

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSetMessageString( WPARAM wParam, LPARAM lParam = 0L );
  virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg LRESULT OnAcadKeepFocus( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

protected:
  CDosEdit m_EditCtrl;            // Edit control
  CDosToolBar m_ToolBarCtrl;      // Toolbar control
  CDosStatusBar m_StatusBarCtrl;  // Status bar control
  BOOL m_bStatusBar;              // Status bar visibility
	BOOL m_bToolBar;                // Toolbar visibility
  CFont m_font;                   // Current font
	UINT m_nIDTracking;
	UINT m_nIDLastMessage;
	UINT_PTR m_nTimer;
};
