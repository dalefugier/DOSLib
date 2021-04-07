////////////////////////////////////////////////////////////////
// func_dialogs.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosGetIntDialog.h"
#include "DosGetRealDialog.h"
#include "DosGetStringDialog.h"
#include "DosGetPwdDialog.h"
#include "DosColorDialog.h"
#include "DosHtmlDialog.h"
#include "DosHtmlDialogEx.h"
#include "DosSplash.h"
#include "DosCheckListDialog.h"
#include "DosListBoxDialog.h"
#include "DosDwgPreviewDialog.h"
#include "DosEditBoxDialog.h"
#include "DosPropListDialog.h"
#include "DosMultiListDialog.h"
#include "DosComboListDialog.h"
#include "DosEditListDialog.h"
#include "DosPopupMenu.h"
#include "DosMessageBox.h"
#include "DosMessageDialog.h"
#include "DosGetDateDialog.h"
#include "DosOrderListDialog.h"
#include "DosDualListDialog.h"

////////////////////////////////////////////////////////////////
// dos_getint
int CDOSLibApp::ads_dos_getint()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosGetIntDialog dlg(acedGetAcadFrame());

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  dlg.SetTitle(strTitle);
  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  dlg.SetPrompt(strPrompt);
  args.Next();

  int n = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(n))
      return RSERR;

    if (n < 0 || n > 7)
    {
      acutPrintf(L"DOSLib error: invalid bit-code value.\n");
      return RSERR;
    }

    if (n & 1)
      dlg.SetAllowEmpty(FALSE);
    if (n & 2)
      dlg.SetAllowZero(FALSE);
    if (n & 4)
      dlg.SetAllowNegative(FALSE);

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(n))
        return RSERR;
      dlg.SetValue(n);

      args.Next();
      if (!args.IsEmpty())
      {
        if (!args.IsTrue() && !args.IsNil())
        {
          if (!args.GetInteger(n))
            return RSERR;
          dlg.SetMax(n);
        }

        args.Next();
        if (!args.IsEmpty())
        {
          if (!args.GetInteger(n))
            return RSERR;
          dlg.SetMin(n);
        }
      }
    }
  }

  if (dlg.DoModal() == IDOK)
  {
    if (dlg.IsEmpty())
      acedRetNil();
    else
      acedRetInt(dlg.Value());
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getreal
int CDOSLibApp::ads_dos_getreal()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  CDosGetRealDialog dlg(acedGetAcadFrame());
  dlg.m_sTitle = strTitle;
  dlg.m_sPrompt = strPrompt;

  if (args.Next())
  {
    int n = 0;
    if (!args.GetInteger(n))
      return RSERR;

    if (n < 0 || n > 7)
    {
      acutPrintf(L"DOSLib error: invalid bit-code value.\n");
      return RSERR;
    }

    if (n & 1)
      dlg.SetAllowEmpty(FALSE);
    if (n & 2)
      dlg.SetAllowZero(FALSE);
    if (n & 4)
      dlg.SetAllowNegative(FALSE);

    if (args.Next())
    {
      double d = 0.0;
      if (!args.GetDouble(d))
        return RSERR;
      dlg.SetValue(d);

      if (args.Next())
      {
        if (!args.IsTrue() && !args.IsNil())
        {
          if (!args.GetDouble(d))
            return RSERR;
          dlg.SetMax(d);
        }

        if (args.Next())
        {
          if (!args.GetDouble(d))
            return RSERR;
          dlg.SetMin(d);
        }
      }
    }
  }

  if (dlg.DoModal() == IDOK)
  {
    if (dlg.IsEmpty())
      acedRetNil();
    else
      acedRetReal(dlg.Value());
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getstring
int CDOSLibApp::ads_dos_getstring()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  CString strString;
  int nLength = 0;

  if (args.IsEmpty())
  {
    strString.Empty();
  }
  else
  {
    if (!args.GetString(strString))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(nLength))
        return RSERR;

      if (nLength <= 0)
        nLength = 0;
    }
  }

  CDosGetStringDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_String = strString;
  dlg.m_Length = nLength;

  if (dlg.DoModal() == IDOK)
    acedRetStr(dlg.m_String);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getpassword
