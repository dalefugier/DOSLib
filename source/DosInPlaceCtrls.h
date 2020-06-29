/////////////////////////////////////////////////////////////////////////////
// DosInPlaceCtrls.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosInPlace
{
public:
  static LPCTSTR MsgString();

  enum
  {
    ipek_key = 1,
    ipek_action = 2,
    ipek_focus = 3
  };

  typedef struct tagIPCENDEDITINFO
  {
    HWND hwndSrc;    // Event source
    UINT uKind;      // Event (key press, focus change, etc )
    union
    {
      UINT uID;      // Control ID (for actions)
      UINT nVirtKey; // Virtual key
      HWND hNewWnd;  // New window with focus
    };
    POINT pt;         // Event point, in screen coordinates
  } IPCENDEDITINFO, FAR *LPIPCENDEDITINFO;

  static UINT const m_IPCMsg;
};


class CDosInPlaceEdit : public CEdit
{
  DECLARE_DYNCREATE(CDosInPlaceEdit)

public:
  CDosInPlaceEdit();
  virtual ~CDosInPlaceEdit();

  void Show(CRect rcEdit);
  void Hide();

private:
  HWND m_hParent;

public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

class CDosInPlaceButton : public CButton
{
  DECLARE_DYNCREATE(CDosInPlaceButton)

public:
  CDosInPlaceButton();
  virtual ~CDosInPlaceButton();

  void Show(CRect rcButton);
  void Hide();

private:
  HWND m_hParent;
  BOOL m_bInAction;

private:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnClick();

protected:
  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
