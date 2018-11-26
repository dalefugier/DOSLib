/*
Module : ToasterWnd.cpp
Purpose: Defines the implementation for an MFC class which implements a MSN Messenger style notification window 
         aka a "Toast" window, since it pops up!. Based in part on a C# implementation of a similiar type
         window by Robert Misiak on codeproject (http://www.codeproject.com/cs/miscctrl/RobMisNotifyWindow.asp)
Created: PJN / 22-04-2005
History: PJN / 23-04-2005 1. Title and body text can now be optionally made hot or not.
                          2. Icon can now be hot.
         PJN / 24-04-2005 1. Code now uses SHAppBarMessage to determine the task bar location.
                          2. popup window is now by default offset horizontally as well as vertically by a 
                          scrollbar
         PJN / 25-04-2005 1. Now supports a new transparency animation effect (if on Win2k or later), via a new 
                          the m_bTransparent member variable. Thanks to Remio for this really cool addition.
         PJN / 12-05-2006 1. Fixed a minor bug in CToasterWnd::Construct where function pointers were not been 
                          checked correctly. Thanks to Wes Aday for reporting this issue.
                          2. Fixed a bug in CToasterWnd::OnEraseBkgnd where the vertex values were not being set
                          correctly for the BackgroundDiagonalGradient and ForwardDiagonalGradient styles. Again 
                          thanks to Wes Aday for reporting this bug.
         PJN / 08-07-2006 1. Code now uses newer C++ style casts instead of C style casts.
                          2. Updated the code to clean compile on VC 2005
                          3. Close button is now shown hot only when the cursor is within its bounding rect.
                          4. Reworked the notification API somewhat. If you are using notifications, then you
                          should review the new layout of the CToasterNotification class.
                          5. Class now allows you to put HTML text into the toast window. This is provided by the
                          QHTM HTML control provided at http://www.gipsysoft.com/qhtm/. You may need to purchase a
                          license (depending on the type of application you are developing) as well as follow their
                          instructions on deploying it if you want this functionality with CToasterWnd. This functionality
                          by default is not included in CToasterWnd and is only available when you define the 
                          preprocessor macro "CTOASTERWND_HTML_SUPPORT".
                          6. Included support for a new "NoAnimation" animation style.
         PJN / 14-12-2006 1. Code now uses class name constant for QHTML control
                          2. Updated sample app code to clean compile on VC 2005
                          3. Macro to control inclusion of QHTM is now "CTOASTERWND_QHTM_SUPPORT"
                          4. Control now allows you to use the HTMLayout control for HTML support for the toast window.
                          For more information about HTMLayout, please see http://www.terrainformatica.com/htmlayout/
         PJN / 17-03-2007 1. Updated copyright details.
                          2. Removed the automatic inclusion of the various HTMLayout behaviors header files. This avoids
                          causing potential linking errors in client apps, since the behavior objects are declared statically 
                          twice. Note that if you want to support HTMLayout behaviors, then it is the responsibility of
                          client apps to pull in the required HTMLayout header files. Thanks to Itamar Syn-Hershko for 
                          reporting this problem.
                          3. Addition of 2 simple LoadHTML and LoadUrl helper methods to allow the toaster window to
                          easily be populated with HTML when using the HTMLayout control. Thanks to Itamar Syn-Hershko for 
                          this nice addition.
                          4. Addition of a m_dwHTML and m_pbHTML member variables when you are using the HTMLayout control.
                          If you populate these values, then the HTML for the control will be taken from these variables
                          instead of from "m_sText". Again thanks to Itamar Syn-Hershko for this nice addition.
         PJN / 31-03-2007 1. The existing CToasterWnd::Show method has been internally refactored into a Create method and
                          Popup method. This allows client code to perform any custom initialization on the control
                          prior to it actually being shown to the end user. Thanks to Itamar Syn-Hershko for prompting
                          this suggestion.
         PJN / 01-04-2007 1. Fixed a bug in the LoadHTML and LoadUrl methods where they were incorrectly using the main
                          window handle of the toaster window instead of the window handle of the HTMLayout child control.
                          Thanks to Itamar Syn-Hershko for reporting this bug.
         PJN / 21-07-2007 1. Updated usage instructions about Platform SDK.
                          2. The CToasterNotification notifier interface has been extended to support a 
                          "OnHTMLayoutLoadData" method. This allows client code to customize the loading of data when 
                          using the HTMLayout control without the need to implement a derived class of CToasterWnd. Thanks 
                          to Itamar Syn-Hershko for reporting this suggestion.
                          3. OnHTMLLayoutHyperLink method in CToasterNotification has been renamed to OnHTMLayoutHyperLink
                          4. OnHTMLLayoutCommand method in CToasterNotification has been renamed to OnHTMLayoutCommand
         PJN / 20-08-2007 1. Removed an unnecessary ASSERT in CToasterWnd::Popup.
                          2. Internal animation state variable is reset in Close(), OnDestroy(), and HandleClosing methods.
                          3. Updated the sample app to show how you can reuse a CToasterWnd window if you so desire. Thanks 
                          to Itamar Syn-Hershko for suggesting this update.
         PJN / 13-07-2008 1. Updated copyright details.
                          2. Updated sample app to clean compile on VC 2008
                          3. The code has now been updated to support VC 2005 or later only. 
                          4. Code now compiles cleanly using Code Analysis (/analyze)
                          5. Removed VC 6 style AppWizard comments from the code.
         PJN / 17-08-2008 1. Fixed a bug where if the calculated body text height was larger than the allowable window size, 
                          you previously got the title text intersecting with the title text. Thanks to Steven Dwyer for 
                          reporting this issue.
                          2. Changed the default DrawText formatting flags for the body text to 
                          DT_NOPREFIX | DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL. This is a better default value for the
                          typical body text
                          3. Changed the default DrawText formatting flags for the title text to 
                          DT_NOPREFIX | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS. This is a better default value for the
                          typical title text
         PJN / 10-12-2008 1. The timer interval used for the opening and closing animation is now controllable via a member
                          variable
                          2. The size of the pixels in which to increase or decrease the window size during the animation
                          can now be controlled via a member variable
                          3. Fixed a bug where the window could end completely transparent after the initial opening animation
                          4. Fixed a bug where the final size of the window could end up incorrect
                          5. The control now uses double buffered drawing to help eliminate flickering when the opening and
                          closing animation is happening. Thanks to "sky" for prompting this update.
         PJN / 14-12-2008 1. The control now supports drawing an alpha blended image in its client area. This is achieved using
                          the new member variables: m_Logo, m_rectLogoSrc, m_rectLogoDest and m_nLogoSrcAlpha. The code 
                          internally uses the CImage class and its support for alpha blending via the Win32 API function
                          AlphaBlend. To use this functionality in the control you must define the preprocessor variable
                          WINVER to be >= 500. Thanks to "Sky" for prompting this update.
         PJN / 25-08-2013 1. Updated copyright details
                          2. Code now uses CDC::GradientFill instead of using function pointers directly
                          3. Code now uses CWnd::SetLayeredWindowAttributes instead of using function pointer directly
                          4. Code no longer uses LoadLibrary without an absolute path when loading UxTheme and m_hMsimg32 dlls. 
                          This avoids DLL planting security issues.
                          5. Demo app now includes a check box for enabling / disabling the background image
                          6. The class now includes support for a new "m_bOnlyCloseOnUser" boolean setting. This specifies 
                          whether the notification window can only be closed based on user action i.e. not auto close because 
                          of an animation
                          7. Fixed a bug where window would not be drawn correctly when using the HTMLayout control due to the 
                          OnPaint method being incorrect
                          8. The default value for "m_AnimationStyle" is set based on the value returned from
                          SystemParametersInfo(SPI_GETANIMATION)
         PJN / 26-07-2015 1. Updated copyright details.
                          2. Updated the code to clean compile on VC 2013 & 2015
         PJN / 28-10-2017 1. Updated copyright details.
                          2. Replaced NULL with nullptr throughout the codebase. This means that the code now requires 
                          VC 2010 at a minimum to compile
                          3. Added SAL annotations to all the code
                          4. Removed the code which used function pointers when calling the theming APIs
                          5. Removed QHTM support code from the codebase as that control is no longer available.
                          6. Updated the code to support sciter (http://sciter.com) instead of the older HTMLayout for 
                          HTML toasts.
                          7. Made the CToasterNotification interface part of the CToasterWnd class.
                          8. Replaced BOOL throughout the codebase with bool

Copyright (c) 2005 - 2017 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "ToasterWnd.h"

#ifndef _UXTHEME_H_
#pragma message("To avoid this message, please put uxtheme.h in your pre compiled header (normally stdafx.h)")
#include <uxtheme.h>
#endif  //#ifndef _UXTHEME_H_


//////////////////////////////// Statics / Macros /////////////////////////////

#ifndef WP_CLOSEBUTTON
#define WP_CLOSEBUTTON 18
#endif  #ifndef WP_CLOSEBUTTON

#ifndef CBS_PUSHED
#define CBS_PUSHED 3
#endif  //#ifndef CBS_PUSHED

#ifndef CBS_HOT
#define CBS_HOT 2
#endif  //#ifndef CBS_HOT

const UINT TOASTERWND_TIMER_ID = 1;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////// Implementation /////////////////////////////

BEGIN_MESSAGE_MAP(CToasterWnd, CFrameWnd)
  ON_WM_DESTROY()
  ON_WM_TIMER()
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_SETCURSOR()
  ON_WM_CLOSE()
  ON_WM_CREATE()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CToasterWnd::CToasterWnd() : m_dwTimerInterval(15),
                             m_dwAnimationMoveSize(5),
                             m_colorTitle(GetSysColor(COLOR_WINDOWTEXT)),
                             m_colorText(m_colorTitle),
                             m_colorBackground(RGB(70, 130, 180)),
                             m_colorGradient(RGB(0xF5, 0xF5, 0xF5)),
                             m_dwWaitTime(10000), //Default to 10 seconds
                             m_nHeight(116),
                             m_nWidth(181),
                             m_nLogoSrcAlpha(255),
                             m_rectLogoSrc(0, 0, 181, 116),
                             m_rectLogoDest(0, 0, 181, 116),
                             m_BackgroundStyle(BackwardDiagonalGradient),
                             m_bWaitOnMouseOver(true),
                             m_bOnlyCloseOnUser(false),
                             m_nTimerID(0),
                             m_AnimationState(None),
                             m_bClosePressed(false),
                             m_bCloseHot(false),
                             m_bTitlePressed(false),
                             m_bIconPressed(false),
                             m_bBodyTextPressed(false),
                             m_dwTitleStyle(DT_NOPREFIX | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS),
                             m_dwTextStyle(DT_NOPREFIX | DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL),
                             m_TitleIcon(nullptr),
                             m_PopupPosition(AutoDetect),
                             m_actualPopupPosition(AutoDetect),
                             m_nCXAreaEdge(GetSystemMetrics(SM_CXVSCROLL)),
                             m_nCYAreaEdge(GetSystemMetrics(SM_CYHSCROLL)),
                             m_bUseThemingForCloseButton(true),
                             m_bSafeToClose(false),
                             m_bTitleHot(true),
                             m_bTextHot(true),
                             m_bIconHot(true),
                             m_bTransparent(true),
                           #ifdef CTOASTERWND_SCITER_SUPPORT
                             m_bHTML(false),
                             m_nHTML(0),
                             m_pbHTML(nullptr),
                           #endif  //#ifdef CTOASTERWND_SCITER_SUPPORT
                             m_cursorHand(LoadCursor(nullptr, IDC_HAND))
{
  ANIMATIONINFO ai;
  ai.cbSize = sizeof(ai);
  if (SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0))
    m_AnimationStyle = (ai.iMinAnimate == 0) ? NoAnimation : AutoSet;
  else
    m_AnimationStyle = AutoSet;
  m_actualAnimationStyle = m_AnimationStyle;
}

CToasterWnd::~CToasterWnd()
{
  if (m_cursorHand != nullptr)
  {
    DestroyCursor(m_cursorHand);
    m_cursorHand = nullptr;
  }
  if (m_TitleIcon != nullptr)
  {
    DestroyIcon(m_TitleIcon);
    m_TitleIcon = nullptr;
  }
}

CRect CToasterWnd::CalculateInitialPopupPosition()
{
  //Get the task bar position
  APPBARDATA abd;
  abd.cbSize = sizeof(abd);
  abd.hWnd = ::FindWindow(_T("Shell_TrayWnd"), nullptr);
  SHAppBarMessage(ABM_GETTASKBARPOS, &abd);

  //Work out the popup position to use if "AutoDetect" was specified
  m_actualPopupPosition = m_PopupPosition;
  if (m_actualPopupPosition == AutoDetect)
  {
    switch (abd.uEdge)
    {
      case ABE_BOTTOM:
      {
        m_actualPopupPosition = BottomRight;
        break;
      }
      case ABE_LEFT:
      {
        m_actualPopupPosition = BottomLeft;
        break;
      }
      case ABE_TOP:
      {
        m_actualPopupPosition = TopRight;
        break;
      }
      case ABE_RIGHT:
      {
        m_actualPopupPosition = BottomRight;
        break;
      }
      default:
      {
        m_actualPopupPosition = BottomRight; //Default to something if SHAppBarMessage failed (e.g Explorer gone AWOL)
        break;
      }
    }
  }

  //Work out the animation style if "AutoSet" was specified
  m_actualAnimationStyle = m_AnimationStyle;
  if (m_actualAnimationStyle == AutoSet)
  {
    switch (abd.uEdge)
    {
      case ABE_BOTTOM:
      {
        m_actualAnimationStyle = PopUp;
        break;
      }
      case ABE_LEFT:
      {
        m_actualAnimationStyle = PopLeft;
        break;
      }
      case ABE_TOP:
      {
        m_actualAnimationStyle = PopDown;
        break;
      }
      case ABE_RIGHT:
      {
        m_actualAnimationStyle = PopRight;
        break;
      }
      default:
      {
        m_actualAnimationStyle = PopUp; //Default to something if SHAppBarMessage failed (e.g Explorer gone AWOL)
        break;
      }
    }
  }

  //Work out where the window should be initially shown
  CRect rectInitialPos;
  switch (m_actualAnimationStyle)
  {
    case PopUp:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge + m_nHeight);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(false);
          break;
        }
      }
      break;
    }
    case PopDown:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - 8, m_rectScreen.top + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(false);
          break;
        }
      }
      break;
    }
    case PopLeft:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(false);
          break;
        }
      }
      break;
    }
    case PopRight:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(false);
          break;
        }
      }
      break;
    }
    case NoAnimation:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge + m_nHeight);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(false);
          break;
        }
      }
      break;
    }
    default:
    {
      ASSERT(false);
      break;
    }
  }

  return rectInitialPos;
}

void CToasterWnd::CreateFonts()
{
  //Create the various fonts if not already created
  CFont* pCurrentFont = GetFont();
  CFont defaultGUIFont;
  if (pCurrentFont == nullptr)
  {
    defaultGUIFont.Attach(GetStockObject(DEFAULT_GUI_FONT));
    pCurrentFont = &defaultGUIFont;
  }
  ASSERT(pCurrentFont != nullptr);
  if (m_fontTitle.operator HFONT() == nullptr)
  {
    LOGFONT lf;
    pCurrentFont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    m_fontTitle.CreateFontIndirect(&lf);
  }
  if (m_fontText.operator HFONT() == nullptr)
  {
    LOGFONT lf;
    pCurrentFont->GetLogFont(&lf);
    m_fontText.CreateFontIndirect(&lf);
  }
}

bool CToasterWnd::Create()
{
  //validate our parameters
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
    ASSERT((m_pbHTML && m_nHTML) || m_sText.GetLength()); //If we are using sciter, we support loading from the raw byte pointer or from the CString member variable
#else  
  ASSERT(m_sText.GetLength()); //What no Text!!
#endif  //#ifdef CTOASTERWND_SCITER_SUPPORT

  //Get the working area for the primary monitor
  if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rectScreen, 0))
    return false;
  
  //Do the initial calculation. Note we allow the notifier an initial crack
  //at deciding at where the window should be located. This means that
  //notifiers could potentially handle stacking of multiple windows similiar
  //to what MSN Messenger does
  m_rectInitialPos = CalculateInitialPopupPosition();

  //create the window using the appropiate styles 
  DWORD dwExFlags = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
  if (m_bTransparent && (m_actualAnimationStyle != NoAnimation))
    dwExFlags |= WS_EX_LAYERED;
  if (!__super::Create(nullptr, m_sTitle, WS_POPUP, m_rectInitialPos, nullptr, nullptr, dwExFlags))
    return false;

  //Do all the various calculation / setup code (if required)
  bool bNeedInitCode = true;
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
  {
    bNeedInitCode = false;

    m_rectClose = CRect(-1, -1, -1, -1);
    m_rectIcon = CRect(-1, -1, -1, -1);
    m_rectTitle = CRect(-1, -1, -1, -1);
    m_rectBodyText = CRect(-1, -1, -1, -1);
  }
#endif  //#ifdef CTOASTERWND_SCITER_SUPPORT
  if (bNeedInitCode)
  {
    //Create the fonts
    CreateFonts();

    //Calculate the position of the close button
    CRect rectClient;
    GetClientRect(&rectClient);

    if (m_actualAnimationStyle == PopLeft || m_actualAnimationStyle == PopRight)
      m_rectClose = CRect(rectClient.left + m_nWidth - 26, 5, rectClient.left + m_nWidth - 13, 18);
    else
      m_rectClose = CRect(rectClient.right - 26, 5, rectClient.right - 13, 18);

    //Calculate the position of the icon
    if (m_TitleIcon)
      m_rectIcon = CRect(rectClient.left + 8, 5, GetSystemMetrics(SM_CXSMICON) + rectClient.left + 8, GetSystemMetrics(SM_CYSMICON) + 5);
    else
      m_rectIcon = CRect(-1, -1, -1, -1);

    //Calculate the position of the title text
    int nOffset;
    if (m_sTitle.GetLength())
    {
      CDC* pDC = GetDC();
      if (pDC == nullptr)
        return false;
      CFont* pOldFont = pDC->SelectObject(&m_fontTitle);
      CSize sizeTitle = pDC->GetTextExtent(m_sTitle);
      m_rectTitle = CRect(max(5, m_rectIcon.right + 5), 5, m_rectClose.left - 5, 5 + sizeTitle.cy);
      nOffset = max(max(m_rectTitle.bottom + 5, m_rectClose.bottom + 5), m_rectIcon.bottom + 5);

      if (pOldFont)
        pDC->SelectObject(pOldFont);

      ReleaseDC(pDC);
    }
    else
    {
      nOffset = m_rectClose.bottom + 2;
      m_rectTitle = CRect(-1, -1, -1, -1);
    }

    //Calculate the position of the body text
    int nBodyTextRight;
    if (m_actualAnimationStyle == PopLeft || m_actualAnimationStyle == PopRight)
    {
      nBodyTextRight = rectClient.left + m_nWidth - 7;
      m_rectBodyText = CRect(7, nOffset, nBodyTextRight, nOffset);
    }
    else
    {
      nBodyTextRight = rectClient.right - 7;
      m_rectBodyText = CRect(7, nOffset, nBodyTextRight, nOffset);
    }
    CDC* pDC = GetDC();
    if (pDC == nullptr)
      return false;
    CFont* pOldFont = pDC->SelectObject(&m_fontText);
    pDC->DrawText(m_sText, &m_rectBodyText, m_dwTextStyle | DT_CALCRECT);
    m_rectBodyText.right = nBodyTextRight;
    int nBodyHeight = m_rectBodyText.Height();
    int nNewTop = (m_nHeight - nBodyHeight - nOffset - 5) / 2 + nOffset;
    m_rectBodyText = CRect(m_rectBodyText.left, nNewTop, m_rectBodyText.right, nNewTop + nBodyHeight);
    if (m_rectBodyText.top < nOffset)
      m_rectBodyText.top = nOffset;
    if (m_rectBodyText.bottom > (m_nHeight - 7))
      m_rectBodyText.bottom = (m_nHeight - 7);
  
    if (pOldFont)
      pDC->SelectObject(pOldFont);
  
    ReleaseDC(pDC);
  }

  return true;
}

void CToasterWnd::Popup()
{
  //Do the initial calculation. Note we allow the virtual method an initial crack at deciding at where the window should be located. This means that 
  //notifiers could potentially handle stacking of multiple windows similiar to what MSN Messenger does
  m_rectInitialPos = CalculateInitialPopupPosition();

  //Reset the window position to be the initial popup position
  SetWindowPos(nullptr, m_rectInitialPos.left, m_rectInitialPos.top, m_rectInitialPos.Width(), m_rectInitialPos.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

  //Create the timer and set the state machine state
  if (m_actualAnimationStyle != NoAnimation)
  {
    m_nTimerID = SetTimer(TOASTERWND_TIMER_ID, m_dwTimerInterval, nullptr);
    m_AnimationState = Opening;
  }
  else
  {
    if (!m_bOnlyCloseOnUser)
      m_nTimerID = SetTimer(TOASTERWND_TIMER_ID, m_dwWaitTime, nullptr);
    m_AnimationState = Showing;
  }

  //Show the window (without activation)
  ShowWindow(SW_SHOWNOACTIVATE);

  //Inform the notifier
  OnStateChange();
}

bool CToasterWnd::Show()
{
  //Call our two helper functions to do the heavy lifting
  if (!Create())
    return false;
  Popup();
  return true;
}

void CToasterWnd::Hide()
{
  if (m_AnimationState == Opening || m_AnimationState == Showing)
  {
    //Change to the closing state and reset the timer
    m_AnimationState = Closing;
    SetTimer(m_nTimerID, m_dwTimerInterval, nullptr);
    OnStateChange();
  }
}

void CToasterWnd::Close()
{
  m_bSafeToClose = true;
  m_AnimationState = None;
  DestroyWindow();
}

void CToasterWnd::OnDestroy() 
{
  //Kill the timer if active
  if (m_nTimerID)
  {
    KillTimer(m_nTimerID);
    m_nTimerID = 0;
  }
  
  //Let the base class do its thing
  __super::OnDestroy();
  
  m_AnimationState = None;
}

bool CToasterWnd::UpdateTransparency(_In_ BYTE byAlpha)
{
  //Call the SetLayeredWindowAttributes to do the work for us
  BOOL bSuccess = FALSE;
  if (m_bTransparent)
    bSuccess = SetLayeredWindowAttributes(0, byAlpha, LWA_ALPHA);
  else
    bSuccess = TRUE;

  return bSuccess ? true : false;
}

void CToasterWnd::HandleFullyShownTimer()
{
  //We have fully opened, change to the showing state and reset the timer
  m_AnimationState = Showing;
  UpdateTransparency(255);

  //Only start the wait timer for closure if allowed
  if (m_bOnlyCloseOnUser)
    KillTimer(m_nTimerID);
  else
    SetTimer(m_nTimerID, m_dwWaitTime, nullptr);

  //Let the notifier know
  OnStateChange();
}

void CToasterWnd::OnTimer(UINT_PTR nIDEvent)
{
  if (nIDEvent == m_nTimerID) //Is it the timer we are interested in?
  {
    switch (m_AnimationState)
    {
      case Opening:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);
        
        switch (m_actualAnimationStyle)
        {
          case PopUp:
          {
            if ((rectWindow.Height() >= m_nHeight))
              HandleFullyShownTimer();
            else
            {
              rectWindow.top -= m_dwAnimationMoveSize;
              int nFinalTop = rectWindow.bottom - m_nHeight;
              if (rectWindow.top < nFinalTop)
                rectWindow.top = nFinalTop;
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopDown:
          {
            if (rectWindow.Height() >= m_nHeight)
              HandleFullyShownTimer();
            else
            {
              rectWindow.bottom += m_dwAnimationMoveSize;
              int nFinalBottom = rectWindow.top + m_nHeight;
              if (rectWindow.bottom > nFinalBottom)
                rectWindow.bottom = nFinalBottom;
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopLeft:
          {
            if ((rectWindow.Width() >= m_nWidth))
              HandleFullyShownTimer();
            else
            {
              rectWindow.right += m_dwAnimationMoveSize;
              int nFinalRight = rectWindow.left + m_nWidth;
              if (rectWindow.right > nFinalRight)
                rectWindow.right = nFinalRight;
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case PopRight:
          {
            if ((rectWindow.Width() >= m_nWidth))
              HandleFullyShownTimer();
            else
            {
              rectWindow.left -= m_dwAnimationMoveSize;
              int nFinalLeft = rectWindow.right - m_nWidth;
              if (rectWindow.left < nFinalLeft)
                rectWindow.left = nFinalLeft;
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          default:
          {
            ASSERT(false);
            break;
          }
        }
        break;
      }
      case Showing:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);

        CPoint cursorPos;
        GetCursorPos(&cursorPos);
        if (!m_bWaitOnMouseOver || !rectWindow.PtInRect(cursorPos))
        {
          if (m_actualAnimationStyle != NoAnimation)
            Hide();
          else
            HandleClosing(false);
        }
        break;
      }
      case Closing:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);

        switch (m_actualAnimationStyle)
        {
          case PopUp:
          {
            rectWindow.top += m_dwAnimationMoveSize;
            if (rectWindow.Height() <= 0)
              HandleClosing(false);
            else
            {
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopDown:
          {
            rectWindow.bottom -= m_dwAnimationMoveSize;
            if (rectWindow.Height() <= 0)
              HandleClosing(false);
            else
            {
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopLeft:
          {
            rectWindow.right -= m_dwAnimationMoveSize;
            if (rectWindow.Width() <= 0)
              HandleClosing(false);
            else
            {
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case PopRight:
          {
            rectWindow.left += m_dwAnimationMoveSize;
            if (rectWindow.Width() <= 0)
              HandleClosing(false);
            else
            {
              SetWindowPos(nullptr, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case NoAnimation:
          {
            //Close the window down straight away
            HandleClosing(false);
            break;
          }
          default:
          {
            ASSERT(false);
            break;
          }
        }
        break;
      }
      default:
      {
        ASSERT(false);
        break;
      }
    }
  }
  else
  {
    //Let the base class do its thing
    __super::OnTimer(nIDEvent);
  }
}

HRESULT CToasterWnd::LoadLogo(_In_z_ LPCTSTR pszFilename)
{
  //HRESULT hr = m_Logo.Load(pszFilename);
  //if (SUCCEEDED(hr))
  //{
  //  //By default set the source and destination rects of the logo to be the same size as the bitmap
  //  //Derived classes can change this or you could directly change "m_rectLogoSrc" or "m_rectLogoDest"
  //  //after calling this function
  //  m_rectLogoSrc = CRect(0, 0, m_Logo.GetWidth(), m_Logo.GetHeight());
  //  m_rectLogoDest = m_rectLogoSrc;
  //}

  //return hr;
  return S_FALSE;
}

HRESULT CToasterWnd::LoadLogo(_In_ IStream* pStream)
{
  //HRESULT hr = m_Logo.Load(pStream);
  //if (SUCCEEDED(hr))
  //{
  //  //By default set the source and destination rects of the logo to be the same size as the bitmap
  //  //Derived classes can change this or you could directly change "m_rectLogoSrc" or "m_rectLogoDest"
  //  //after calling this function
  //  m_rectLogoSrc = CRect(0, 0, m_Logo.GetWidth(), m_Logo.GetHeight());
  //  m_rectLogoDest = m_rectLogoSrc;
  //}

  //return hr;
  return S_FALSE;
}

void CToasterWnd::OnPaint()
{
  //create the device context for painting
  CPaintDC dc(this);

  //Nothing else to do if in HTML mode
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
    return;
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

  //Get the client area
  CRect rectClient;
  GetClientRect(&rectClient);
  int nClientWidth = rectClient.Width();
  int nClientHeight = rectClient.Height();

  //Create a memory device context to implement our double buffered painting
  CDC memDC;
  memDC.CreateCompatibleDC(&dc);
  CBitmap bmp;
  bmp.CreateCompatibleBitmap(&dc, nClientWidth, nClientHeight);
  CBitmap* pOldBitmap = memDC.SelectObject(&bmp);

  bool bDoFillSolidRect = true;
  switch (m_BackgroundStyle)
  {
    case BackwardDiagonalGradient:
    {
      bDoFillSolidRect = false;
      TRIVERTEX vert[4];
      vert[0].x = rectClient.left;
      vert[0].y = rectClient.top;
      vert[0].Red = static_cast<COLOR16>((GetRValue(m_colorBackground) - GetRValue(m_colorGradient)) * 128);
      vert[0].Green = static_cast<COLOR16>((GetGValue(m_colorBackground) - GetGValue(m_colorGradient)) * 128);
      vert[0].Blue = static_cast<COLOR16>((GetBValue(m_colorBackground) - GetBValue(m_colorGradient)) * 128);
      vert[0].Alpha = 0;

      vert[1].x = rectClient.right;
      vert[1].y = rectClient.top;
      vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
      vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
      vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
      vert[1].Alpha = 0;

      vert[2].x = rectClient.right;
      vert[2].y = rectClient.bottom;
      vert[2].Red = vert[0].Red;
      vert[2].Green = vert[0].Green;
      vert[2].Blue = vert[0].Blue;
      vert[2].Alpha = 0;

      vert[3].x = rectClient.left;
      vert[3].y = rectClient.bottom;
      vert[3].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
      vert[3].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
      vert[3].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
      vert[3].Alpha = 0;

      GRADIENT_TRIANGLE gt[2];
      gt[0].Vertex1 = 0;
      gt[0].Vertex2 = 1;
      gt[0].Vertex3 = 2;
      gt[1].Vertex1 = 0;
      gt[1].Vertex2 = 2;
      gt[1].Vertex3 = 3;
      memDC.GradientFill(vert, 4, &gt, 2, GRADIENT_FILL_TRIANGLE);
      break;
    }
    case ForwardDiagonalGradient:
    {
      bDoFillSolidRect = false;
      TRIVERTEX vert[4];
      vert[0].x = rectClient.left;
      vert[0].y = rectClient.top;
      vert[0].Red = static_cast<COLOR16>((GetRValue(m_colorBackground) - GetRValue(m_colorGradient)) * 128);
      vert[0].Green = static_cast<COLOR16>((GetGValue(m_colorBackground) - GetGValue(m_colorGradient)) * 128);
      vert[0].Blue = static_cast<COLOR16>((GetBValue(m_colorBackground) - GetBValue(m_colorGradient)) * 128);
      vert[0].Alpha = 0;

      vert[1].x = rectClient.right;
      vert[1].y = rectClient.top;
      vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
      vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
      vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
      vert[1].Alpha = 0;

      vert[2].x = rectClient.right;
      vert[2].y = rectClient.bottom;
      vert[2].Red = vert[0].Red;
      vert[2].Green = vert[0].Green;
      vert[2].Blue = vert[0].Blue;
      vert[2].Alpha = 0;

      vert[3].x = rectClient.left;
      vert[3].y = rectClient.bottom;
      vert[3].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
      vert[3].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
      vert[3].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
      vert[3].Alpha = 0;

      GRADIENT_TRIANGLE gt[2];
      gt[0].Vertex1 = 0;
      gt[0].Vertex2 = 1;
      gt[0].Vertex3 = 2;
      gt[1].Vertex1 = 0;
      gt[1].Vertex2 = 2;
      gt[1].Vertex3 = 3;
      memDC.GradientFill(vert, 4, &gt, 2, GRADIENT_FILL_TRIANGLE);
      break;
    }
    case HorizontalGradient: //deliberate fallthrough
    case VerticalGradient:
    {
      bDoFillSolidRect = false;
      TRIVERTEX vert[2];
      vert[0].x = rectClient.left;
      vert[0].y = rectClient.top;
      vert[0].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
      vert[0].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
      vert[0].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
      vert[0].Alpha = 0;

      vert[1].x = rectClient.right;
      vert[1].y = rectClient.bottom;
      vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
      vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
      vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
      vert[1].Alpha = 0;

      GRADIENT_RECT gr;
      gr.UpperLeft = 0;
      gr.LowerRight = 1;
      memDC.GradientFill(vert, 2, &gr, 1, (m_BackgroundStyle == HorizontalGradient) ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
      break;
    }
    case Solid:
    {
      bDoFillSolidRect = true;
      break;
    }
    default:
    {
      ASSERT(false);
      break;
    }
  }

  if (bDoFillSolidRect)
    memDC.FillSolidRect(&rectClient, m_colorBackground);

  //Draw the logo
  DrawLogo(&memDC);

  //Draw the icon
  if (m_TitleIcon != nullptr)
    DrawIconEx(memDC.GetSafeHdc(), m_rectIcon.left, m_rectIcon.top, m_TitleIcon, m_rectIcon.Width(), m_rectIcon.Height(), 0, nullptr, DI_NORMAL);
  
  //Draw the close button
  DrawCloseButton(&memDC);

  //Draw text transparently
  int nOldBkMode = memDC.SetBkMode(TRANSPARENT);

  //Draw the title text
  if (m_sTitle.GetLength())
  { 
    CFont* pOldFont = memDC.SelectObject(&m_fontTitle);
    COLORREF nOldColor = memDC.SetTextColor(m_colorTitle);
    memDC.DrawText(m_sTitle, m_rectTitle, m_dwTitleStyle);
    memDC.SetTextColor(nOldColor);
    if (pOldFont)
      memDC.SelectObject(pOldFont);
  }

  //Draw the body text
  CFont* pOldFont = memDC.SelectObject(&m_fontText);
  COLORREF nOldColor = memDC.SetTextColor(m_colorText);
  memDC.DrawText(m_sText, m_rectBodyText, m_dwTextStyle);
  memDC.SetTextColor(nOldColor);
  if (pOldFont)
    memDC.SelectObject(pOldFont);

  memDC.SetBkMode(nOldBkMode);

  //Transfer the memory DC to the screen
  dc.BitBlt(0, 0, nClientWidth, nClientHeight, &memDC, 0, 0, SRCCOPY);

  //Restore the DC
  memDC.SelectObject(pOldBitmap);
}

BOOL CToasterWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
  //Do not do any background drawing since all our drawing is done in OnPaint
  return TRUE;
}

void CToasterWnd::DrawLogo(_In_ CDC* pDC)
{
  //if (m_Logo.operator HBITMAP())
  //  m_Logo.AlphaBlend(pDC->GetSafeHdc(), m_rectLogoDest, m_rectLogoSrc, m_nLogoSrcAlpha, AC_SRC_OVER);
}

void CToasterWnd::DrawCloseButton(_In_ CDC* pDC)
{
  BOOL bAppThemed = IsAppThemed();
  if (m_bUseThemingForCloseButton && bAppThemed)
    DrawThemeCloseButton(pDC);
  else
    DrawLegacyCloseButton(pDC);
}

void CToasterWnd::DrawThemeCloseButton(_In_ CDC* pDC)
{
  HTHEME hTheme = OpenThemeData(GetSafeHwnd(), L"Window");
  if (hTheme == nullptr)
    DrawLegacyCloseButton(pDC);
  else
  {
    DrawThemeBackground(hTheme, pDC->GetSafeHdc(), WP_CLOSEBUTTON, m_bClosePressed ? CBS_PUSHED : (m_bCloseHot ? CBS_HOT : 0), m_rectClose, nullptr);
    CloseThemeData(hTheme);
  }
}

void CToasterWnd::DrawLegacyCloseButton(_In_ CDC* pDC)
{
  if (m_bClosePressed)
    pDC->DrawFrameControl(&m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_PUSHED);
  else
    pDC->DrawFrameControl(&m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE);
}

void CToasterWnd::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
  //Nothing else to do if in HTML mode
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
    return;
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

  if (m_rectClose.PtInRect(point))
  {
    m_bClosePressed = true;
    InvalidateRect(&m_rectClose);
  }
  else if (m_bTitleHot && m_rectTitle.PtInRect(point))
  {
    m_bTitlePressed = true;
  }
  else if (m_bTextHot && m_rectBodyText.PtInRect(point))
  {
    m_bBodyTextPressed = true;
  }
  else if (m_bIconHot && m_rectIcon.PtInRect(point))
  {
    m_bIconPressed = true;
  }
}

void CToasterWnd::HandleClosing(_In_ bool bButton)
{
  //Inform the notifier if we have one
  OnClose(bButton);

  //Kill the timer if running
  if (m_nTimerID)
  {
    KillTimer(m_nTimerID);
    m_nTimerID = 0;
  }

  //just hide ourselves
  ShowWindow(SW_HIDE);

  m_AnimationState = None;

  //Reset our transparency
  UpdateTransparency(255);
}

void CToasterWnd::OnLButtonUp(UINT /*nFlags*/, CPoint point)
{
  //Nothing else to do if in HTML mode
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
    return;
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

  if (m_bClosePressed)
  {
    m_bClosePressed = false;
    InvalidateRect(&m_rectClose);
    if (m_rectClose.PtInRect(point))
      HandleClosing(true);
  }
  else if (m_bTitlePressed)
  {
    m_bTitlePressed = false;
    if (m_rectTitle.PtInRect(point))
      OnTitleTextClicked();
  }
  else if (m_bBodyTextPressed)
  {
    m_bBodyTextPressed = false;
    if (m_rectBodyText.PtInRect(point))
      OnBodyTextClicked();
  }
  else if (m_bIconPressed)
  {
    m_bIconPressed = false;
    if (m_rectIcon.PtInRect(point))
      OnIconClicked();
  }
}

