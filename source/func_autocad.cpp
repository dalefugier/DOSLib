/////////////////////////////////////////////////////////////////////////////
// func_autocad.cpp
//
// Copyright ?1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosLayerListDialog.h"
#include "DosStringArray.h"

#if defined(ARX21) || defined(ARX22) || defined(ARX23) || defined(ARX24) || defined(BRX19)  || defined(BRX20)
extern int acdbSetDbmod(class AcDbDatabase* pDb, int newVal);
#else
extern long acdbSetDbmod(class AcDbDatabase* pDb, long newVal);
#endif

using namespace Gdiplus;

static int GetEncoderClsid(const wchar_t* pFormat, CLSID* pClsid)
{
  if (nullptr == pFormat || nullptr == pClsid)
    return -1;

  unsigned int count = 0;  // number of image encoders
  unsigned int size = 0;   // size of the image encoder array in bytes
  GetImageEncodersSize(&count, &size);
  if (0 == size)
    return -1;

  ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
  if (nullptr == pImageCodecInfo)
    return -1;

  GetImageEncoders(count, size, pImageCodecInfo);

  for (unsigned int i = 0; i < count; ++i)
  {
    if (wcscmp(pImageCodecInfo[i].MimeType, pFormat) == 0)
    {
      *pClsid = pImageCodecInfo[i].Clsid;
      free(pImageCodecInfo);
      return i;
    }
  }

  free(pImageCodecInfo);
  return -1;
}