int CDOSLibApp::ads_dos_getpassword()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  CString strString;
  int nLength = 0;

  if (args.IsEmpty())
  {
    strString.Empty();
  }
  else
  {
    if (!args.GetString(strString))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetInteger(nLength))
        return RSERR;

      if (nLength <= 0)
        nLength = 0;
    }
  }

  CDosGetPwdDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_String = strString;
  dlg.m_Length = nLength;

  if (dlg.DoModal() == IDOK)
    acedRetStr(dlg.m_String);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getcolor
int CDOSLibApp::ads_dos_getcolor()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  COLORREF color = RGB(0, 0, 0);

  if (args.IsEmpty())
  {
    color = RGB(0, 0, 0);
  }
  else if (args.IsInteger())
  {
    int nColor = 0;
    if (!args.GetInteger(nColor))
      return RSERR;
    color = (long)nColor;
  }
  else if (args.Is3dPoint())
  {
    ads_point pt;
    if (!args.GetPoint(pt))
      return RSERR;

    if (pt[0] < 0.0 || pt[0] > 255.0)
    {
      acutPrintf(L"DOSLib error: invalid red value.\n");
      return RSERR;
    }

    if (pt[1] < 0.0 || pt[1] > 255.0)
    {
      acutPrintf(L"DOSLib error: invalid green value.\n");
      return RSERR;
    }

    if (pt[2] < 0.0 || pt[2] > 255.0)
    {
      acutPrintf(L"DOSLib error: invalid blue value.\n");
      return RSERR;
    }

    color = RGB((int)pt[0], (int)pt[1], (int)pt[2]);
  }
  else
  {
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  CDosColorDialog dlg(0, 0, acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
  dlg.m_cc.rgbResult = color;
  if (dlg.DoModal() == IDOK)
  {
    color = dlg.m_cc.rgbResult;
    struct resbuf* retlist = acutBuildList(
      RTSHORT, (short)GetRValue(color),
      RTSHORT, (short)GetGValue(color),
      RTSHORT, (short)GetBValue(color),
      0
      );
    acedRetList(retlist);
    acutRelRb(retlist);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_htmlbox
int CDOSLibApp::ads_dos_htmlbox()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosHtmlDialog dlg(acedGetAcadFrame());

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;
  dlg.SetTitle(strTitle);

  args.Next();

  CString strAddress;
  if (!args.GetString(strAddress))
    return RSERR;
  dlg.SetAddress(strAddress);

  args.Next();

  int dx = 0, dy = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(dx))
      return RSERR;

    args.Next();

    if (!args.GetInteger(dy))
      return RSERR;

    if (dx < 1 || dy < 1)
    {
      acutPrintf(L"DOSLib error: invalid dialog window size.\n");
      return RSERR;
    }

    dlg.SetInitSize(dx, dy);
  }

  dlg.DoModal();
  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_htmldialog
int CDOSLibApp::ads_dos_htmldialog()
{
  CAdsArgs args;

  CString strFileName, strOptions;
  COleSafeArray saArgs;
  bool bModeless = false;

  if (!args.GetString(strFileName))
    return RSERR;

  wchar_t szFileName[512] = L"";
  if (RTNORM != DOS_FindFile(strFileName, szFileName, 512))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetSafeArray(saArgs, true))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetString(strOptions))
        return RSERR;

      args.Next();

      if (!args.IsEmpty())
      {
        if (!args.GetBoolean(bModeless))
          return RSERR;
      }
    }
  }

  CDosHtmlDialogEx dlg(szFileName);
  dlg.SetArguments(saArgs.Detach());
  dlg.SetOptions(strOptions);

  if (bModeless)
  {
    bool rc = dlg.DoModeless();
    if (rc)
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    bool rc = dlg.DoModal();
    if (rc)
    {
      COleVariant va = dlg.GetResult();
      if (CAdsRetList::IsArray(va))
      {
        CAdsRetList result;
        if (result.Add(va))
          acedRetList(result);
        else
          acedRetNil();
      }
      else
      {
        const VARIANT* pva = &va;
        while (pva->vt == (VT_BYREF | VT_VARIANT))
          pva = pva->pvarVal;

        switch (pva->vt)
        {
        case VT_BOOL:
          (pva->boolVal == VARIANT_TRUE) ? acedRetT() : acedRetNil();
          break;
        case VT_I2:
          acedRetInt((int)pva->iVal);
          break;
        case VT_I4:
          acedRetInt((int)pva->lVal);
          break;
        case VT_R4:
          acedRetReal((double)pva->fltVal);
          break;
        case VT_R8:
          acedRetReal((double)pva->dblVal);
          break;
        case VT_BSTR:
        {
          CString str = pva->bstrVal;
          acedRetStr(str);
        }
        break;
        default:
          acedRetNil();
          return false;
        }
      }
    }
    else
      acedRetNil();
  }

  saArgs.Clear();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_splash