BOOL CToasterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
#ifdef CTOASTERWND_SCITER_SUPPORT
  BOOL bHandleCursor = !m_bHTML;
#else
  BOOL bHandleCursor = TRUE;
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

  const MSG* pMsg = GetCurrentMessage();
  CPoint pt(pMsg->pt);
  ScreenToClient(&pt);

  if (bHandleCursor && m_rectClose.PtInRect(pt) || (m_bTitleHot && m_rectTitle.PtInRect(pt)) || (m_bTextHot && m_rectBodyText.PtInRect(pt)) || (m_bIconHot && m_rectIcon.PtInRect(pt)))
  {
    SetCursor(m_cursorHand);
    return TRUE;
  }
  else
    return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CToasterWnd::OnClose()
{
  //Only really close the window if the m_bSafeToClose flag
  //is set. Doing this ensures that client code will always
  //has a valid pointer to us, which they can then use to 
  //synchronously close this instance via the Close() method
  if (m_bSafeToClose)
    __super::OnClose();
  else
    ShowWindow(SW_HIDE);
}

#ifdef CTOASTERWND_SCITER_SUPPORT
LRESULT CToasterWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (m_bHTML)
  {
    BOOL bHandled = FALSE;
    LRESULT lResult = SciterProcND(this->GetSafeHwnd(), message, wParam, lParam, &bHandled);
    if (bHandled)     //if it was handled by the Sciter
      return lResult; //then no further processing is required.
  }

    //Let the base class do its thing
  return __super::WindowProc(message, wParam, lParam);
}

