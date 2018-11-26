///////////////////////////////////////////////////////////////////////////////
// func_drives.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"

#include <winioctl.h>

////////////////////////////////////////////////////////////////
// dos_chkdsk
int CDOSLibApp::ads_dos_chkdsk()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;

  strDrive.AddBackslash();

  unsigned __int64 i64FreeBytesToCaller;
  unsigned __int64 i64TotalBytes;
  unsigned __int64 i64FreeBytes;

  BOOL rc = SHGetDiskFreeSpace(
    strDrive,
    (PULARGE_INTEGER)&i64FreeBytesToCaller,
    (PULARGE_INTEGER)&i64TotalBytes,
    (PULARGE_INTEGER)&i64FreeBytes
    );

  if (!rc)
  {
    acedRetNil();
    return RSRSLT;
  }

  unsigned __int64 i64UsedBytes = (__int64)i64TotalBytes - i64FreeBytes;

  CString strTotal, strFree, strUsed;
  strTotal.Format(L"%I64u", i64TotalBytes);
  strFree.Format(L"%I64u", i64FreeBytes);
  strUsed.Format(L"%I64u", i64UsedBytes);

  LPTSTR lpStop = 0;
  double total = (double)_tcstod(strTotal, &lpStop);
  double used = (double)_tcstod(strUsed, &lpStop);
  double free = (double)_tcstod(strFree, &lpStop);

  struct resbuf* retlist = acutBuildList(
    RTREAL, total,
    RTREAL, used,
    RTREAL, free,
    0
    );

  if (retlist)
  {
    acedRetList(retlist);
    acutRelRb(retlist);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_drive
int CDOSLibApp::ads_dos_drive()
{
  CAdsArgs args;

  if (args.IsEmpty())
  {
    // Is logical drive
    int drive = _getdrive();
    if (drive >= 1 && drive <= 26)
    {
      CString s;
      s.Format(L"%c:", drive + 'A' - 1);
      s.MakeUpper();
      acedRetStr(s);
      return RSRSLT;
    }

    acedRetNil();
    return RSRSLT;
  }

  int drive = 0;
  if (args.GetDriveNumber(drive))
  {
    // Does logical drive exist?
    if (GetLogicalDrives() & (1 << (drive - 1)))
    {
      if (_chdrive(drive) == 0)
      {
        CString s;
        s.Format(L"%c:", drive + 'A' - 1);
        s.MakeUpper();
        acedRetStr(s);
        return RSRSLT;
      }
    }
  }

  acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_drivep
int CDOSLibApp::ads_dos_drivep()
{
  CAdsArgs args;

  int drive;
  if (!args.GetDriveNumber(drive, false))
    return RSERR;

  if (GetLogicalDrives() & (1 << (drive - 1)))
    acedRetT();
  else
    acedRetNil();
  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_drives
int CDOSLibApp::ads_dos_drives()
{
  TCHAR tchBuffer[_MAX_PATH];
  DWORD dwLength = GetLogicalDriveStrings(_MAX_PATH, tchBuffer);

  CAdsRetList result;
  LPTSTR lpBuffer = 0;
  for (lpBuffer = tchBuffer; *lpBuffer; lpBuffer += _tcslen(lpBuffer) + 1)
  {
    CDosPathString strPath(lpBuffer);
    strPath.RemoveBackslash();
    result.Add(strPath);
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_drivetype
int CDOSLibApp::ads_dos_drivetype()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSRSLT;

  args.Next();

  bool bStr = true;
  if (!args.IsEmpty())
    bStr = false;

  strDrive.AddBackslash();

  unsigned int drive = GetDriveType(strDrive);

  if (bStr)
  {
    switch (drive)
    {
    case DRIVE_NO_ROOT_DIR:
      acedRetStr(L"NO_ROOT_DIR");
      break;
    case DRIVE_REMOVABLE:
      acedRetStr(L"REMOVABLE");
      break;
    case DRIVE_FIXED:
      acedRetStr(L"FIXED");
      break;
    case DRIVE_REMOTE:
      acedRetStr(L"REMOTE");
      break;
    case DRIVE_CDROM:
      acedRetStr(L"CDROM");
      break;
    case DRIVE_RAMDISK:
      acedRetStr(L"RAMDISK");
      break;
    case DRIVE_UNKNOWN:
    default:
      acedRetStr(L"UNKNOWN");
      break;
    }
  }
  else
  {
    acedRetInt(drive);
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_format
int CDOSLibApp::ads_dos_format()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;

  int nDrive = strDrive[0] - 'A' + 1;

  DWORD dw = SHFormatDrive(adsw_acadMainWnd(), nDrive, SHFMT_ID_DEFAULT, 0);
  switch (dw)
  {
  case SHFMT_ERROR:
  case SHFMT_CANCEL:
  case SHFMT_NOFORMAT:
    acedRetNil();
    break;

  default:
    acedRetT();
    break;
  }

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_label
int CDOSLibApp::ads_dos_label()
{
  CAdsArgs args;

  if (args.IsEmpty())
  {
    CDosPathString strDrive;
    if (!args.GetDriveString(strDrive))
      return RSERR;
    strDrive.AddBackslash();

    TCHAR tchVolume[_MAX_PATH];
    TCHAR tchFileSys[_MAX_PATH];
    DWORD dwSerial = 0, dwMax = 0, dwFlags = 0;

    if (GetVolumeInformation(strDrive, tchVolume, _MAX_PATH, &dwSerial, &dwMax, &dwFlags, tchFileSys, _MAX_PATH))
      acedRetStr(tchVolume);
    else
      acedRetNil();
    return RSRSLT;
  }

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;
  strDrive.AddBackslash();

  args.Next();

  if (args.IsEmpty())
  {
    TCHAR tchVolume[_MAX_PATH];
    TCHAR tchFileSys[_MAX_PATH];
    DWORD dwSerial = 0, dwMax = 0, dwFlags = 0;

    if (GetVolumeInformation(strDrive, tchVolume, _MAX_PATH, &dwSerial, &dwMax, &dwFlags, tchFileSys, _MAX_PATH))
      acedRetStr(tchVolume);
    else
      acedRetNil();
    return RSRSLT;
  }

  CString strLabel;
  if (!args.GetString(strLabel))
    return RSERR;

  if (SetVolumeLabel(strDrive, strLabel))
    acedRetStr(strLabel);
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_serialno
int CDOSLibApp::ads_dos_serialno()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;

  strDrive.AddBackslash();

  TCHAR tchVolume[_MAX_PATH];
  TCHAR tchFileSys[_MAX_PATH];
  DWORD dwSerial, dwMax, dwFlags;

  if (GetVolumeInformation(strDrive, tchVolume, _MAX_PATH, &dwSerial, &dwMax, &dwFlags, tchFileSys, _MAX_PATH))
  {
    CString str;
    str.Format(L"%08X", dwSerial);
    str.MakeUpper();
    acedRetStr(str);
  }
  else
    acedRetNil();

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hdserialno
int CDOSLibApp::ads_dos_hdserialno()
{
  CAdsArgs args;

  CDosPathString strDrive;
  if (!args.GetDriveString(strDrive))
    return RSERR;

  CString strPhysicalDrive;
  strPhysicalDrive.Format(L"\\\\.\\%s", strDrive);

  HANDLE hPhysicalDriveIOCTL = CreateFile(strPhysicalDrive, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
  if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
  {
    DWORD dwBytesReturned = 0;

    // Get Physical Drive Information
    VOLUME_DISK_EXTENTS vdExtents;
    ZeroMemory(&vdExtents, sizeof(vdExtents));
    if (!DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, 0, 0, &vdExtents, sizeof(vdExtents), &dwBytesReturned, 0))
    {
      CloseHandle(hPhysicalDriveIOCTL);
      acedRetNil();
      return RSRSLT;
    }

    // Get SMART version information
    GETVERSIONINPARAMS gvopVersionParams;
    ZeroMemory(&gvopVersionParams, sizeof(gvopVersionParams));
    if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_GET_VERSION, 0, 0, &gvopVersionParams, sizeof(gvopVersionParams), &dwBytesReturned, 0))
    {
      CloseHandle(hPhysicalDriveIOCTL);
      acedRetNil();
      return RSRSLT;
    }

    if (gvopVersionParams.bIDEDeviceMap > 0)
    {
      // Setup SMART request...
      SENDCMDINPARAMS InParams =
      {
        IDENTIFY_BUFFER_SIZE, { 0, 1, 1, 0, 0, ((vdExtents.Extents[0].DiskNumber & 1) ? 0xB0 : 0xA0),
        ((gvopVersionParams.bIDEDeviceMap >> vdExtents.Extents[0].DiskNumber & 0x10) ? ATAPI_ID_CMD : ID_CMD) },
        (BYTE)vdExtents.Extents[0].DiskNumber
      };

      DWORD dwBufSize = sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE;
      PSENDCMDOUTPARAMS pbtIDOutCmd = (PSENDCMDOUTPARAMS) new BYTE[dwBufSize];
      ZeroMemory(pbtIDOutCmd, dwBufSize);

      // Get SMART information
      if (DeviceIoControl(hPhysicalDriveIOCTL, SMART_RCV_DRIVE_DATA, &InParams, sizeof(SENDCMDINPARAMS), pbtIDOutCmd, dwBufSize, &dwBytesReturned, 0))
      {
        // Little Endian To Big Endian
        USHORT* pIDSector = (USHORT*)pbtIDOutCmd->bBuffer;
        for (int nShort = 10; nShort < 21; nShort++)
          pIDSector[nShort] = (((pIDSector[nShort] & 0x00FF) << 8) + ((pIDSector[nShort] & 0xFF00) >> 8));

        // Get Drive Serial Number
        char* lpszSerialNumber = new char[21];
        ZeroMemory(lpszSerialNumber, 21);
        RtlCopyMemory(lpszSerialNumber, &pIDSector[10], 20);

        CString str(lpszSerialNumber);
        str.TrimLeft();
        str.TrimRight();
        str.MakeUpper();
        acedRetStr(str);

        delete lpszSerialNumber; // Don't leak...
      }

      delete pbtIDOutCmd; // Don't leak...
    }
    else
    {
      acedRetNil();
    }

    CloseHandle(hPhysicalDriveIOCTL);
  }

  return RSRSLT;
}