int CDOSLibApp::ads_dos_splash()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  if (!strPath.FileExists())
  {
    acutPrintf(L"DOSLib error: file does not exist.\n");
    return RSERR;
  }

  args.Next();

  int nDuration = 0;
  if (!args.GetInteger(nDuration))
    return RSERR;

  args.Next();

  BOOL bKillOnClick = FALSE;
  if (!args.IsEmpty())
    bKillOnClick = TRUE;

  if (nDuration <= 0)
  {
    acutPrintf(L"DOSLib error: invalid duration argument.\n");
    return RSERR;
  }

  UINT uDuration = (UINT)(nDuration * 1000);

  CDosSplash* pSplash = 0;
  if (bKillOnClick)
    pSplash = new CDosSplash(strPath, uDuration, CDosSplash::KillOnClick | CDosSplash::IgnoreCmdLine, &pSplash);
  else
    pSplash = new CDosSplash(strPath, uDuration, CDosSplash::IgnoreCmdLine, &pSplash);

  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_checklist
int CDOSLibApp::ads_dos_checklist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosCheckListDialog dlg(acedGetAcadFrame());

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;
  dlg.m_Title = strTitle;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;
  dlg.m_Prompt = strPrompt;

  args.Next();

  if (!args.IsListBegin())
    return RSERR;

  args.Next();

  while (!args.IsListEnd())
  {
    if (!args.IsListBegin())
      return RSERR;

    args.Next();

    CString strString;
    if (!args.GetString(strString))
      return RSERR;
    dlg.m_StrArray.Add(strString);

    args.Next();

    bool bEnabled = true;
    if (!args.GetBoolean(bEnabled))
      return RSERR;
    dlg.m_nArray.Add(bEnabled ? TRUE : FALSE);

    args.Next();

    if (!args.IsDot() & !args.IsListEnd())
    {
      acutPrintf(L"DOSLib error: list end expected.\n");
      return RSERR;
    }

    args.Next();
  }

  dlg.m_bShowAddButton = false;
  if (args.Next())
    dlg.m_bShowAddButton = true;

  if (dlg.DoModal() != IDOK)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;
  result.AddLB();

  int i;
  for (i = 0; i < dlg.m_nArray.GetCount(); i++)
  {
    result.AddLB();
    result.Add(dlg.m_StrArray[i]);
    result.AddDot();
    result.Add((short)dlg.m_nArray[i]);
    result.AddLE();
  }
  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_listbox
int CDOSLibApp::ads_dos_listbox()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  CStringArray strarray;
  if (!args.GetStringList(strarray))
    return RSERR;

  args.Next();

  CString strDefault;
  if (!args.IsEmpty())
  {
    if (!args.GetString(strDefault))
      return RSERR;
  }

  CDosListBoxDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_StrArray.Append(strarray);
  dlg.m_Default = strDefault;
  if (dlg.DoModal() == IDOK)
    acedRetStr(dlg.m_Val);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dwgpreview
int CDOSLibApp::ads_dos_dwgpreview()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CDosPathString strPath;
  GetCurrentDirectory(_MAX_PATH, strPath.GetBuffer(_MAX_PATH));
  strPath.ReleaseBuffer();
  strPath.RemoveBackslash();

  unsigned long nColor = 0;
  CStringArray strarray;

  if (!args.IsEmpty())
  {
    if (args.IsString())
    {
      if (!args.GetPathString(strPath))
        return RSERR;
      strPath.FullPath();
      strPath.RemoveBackslash();
    }
    else
    {
      if (!args.GetStringList(strarray))
        return RSERR;
    }

    args.Next();

    if (args.IsEmpty())
      nColor = 0x00000000;
    else
      nColor = 0x00FFFFFF;
  }

  CDosDwgPreviewDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Color = nColor;
  if (strarray.GetCount() > 0)
  {
    dlg.m_StrArray.Append(strarray);
    dlg.m_Prompt = L"Select a drawing file.";
  }
  else
  {
    dlg.m_Prompt = strPath;
  }

  if (dlg.DoModal() == IDOK)
    acedRetStr(dlg.m_Prompt);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_editbox