HWINDOW CToasterWnd::get_hwnd()
{
  return GetSafeHwnd();
}

HINSTANCE CToasterWnd::get_resource_instance()
{
  return AfxGetApp()->m_hInstance;
}

bool CToasterWnd::handle_mouse(HELEMENT he, MOUSE_PARAMS& params)
{
  return false;
}

bool CToasterWnd::handle_key(HELEMENT he, KEY_PARAMS& params)
{
  return false;
}

bool CToasterWnd::handle_focus(HELEMENT he, FOCUS_PARAMS& params)
{
  return false;
}

bool CToasterWnd::handle_event(HELEMENT he, BEHAVIOR_EVENT_PARAMS& params)
{
  sciter::dom::element root = get_root();

  //Provide default implementations for the various ui elements. Note that
  //it is the responsibility of client code to setup the sciter HTML DOM
  //with elements with id's of "closebutton", titletext", "icon" and / or "bodytext"

  //Provide a default implementation for a "closebutton" click behaviour
  sciter::dom::element closebutton = root.get_element_by_id(L"closebutton");
  if ((params.he == closebutton) && (params.cmd == BUTTON_CLICK))
  {
    HandleClosing(true);
    return true;
  }

  //Provide a default implementation for a "titletext" click behaviour
  sciter::dom::element titletext = root.get_element_by_id(L"titletext");
  if ((params.he == titletext) && (params.cmd == (HYPERLINK_CLICK | SINKING)))
  {
    OnTitleTextClicked();
    return true;
  }

  //Provide a default implementation for a "icon" click behaviour
  sciter::dom::element icon = root.get_element_by_id(L"icon");
  if ((params.he == icon) && (params.cmd == (HYPERLINK_CLICK | SINKING)))
  {
    OnIconClicked();
    return true;
  }

  //Provide a default implementation for a "bodytext" click behaviour
  sciter::dom::element bodytext = root.get_element_by_id(L"bodytext");
  if ((params.he == bodytext) && (params.cmd == (HYPERLINK_CLICK | SINKING)))
  {
    OnBodyTextClicked();
    return true;
  }

  return false;
}

