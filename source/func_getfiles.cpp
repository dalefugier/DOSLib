/////////////////////////////////////////////////////////////////////////////
// func_getfiles.cpp
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
#include "DosFolderDialog.h"
#include "DosFileDialog.h"
#include "DosStringArray.h"

////////////////////////////////////////////////////////////////
// dos_getfilem
int CDOSLibApp::ads_dos_getfilem()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CDosPathString strInitialDir;
  if (!args.GetPathString(strInitialDir))
    return RSERR;
  strInitialDir.RemoveBackslash();

  args.Next();

  CString strFilter;
  if (!args.GetString(strFilter))
    return RSERR;

  CDosFileDialog dlg(
    NULL,
    NULL,
    OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
    strFilter,
    AcUiMainWindow()
    );

  dlg.m_ofn.nFilterIndex = 1;
  dlg.m_ofn.lpstrInitialDir = strInitialDir;
  dlg.m_ofn.lpstrTitle = strTitle;

  CAdsRetList result;

  if (dlg.DoModal() == IDOK)
  {
    POSITION pos = dlg.GetStartPosition();
    if (pos != 0)
    {
      // For whatever reason, CFileDialog returns the last picked object
      // as the first object in the list. So, we'll sort the list
      // before returning.

      CDosPathString str = dlg.GetNextPathName(pos);
      result.Add(str.GetPath(true));

      CDosStringArray files;
      files.Add(str.GetFileName());

      while (pos != 0)
      {
        str = dlg.GetNextPathName(pos);
        files.Add(str.GetFileName());
      }

      files.Sort();

      int i;
      for (i = 0; i < files.GetCount(); i++)
        result.Add(files[i]);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getfiled
int CDOSLibApp::ads_dos_getfiled()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CDosPathString strInitialDir;
  if (!args.GetPathString(strInitialDir))
    return RSERR;
  strInitialDir.RemoveBackslash();

  args.Next();

  CString strFilter;
  if (!args.GetString(strFilter))
    return RSERR;

  CFileDialog dlg(
    TRUE, 0, 0,
    OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
    strFilter,
    AcUiMainWindow()
    );

  dlg.m_ofn.nFilterIndex = 1;
  dlg.m_ofn.lpstrInitialDir = strInitialDir;
  dlg.m_ofn.lpstrTitle = strTitle;

  if (dlg.DoModal() == IDOK)
    acedRetStr(dlg.GetPathName());
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getdir
int CDOSLibApp::ads_dos_getdir()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strCaption = L"Browse for folder";
  CString strTitle = L"Please select a folder below. Then click OK.";
  bool bNewFolderButton = false;

  CDosPathString strRoot;
  GetCurrentDirectory(_MAX_PATH, strRoot.GetBuffer(_MAX_PATH));
  strRoot.ReleaseBuffer();

  if (!args.IsEmpty())
  {
    if (!args.GetString(strCaption))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetPathString(strRoot))
        return RSERR;
      strRoot.RemoveBackslash();

      args.Next();

      if (!args.IsEmpty())
      {
        if (!args.GetString(strTitle))
          return RSERR;

        args.Next();

        // https://github.com/dalefugier/DOSLib/issues/16
        //if (!args.IsEmpty())
        if (args.IsTrue())
          bNewFolderButton = true;
      }
    }
  }

  UINT ulFlags = 0;
  if (bNewFolderButton)
    ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE;
  else
    ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;

  CDosFolderDialog dlg(adsw_acadMainWnd());
  dlg.SetDialogCaption(strCaption);
  dlg.SetDialogTitle(strTitle);
  dlg.SetDialogFolder(strRoot);
  dlg.SetDialogFlags(ulFlags);
  if (dlg.DoModal() == IDOK)
  {
    CDosPathString strFolder = dlg.GetFolderName();
    if (!strFolder.IsRoot())
      strFolder.AddBackslash();
    acedRetStr(strFolder);
  }
  else
    acedRetNil();

  return RSRSLT;
}

static int ResbufToStringArray(struct resbuf* result, CStringArray& a)
{
  struct resbuf* rb = result;
  while (rb)
  {
    if (rb->restype == RTSTR)
    {
      CString s(rb->resval.rstring);
      a.Add(s);
    }
    rb = rb->rbnext;
  }
  return (int)a.GetCount();
}

////////////////////////////////////////////////////////////////
// dos_getfilenav
int CDOSLibApp::ads_dos_getfilenav()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CString strTitle;
  if (!args.GetString(strTitle))
    return RSERR;

  args.Next();

  CDosPathString strDefault;
  if (!args.GetPathString(strDefault))
    return RSERR;

  // 1-May-2007 - Do not add backslash to end of path string.
  // Users can control this by setting flag value 256 (bit 8)
  // strDefault.AddBackslash();

  args.Next();

  CString strExt;
  if (!args.GetString(strExt))
    return RSERR;

  args.Next();

  int flags = 0;
  if (!args.GetInteger(flags))
    return RSERR;

  struct resbuf* rb = acutNewRb(RTSTR);
  if (!rb)
    return RSERR;

  if (RTNORM == acedGetFileNavDialog(strTitle, strDefault, strExt, L"DOSLib:GetFileNav", flags, &rb))
  {
    if (flags & 2048)
    {
      CDosPathString str(rb->resval.rstring);
      str.AddBackslash();
      acedRetStr(str);
    }
    else if (flags & 4096)
    {
      CStringArray strarray;
      int i, count = ResbufToStringArray(rb, strarray);
      if (count > 0)
      {
        CAdsRetList result;
        CDosPathString str = strarray[0];
        result.Add(str.GetPath(true));
        result.Add(str.GetFileName());
        for (i = 1; i < count; i++)
        {
          str = strarray[i];
          result.Add(str.GetFileName());
        }
        acedRetList(result);
      }
      else
      {
        acedRetNil();
      }
    }
    else
    {
      CString str(rb->resval.rstring);
      acedRetStr(str);
    }
  }
  else
  {
    acedRetNil();
  }

  acutRelRb(rb); // final release

  return RSRSLT;
}