////////////////////////////////////////////////////////////////
// dos_extractpreview
int CDOSLibApp::ads_dos_extractpreview()
{
  CAdsArgs args;

  AcApDocument* pDoc = acDocManager->curDocument();
  if (!pDoc)
    return RSERR;

  CDosPathString strFile = pDoc->fileName();
  CDosPathString strBitmap = strFile;
  strBitmap.RenameExtension(L".bmp");

  if (!args.IsEmpty())
  {
    if (!args.GetString(strBitmap))
      return RSERR;

    strBitmap.FullPath();

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetString(strFile))
        return RSRSLT;

      strFile.FullPath();
      if (!strFile.FileExists())
        return RSRSLT;
    }
  }

  wchar_t drive[_MAX_DRIVE];
  wchar_t dir[_MAX_DIR];
  wchar_t fname[_MAX_FNAME];
  wchar_t ext[_MAX_EXT];
  _wsplitpath_s(strBitmap, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  CString strExt(ext);

  int nFormat = 0;
  if (strExt.CompareNoCase(L".bmp") == 0)
    nFormat = 0;
  else if (strExt.CompareNoCase(L".jpg") == 0)
    nFormat = 1;
  else if (strExt.CompareNoCase(L".jpeg") == 0)
    nFormat = 1;
  else if (strExt.CompareNoCase(L".gif") == 0)
    nFormat = 2;
  else if (strExt.CompareNoCase(L".tiff") == 0)
    nFormat = 3;
  else if (strExt.CompareNoCase(L".tif") == 0)
    nFormat = 3;
  else if (strExt.CompareNoCase(L".png") == 0)
    nFormat = 4;
  else
  {
    acutPrintf(L"DOSLib error: unknown bitmap format.\n");
    acedRetNil();
    return RSRSLT;
  }

  CLSID encoder;
  int rc;

  if (nFormat == 0)
    rc = GetEncoderClsid(L"image/bmp", &encoder);
  else if (nFormat == 1)
    rc = GetEncoderClsid(L"image/jpeg", &encoder);
  else if (nFormat == 2)
    rc = GetEncoderClsid(L"image/gif", &encoder);
  else if (nFormat == 3)
    rc = GetEncoderClsid(L"image/tiff", &encoder);
  else if (nFormat == 4)
    rc = GetEncoderClsid(L"image/png", &encoder);
  else
  {
    acutPrintf(L"DOSLib error: invalid bitmap format.\n");
    acedRetNil();
    return RSRSLT;
  }

  if (rc == -1)
  {
    acedRetNil();
    return RSRSLT;
  }

  HBITMAP hBitmap = 0;
  HPALETTE hPal = 0;
  if (!acdbGetPreviewBitmapFromDwg(strFile, &hBitmap, &hPal))
  {
    acedRetNil();
    return RSRSLT;
  }

  CStringW wstrBitmap = strBitmap;
  Bitmap image(hBitmap, hPal);
  image.Save(wstrBitmap, &encoder, NULL);

  DeleteObject(hBitmap);
  DeleteObject(hPal);

  acedRetStr(strBitmap);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_layerlistbox
int CDOSLibApp::ads_dos_layerlistbox()
{
  CAdsArgs args;
  CAcModuleResourceOverride myResources;

  CDosLayerListDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));

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

  int nFlags = 0;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(nFlags))
      return RSERR;
    nFlags = abs(nFlags);
  }

  if (nFlags & 1) dlg.HideOnLayers();
  if (nFlags & 2) dlg.HideOffLayers();
  if (nFlags & 4) dlg.HideThawedLayers();
  if (nFlags & 8) dlg.HideFrozenLayers();
  if (nFlags & 16) dlg.HideUnlockedLayers();
  if (nFlags & 32) dlg.HideLockedLayers();
  if (nFlags & 64) dlg.HideStdLayers();
  if (nFlags & 128) dlg.HideXrefLayers();
  if (nFlags & 256) dlg.SetMultipleSelection();
  if (nFlags & 512) dlg.HidePlotLayers();
  if (nFlags & 1024) dlg.HideNoPlotLayers();
  if (nFlags & 2048) dlg.HideUsedLayers();
  if (nFlags & 4096) dlg.HideUnusedLayers();

  if (dlg.DoModal() == IDCANCEL)
  {
    acedRetNil();
    return RSRSLT;
  }

  if (dlg.MultipleSelection())
  {
    CAdsRetList result;

    int i;
    for (i = 0; i < dlg.m_layers.GetCount(); i++)
      result.Add(dlg.m_layers[i]);

    if (result.IsEmpty())
      acedRetNil();
    else
      acedRetList(result);
  }
  else
  {
    acedRetStr(dlg.m_layer);
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_closeall
int CDOSLibApp::ads_dos_closeall()
{
  AcApDocumentIterator* pIterator = acDocManager->newAcApDocumentIterator();
  if (!pIterator)
    return RSERR;

  AcApDocument* pDoc = 0;
  for (; !pIterator->done(); pIterator->step())
  {
    pDoc = pIterator->document();
    if (pDoc)
      acDocManager->sendStringToExecute(pDoc, L"\3\3_.close", false, true);
    else
    {
      delete pIterator;
      pIterator = 0;
      return RSERR;
    }
  }

  delete pIterator;
  pIterator = 0;

  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_saveall
int CDOSLibApp::ads_dos_saveall()
{
  AcApDocumentIterator* pIterator = acDocManager->newAcApDocumentIterator();
  if (!pIterator)
    return RSERR;

  AcApDocument* pDoc = 0;
  for (; !pIterator->done(); pIterator->step())
  {
    pDoc = pIterator->document();
    if (pDoc)
      acDocManager->sendStringToExecute(pDoc, L"\3\3_.qsave ", false, true);
    else
    {
      delete pIterator;
      pIterator = 0;
      return RSERR;
    }
  }

  delete pIterator;
  pIterator = 0;

  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isbreak
int CDOSLibApp::ads_dos_isbreak()
{
  DOS_Pump();
  if (acedUsrBrk())
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_lisplist
int CDOSLibApp::ads_dos_lisplist()
{
  CAdsArgs args;

  bool bFullPath = false;
  if (!args.IsEmpty())
    bFullPath = true;

  AcApDocument* pDoc = curDoc();
  if (!pDoc)
    return RSERR;

  CAdsRetList result;

  int i;
  for (i = 0; i < pDoc->GetCountOfLispList(); i++)
  {
    AcLispAppInfo* pInfo = pDoc->GetItemOfLispList(i);
    if (!pInfo)
      continue;

    CString strPath = pInfo->appFileName;

    if (!bFullPath)
    {
      wchar_t drive[_MAX_DRIVE];
      wchar_t dir[_MAX_DIR];
      wchar_t fname[_MAX_FNAME];
      wchar_t ext[_MAX_EXT];
      _wsplitpath_s(strPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
      strPath = fname;
      strPath += ext;
    }

    result.Add(strPath);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_arxlist
int CDOSLibApp::ads_dos_arxlist()
{
  CAdsArgs args;

  bool bFullPath = false;
  if (!args.IsEmpty())
    bFullPath = true;

  CAdsRetList result;

  struct resbuf *rb1 = 0, *rb2 = 0;
  for (rb2 = rb1 = acedArxLoaded(); rb2 != 0; rb2 = rb2->rbnext)
  {
    if (rb2->restype == RTSTR)
    {
      CString str(rb2->resval.rstring);
      if (bFullPath)
      {
        HMODULE hModule = GetModuleHandle(str);
        if (hModule != NULL)
        {
          TCHAR tchFileName[_MAX_PATH];
          DWORD dwSize = _MAX_PATH;
          DWORD dwResult = GetModuleFileName(hModule, tchFileName, dwSize);
          if (dwResult)
            result.Add(tchFileName);
        }
      }
      else
      {
        result.Add(str);
      }
    }
  }
  acutRelRb(rb1);

  if (result.IsEmpty())
    return acedRetNil();
  else
    return acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_progbar
int CDOSLibApp::ads_dos_progbar()
{
  CAdsArgs args;

  // Finish
  if (args.IsEmpty())
  {
    acedRestoreStatusBar();
    acedRetNil();

    return RSRSLT;
  }

  // Step it
  if (args.IsInteger())
  {
    int nPos = 0;
    if (!args.GetInteger(nPos))
      return RSERR;

    if (acedSetStatusBarProgressMeterPos(nPos) == 0)
      acedRetT();
    else
      acedRetNil();

    return RSRSLT;
  }

  // Start
  CString strLabel;
  if (!args.GetString(strLabel))
    return RSERR;

  args.Next();

  int nMaxPos = 0;
  if (!args.GetInteger(nMaxPos))
    return RSERR;

  if (acedSetStatusBarProgressMeter(strLabel, 0, nMaxPos) == 0)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_show
int CDOSLibApp::ads_dos_show()
{
  CAdsArgs args;

  int nCmdShow;
  if (!args.GetInteger(nCmdShow))
    return RSERR;

  switch (nCmdShow)
  {
  case 0:
    nCmdShow = SW_SHOWMAXIMIZED;
    break;
  case 1:
    nCmdShow = SW_SHOWMINIMIZED;
    break;
  case 2:
    nCmdShow = SW_RESTORE;
    break;
  default:
    acutPrintf(L"DOSLib error: invalid argument.\n");
    acedRetNil();
    return RSRSLT;
  }

  ::ShowWindow(adsw_acadMainWnd(), nCmdShow);
  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cmdline
int CDOSLibApp::ads_dos_cmdline()
{
  CAdsArgs args;

  CWnd* pDock = acedGetAcadTextCmdLine();
  if (!pDock)
  {
    acedRetNil();
    return RSRSLT;
  }

  CWnd* pChild = pDock->GetWindow(GW_CHILD);
  if (!pChild)
  {
    acedRetNil();
    return RSRSLT;
  }

  CString str;
  if (args.IsEmpty())
  {
    pChild->GetWindowText(str);
    acedRetStr(str);
  }
  else
  {
    pChild = pChild->GetNextWindow();
    if (pChild)
    {
      pChild->GetWindowText(str);
      acedRetStr(str);
    }
    else
    {
      acedRetNil();
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_istextscr
int CDOSLibApp::ads_dos_istextscr()
{
  CAdsArgs args;

  CWnd* pDock = (CEdit*)acedGetAcadTextCmdLine();
  if (!pDock)
  {
    acedRetNil();
    return RSRSLT;
  }

  CWnd* pChild = pDock->GetWindow(GW_CHILD);
  if (!pChild)
  {
    acedRetNil();
    return RSRSLT;
  }

  pChild = pChild->GetNextWindow();
  if (pChild && pChild->IsWindowVisible())
  {
    // AutoCAD never minimized the text window.  Instead, they move it way off of the 
    // screen.  Thus, we we cannot intersect the screen rect with the window rect, 
    // then the text window must be "pseudo-minimized."

    CRect rect, window_rect, screen_rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    pChild->GetWindowRect(window_rect);
    if (rect.IntersectRect(screen_rect, window_rect))
      acedRetT();
    else
      acedRetNil();
  }
  else
  {
    acedRetNil();
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_curvearea
int CDOSLibApp::ads_dos_curvearea()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = NULL;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead))
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
  if (pCurve == NULL)
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a curve.\n");
    return RSERR;
  }

  double area = 0.0;
  if (Adesk::kFalse == pCurve->isClosed() || Acad::eOk != pCurve->getArea(area))
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a closed curve.\n");
    return RSERR;
  }

  pEnt->close();
  acedRetReal(area);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_massprops
int CDOSLibApp::ads_dos_massprops()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = NULL;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead))
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  AcDb3dSolid* pSolid = AcDb3dSolid::cast(pEnt);
  if (pSolid == NULL)
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a solid.\n");
    return RSERR;
  }

  double volume;
  AcGePoint3d centroid;
  double momInertia[3];
  double prodInertia[3];
  double prinMoments[3];
  AcGeVector3d prinAxes[3];
  double radiiGyration[3];
  AcDbExtents extents;

  if (pSolid->getMassProp(volume, centroid, momInertia, prodInertia, prinMoments, prinAxes, radiiGyration, extents) == Acad::eOk)
  {
    struct resbuf* prbResult = acutBuildList(
      RTREAL, volume,
      RTLB,
      RTREAL, centroid[X],
      RTREAL, centroid[Y],
      RTREAL, centroid[Z],
      RTLE,
      RTLB,
      RTREAL, momInertia[X],
      RTREAL, momInertia[Y],
      RTREAL, momInertia[Z],
      RTLE,
      RTLB,
      RTREAL, prodInertia[X],
      RTREAL, prodInertia[Y],
      RTREAL, prodInertia[Z],
      RTLE,
      RTLB,
      RTREAL, prinMoments[X],
      RTREAL, prinMoments[Y],
      RTREAL, prinMoments[Z],
      RTLE,
      RTLB,
      RTLB,
      RTREAL, prinAxes[X][X],
      RTREAL, prinAxes[X][Y],
      RTREAL, prinAxes[X][Z],
      RTLE,
      RTLB,
      RTREAL, prinAxes[Y][X],
      RTREAL, prinAxes[Y][Y],
      RTREAL, prinAxes[Y][Z],
      RTLE,
      RTLB,
      RTREAL, prinAxes[Z][X],
      RTREAL, prinAxes[Z][Y],
      RTREAL, prinAxes[Z][Z],
      RTLE,
      RTLE,
      RTLB,
      RTREAL, radiiGyration[X],
      RTREAL, radiiGyration[Y],
      RTREAL, radiiGyration[Z],
      RTLE,
      RTLB,
      RTLB,
      RTREAL, extents.minPoint()[X],
      RTREAL, extents.minPoint()[Y],
      RTREAL, extents.minPoint()[Z],
      RTLE,
      RTLB,
      RTREAL, extents.maxPoint()[X],
      RTREAL, extents.maxPoint()[Y],
      RTREAL, extents.maxPoint()[Z],
      RTLE,
      RTLE,
      0);

    acedRetList(prbResult);
    acutRelRb(prbResult);
  }
  else
  {
    acedRetNil();
  }

  pEnt->close();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isvlide
int CDOSLibApp::ads_dos_isvlide()
{
  CWnd* pWnd = CWnd::FindWindow(L"VisualLisp", 0);
  if (pWnd && pWnd->IsWindowVisible())
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acadmem
int CDOSLibApp::ads_dos_acadmem()
{
  DWORD dwProcessID = GetCurrentProcessId();
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
  if (hProcess)
  {
    PROCESS_MEMORY_COUNTERS pmc;
    ZeroMemory(&pmc, sizeof(PROCESS_MEMORY_COUNTERS));
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
      CAdsRetList result;
      result.Add((int)pmc.WorkingSetSize);
      result.Add((int)pmc.PeakWorkingSetSize);
      result.Add((int)pmc.PageFaultCount);
      result.Add((int)pmc.PagefileUsage);
      result.Add((int)pmc.PeakPagefileUsage);
      result.Add((int)pmc.QuotaPagedPoolUsage);
      result.Add((int)pmc.QuotaPeakPagedPoolUsage);
      result.Add((int)pmc.QuotaNonPagedPoolUsage);
      result.Add((int)pmc.QuotaPeakNonPagedPoolUsage);
      acedRetList(result);
    }
    else
      acedRetNil();
    CloseHandle(hProcess);
  }
  else
    acedRetNil();

  return RSRSLT;
}

static void ReadXrefs(AcDbDatabase* db)
{
  if (!db)
    return;

  CAdsRetList result;

  AcDbBlockTable* pBlockTable = NULL;
  AcDbBlockTableIterator* pBlockTableIterator = NULL;
  AcDbBlockTableRecord* pBlockRecord = NULL;

  Acad::ErrorStatus es = db->getSymbolTable(pBlockTable, AcDb::kForRead);
  if (es == Acad::eOk)
  {
    es = pBlockTable->newIterator(pBlockTableIterator, true, true);
    if (es == Acad::eOk)
    {
      for (; !pBlockTableIterator->done(); pBlockTableIterator->step())
      {
        es = pBlockTableIterator->getRecord(pBlockRecord, AcDb::kForRead, false);
        if (es == Acad::eOk)
        {
          bool bIsXRef = pBlockRecord->isFromExternalReference();
          if (bIsXRef == true)
          {
            wchar_t* pszPath = NULL;
            pBlockRecord->pathName(pszPath);
            result.Add(pszPath);
            free(pszPath);
          }
          pBlockRecord->close();
        }
      }
    }
    pBlockTable->close();
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);
}

////////////////////////////////////////////////////////////////
// dos_xreflist
int CDOSLibApp::ads_dos_xreflist()
{
  CAdsArgs args;

  if (args.IsEmpty())
  {
    ReadXrefs(acdbHostApplicationServices()->workingDatabase());
    return RSRSLT;
  }

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  bool bReadOnly = false;
  AcDbDatabase db(Adesk::kFalse);

#if defined(ARX21) || defined(ARX22) || defined(ARX23) || defined(ARX24)
  Acad::ErrorStatus es = db.readDwgFile(strPath, AcDbDatabase::kForReadAndReadShare, false);
#else
  Acad::ErrorStatus es = db.readDwgFile(strPath, _SH_DENYWR, false);
#endif

  if (es != Acad::eOk)
  {
    if (es == Acad::eFileInternalErr)
    {
      TCHAR tchTempPath[_MAX_PATH];
      TCHAR tchTempName[_MAX_PATH];

      GetTempPath(_MAX_PATH, tchTempPath);
      GetTempFileName(tchTempPath, L"dwg", 0, tchTempName);

      if (!CopyFile(strPath, tchTempName, FALSE))
      {
        acedRetNil();
        return RSRSLT;
      }

#if defined(ARX21) || defined(ARX22) || defined(ARX23) || defined(ARX24)
      es = db.readDwgFile(tchTempName, AcDbDatabase::kForReadAndReadShare, false);
#else
      es = db.readDwgFile(tchTempName, _SH_DENYWR, false);
#endif

      if (es != Acad::eOk)
      {
        DeleteFile(tchTempName);
        acedRetNil();
        return RSRSLT;
      }

      bReadOnly = true;
      strPath = tchTempName;
    }
  }

  ReadXrefs(&db);

  if (bReadOnly)
  {
    DeleteFile(strPath);
  }

  return RSRSLT;
}


static int GetNearestACI(int red, int green, int blue)
{
  int minndx = 0;
  int mindst = 2147483647L;
  int i;
  for (i = 1; i < 255; i++)
  {
    int acirgb = acedGetRGB(i);
    int r = (acirgb & 0xffL);
    int g = (acirgb & 0xff00L) >> 8;
    int b = acirgb >> 16;
    int dst = abs(r - red) + abs(g - green) + abs(b - blue);
    if (dst < mindst)
    {
      minndx = i;
      mindst = dst;
    }
  }
  return minndx;
}

static int GetNearestACI(int rgb)
{
  int r = GetRValue(rgb);
  int g = GetGValue(rgb);
  int b = GetBValue(rgb);
  return GetNearestACI(r, g, b);
}

////////////////////////////////////////////////////////////////
// dos_rgbtogray
int CDOSLibApp::ads_dos_rgbtogray()
{
  CAdsArgs args;

  int r = 0, g = 0, b = 0;

  if (!args.GetInteger(r))
    return RSERR;

  args.Next();
  if (args.IsEmpty())
  {
    if (r < 0 || r > RGB(255, 255, 255))
    {
      acutPrintf(L"DOSLib error: invalid rgb value.\n");
      return RSERR;
    }

    int aci = GetNearestACI(r);
    acedRetInt(aci);
    return RSRSLT;
  }

  if (!args.GetInteger(g))
    return RSERR;

  args.Next();

  if (!args.GetInteger(b))
    return RSERR;

  if (r < 0 || r > 255)
  {
    acutPrintf(L"DOSLib error: invalid red value.\n");
    return RSERR;
  }

  if (g < 0 || g > 255)
  {
    acutPrintf(L"DOSLib error: invalid green value.\n");
    return RSERR;
  }

  if (b < 0 || b > 255)
  {
    acutPrintf(L"DOSLib error: invalid blue value.\n");
    return RSERR;
  }

  COLORREF cr = (COLORREF)((r * 0.30) + (g * 0.59) + (b * 0.11));

  CAdsRetList result;
  result.Add((short)cr);
  result.Add((short)cr);
  result.Add((short)cr);
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rgbtoaci
int CDOSLibApp::ads_dos_rgbtoaci()
{
  CAdsArgs args;

  int r = 0, g = 0, b = 0;

  if (!args.GetInteger(r))
    return RSERR;

  args.Next();
  if (args.IsEmpty())
  {
    if (r < 0 || r > RGB(255, 255, 255))
    {
      acutPrintf(L"DOSLib error: invalid rgb value.\n");
      return RSERR;
    }

    int aci = GetNearestACI(r);
    acedRetInt(aci);
    return RSRSLT;
  }

  if (!args.GetInteger(g))
    return RSERR;

  args.Next();

  if (!args.GetInteger(b))
    return RSERR;

  if (r < 0 || r > 255)
  {
    acutPrintf(L"DOSLib error: invalid red value.\n");
    return RSERR;
  }

  if (g < 0 || g > 255)
  {
    acutPrintf(L"DOSLib error: invalid green value.\n");
    return RSERR;
  }

  if (b < 0 || b > 255)
  {
    acutPrintf(L"DOSLib error: invalid blue value.\n");
    return RSERR;
  }

  int aci = GetNearestACI(r, g, b);
  acedRetInt(aci);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acitorgb
int CDOSLibApp::ads_dos_acitorgb()
{
  CAdsArgs args;

  int nColor;
  if (!args.GetInteger(nColor))
    return RSERR;

  if (nColor < 1 && nColor > 255)
  {
    acutPrintf(L"DOSLib error: invalid aci value.\n");
    return RSERR;
  }

  COLORREF cr = acedGetRGB(nColor);
  CAdsRetList result;
  result.Add((short)GetRValue(cr));
  result.Add((short)GetGValue(cr));
  result.Add((short)GetBValue(cr));
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rgbtohls
int CDOSLibApp::ads_dos_rgbtohls()
{
  CAdsArgs args;

  int r = 0, g = 0, b = 0;

  if (!args.GetInteger(r))
    return RSERR;

  args.Next();
  if (args.IsEmpty())
  {
    if (r < 0 || r > RGB(255, 255, 255))
    {
      acutPrintf(L"DOSLib error: invalid rgb value.\n");
      return RSERR;
    }

    WORD h = 0, l = 0, s = 0;
    ColorRGBToHLS(r, &h, &l, &s);

    CAdsRetList result;
    result.Add((short)h);
    result.Add((short)l);
    result.Add((short)s);
    acedRetList(result);
    return RSRSLT;
  }

  if (!args.GetInteger(g))
    return RSERR;

  args.Next();

  if (!args.GetInteger(b))
    return RSERR;

  if (r < 0 || r > 255)
  {
    acutPrintf(L"DOSLib error: invalid red value.\n");
    return RSERR;
  }

  if (g < 0 || g > 255)
  {
    acutPrintf(L"DOSLib error: invalid green value.\n");
    return RSERR;
  }

  if (b < 0 || b > 255)
  {
    acutPrintf(L"DOSLib error: invalid blue value.\n");
    return RSERR;
  }

  WORD h = 0, l = 0, s = 0;
  ColorRGBToHLS(RGB(r, g, b), &h, &l, &s);

  CAdsRetList result;
  result.Add((short)h);
  result.Add((short)l);
  result.Add((short)s);
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hlstorgb
int CDOSLibApp::ads_dos_hlstorgb()
{
  CAdsArgs args;

  int h = 0, l = 0, s = 0;

  if (!args.GetInteger(h))
    return RSERR;

  args.Next();

  if (!args.GetInteger(l))
    return RSERR;

  args.Next();

  if (!args.GetInteger(s))
    return RSERR;

  if (h < 0 || h > 360)
  {
    acutPrintf(L"DOSLib error: invalid hue value.\n");
    return RSERR;
  }

  if (l < 0)
  {
    acutPrintf(L"DOSLib error: invalid luminance value.\n");
    return RSERR;
  }

  if (s < 0)
  {
    acutPrintf(L"DOSLib error: invalid saturation value.\n");
    return RSERR;
  }

  COLORREF cr = ColorHLSToRGB((WORD)h, (WORD)l, (WORD)s);

  CAdsRetList result;
  result.Add((short)GetRValue(cr));
  result.Add((short)GetGValue(cr));
  result.Add((short)GetBValue(cr));
  acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dwgver
int CDOSLibApp::ads_dos_dwgver()
{
  CAdsArgs args;

  CDosPathString strPath;
  if (!args.GetPathString(strPath))
    return RSERR;

  strPath.FullPath();

  AcDbDatabase* pDb = new AcDbDatabase(Adesk::kFalse, Adesk::kTrue);
  Acad::ErrorStatus es = pDb->readDwgFile(strPath);
  if (es != Acad::eOk)
  {
    delete pDb;
    acutPrintf(L"DOSLib error: cannot read drawing file.\n");
    acedRetNil();
    return RSRSLT;
  }

  // Get the DWG version number.
  AcDb::AcDbDwgVersion dwgVer = pDb->originalFileVersion();

  delete pDb;
  pDb = NULL;

  CString str;

  switch (dwgVer)
  {
  case AcDb::kDHL_MC0_0:
    str = L"AutoCAD 1.0";
    break;
  case AcDb::kDHL_AC1_2:
    str = L"AutoCAD 1.2";
    break;
  case AcDb::kDHL_AC1_40:
    str = L"AutoCAD 1.40";
    break;
  case AcDb::kDHL_AC1_50:
    str = L"AutoCAD 1.50";
    break;
  case AcDb::kDHL_AC2_20:
    str = L"AutoCAD 2.20";
    break;
  case AcDb::kDHL_AC2_10:
    str = L"AutoCAD 2.10";
    break;
  case AcDb::kDHL_AC2_21:
    str = L"AutoCAD 2.21";
    break;
  case AcDb::kDHL_AC2_22:
  case AcDb::kDHL_1001:
    str = L"AutoCAD 2.22";
    break;
  case AcDb::kDHL_1002:
    str = L"AutoCAD 2.5";
    break;
  case AcDb::kDHL_1003:
    str = L"AutoCAD 2.6";
    break;
  case AcDb::kDHL_1004:
  case AcDb::kDHL_1005:
    str = L"Release 9";
    break;
  case AcDb::kDHL_1006:
  case AcDb::kDHL_1007:
  case AcDb::kDHL_1008:
    str = L"Release 10";
    break;
  case AcDb::kDHL_1009:
  case AcDb::kDHL_1010:
  case AcDb::kDHL_1011:
    str = L"Release 11/12";
    break;
  case AcDb::kDHL_1012:
    str = L"Release 13";
    break;
  case AcDb::kDHL_1013:
  case AcDb::kDHL_1014:
    str = L"Release 14";
    break;
  case AcDb::kDHL_1500:
  case AcDb::kDHL_1015:
    str = L"AutoCAD 2000";
    break;
  case AcDb::kDHL_1800a:
  case AcDb::kDHL_1800:
    str = L"AutoCAD 2004";
    break;
  case AcDb::kDHL_2100a:
  case AcDb::kDHL_1021:
    str = L"AutoCAD 2007";
    break;
  case AcDb::kDHL_2400a:
  case AcDb::kDHL_1024:
    str = L"AutoCAD 2010";
    break;
  case AcDb::kDHL_2700a:
  case AcDb::kDHL_1027:
    str = L"AutoCAD 2013";
    break;
#if _ARX_VER > 21 || _BRX_VER > 17
  case AcDb::kDHL_3200a:
  case AcDb::kDHL_1032:
    str = L"AutoCAD 2018";
    break;
#endif
  default:
    str = L"Unknown";
  }

  acedRetStr(str);

  return RSRSLT;
}

static bool ListSorterEx(struct resbuf* rb, CAdsRetList& result)
{
  if (0 == rb)
    return false;

  CDosNumberArray numbers;
  CDosStringArray strings;

  while (rb)
  {
    switch (rb->restype)
    {
    case RTSTR:
      strings.Add(CString(rb->resval.rstring));
      break;

    case RTSHORT:
      numbers.Add((double)rb->resval.rint);
      break;

    case RTLONG:
      numbers.Add((double)rb->resval.rlong);
      break;

    case RTREAL:
      numbers.Add(rb->resval.rreal);
      break;

    case RTPOINT:
      numbers.Add(rb->resval.rpoint[0]);
      numbers.Add(rb->resval.rpoint[1]);
      break;

    case RT3DPOINT:
      numbers.Add(rb->resval.rpoint[0]);
      numbers.Add(rb->resval.rpoint[1]);
      numbers.Add(rb->resval.rpoint[2]);
      break;
    }

    rb = rb->rbnext;
  }

  if (0 == numbers.GetCount() && 0 == strings.GetCount())
    return false;

  if (numbers.GetCount() > 1)
    numbers.Sort();

  if (strings.GetCount() > 1)
    strings.Sort();

  int i;

  for (i = 0; i < numbers.GetCount(); i++)
    result.Add(numbers[i]);

  for (i = 0; i < strings.GetCount(); i++)
    result.Add(strings[i]);

  return true;
}

// Obsolete
static struct resbuf* ListSorter(struct resbuf* ab)
{
  int fi1 = 0, fi2 = 0, totcnt = 0;
  struct resbuf* link, *j, *p, *t, *rb, *rbb;

  // Make a copy of the link list (ab)
  for (t = rb = 0, rbb = ab; rbb; rbb = rbb->rbnext)
  {
    if (0 == t)
      t = rb = acutNewRb(rbb->restype);
    else
    {
      rb->rbnext = acutNewRb(rbb->restype);
      rb = rb->rbnext;
    }

    switch (rb->restype)
    {
    case RTSTR:
      if (rbb->resval.rstring == 0)
        break;
      rb->resval.rstring = new wchar_t[wcslen(rbb->resval.rstring) + 1];
      wcscpy(rb->resval.rstring, rbb->resval.rstring);
      ++fi1;
      break;

    case RTSHORT:
      rb->resval.rint = rbb->resval.rint;
      ++fi1;
      break;

    case RTREAL:
      rb->resval.rreal = rbb->resval.rreal;
      ++fi1;
      break;
    }
  }

  link = t;
  if (link == 0)
    return 0;

  for (rb = link; rb; rbb = rb, rb = rb->rbnext)
  {
    if (rb->restype == RTLB)
    {
      rbb = link;
      link = rb = link->rbnext;
      rbb->rbnext = 0;
      acutRelRb(rbb);
    }
    else if (rb->restype == RTLE)
    {
      rbb->rbnext = 0;
      acutRelRb(rb);
      rb = 0;
      break;
    }
  }

  for (fi1 = -1, totcnt = 0; fi1 < totcnt; ++fi1)
  {
    int type = link->restype;
    for (fi2 = 0, p = j = link; j->rbnext; ++fi2)
    {
      if (j->restype != type)
        return 0;

      switch (j->restype)
      {
      case RTSHORT:
        if (j->rbnext->restype != RTSHORT)
        {
          j = j->rbnext;
          break;
        }
        if (j->resval.rint > j->rbnext->resval.rint)
        {
          t = j->rbnext;
          p->rbnext = j->rbnext;
          j->rbnext = j->rbnext->rbnext;
          t->rbnext = j;
          if (j == link)
            p = link = t;
          else p = t;
        }
        else
        {
          p = j;
          j = j->rbnext;
        }
        break;

      case RTREAL:
        if (j->rbnext->restype != RTREAL)
        {
          j = j->rbnext;
          break;
        }
        if (j->resval.rreal > j->rbnext->resval.rreal)
        {
          t = j->rbnext;
          p->rbnext = j->rbnext;
          j->rbnext = j->rbnext->rbnext;
          t->rbnext = j;
          if (j == link)
            p = link = t;
          else
            p = t;
        }
        else
        {
          p = j;
          j = j->rbnext;
        }
        break;

      case RTSTR:
        if (j->rbnext->restype != RTSTR)
        {
          j = j->rbnext;
          break;
        }
        if (wcscmp(j->resval.rstring, j->rbnext->resval.rstring) >= 0)
        {
          t = j->rbnext;
          p->rbnext = j->rbnext;
          j->rbnext = j->rbnext->rbnext;
          t->rbnext = j;
          if (j == link)
            p = link = t;
          else
            p = t;
        }
        else
        {
          p = j;
          j = j->rbnext;
        }
        break;

      default:
        j = j->rbnext;
        break;

      }

      if ((totcnt && fi2 >= totcnt - fi1) || j == 0)
        break;
    }

    if (!totcnt)
      totcnt = fi2;
  }

  return link;
}

////////////////////////////////////////////////////////////////
// dos_sortlist
int CDOSLibApp::ads_dos_sortlist()
{
  CAdsArgs args;

  if (args.IsEmpty())
    return RSERR;

  CAdsRetList result;
  bool rc = ListSorterEx(args.Resbuf(), result);
  if (rc)
    acedRetList(result);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_showobjects
int CDOSLibApp::ads_dos_showobjects()
{
  AcDbObjectIdArray arr;

  CAdsArgs args;
  if (args.IsEmpty())
  {
    arr.setPhysicalLength(acdbHostApplicationServices()->workingDatabase()->approxNumObjects());

    AcDbBlockTable* pTable = 0;
    acdbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, AcDb::kForRead);
    if (0 == pTable)
      return RSERR;

    AcDbBlockTableRecord* pRecord = 0;
    pTable->getAt(ACDB_MODEL_SPACE, pRecord, AcDb::kForRead);
    pTable->close();
    if (0 == pRecord)
      return RSERR;

    AcDbBlockTableRecordIterator* pIter = 0;
    for (pRecord->newIterator(pIter); !pIter->done(); pIter->step())
    {
      AcDbObjectId id;
      pIter->getEntityId(id);
      arr.append(id);
    }

    delete pIter;
    pIter = 0;

    pRecord->close();
  }
  else if (args.IsSelectionSet())
  {
    if (!args.GetEntityList(arr))
      return RSERR;
  }
  else if (args.IsEntity())
  {
    AcDbObjectId e;
    if (!args.GetEntity(e))
      return RSERR;
    arr.append(e);
  }
  else
    return RSERR;

  AcDbEntity* pEnt = 0;
  int i, count = 0;
  for (i = 0; i < arr.length(); i++)
  {
    if (Acad::eOk == acdbOpenAcDbEntity(pEnt, arr[i], AcDb::kForWrite))
    {
      if (AcDb::kInvisible == pEnt->visibility())
      {
        pEnt->setVisibility(AcDb::kVisible);
        count++;
      }
      pEnt->close();
    }
  }

  acedRetInt(count);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hideobjects
int CDOSLibApp::ads_dos_hideobjects()
{
  CAdsArgs args;

  AcDbObjectIdArray arr;
  if (args.IsSelectionSet())
  {
    if (!args.GetEntityList(arr))
      return RSERR;
  }
  else if (args.IsEntity())
  {
    AcDbObjectId e;
    if (!args.GetEntity(e))
      return RSERR;
    arr.append(e);
  }
  else
    return RSERR;

  AcDbEntity* pEnt = 0;
  int i, count = 0;
  for (i = 0; i < arr.length(); i++)
  {
    if (Acad::eOk == acdbOpenAcDbEntity(pEnt, arr[i], AcDb::kForWrite))
    {
      if (AcDb::kVisible == pEnt->visibility())
      {
        pEnt->setVisibility(AcDb::kInvisible);
        count++;
      }
      pEnt->close();
    }
  }

  acedRetInt(count);

  return RSRSLT;
}

enum IncidenceType
{
  kIncidenceToLeft = 0,
  kIncidenceToRight = 1,
  kIncidenceToFront = 2,
  kIncidenceUnknown
};

IncidenceType CurveIncidence(AcDbCurve* pCurve, double param, AcGeVector3d dir, AcGeVector3d normal)
{
  if (!pCurve)
    return kIncidenceUnknown;

  AcGeVector3d deriv1;
  pCurve->getFirstDeriv(param, deriv1);
  if (deriv1.isParallelTo(dir))
  {
    // need second degree analysis
    AcGeVector3d deriv2;
    pCurve->getSecondDeriv(param, deriv2);
    if (deriv2.isZeroLength() || deriv2.isParallelTo(dir))
      return kIncidenceToFront;
    else if (deriv2.crossProduct(dir).dotProduct(normal) < 0)
      return kIncidenceToRight;
    else
      return kIncidenceToLeft;
  }

  if (deriv1.crossProduct(dir).dotProduct(normal) < 0)
    return kIncidenceToLeft;
  else
    return kIncidenceToRight;
}

Adesk::Boolean IsInsideCurve(AcDbCurve* pCurve, AcGePoint3d testPt)
{
  if (!pCurve || !pCurve->isClosed())
    return Adesk::kFalse;

  AcDb2dPolyline* p2dPoly = AcDb2dPolyline::cast(pCurve);
  if (p2dPoly != NULL && p2dPoly->polyType() != AcDb::k2dSimplePoly)
    return Adesk::kFalse; // Not supported

  AcGePoint3d ptOnCurve;
  Acad::ErrorStatus es = pCurve->getClosestPointTo(testPt, ptOnCurve);
  if (testPt == ptOnCurve)
    return Adesk::kTrue;

  // check its planar
  AcGePlane plane;
  AcDb::Planarity planarity;
  es = pCurve->getPlane(plane, planarity);
  if (es != Acad::eOk || planarity != AcDb::kPlanar)
    return Adesk::kFalse;

  // make the test ray from the plane
  AcGeVector3d normal = plane.normal();
  AcGeVector3d testVector = normal.perpVector();
  AcDbRay ray;
  ray.setBasePoint(testPt);
  ray.setUnitDir(testVector);

  AcGePoint3dArray IntersectionPoints;
  es = pCurve->intersectWith(&ray, AcDb::kOnBothOperands, IntersectionPoints);
  if (es != Acad::eOk)
    return Adesk::kFalse;

  int numberOfInters = IntersectionPoints.length();
  if (numberOfInters == 0)
    return Adesk::kFalse; // must be outside

  int i, nGlancingHits = 0;
  double epsilon = 2e-6; // trust me on this...
  for (i = 0; i < numberOfInters; i++)
  {
    // get the first point, and get its parameter
    AcGePoint3d hitPt = IntersectionPoints[i];
    double hitParam = 0.0;
    es = pCurve->getParamAtPoint(hitPt, hitParam);
    if (es != Acad::eOk)
      return Adesk::kFalse;

    double inParam = hitParam - epsilon;
    double outParam = hitParam + epsilon;

    IncidenceType inIncidence = CurveIncidence(pCurve, inParam, testVector, normal);
    IncidenceType outIncidence = CurveIncidence(pCurve, outParam, testVector, normal);
    if ((inIncidence == kIncidenceToRight && outIncidence == kIncidenceToLeft) ||
      (inIncidence == kIncidenceToLeft && outIncidence == kIncidenceToRight))
    {
      nGlancingHits++;
    }
  }

  return ((numberOfInters + nGlancingHits) % 2 == 1);
}

////////////////////////////////////////////////////////////////
// dos_isinsidecurve
int CDOSLibApp::ads_dos_isinsidecurve()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  args.Next();

  ads_point pt;
  if (!args.GetPoint(pt))
    return RSERR;

  AcDbEntity* pEnt = NULL;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead))
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
  if (pCurve == NULL)
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a curve.\n");
    return RSERR;
  }

  AcGePoint3d testPt(pt[0], pt[1], pt[2]);
  Adesk::Boolean rc = IsInsideCurve(pCurve, testPt);
  pEnt->close();

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_plinecentroid
int CDOSLibApp::ads_dos_plinecentroid()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = NULL;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead))
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  // Check if entity type is polyline or not (AcDb2dpolyline or AcDbpolyline)
  if (!pEnt->isKindOf(AcDb2dPolyline::desc()) && !pEnt->isKindOf(AcDbPolyline::desc()))
  {
    acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
    pEnt->close();
    return RSERR;
  }

  if (pEnt->isKindOf(AcDb2dPolyline::desc()) && !((AcDb2dPolyline*)pEnt)->isClosed())
  {
    acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
    pEnt->close();
    return RSERR;
  }

  if (pEnt->isKindOf(AcDbPolyline::desc()) && !((AcDbPolyline*)pEnt)->isClosed())
  {
    acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
    pEnt->close();
    return RSERR;
  }

  AcDbVoidPtrArray curves;
  curves.append(pEnt);
  pEnt->close();

  AcDbVoidPtrArray regions;
  if (Acad::eOk != AcDbRegion::createFromCurves(curves, regions))
  {
    acedRetNil();
    return RSRSLT;
  }

  AcDbRegion* pReg = AcDbRegion::cast((AcRxObject*)regions[0]);
  if (0 == pReg)
  {
    acedRetNil();
    return RSRSLT;
  }

  AcGePlane plane;
  pReg->getPlane(plane);

  AcGePoint3d origin;
  AcGeVector3d xAxis;
  AcGeVector3d yAxis;
  plane.getCoordSystem(origin, xAxis, yAxis);

  double perimeter, area;
  AcGePoint2d centroid;
  double momInertia[2];
  double prodInertia = 0.0;
  double prinMoments[2];
  AcGeVector2d prinAxes[2];
  double radiiGyration[2];
  AcGePoint2d extentsLow, extentsHigh;

  pReg->getAreaProp(
    origin,
    xAxis,
    yAxis,
    perimeter,
    area,
    centroid,
    momInertia,
    prodInertia,
    prinMoments,
    prinAxes,
    radiiGyration,
    extentsLow,
    extentsHigh
  );

  AcGeVector3d normal;
  pReg->getNormal(normal);
  pReg->close();

  AcGeMatrix3d mat;
  mat.setCoordSystem(origin, xAxis, yAxis, normal);

  AcGePoint3d cpt(centroid[0], centroid[1], 0.0);
  cpt.transformBy(mat);

  ads_point pt;
  pt[0] = cpt.x;
  pt[1] = cpt.y;
  pt[2] = cpt.z;

  acedRetPoint(pt);
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hatcharea
int CDOSLibApp::ads_dos_hatcharea()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = 0;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead) || 0 == pEnt)
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  if (!pEnt->isKindOf(AcDbHatch::desc()))
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a hatch.\n");
    return RSERR;
  }

  AcDbHatch* pHatch = AcDbHatch::cast(pEnt);
  if (0 == pHatch)
  {
    pEnt->close();
    acutPrintf(L"DOSLib error: entity not a hatch.\n");
    return RSERR;
  }

  double total_area = 0.0;
  int i, loop_count = pHatch->numLoops();
  for (i = 0; i < loop_count; i++)
  {
    double area = 0.0;
#if defined(ARX21) || defined(ARX22) || defined(ARX23) || defined(ARX24) || defined(ZRX20) || defined(ZRX21)
    Adesk::Int32 loopType = 0;
#else
    long loopType = 0;
#endif

    if (pHatch->loopTypeAt(i) & AcDbHatch::kPolyline)
    {
      AcGePoint2dArray vertices;
      AcGeDoubleArray bulges;
      pHatch->getLoopAt(i, loopType, vertices, bulges);
      int vx, nVertices = vertices.length();
      AcDbPolyline testPoly(nVertices);
      for (vx = 0; vx < nVertices; vx++)
      {
        double bulge = (bulges.length() < nVertices ? 0.0 : bulges[vx]);
        testPoly.addVertexAt(vx, vertices[vx], bulge);
      }
      testPoly.getArea(area);
    }
    else
    {
      AcGePoint2dArray vertices;
      AcGeDoubleArray bulges;
      AcGeVoidPointerArray edgePtrs;
      AcGeIntArray edgeTypes;
      pHatch->getLoopAt(i, loopType, edgePtrs, edgeTypes);
      AcGeCompositeCurve2d compCurve(edgePtrs);
      AcGeInterval interval;
      compCurve.getInterval(interval);
      compCurve.area(interval.lowerBound(), interval.upperBound(), area);
      area = fabs(area);
    }

    if (loop_count > 1 && !(loopType & AcDbHatch::kExternal))
      area = -area;

    total_area += area;
  }

  pEnt->close();

  acedRetReal(total_area);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cmdargs
int CDOSLibApp::ads_dos_cmdargs()
{
  CAdsRetList result;

  int count = 0;
  LPWSTR* pszArgs = CommandLineToArgvW(GetCommandLineW(), &count);
  if (pszArgs)
  {
    int i;
    for (i = 0; i < count; i++)
      result.Add(pszArgs[i]);

    LocalFree(pszArgs);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getsecret

ACHAR* g_result = 0; // global result
int g_length = 0;    // global result length
int g_cronly = 0;    // global cronly

#if defined(ARX23) || defined(ARX24) || defined(ZRX21)
static bool MessageFilter(MSG* pMsg)
#else
static BOOL MessageFilter(MSG* pMsg)
#endif
{
  if (pMsg && pMsg->message == WM_CHAR)
  {
    int keycode = (int)pMsg->wParam;
    if (keycode != 8 && keycode != 13 && keycode != 27 && ((keycode != 32) || g_cronly))
    {
      // keycode 8 is Backspace   
      // keycode 27 is Escape   
      // keycode 32 is Space (only avoided if cronly is used)   
      pMsg->wParam = 42;     // For Security against subsequent hooks, and display,   
      pMsg->lParam = 589825; // Set these two to reflect '*' keystroke   
      g_result[g_length] = keycode; // keep the real keycode.   
      g_length++;
    }
    if (keycode == 8 && g_length)
      g_length--;
  }
  return FALSE;
}

static int adskGetPassword(int cronly, const ACHAR* prompt, ACHAR* result)
{
  g_result = result;
  g_length = 0;
  g_cronly = cronly;

  acedRegisterFilterWinMsg(MessageFilter);
  ACHAR temp[135];
  int rc = acedGetString(cronly, prompt, temp);
  acedRemoveFilterWinMsg(MessageFilter);

  result[g_length] = '\0';

  g_result = 0;
  g_length = 0;
  g_cronly = 0;

  return rc;
}

int CDOSLibApp::ads_dos_getsecret()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int cronly = 0;
  if (!args.IsEmpty())
    cronly = 1;

  ACHAR result[135];
  if (adskGetPassword(cronly, str, result) == RTNORM)
    acedRetStr(result);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cleardbmod

int CDOSLibApp::ads_dos_cleardbmod()
{
  CAdsArgs args;

  if (args.IsEmpty())
  {
    AcApDocument* pDoc = acDocManager->curDocument();
    if (pDoc)
      acdbSetDbmod(pDoc->database(), 0);
  }
  else
  {
    AcApDocumentIterator* pIterator = acDocManager->newAcApDocumentIterator();
    if (pIterator)
    {
      AcApDocument* pDoc = 0;
      for (; !pIterator->done(); pIterator->step())
      {
        pDoc = pIterator->document();
        if (pDoc)
          acdbSetDbmod(pDoc->database(), 0);
      }
    }

    delete pIterator;
    pIterator = 0;
  }

  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_summaryinfo

static bool GetSummaryInfo(int item, CString& str)
{
  bool rc = false;
  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      ACHAR* info = 0;
      switch (item)
      {
      case 0:
        es = si->getTitle(info);
        break;
      case 1:
        es = si->getSubject(info);
        break;
      case 2:
        es = si->getAuthor(info);
        break;
      case 3:
        es = si->getKeywords(info);
        break;
      case 4:
        es = si->getComments(info);
        break;
      case 5:
        es = si->getLastSavedBy(info);
        break;
      case 6:
        es = si->getRevisionNumber(info);
        break;
      case 7:
        es = si->getHyperlinkBase(info);
        break;
      }

      if (es == Acad::eOk && info)
      {
        str = info;
        acdbFree(info);
        info = 0;
        rc = true;
      }
    }
  }
  return rc;
}

static bool SetSummaryInfo(int item, const wchar_t* info)
{
  bool rc = false;
  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      switch (item)
      {
      case 0:
        es = si->setTitle(info);
        break;
      case 1:
        es = si->setSubject(info);
        break;
      case 2:
        es = si->setAuthor(info);
        break;
      case 3:
        es = si->setKeywords(info);
        break;
      case 4:
        es = si->setComments(info);
        break;
      case 5:
        es = si->setLastSavedBy(info);
        break;
      case 6:
        es = si->setRevisionNumber(info);
        break;
      case 7:
        es = si->setHyperlinkBase(info);
        break;
      }

      if (es == Acad::eOk)
      {
        es = acdbPutSummaryInfo(si);
        if (es == Acad::eOk)
          rc = true;
      }
    }
  }
  return rc;
}

int CDOSLibApp::ads_dos_summaryinfo()
{
  CAdsArgs args;

  int item = -1;
  if (args.IsEmpty())
  {
    CAdsRetList result;
    result.AddLB();

    int i;
    for (i = 0; i < 8; i++)
    {
      result.AddLB();
      result.Add(i);
      result.AddDot();
      CString str;
      GetSummaryInfo(i, str);
      result.Add(str);
      result.AddLE();
    }
    result.AddLE();

    if (result.IsEmpty())
      acedRetNil();
    else
      acedRetList(result);

    return RSRSLT;
  }

  else if (args.IsNil())
  {
    int i;
    for (i = 0; i < 8; i++)
      SetSummaryInfo(i, 0);
    acedRetT();
    return RSRSLT;
  }

  else if (!args.GetInteger(item) || item < 0 || item > 7)
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    CString value;
    if (GetSummaryInfo(item, value))
      acedRetStr(value);
    else
      acedRetNil();
  }
  else
  {
    CString value;
    if (args.GetString(value))
    {
      if (SetSummaryInfo(item, value.IsEmpty() ? (const wchar_t*)0 : value))
        acedRetT();
      else
        acedRetNil();
    }
    else if (args.IsNil())
    {
      if (SetSummaryInfo(item, (const wchar_t*)0))
        acedRetT();
      else
        acedRetNil();
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_custominfo

static void ReturnAllCustomSummaryInfo()
{
  CAdsRetList result;

  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      int i, count = si->numCustomInfo();
      if (count)
      {
        result.AddLB();
        for (i = 0; i < count; i++)
        {
          ACHAR* key = 0;
          ACHAR* value = 0;
          es = si->getCustomSummaryInfo(i, key, value);
          if (es == Acad::eOk && key && value)
          {
            result.AddLB();
            result.Add(key);
            result.AddDot();
            result.Add(value);
            result.AddLE();
          }
          acdbFree(key);
          acdbFree(value);
        }
        result.AddLE();
      }
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);
}

static void DeleteAllCustomSummaryInfo()
{
  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      int i, count = si->numCustomInfo();
      for (i = 0; i < count; i++)
        es = si->deleteCustomSummaryInfo(i);
      es = acdbPutSummaryInfo(si);
    }
  }
}

static bool GetCustomSummaryInfo(const wchar_t* key, CString& str)
{
  bool rc = false;
  if (0 == key || 0 == key[0])
    return rc;

  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      ACHAR* value = 0;
      es = si->getCustomSummaryInfo(key, value);
      if (es == Acad::eOk && value)
      {
        str = value;
        acdbFree(value);
        rc = true;
      }
    }
  }
  return rc;
}

static bool SetCustomSummaryInfo(const wchar_t* key, const wchar_t* value)
{
  bool rc = false;
  if (0 == key || 0 == key[0])
    return rc;

  AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
  if (db)
  {
    AcDbDatabaseSummaryInfo* si = 0;
    Acad::ErrorStatus es = acdbGetSummaryInfo(db, si);
    if (es == Acad::eOk && si)
    {
      CString str(value);
      if (str.IsEmpty())
      {
        es = si->deleteCustomSummaryInfo(key);
      }
      else
      {
        ACHAR* str = 0;
        es = si->getCustomSummaryInfo(key, str);
        if (es == Acad::eOk)
        {
          acdbFree(str);
          es = si->setCustomSummaryInfo(key, value);
        }
        else
        {
          es = si->addCustomSummaryInfo(key, value);
        }
      }
      if (es == Acad::eOk)
      {
        es = acdbPutSummaryInfo(si);
        if (es == Acad::eOk)
          rc = true;
      }
    }
  }
  return rc;
}

int CDOSLibApp::ads_dos_custominfo()
{
  CAdsArgs args;

  CString key;
  if (args.IsEmpty())
  {
    ReturnAllCustomSummaryInfo();
    return RSRSLT;
  }

  else if (args.IsNil())
  {
    DeleteAllCustomSummaryInfo();
    acedRetT();
    return RSRSLT;
  }

  else if (!args.GetString(key))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    CString value;
    if (GetCustomSummaryInfo(key, value))
      acedRetStr(value);
    else
      acedRetNil();
  }
  else
  {
    CString value;
    if (args.GetString(value))
    {
      if (SetCustomSummaryInfo(key, value.IsEmpty() ? (const wchar_t*)0 : value))
        acedRetT();
      else
        acedRetNil();
    }
    else if (args.IsNil())
    {
      if (SetCustomSummaryInfo(key, (const wchar_t*)0))
        acedRetT();
      else
        acedRetNil();
    }
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_olelist

static INT_PTR GetOleLinks(const wchar_t* entry_name, CStringArray& links)
{
  if (0 == entry_name || 0 == entry_name[0])
    return 0;

  AcDbHostApplicationServices* pHAS = acdbHostApplicationServices();
  if (0 == pHAS)
    return 0;

  AcDbDatabase* pDB = pHAS->workingDatabase();
  if (0 == pDB)
    return 0;

  AcDbBlockTable* pBT = 0;
  Acad::ErrorStatus es = pDB->getBlockTable(pBT, AcDb::kForRead);
  pBT->close();
  if (Acad::eOk != es || 0 == pBT)
    return 0;

  AcDbBlockTableRecord* pBTR = 0;
  es = pBT->getAt(entry_name, pBTR, AcDb::kForRead);
  pBT->close();
  if (Acad::eOk != es || 0 == pBTR)
    return 0;

  AcDbBlockTableRecordIterator* pIter = 0;
  es = pBTR->newIterator(pIter);
  pBTR->close();
  if (Acad::eOk != es || 0 == pIter)
    return 0;

  INT_PTR link_count = links.GetCount();

  while (!pIter->done())
  {
    AcDbEntity* pEnt = 0;
    es = pIter->getEntity(pEnt, AcDb::kForRead);
    if (Acad::eOk == es && pEnt)
    {
      AcDbOle2Frame* pOle = AcDbOle2Frame::cast(pEnt);
      if (pOle)
      {
        if (OT_LINK == pOle->getType())
        {
          wchar_t szBuffer[_MAX_PATH];
          wmemset(szBuffer, 0, _MAX_PATH);
          Adesk::Boolean rc = pOle->getLinkPath(szBuffer, _MAX_PATH);
          if (Adesk::kTrue == rc)
            links.Add(szBuffer);
        }
      }
      pEnt->close();
      pIter->step();
    }
  }

  delete pIter;
  pIter = 0;

  return links.GetCount() - link_count;
}

int CDOSLibApp::ads_dos_olelist()
{
  CAdsArgs args;

  int mode = 0;

  if (args.IsEmpty())
    mode = 0;
  else
  {
    if (!args.GetInteger(mode))
      return RSERR;

    mode = DOS_CLAMP(mode, 0, 2);
  }

  CStringArray links;
  switch (mode)
  {
  case 1: // model space
    GetOleLinks(ACDB_MODEL_SPACE, links);
    break;
  case 2: // paoer space
    GetOleLinks(ACDB_PAPER_SPACE, links);
    break;
  default:
    GetOleLinks(ACDB_MODEL_SPACE, links);
    GetOleLinks(ACDB_PAPER_SPACE, links);
    break;
  }

  if (links.GetCount())
  {
    CAdsRetList result;
    int i;
    for (i = 0; i < (int)links.GetCount(); i++)
      result.Add(links[i]);
    acedRetList(result);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_imagelist

int CDOSLibApp::ads_dos_imagelist()
{
  AcDbHostApplicationServices* pHAS = acdbHostApplicationServices();
  if (0 == pHAS)
  {
    acedRetNil();
    return RSRSLT;
  }

  AcDbDatabase* pDB = pHAS->workingDatabase();
  if (0 == pDB)
  {
    acedRetNil();
    return RSRSLT;
  }

  AcDbDictionary* pDict = 0;
  Acad::ErrorStatus es = pDB->getNamedObjectsDictionary(pDict, AcDb::kForRead);
  if (Acad::eOk != es || 0 == pDict)
  {
    acedRetNil();
    return RSRSLT;
  }

  AcDbObject* pObj = 0;
  es = pDict->getAt(L"ACAD_IMAGE_DICT", pObj, AcDb::kForRead);
  pDict->close();
  if (Acad::eOk != es || 0 == pObj)
  {
    acedRetNil();
    return RSRSLT;
  }

  AcDbDictionary* pDictImg = AcDbDictionary::cast(pObj);
  if (0 == pDictImg)
  {
    pObj->close();
    acedRetNil();
    return RSRSLT;
  }

  AcDbDictionaryIterator* pIter = pDictImg->newIterator();
  if (0 == pIter)
  {
    pDictImg->close();
    acedRetNil();
    return RSRSLT;
  }

  CAdsRetList result;
  for (; !pIter->done(); pIter->next())
  {
    pObj = 0;
    es = pIter->getObject(pObj, AcDb::kForRead);
    if (Acad::eOk == es && pObj)
    {
      AcDbRasterImageDef* pImgDef = AcDbRasterImageDef::cast(pObj);
      if (pImgDef)
      {
        result.Add(pImgDef->activeFileName());
        pImgDef->close();
      }
      else
        pObj->close();
    }
  }

  delete pIter;
  pIter = 0;

  pDictImg->close();

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_purgexdata
int CDOSLibApp::ads_dos_purgexdata()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = 0;
  Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, entId, AcDb::kForWrite);
  if (es != Acad::eOk || 0 == pEnt)
    return RSERR;

  bool rc = false;
  resbuf* xdata = 0;
  while (xdata = pEnt->xData(0))
  {
    resbuf* xdata_next = xdata->rbnext;
    xdata->rbnext = 0;
    pEnt->setXData(xdata);
    xdata->rbnext = xdata_next;
    acutRelRb(xdata);
    rc = true;
  }
  pEnt->close();

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}


////////////////////////////////////////////////////////////////
// dos_proxycount
int CDOSLibApp::ads_dos_proxycount()
{
  AcDbObjectIdArray proxies;

  AcDbBlockTable* pTable = 0;
  acdbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, AcDb::kForRead);
  if (0 == pTable)
    return RSERR;

  AcDbBlockTableIterator* pTableIter = 0;
  for (pTable->newIterator(pTableIter); !pTableIter->done(); pTableIter->step())
  {
    AcDbBlockTableRecord* pRecord = 0;
    pTableIter->getRecord(pRecord, AcDb::kForRead);
    if (0 == pRecord)
      continue;

    AcDbBlockTableRecordIterator* pRecordIter = 0;
    for (pRecord->newIterator(pRecordIter); !pRecordIter->done(); pRecordIter->step())
    {
      AcDbEntity* pEnt = 0;
      pRecordIter->getEntity(pEnt, AcDb::kForRead);
      if (pEnt)
      {
        if (pEnt->isKindOf(AcDbProxyEntity::desc()))
          proxies.append(pEnt->objectId());
        pEnt->close();
      }
    }

    delete pRecordIter;
    pRecord->close();
  }

  delete pTableIter;

  acedRetInt(proxies.length());

  return RSRSLT;
}

// It is possible to use ObjectARX to change all proxy entities for something else.
// The attached sample does the following: Iterate through the entire drawing, and
// swaps all proxy entities for anonymous blocks. The blocks contain the equivalent
// geometry of the proxy entities. Note that this will not allow the creating entity
// to recover the information stored in the original entities.

////////////////////////////////////////////////////////////////
// dos_proxyclean
int CDOSLibApp::ads_dos_proxyclean()
{
  AcDbObjectIdArray proxies;

  AcDbBlockTable* pTable = 0;
  acdbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, AcDb::kForWrite);
  if (0 == pTable)
    return RSERR;

  AcDbBlockTableIterator* pTableIter = 0;
  for (pTable->newIterator(pTableIter); !pTableIter->done(); pTableIter->step())
  {
    AcDbBlockTableRecord* pRecord = 0;
    pTableIter->getRecord(pRecord, AcDb::kForWrite);
    if (0 == pRecord)
      continue;

    AcDbBlockTableRecordIterator* pRecordIter = 0;
    for (pRecord->newIterator(pRecordIter); !pRecordIter->done(); pRecordIter->step())
    {
      AcDbEntity* pEnt = 0;
      pRecordIter->getEntity(pEnt, AcDb::kForWrite);
      if (pEnt)
      {
        if (pEnt->isKindOf(AcDbProxyEntity::desc()))
        {
          proxies.append(pEnt->objectId());
        }
        pEnt->close();
      }
    }

    delete pRecordIter;
    pRecord->close();
  }

  delete pTableIter;

  if (Acad::eOk != pTable->upgradeOpen())
  {
    //pTable->close();  
    //return RSERR;
  }

  int i, j, nProxies = proxies.length();
  int num_cleaned = 0;
  for (i = 0; i < nProxies; i++)
  {
    AcDbProxyEntity* pProxy = 0;
    AcDbObject* pObj = 0;
    acdbOpenAcDbObject(pObj, proxies[i], AcDb::kForWrite);
    pProxy = AcDbProxyEntity::cast(pObj);
    if (0 == pProxy)
    {
      pObj->close();
      continue;
    }

    AcDbVoidPtrArray explodedEnts;
    pProxy->explode(explodedEnts);
    int nExplodedEnts = explodedEnts.length();
    if (nExplodedEnts > 0)
    {
      AcDbBlockTableRecord* pRecord = new AcDbBlockTableRecord();
      pRecord->setName(L"*U");
      AcDbObjectId blockId;
      pTable->add(blockId, pRecord);
      for (j = 0; j < nExplodedEnts; j++)
      {
        AcDbEntity* pEnt = (AcDbEntity*)(explodedEnts[j]);
        pRecord->appendAcDbEntity(pEnt);
        pEnt->setColorIndex(0);
        pEnt->close();
      }
      pRecord->close();

      AcDbBlockTableRecord* pOwningRecord = 0;
      acdbOpenObject(pOwningRecord, pProxy->ownerId(), AcDb::kForWrite);
      if (pOwningRecord)
      {
        AcDbBlockReference* pRef = new AcDbBlockReference;
        pRef->setBlockTableRecord(blockId);
        pOwningRecord->close();
        pProxy->upgradeOpen();
        pProxy->handOverTo(pRef);
        pRef->setColor(pProxy->color());
        pRef->setLayer(pProxy->layerId());
        pRef->setVisibility(pProxy->visibility());
        delete pProxy;
        pRef->close();

        num_cleaned++;
      }
    }
    else
    {
      pProxy->close();
    }
  }

  pTable->close();

  acedRetInt(num_cleaned);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_plinewinding

Adesk::Boolean PolylineWindingNumber(AcDbPolyline* pPoly, int& winding)
{
  if (0 == pPoly || Adesk::kFalse == pPoly->isClosed())
    return Adesk::kFalse;

  double turn = 0;
  int nSegs = pPoly->numVerts();
  AcGeVector3dArray startTans(nSegs);
  AcGeVector3dArray endTans(nSegs);

  int i;
  for (i = 0; i < nSegs; i++)
  {
    if (pPoly->segType(i) == AcDbPolyline::kArc)
    {
      AcGeCircArc2d arc;
      pPoly->getArcSegAt(i, arc);
      AcGeVector2d startTan;
      AcGeVector2d endTan;
      AcGeVector2dArray startDerivs;
      arc.evalPoint(arc.startAng(), 1, startDerivs);
      startTan = startDerivs[0];
      AcGeVector2dArray endDerivs;
      arc.evalPoint(arc.endAng(), 1, endDerivs);
      endTan = endDerivs[0];
      startTans.append(AcGeVector3d(startTan.x, startTan.y, 0.0));
      endTans.append(AcGeVector3d(endTan.x, endTan.y, 0.0));
      double ang = arc.endAng() - arc.startAng();
      turn += arc.isClockWise() ? -ang : ang;
    }
    else if (pPoly->segType(i) == AcDbPolyline::kLine)
    {
      AcGeLineSeg2d line;
      pPoly->getLineSegAt(i, line);
      AcGeVector2d tan2d = line.endPoint() - line.startPoint();
      AcGeVector3d tan = AcGeVector3d(tan2d.x, tan2d.y, 0.0);
      startTans.append(tan);
      endTans.append(tan);
    }
  }
  nSegs = startTans.length();

  for (i = 0; i < nSegs; i++)
  {
    double angle = endTans[i].angleTo(startTans[(i + 1) % nSegs]);
    AcGeVector3d norm = endTans[i].crossProduct(startTans[(i + 1) % nSegs]);
    angle = norm.z > 0 ? angle : -angle;
    turn += angle;
  }

  turn = turn / DOS_TWOPI;
  double lower = floor(turn);
  double tol = 1e-6;
  if ((turn - lower) < tol)
    winding = (int)lower;
  else if (((lower + 1) - turn) < tol)
    winding = (int)(lower + 1);
  else
    return Adesk::kFalse;

  return Adesk::kTrue;
}

int CDOSLibApp::ads_dos_plinewinding()
{
  CAdsArgs args;

  AcDbObjectId entId;
  if (!args.GetEntity(entId))
    return RSERR;

  AcDbEntity* pEnt = 0;
  if (Acad::eOk != acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead))
  {
    acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
    return RSERR;
  }

  int winding = 0;
  Adesk::Boolean rc = Adesk::kFalse;

  AcDbPolyline* pPolyEnt = 0;
  AcDb2dPolyline* pPoly2dEnt = 0;

  if (pPolyEnt = AcDbPolyline::cast(pEnt))
  {
    if (Adesk::kTrue == pPolyEnt->isClosed())
      rc = PolylineWindingNumber(pPolyEnt, winding);
    else
      acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
  }
  else if (pPoly2dEnt = AcDb2dPolyline::cast(pEnt))
  {
    if (Adesk::kTrue == pPoly2dEnt->isClosed())
    {
      pPolyEnt = new AcDbPolyline();
      if (pPolyEnt)
      {
        if (Acad::eOk == pPolyEnt->convertFrom((AcDbEntity*&)pPoly2dEnt, Adesk::kFalse))
          rc = PolylineWindingNumber(pPolyEnt, winding);
        delete pPolyEnt;
      }
    }
    else
      acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
  }
  else
    acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
  pEnt->close();

  if (rc == Adesk::kTrue)
    acedRetInt(winding);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acadname
int CDOSLibApp::ads_dos_acadname()
{
#ifdef BRX13
  const wchar_t* pszKey = acdbHostApplicationServices()->getRegistryProductRootKey();
#else
  const wchar_t* pszKey = acdbHostApplicationServices()->getMachineRegistryProductRootKey();
#endif

  if (pszKey && pszKey[0])
  {
    CRegKey reg;
    long rc = reg.Open(HKEY_LOCAL_MACHINE, pszKey, KEY_READ);
    if (rc == ERROR_SUCCESS)
    {
      CString str;
      ULONG num_chars = _MAX_PATH;
      rc = reg.QueryStringValue(L"ProductName", str.GetBuffer(num_chars), &num_chars);
      str.ReleaseBuffer();
      reg.Close();
      if (rc == ERROR_SUCCESS && !str.IsEmpty())
      {
        acedRetStr(str);
        return RSRSLT;
      }
    }
  }

  acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_slblist

static const char SLIDEHEADER[32] = "AutoCAD Slide Library 1.0\015\012\032";

typedef struct tagSlideEntry
{
  char m_name[32];
  DWORD m_offset;
} SLIDEENTRY, *LPSLIDEENTRY;

int CDOSLibApp::ads_dos_slblist()
{
  CAdsArgs args;

  CDosPathString path;
  if (!args.GetPathString(path) || !path.FileExists())
    return RSERR;

  bool bSort = false;
  bool bAscending = true;

  args.Next();

  if (!args.IsEmpty())
  {
    bSort = true;
    if (args.IsNil())
      bAscending = false;
  }

  CFileException ex;
  CFile file;
  if (!file.Open(path, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan, &ex))
  {
    wchar_t szError[1024];
    ex.GetErrorMessage(szError, 1024);
    DOS_PrintError(szError);
    acedRetNil();
    return RSRSLT;
  }

  char header[32];
  memset(&header, 0, 32);
  if (32 != file.Read(header, 32) || 0 != memcmp(header, SLIDEHEADER, 32))
  {
    file.Close();
    acedRetNil();
    return RSRSLT;
  }

  CDosStringArray names;

  SLIDEENTRY entry;
  size_t length = 0;
  do
  {
    memset(&entry, 0, sizeof(SLIDEENTRY));
    if (sizeof(SLIDEENTRY) == file.Read(&entry, sizeof(SLIDEENTRY)))
    {
      if (entry.m_name && entry.m_name[0])
        names.Add(CString(entry.m_name));
      else
        break;
    }
    else
      break;
  } while (true);

  file.Close();

  if (bSort)
    names.Sort(false, bAscending);

  CAdsRetList result;
  for (int i = 0; i < (int)names.GetCount(); i++)
    result.Add(names[i]);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}