bool CToasterWnd::handle_method_call(HELEMENT he, METHOD_PARAMS& params)
{
  return false;
}
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

int CToasterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  //Let the base class do its thing
  if (__super::OnCreate(lpCreateStruct) == -1)
    return -1;

#if defined(CTOASTERWND_SCITER_SUPPORT)
  //Hook up HTMLayout if we are doing HTML
  if (m_bHTML)
  {
    bool bSuccess = false;
    if (m_nHTML && m_pbHTML)
      bSuccess = load_html(m_pbHTML, m_nHTML, m_sBaseURL.GetLength() ? CStringW(m_sBaseURL).GetString() : nullptr);
    else if (m_sText.GetLength())
    {
      CStringA sAsciiText(m_sText);
      bSuccess = load_html(reinterpret_cast<LPCBYTE>(sAsciiText.GetBuffer()), static_cast<UINT>(sAsciiText.GetLength()), m_sBaseURL.GetLength() ? CStringW(m_sBaseURL).GetString() : nullptr);
      sAsciiText.ReleaseBuffer();
    }
    if (!bSuccess)
      return -1;
    setup_callback();
    sciter::attach_dom_event_handler(this->get_hwnd(), this);
  }
#endif  //#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT)

  return 0;
}

void CToasterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
  //Let the base class do its thing
  __super::OnMouseMove(nFlags, point);

  //Nothing else to do if in HTML mode