int CDOSLibApp::ads_dos_editbox()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle, strPrompt, strString;

  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  if (args.IsString())
  {
    if (!args.GetString(strString))
      return RSERR;
  }

  strString.Remove('\r');
  strString.Replace(L"\n", L"\r\n");

  CDosEditBoxDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_String = strString;
  if (dlg.DoModal() == IDOK)
  {
    strString = dlg.m_String;
    strString.Remove('\r');
    acedRetStr(strString);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_proplist
int CDOSLibApp::ads_dos_proplist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosPropListDialog dlg(acedGetAcadFrame());

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;
  dlg.m_Title = strTitle;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;
  dlg.m_Prompt = strPrompt;

  args.Next();

  if (!args.IsListBegin())
    return RSERR;

  args.Next();

  while (!args.IsListEnd())
  {
    if (!args.IsListBegin())
      return RSERR;

    args.Next();

    CString strString;
    if (!args.GetString(strString))
      return RSERR;
    dlg.m_StrArray.Add(strString);

    args.Next();

    CString strValue;
    if (!args.GetString(strValue, true))
      return RSERR;
    dlg.m_ValArray.Add(strValue);

    args.Next();

    if (!args.IsDot() & !args.IsListEnd())
    {
      acutPrintf(L"DOSLib error: list end expected.\n");
      return RSERR;
    }

    args.Next();
  }

  if (dlg.DoModal() != IDOK)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;
  result.AddLB();

  int i;
  for (i = 0; i < dlg.m_ValArray.GetCount(); i++)
  {
    result.AddLB();
    result.Add(dlg.m_StrArray[i]);
    result.AddDot();
    result.Add(dlg.m_ValArray[i], true);
    result.AddLE();
  }
  result.AddLE();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_multilist
int CDOSLibApp::ads_dos_multilist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle, strPrompt;
  CStringArray string_array;
  CArray<int, int> selected_array;

  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  if (!args.GetStringList(string_array))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (args.Is2dPoint())
    {
      ads_point pt;
      if (!args.GetPoint(pt))
        return RSERR;
      selected_array.Add((int)pt[0]);
      selected_array.Add((int)pt[1]);
    }
    else if (args.Is3dPoint())
    {
      ads_point pt;
      if (!args.GetPoint(pt))
        return RSERR;
      selected_array.Add((int)pt[0]);
      selected_array.Add((int)pt[1]);
      selected_array.Add((int)pt[2]);
    }
    else
    {
      if (!args.GetIntegerList(selected_array))
        return RSERR;
    }
  }

  CDosMultiListDialog dlg(acedGetAcadFrame());
  dlg.m_title = strTitle;
  dlg.m_prompt = strPrompt;
  dlg.m_string_array.Append(string_array);
  dlg.m_selected_array.Append(selected_array);

  if (dlg.DoModal() != IDOK)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  int i;
  for (i = 0; i < dlg.m_result_array.GetCount(); i++)
    result.Add(dlg.m_result_array[i]);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_combolist
