/////////////////////////////////////////////////////////////////////////////
// func_encrypt.cpp
//
// Copyright (c) 1992-2026, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"

#define KEYSIZE 512

/// <summary>
/// GetCell is called by Mutate to determine whether a specified cell is
/// currently set to 0 or 1.
/// </summary>
/// <param name="pGrid">Pointer to grid</param>
/// <param name="nWidth">Grid width</param>
/// <param name="i">Cell's column number</param>
/// <param name="j">Cell's row number</param>
/// <returns>
/// TRUE = Cell's value is 1
/// FALSE = Cell's value is 0
/// </returns>
static BOOL GetCell(char* pGrid, UINT nWidth, UINT i, UINT j)
{
  UINT nBitNumber = ((j * nWidth) + i) & 7;
  UINT nByteNumber = ((j * nWidth) + i) >> 3;
  return(*(pGrid + nByteNumber) & (1 << nBitNumber)) ? TRUE : FALSE;
}

/// <summary>
/// SetCell is called by Mutate to set a cell value to 0 or 1. 
/// </summary>
/// <param name="pGrid">Pointer to grid</param>
/// <param name="nWidth"></param>
/// <param name="i">Cell's column number</param>
/// <param name="j">Cell's row number</param>
/// <param name="bCellState">New cell value (TRUE = 1, FALSE = 0)</param>
static void SetCell(char* pGrid, UINT nWidth, UINT i, UINT j, BOOL bCellState)
{
  UINT nBitNumber = ((j * nWidth) + i) & 7;
  UINT nByteNumber = ((j * nWidth) + i) >> 3;
  if (bCellState)
    *(pGrid + nByteNumber) |= (1 << nBitNumber);
  else
    *(pGrid + nByteNumber) &= (~(1 << nBitNumber));
}

/// <summary>
/// Mutate generates a mutated key from an input key using a cellular
/// automaton process similar to the one that drives John Conway's Life,
/// which was described in the October 1970 issue of Scientific American.
/// Each bit in the input key represents a "cell" in a grid, and each cell
/// is set to 1 or 0 according to its current state and the states of its
/// neighbors.
/// </summary>
/// <param name="pInputKey">Pointer to input key</param>
/// <param name="pOutputKey">Pointer to output key</param>
/// <param name="nWidth">Grid width (in cells)</param>
/// <param name="nHeight">Grid height (in cells)</param>
/// <remarks>
/// (nWidth * nHeight) must equal the key size in bits.
/// </remarks>
static void Mutate(char* pInputKey, char* pOutputKey, UINT nWidth, UINT nHeight)
{
  UINT i, j;
  for (i = 0; i < nHeight; i++)
  {
    UINT nLastRow = (i == 0) ? nHeight - 1 : i - 1;
    UINT nNextRow = (i == (nHeight - 1)) ? 0 : i + 1;

    for (j = 0; j < nWidth; j++)
    {
      UINT nLastCol = (j == 0) ? nWidth - 1 : j - 1;
      UINT nNextCol = (j == (nWidth - 1)) ? 0 : j + 1;

      UINT nNeighbors = 0;

      if (GetCell(pInputKey, nWidth, nLastCol, nLastRow))
        nNeighbors++;
      if (GetCell(pInputKey, nWidth, j, nLastRow))
        nNeighbors++;
      if (GetCell(pInputKey, nWidth, nNextCol, nLastRow))
        nNeighbors++;

      if (GetCell(pInputKey, nWidth, nLastCol, i))
        nNeighbors++;
      if (GetCell(pInputKey, nWidth, nNextCol, i))
        nNeighbors++;

      if (GetCell(pInputKey, nWidth, nLastCol, nNextRow))
        nNeighbors++;
      if (GetCell(pInputKey, nWidth, j, nNextRow))
        nNeighbors++;
      if (GetCell(pInputKey, nWidth, nNextCol, nNextRow))
        nNeighbors++;

      BOOL bCurrentCellAlive = GetCell(pInputKey, nWidth, j, i);

      if ((bCurrentCellAlive && ((nNeighbors == 2) || (nNeighbors == 3))) || (!bCurrentCellAlive && (nNeighbors == 3)))
        SetCell(pOutputKey, nWidth, j, i, TRUE);
      else
        SetCell(pOutputKey, nWidth, j, i, FALSE);
    }
  }
}

////////////////////////////////////////////////////////////////
// dos_encrypt
// 15-Apr-2026 Dale Fugier, https://github.com/dalefugier/DOSLib/issues/43