#ifdef CTOASTERWND_SCITER_SUPPORT
  if (m_bHTML)
    return;
#endif //#ifdef CTOASTERWND_SCITER_SUPPORT

  //See if we are over the close button and if so make it "hot"
  //and force a redraw of it
  CPoint ptCursor;
  GetCursorPos(&ptCursor);
  ScreenToClient(&ptCursor);
  bool bNewCloseHot = m_rectClose.PtInRect(ptCursor) ? true : false;
  if (bNewCloseHot != m_bCloseHot)
  {
    m_bCloseHot = bNewCloseHot;
    InvalidateRect(m_rectClose);
  }
}

void CToasterWnd::OnBodyTextClicked()
{
  TRACE(_T("CToasterWnd::OnBodyTextClicked called\n"));
}

void CToasterWnd::OnTitleTextClicked()
{
  TRACE(_T("CToasterWnd::OnTitleTextClicked called\n"));
}

void CToasterWnd::OnIconClicked()
{
  TRACE(_T("CToasterWnd::OnIconClicked called\n"));
}

void CToasterWnd::OnClose(_In_ bool /*bButton*/)
{
  TRACE(_T("CToasterWnd::OnClose called\n"));
}

void CToasterWnd::OnStateChange()
{
  TRACE(_T("CToasterWnd::OnStateChange called\n"));
}