int CDOSLibApp::ads_dos_combolist()
{
  CAdsArgs args;
  CAcModuleResourceOverride resOverride;

  CString strTitle, strPrompt, strDefault;
  CStringArray string_array;

  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  if (!args.GetStringList(string_array))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetString(strDefault))
      return RSERR;
  }

  CDosComboListDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_StrArray.Append(string_array);
  dlg.m_Default = strDefault;

  if (dlg.DoModal() == IDOK)
  {
    if (dlg.m_Val.IsEmpty())
      acedRetNil();
    else
      acedRetStr(dlg.m_Val);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_editlist
int CDOSLibApp::ads_dos_editlist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle, strPrompt;
  CStringArray string_array;

  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  if (!args.GetStringList(string_array))
    return RSERR;

  CDosEditListDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_StrArray.Append(string_array);

  if (dlg.DoModal() != IDOK)
  {
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;

  int i;
  for (i = 0; i < dlg.m_StrArray.GetCount(); i++)
    result.Add(dlg.m_StrArray[i]);

  if (result.IsEmpty())
    result.Add(false);

  acedRetList(result);

  return RSRSLT;
}

class CDosPopupMenuEx : public CDosPopupMenu
{
public:
  CDosPopupMenuEx(const CStringArray& items);
  void SetModes(const CArray<int, int>& modes);
  void OnInitMenu(HWND hWnd, HMENU hMenu);

private:
  bool m_have_modes;
  CArray<int, int> m_modes;
};

CDosPopupMenuEx::CDosPopupMenuEx(const CStringArray& items)
{
  m_have_modes = false;
  int i;
  for (i = 0; i < items.GetCount(); i++)
  {
    CString str = items[i];
    str.TrimLeft();
    str.TrimRight();
    if (str.IsEmpty())
      AddSeperator();
    else
      AddItem(str);
  }
}

void CDosPopupMenuEx::SetModes(const CArray<int, int>& modes)
{
  m_modes.Copy(modes);
  m_have_modes = true;
}

void CDosPopupMenuEx::OnInitMenu(HWND hWnd, HMENU hMenu)
{
  if (m_have_modes)
  {
    int i, mode = 0;
    for (i = 0; i < m_modes.GetCount(); i++)
    {
      // modes: 0 = enabled, 1 = disabled, 2 = checked, 3 = disabled & checked
      mode = m_modes[i];
      if (mode > -1 && mode < 4)
      {
        if (mode & 1)
          ::EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
        if (mode & 2)
          ::CheckMenuItem(hMenu, i, MF_BYPOSITION | MF_CHECKED);
      }
    }
  }
}

////////////////////////////////////////////////////////////////
// dos_popupmenu
int CDOSLibApp::ads_dos_popupmenu()
{
  CAdsArgs args;

  CStringArray items;
  if (!args.GetStringList(items))
    return RSERR;

  args.Next();

  CArray<int, int> modes;
  bool bHaveModes = false;
  if (!args.IsEmpty())
  {
    bHaveModes = args.GetIntegerList(modes);
  }

  POINT p2d;
  ::GetCursorPos(&p2d);
  CWnd* pWnd = acedGetAcadFrame();
  if (pWnd)
    pWnd->ScreenToClient(&p2d);

  CDosPopupMenuEx menu(items);
  if (bHaveModes)
    menu.SetModes(modes);

  int rc = menu.PopUp(pWnd, p2d, p2d);
  if (rc > -1)
    acedRetInt(rc);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_msgbox
int CDOSLibApp::ads_dos_msgbox()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strText;
  if (!args.GetString(strText))
    return RSERR;

  args.Next();

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  int nButtons = 0;
  if (!args.GetInteger(nButtons))
    return RSERR;

  args.Next();

  int nIcon = 0;
  if (!args.GetInteger(nIcon))
    return RSERR;

  args.Next();

  bool bUseExtended = false;
  int nDuration = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(nDuration))
      return RSERR;

    bUseExtended = true;
  }

  UINT uButton = 0;
  switch (nButtons)
  {
  case 0:
    uButton = MB_ABORTRETRYIGNORE;
    break;
  case 1:
    uButton = MB_OK;
    break;
  case 2:
    uButton = MB_OKCANCEL;
    break;
  case 3:
    uButton = MB_RETRYCANCEL;
    break;
  case 4:
    uButton = MB_YESNO;
    break;
  case 5:
    uButton = MB_YESNOCANCEL;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  UINT uIcon = 0;
  switch (nIcon)
  {
  case -1:
    break;
  case 0:
    uIcon = MB_ICONASTERISK;
    break;
  case 1:
    uIcon = MB_ICONEXCLAMATION;
    break;
  case 2:
    uIcon = MB_ICONHAND;
    break;
  case 3:
    uIcon = MB_ICONINFORMATION;
    break;
  case 4:
    uIcon = MB_ICONQUESTION;
    break;
  case 5:
    uIcon = MB_ICONSTOP;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  int nReturn = 0;
  if (bUseExtended == false)
    nReturn = MessageBox(adsw_acadMainWnd(), strText, strTitle, uIcon | uButton | MB_APPLMODAL);
  else
  {
    CDosMessageBox* pMsgBox = new CDosMessageBox(adsw_acadMainWnd(), strText, strTitle, uIcon | uButton | MB_APPLMODAL, (UINT)nDuration * 1000, FALSE);
    nReturn = pMsgBox->DoMessageBox();
    delete pMsgBox;
    pMsgBox = 0;
  }

  switch (nReturn)
  {
  case IDABORT:
    acedRetInt(0);
    break;
  case IDCANCEL:
    acedRetInt(1);
    break;
  case IDIGNORE:
    acedRetInt(2);
    break;
  case IDNO:
    acedRetInt(3);
    break;
  case IDOK:
    acedRetInt(4);
    break;
  case IDRETRY:
    acedRetInt(5);
    break;
  case IDYES:
    acedRetInt(6);
    break;
  default:
    acedRetNil();
    break;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_msgboxex
//
int CDOSLibApp::ads_dos_msgboxex()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strText;
  if (!args.GetString(strText))
    return RSERR;

  args.Next();

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CStringArray strarray;
  if (!args.GetStringList(strarray))
    return RSERR;

  if (strarray.GetCount() > 5)
  {
    acutPrintf(L"DOSLib error: too many button strings.\n");
    return RSERR;
  }

  args.Next();

  int nIcon = -1;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(nIcon))
      return RSERR;
  }

  CDosMessageDialog dlg(acedGetAcadFrame());

  switch (nIcon)
  {
  case -1:
    break;
  case 0:
    dlg.SetStandardIcon(IDI_ASTERISK);
    break;
  case 1:
    dlg.SetStandardIcon(IDI_EXCLAMATION);
    break;
  case 2:
    dlg.SetStandardIcon(IDI_HAND);
    break;
  case 3:
    dlg.SetStandardIcon(IDI_ASTERISK);
    break;
  case 4:
    dlg.SetStandardIcon(IDI_QUESTION);
    break;
  case 5:
    dlg.SetStandardIcon(IDI_HAND);
    break;
  default:
    ads_printf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  dlg.SetTitle(strTitle);
  dlg.SetRtf(FALSE);
  dlg.SetMsg(strText);

  int i;
  for (i = 0; i < strarray.GetCount(); i++)
  {
    BOOL bIsDefault = (i == 0) ? TRUE : FALSE;
    BOOL bIsEscape = (i == strarray.GetCount() - 1) ? TRUE : FALSE;
    dlg.AddButton(i + 1, bIsDefault, bIsEscape, strarray[i]);
  }

  int rc = (int)dlg.DoModal();

  acedRetInt(rc - 1);

  return RSRSLT;
}

// This function is a wrapper for the shell function SHMessageBoxCheck.
int MessageBoxCheck(HWND hwnd, LPCTSTR pszText, LPCTSTR pszTitle, UINT uType, int iDefault, LPCTSTR pszRegVal)
{
  typedef int (WINAPI* PSHMBC) (HWND, LPCTSTR, LPCTSTR, UINT, int, LPCTSTR);
  int rc = -1;
  WORD ordinal = (sizeof(TCHAR) == sizeof(WCHAR) ? 191 : 185);

  HMODULE hShlwapi = LoadLibrary(TEXT("shlwapi.dll"));
  if (hShlwapi)
  {
    PSHMBC shMessageBoxCheck = (PSHMBC)GetProcAddress(hShlwapi, (LPCSTR)ordinal);
    if (shMessageBoxCheck)
    {
      // For some reason SHMessageBoxCheck seems to require that the thread has a
      // message queue to work correctly. So we call PeekMessage to force the
      // creation of a message queue.
      MSG msg;
      PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
      rc = shMessageBoxCheck(hwnd, pszText, pszTitle, uType, iDefault, pszRegVal);
    }
    FreeLibrary(hShlwapi);
  }

  return rc;
}

////////////////////////////////////////////////////////////////
// dos_msgboxchk
int CDOSLibApp::ads_dos_msgboxchk()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strText;
  if (!args.GetString(strText))
    return RSERR;

  args.Next();

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  int nButtons = 0;
  if (!args.GetInteger(nButtons))
    return RSERR;

  args.Next();

  int nIcon = 0;
  if (!args.GetInteger(nIcon))
    return RSERR;

  args.Next();

  int nDefault = 0;
  if (!args.GetInteger(nDefault))
    return RSERR;

  args.Next();

  CString strRegVal;
  if (!args.GetString(strRegVal))
    return RSERR;

  UINT uButton = 0;
  switch (nButtons)
  {
  case 0:
    uButton = MB_ABORTRETRYIGNORE;
    break;
  case 1:
    uButton = MB_OK;
    break;
  case 2:
    uButton = MB_OKCANCEL;
    break;
  case 3:
    uButton = MB_RETRYCANCEL;
    break;
  case 4:
    uButton = MB_YESNO;
    break;
  case 5:
    uButton = MB_YESNOCANCEL;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  UINT uIcon = 0;
  switch (nIcon)
  {
  case -1:
    break;
  case 0:
    uIcon = MB_ICONASTERISK;
    break;
  case 1:
    uIcon = MB_ICONEXCLAMATION;
    break;
  case 2:
    uIcon = MB_ICONHAND;
    break;
  case 3:
    uIcon = MB_ICONINFORMATION;
    break;
  case 4:
    uIcon = MB_ICONQUESTION;
    break;
  case 5:
    uIcon = MB_ICONSTOP;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  if (nDefault >= -1 && nDefault <= 6)
  {
    acutPrintf(L"DOSLib error: invalid argument.\n");
    return RSERR;
  }

  CString str;
  str.Format(L"doslib_%s", strRegVal);
  int nReturn = MessageBoxCheck(adsw_acadMainWnd(), strText, strTitle, uIcon | uButton | MB_APPLMODAL, nDefault, str);

  switch (nReturn)
  {
  case IDABORT:
    acedRetInt(0);
    break;
  case IDCANCEL:
    acedRetInt(1);
    break;
  case IDIGNORE:
    acedRetInt(2);
    break;
  case IDNO:
    acedRetInt(3);
    break;
  case IDOK:
    acedRetInt(4);
    break;
  case IDRETRY:
    acedRetInt(5);
    break;
  case IDYES:
    acedRetInt(6);
    break;
  default:
    acedRetInt(nReturn);
    break;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_linetype
//

/*
  Description:
    Displays AutoCAD's linetype selection dialog
  Parameters:
    old_linetypeId         [in]  - ObjectId for current linetype: acdbCurDwg()->celtype()
    bIncludeByBlockByLayer [in]  - Display "bylayer" and "byblock" items
    linetype_name,         [out] - Name of selected linetype
    new_linetypeId         [out] - ObjectId for selected linetype
  Returns:
    true or false
*/
#if defined(ZRX20) || defined(ZRX21)
#define acedLinetypeDialog zcedLinetypeDialog
#endif
bool __cdecl acedLinetypeDialog(
class AcDbObjectId old_linetypeId,
  bool bIncludeByBlockByLayer,
  wchar_t*& linetype_name,
class AcDbObjectId& new_linetypeid
  );

int CDOSLibApp::ads_dos_linetypebox()
{
  CString strLinetype;
  bool bInclude = false;

  CAdsArgs args;

  if (!args.IsEmpty())
  {
    if (!args.IsNil() && !args.GetString(strLinetype))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetBoolean(bInclude))
        return RSERR;
    }
  }

  // find the linetype id
  AcDbObjectId old_linetypeId;
  if (!strLinetype.IsEmpty())
  {
    AcDbLinetypeTable* pLinetypeTbl = 0;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLinetypeTbl, AcDb::kForRead);
    if (pLinetypeTbl)
    {
      // Create a new iterator that starts at table beginning and skips deleted.
      AcDbLinetypeTableIterator* pLtIterator = 0;
      pLinetypeTbl->newIterator(pLtIterator);
      if (pLtIterator)
      {
        // Walk the table, getting every table record
        AcDbLinetypeTableRecord* pLtTableRcd = 0;
        wchar_t* pLtName = 0;
        for (; !pLtIterator->done(); pLtIterator->step())
        {
          pLtIterator->getRecord(pLtTableRcd, AcDb::kForRead);
          pLtTableRcd->getName(pLtName);
          int rc = strLinetype.CompareNoCase(pLtName);
          free(pLtName);
          if (rc == 0)
            old_linetypeId = pLtTableRcd->objectId();
          pLtTableRcd->close();
          if (rc == 0)
            break;
        }
        delete pLtIterator;
        pLinetypeTbl->close();
      }
    }
  }
  else
  {
    old_linetypeId = acdbCurDwg()->celtype();
  }

  AcDbObjectId new_linetypeId;
  wchar_t* linetype_name = 0;
  if (acedLinetypeDialog(old_linetypeId, bInclude, linetype_name, new_linetypeId) && linetype_name)
  {
    acedRetStr(linetype_name);
    free(linetype_name);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getdate
int CDOSLibApp::ads_dos_getdate()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CTime start = CTime::GetCurrentTime();

  if (!args.IsEmpty())
  {
    CArray<int, int> t;
    if (!args.GetIntegerList(t) || t.GetCount() < 3)
      return RSERR;

    start = CTime(t[0], t[1], t[2], 0, 0, 0);
  }

  CDosGetDateDialog dlg;
  dlg.m_title = strTitle;
  dlg.m_start = start;

  if (dlg.DoModal() == IDOK)
  {
    CAdsRetList result;
    result.Add(dlg.m_time.GetYear());
    result.Add(dlg.m_time.GetMonth());
    result.Add(dlg.m_time.GetDay());
    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_orderlist
int CDOSLibApp::ads_dos_orderlist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  CStringArray strarray;
  if (!args.GetStringList(strarray))
    return RSERR;

  args.Next();

  bool bStrings = true;
  if (!args.IsEmpty())
    bStrings = false;

  CDosOrderListDialog dlg(acedGetAcadFrame());
  dlg.m_Title = strTitle;
  dlg.m_Prompt = strPrompt;
  dlg.m_StrArray.Append(strarray);
  if (dlg.DoModal() == IDOK)
  {
    CAdsRetList result;

    if (bStrings)
    {
      for (int i = 0; i < dlg.m_StrArray.GetCount(); i++)
        result.Add(dlg.m_StrArray[i]);
    }
    else
    {
      for (int i = 0; i < dlg.m_IntArray.GetCount(); i++)
        result.Add((int)dlg.m_IntArray[i]);
    }

    if (result.IsEmpty())
      acedRetNil();
    else
      acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_duallist
int CDOSLibApp::ads_dos_duallist()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CString strPrompt;
  if (!args.GetString(strPrompt))
    return RSERR;

  args.Next();

  CStringArray strarray1;
  if (args.IsNil())
  {
  }
  else
  {
    if (!args.GetStringList(strarray1, true))
      return RSERR;
  }

  args.Next();

  CStringArray strarray2;
  if (args.IsNil())
  {
  }
  else
  {
    if (!args.GetStringList(strarray2, true))
      return RSERR;
  }

  args.Next();

  CString strPromptList1, strPromptList2;
  if (!args.IsEmpty())
  {
    if (!args.GetString(strPromptList1))
      return RSERR;

    args.Next();

    if (!args.GetString(strPromptList2))
      return RSERR;
  }

  CDosDualListDialog dlg(acedGetAcadFrame());
  dlg.m_title = strTitle;
  dlg.m_prompt = strPrompt;
  dlg.m_prompt_list1 = strPromptList1;
  dlg.m_prompt_list2 = strPromptList2;
  dlg.m_list1.Append(strarray1);
  dlg.m_list2.Append(strarray2);
  if (dlg.DoModal() == IDOK)
  {
    int i;

    CAdsRetList result;
    result.AddLB();

    result.AddLB();
    for (i = 0; i < dlg.m_list1.GetCount(); i++)
      result.Add(dlg.m_list1[i]);
    result.AddLE();

    result.AddLB();
    for (i = 0; i < dlg.m_list2.GetCount(); i++)
      result.Add(dlg.m_list2[i]);
    result.AddLE();

    result.AddLE();

    acedRetList(result);
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}