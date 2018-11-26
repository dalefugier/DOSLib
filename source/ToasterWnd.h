/*
Module : ToasterWnd.h
Purpose: Defines the interface for an MFC class which implements a MSN Messenger style notification window 
         aka a "Toast" window, since it pops up!. Based in part on a C# implementation of a similiar type
         window by Robert Misiak on codeproject (http://www.codeproject.com/cs/miscctrl/RobMisNotifyWindow.asp)
Created: PJN / 22-04-2005

Copyright (c) 2005 - 2017 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////////////////// Macros / Defines /////////////////////////////////

#pragma once

#ifndef __TOASTERWND_H__
#define __TOASTERWND_H__

#ifndef CTOASTERWND_EXT_CLASS
#define CTOASTERWND_EXT_CLASS
#endif //#ifndef CTOASTERWND_EXT_CLASS


////////////////////////////// Includes ///////////////////////////////////////

#ifdef CTOASTERWND_SCITER_SUPPORT
#ifndef __SCITER_X__
#include <sciter-x.h> //If you get a compilation error on this line, then you need to download, install and purchase a license for sciter (http://sciter.com)
#endif //#ifndef __SCITER_X__
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

#ifndef __ATLIMAGE_H__
#pragma message("To avoid this message, please put atlimage.h in your pre compiled header (normally stdafx.h)")
#include <atlimage.h> //CImage support
#endif //#ifndef __ATLIMAGE_H__


////////////////////////////// Classes ////////////////////////////////////////

//The class which encapsulates the "Toaster" notification window
class CTOASTERWND_EXT_CLASS CToasterWnd : public CFrameWnd
#ifdef CTOASTERWND_SCITER_SUPPORT
    , public sciter::host<CToasterWnd>,
      public sciter::event_handler
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT
{
public:
//Enums
  enum BackgroundStyle
  {
    BackwardDiagonalGradient = 0,
    ForwardDiagonalGradient  = 1,
    HorizontalGradient       = 2,
    VerticalGradient         = 3,
    Solid                    = 4
  };

  enum AnimationStyle
  {
    AutoSet     = 0,
    PopUp       = 1,
    PopDown     = 2,
    PopLeft     = 3,
    PopRight    = 4,
    NoAnimation = 5
  };

  enum PopupPosition
  {
    AutoDetect  = 0,
    BottomRight = 1,
    BottomLeft  = 2,
    TopRight    = 3,
    TopLeft     = 4
  };

  enum AnimationState
  {
    Opening = 0,
    Closing = 1,
    Showing = 2,
    None    = 3
  };

//Constructors / Destructors
  CToasterWnd();
  virtual ~CToasterWnd();

//Methods
  virtual bool    Show();                               //Show (aka create and popup) the notification window
  virtual bool    Create();                             //Create the notification window (but do not show yet)
  virtual void    Popup();                              //Popup the notification window (assumes it has already been created)  
  void            Hide();                               //Hides the notification window (with closing animation)
  void            Close();                              //Actually destroys the window
  virtual HRESULT LoadLogo(_In_z_ LPCTSTR pszFilename); //Load up the logo from a file
  virtual HRESULT LoadLogo(_In_ IStream* pStream);      //Load up the logo from a COM stream

//Accessors / Mutators
  AnimationState GetAnimationState() const              //Returns the "state" of the notification window 
  {
    return m_AnimationState; 
  }

//Member variables
  // Note, CImage was throwing an exception on shutdown
  // Since I'm not using it, I commented it out...
  //CImage                m_Logo;                         //The image which will be displayed in the notification window 
  CRect                 m_rectLogoSrc;                  //The source rect of the logo
  CRect                 m_rectLogoDest;                 //The destination rect of the logo
  BYTE                  m_nLogoSrcAlpha;                //The bSrcAlpha value to pass to AlphaBlend
  CString               m_sTitle;                       //The title text to display in the notification window
  CFont                 m_fontTitle;                    //The font to use for title text
  COLORREF              m_colorTitle;                   //The color to use for the title text
  DWORD                 m_dwTitleStyle;                 //The flags to use in the call to DrawText for the title text
  bool                  m_bTitleHot;                    //Is the title text clickable (and produces the hand cursor)
  CString               m_sText;                        //The body text to display in the notification window
  CFont                 m_fontText;                     //The font to use for body text 
  COLORREF              m_colorText;                    //The color to use for the body text
  DWORD                 m_dwTextStyle;                  //The flags to use in the call to DrawText for the body text
  bool                  m_bTextHot;                     //Is the body text clickable (and produces the hand cursor)
  COLORREF              m_colorBackground;              //The color to use for the background
  COLORREF              m_colorGradient;                //The color to use for the gradient on the background
  DWORD                 m_dwWaitTime;                   //The time in milliseconds which the notification should be shown for
  int                   m_nHeight;                      //The height to use for the notification window
  int                   m_nWidth;                       //The width to use for the notification window
  BackgroundStyle       m_BackgroundStyle;              //The style to use for the client area of the notification window
  bool                  m_bWaitOnMouseOver;             //specifies whether or not the notification window should continue to be displayed it the mouse moves inside the client area
  bool                  m_bOnlyCloseOnUser;             //Specifies whether the notification window can only be closed based on user action (i.e. not auto close because of an animation)
  HICON                 m_TitleIcon;                    //The icon to display just before the Title text
  bool                  m_bIconHot;                     //Is the icon clickable (and produces the hand cursor)
  AnimationStyle        m_AnimationStyle;               //How the window should animate
  PopupPosition         m_PopupPosition;                //Where should the notification window be shown at on the primary display
  int                   m_nCXAreaEdge;                  //The horizontal spacing in pixels between the boundary of the notification window and the edge of the working area
  int                   m_nCYAreaEdge;                  //The vertical spacing in pixels between the boundary of the notification window and the edge of the working area
  bool                  m_bUseThemingForCloseButton;    //Should the close button use XP theming?
  bool                  m_bTransparent;                 //Should the animation also use transparency
#ifdef CTOASTERWND_SCITER_SUPPORT
  bool                  m_bHTML;                        //Should we create a HTML based child control instead of doing all our own custom drawing
  UINT                  m_nHTML;                        //Use this value in conjuction with m_pbHTML if you would like to use a raw blob as the HTML rather than using "m_sText"
  LPCBYTE               m_pbHTML;                       //The raw html text to use
  CString               m_sBaseURL;                     //Base URL of the html

//Methods
  HWINDOW      get_hwnd();
  HINSTANCE    get_resource_instance();
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

protected:
//Methods
  virtual void DrawCloseButton(_In_ CDC* pDC);
  virtual void DrawThemeCloseButton(_In_ CDC* pDC);
  virtual void DrawLegacyCloseButton(_In_ CDC* pDC);
  virtual void CreateFonts();
  virtual void HandleFullyShownTimer();
  virtual void DrawLogo(_In_ CDC* pDC);
  virtual void HandleClosing(_In_ bool bButton);
  virtual void OnBodyTextClicked();                     //Called when the user clicks the body text
  virtual void OnTitleTextClicked();                    //Called when the user clicks the title text
  virtual void OnIconClicked();                         //Called when the user clicks the title text
  virtual void OnClose(_In_ bool bButton);              //Called when the notification window is closing
  virtual void OnStateChange();                         //Called when the animation state changes
  virtual CRect CalculateInitialPopupPosition();        //Called to calculate the initial popup position and animation style 
#ifdef CTOASTERWND_SCITER_SUPPORT
  virtual bool handle_mouse(HELEMENT he, MOUSE_PARAMS& params);
  virtual bool handle_key(HELEMENT he, KEY_PARAMS& params);
  virtual bool handle_focus(HELEMENT he, FOCUS_PARAMS& params);
  virtual bool handle_event(HELEMENT he, BEHAVIOR_EVENT_PARAMS& params);
  virtual bool handle_method_call(HELEMENT he, METHOD_PARAMS& params);
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT
  bool UpdateTransparency(_In_ BYTE byAlpha);
#ifdef CTOASTERWND_SCITER_SUPPORT
  virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

//Message handlers
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
  afx_msg void OnClose();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);

//Member variables
  UINT_PTR              m_nTimerID;                     //The timer ID we use for the animation and shutdown
  CRect                 m_rectScreen;                   //The work area of the primary display
  CRect                 m_rectIcon;                     //Client window location of the icon
  CRect                 m_rectClose;                    //Client window location of the close button
  CRect                 m_rectTitle;                    //Client window location of the title text
  CRect                 m_rectBodyText;                 //Client window location of the body text
  AnimationState        m_AnimationState;               //The animation state of the notification window
  bool                  m_bClosePressed;                //Has the closed button been pressed
  bool                  m_bCloseHot;                    //Is this close button "hot"
  bool                  m_bTitlePressed;                //Has the title text been pressed
  bool                  m_bIconPressed;                 //Has the icon been pressed
  bool                  m_bBodyTextPressed;             //Has the body text been pressed
  HCURSOR               m_cursorHand;                   //The hand cursor
  AnimationStyle        m_actualAnimationStyle;         //The actual animation style (based on m_AnimationStyle)
  PopupPosition         m_actualPopupPosition;          //The actual position to use (based on m_PopupPosition)
  bool                  m_bSafeToClose;                 //Set when we want to destroy the window as opposed to just hiding it
  CRect                 m_rectInitialPos;               //The initial creation position
  DWORD                 m_dwTimerInterval;              //The interval in milliseconds for the animation timer
  DWORD                 m_dwAnimationMoveSize;          //The value in pixels in which to increase or decrease the window
                                                        //size during the animation

  DECLARE_MESSAGE_MAP()
};

#endif //#ifndef CTOASTERWND_EXT_CLASS
