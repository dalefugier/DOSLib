/////////////////////////////////////////////////////////////////////////////
// DosDialogBars.h
//
// Copyright © The Fugier Group. All rights reserved.
// DOSLib is a trademark of The Fugier Group.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

//class CDosNotepad;

/////////////////////////////////////////////////////////////////////////////
// CDosToolBar

class CDosToolBar : public CToolBar 
{   
public:
	CDosToolBar();
   
public:   
	virtual ~CDosToolBar();

	BOOL LoadTrueColorToolBar(
    int nButtonWidth,
    UINT uToolBar,
    UINT uToolBarHot = 0,
    UINT uToolBarDisabled = 0
    );

	void EnableButton( int nID, BOOL bEnable = TRUE );
  void AddToolTip();

public:
  afx_msg void OnMouseMove( UINT nFlags, CPoint point );

protected:                
  BOOL SetTrueColorToolBar( UINT uToolBarType, UINT uToolBar, int nButtonWidth );

  afx_msg LRESULT OnIdleUpdateCmdUI( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

protected:                
	CMap<int,int&,BOOL,BOOL&> m_ButtonStates;
  CToolTipCtrl* m_pToolTip;
};        

/////////////////////////////////////////////////////////////////////////////
// CDosStatusBar

class CDosStatusBar : public CStatusBar 
{   
public:
	CDosStatusBar();

public:   
	virtual ~CDosStatusBar();

protected:     
	afx_msg LRESULT OnIdleUpdateCmdUI( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};