int CDOSLibApp::ads_dos_encrypt()
{
  CAdsArgs args;

  CDosPathString strPathW;
  if (!args.GetPathString(strPathW))
    return RSERR;

  strPathW.FullPath();
  if (!strPathW.FileExists())
  {
    acedRetNil();
    return RSRSLT;
  }

  args.Next();

  CString strPasswordW;
  if (!args.GetString(strPasswordW))
    return RSERR;

  CStringA strPath(strPathW);
  CStringA strPassword(strPasswordW);

  char szPassword[_MAX_PATH];
  strcpy_s(szPassword, _MAX_PATH, strPassword);
  size_t length = strlen(szPassword);

  char* pFileData = (char*)LocalAlloc(LPTR, 3 * KEYSIZE);
  if (!pFileData)
  {
    acutPrintf(L"DOSLib Error: out of memory.\n");
    return RSERR;
  }

  char* pKey = pFileData + KEYSIZE;
  char* pTemp = pKey + KEYSIZE;

  // Replicate the password enough times to fill the key
  int i = 0, j = 0;
  while (i < KEYSIZE)
  {
    int count = min((int)length, KEYSIZE - i);
    int j = 0;
    while (j < count)
      pKey[i++] = szPassword[j++];
  }

  // Generate a series of "mutated" numbers from the key using a simple
  // (but non-reversible) one-step cellular automaton process, and XOR the
  // series into the key. Note: The numeric parameters passed to the Mutate
  // function assume a 512-byte key size. If KEYSIZE changes, this statement
  // must be changed, too.
  Mutate(pKey, pTemp, 64, 64);
  i = 0;
  while (i < KEYSIZE)
    pKey[i] ^= pTemp[i++];

  // Generate a series of random numbers and XOR the series into the key.
  i = j = 0;
  while (i < (int)length)
    j += (int)szPassword[i++];
  srand(j);

  i = 0;
  while (i < KEYSIZE)
    pKey[i++] ^= (char)(rand() % 0x100);

  HFILE hFile = _lopen(strPath, OF_READWRITE);
  if (hFile == HFILE_ERROR)
  {
    LocalFree((HLOCAL)pFileData);
    acedRetNil();
    return RSRSLT;
  }

  DWORD dwBytesRemaining = (DWORD)_llseek(hFile, 0, 2);
  _llseek(hFile, 0, 0);

  while (dwBytesRemaining)
  {
    DWORD nBytesToRead = min(KEYSIZE, dwBytesRemaining);
    DWORD nBytesRead = _lread(hFile, pFileData, nBytesToRead);
    if (nBytesRead != nBytesToRead)
    {
      LocalFree((HLOCAL)pFileData);
      _lclose(hFile);
      acedRetNil();
      return RSRSLT;
    }

    UINT n;
    for (n = 0; n < nBytesRead; n++)
      pFileData[n] ^= pKey[n];

    _llseek(hFile, -((long)nBytesRead), 1);

    DWORD nBytesToWrite = nBytesRead;
    DWORD nBytesWritten = _lwrite(hFile, pFileData, nBytesToWrite);
    if (nBytesWritten != nBytesToWrite)
    {
      LocalFree((HLOCAL)pFileData);
      _lclose(hFile);
      acedRetNil();
      return RSRSLT;
    }

    dwBytesRemaining -= (DWORD)nBytesWritten;
  }

  LocalFree((HLOCAL)pFileData);
  _lclose(hFile);

  acedRetStr(strPathW);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dos2unix
int CDOSLibApp::ads_dos_dos2unix()
{
  CAdsArgs args;

  CDosPathString strDosPath;
  if (!args.GetPathString(strDosPath))
    return RSERR;
  strDosPath.FullPath();

  args.Next();

  CDosPathString strUnixPath;
  if (!args.GetPathString(strUnixPath))
    return RSERR;
  strUnixPath.FullPath();

  CFile DosFile, UnixFile;

  if (!DosFile.Open(strDosPath, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
  {
    acutPrintf(L"DOSLib error: cannot open DOS source file.\n");
    return RSERR;
  }

  if (!UnixFile.Open(strUnixPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
  {
    DosFile.Close();
    acutPrintf(L"DOSLib error: cannot open UNIX destination file.\n");
    return RSERR;
  }

  char ch[1];
  UINT nBytesRead;
  ULONGLONG i, nLength = DosFile.GetLength();
  for (i = 0; i < nLength; i++)
  {
    nBytesRead = DosFile.Read(ch, sizeof(ch));
    if (nBytesRead != sizeof(ch))
    {
      DosFile.Close();
      UnixFile.Close();
      acutPrintf(L"DOSLib error: cannot read source file.\n");
      return RSERR;
    }

    if (ch[0] != '\015' && ch[0] != '\032')
      UnixFile.Write(ch, sizeof(ch));
  }

  DosFile.Close();
  UnixFile.Close();

  acedRetStr(strUnixPath);

  return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_unix2dos
int CDOSLibApp::ads_dos_unix2dos()
{
  CAdsArgs args;

  CDosPathString strUnixPath;
  if (!args.GetPathString(strUnixPath))
    return RSERR;
  strUnixPath.FullPath();

  args.Next();

  CDosPathString strDosPath;
  if (!args.GetPathString(strDosPath))
    return RSERR;
  strDosPath.FullPath();

  CFile DosFile, UnixFile;

  if (!UnixFile.Open(strUnixPath, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
  {
    acutPrintf(L"DOSLib error: cannot open UNIX source file.\n");
    return RSERR;
  }

  if (!DosFile.Open(strDosPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
  {
    acutPrintf(L"DOSLib error: cannot open DOS destination file.\n");
    UnixFile.Close();
    return RSERR;
  }

  char ch[1], cr[1];
  cr[0] = '\015';
  UINT nBytesRead;
  ULONGLONG i, nLength = UnixFile.GetLength();
  for (i = 0; i < nLength; i++)
  {
    nBytesRead = UnixFile.Read(ch, sizeof(ch));
    if (nBytesRead != sizeof(ch))
    {
      acutPrintf(L"DOSLib error: cannot read UNIX source file.\n");
      DosFile.Close();
      UnixFile.Close();
      return RSERR;
    }
    if (ch[0] == '\012')
      DosFile.Write(cr, sizeof(cr));
    DosFile.Write(ch, sizeof(ch));
  }

  DosFile.Close();
  UnixFile.Close();

  acedRetStr(strDosPath);

  return RSRSLT;
}

