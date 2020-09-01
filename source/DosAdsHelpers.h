/////////////////////////////////////////////////////////////////////////////
// DosAdsHelpers.h
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

// Compute the number of elements in an array.
#define DOS_ArrayCount(a) (sizeof(a)/sizeof((a)[0]))

/////////////////////////////////////////////////////////////////////////////
// CAdsResbuf

class CAdsResbuf
{
public:
  CAdsResbuf();
  CAdsResbuf(double d);
  CAdsResbuf(const wchar_t* s);
  CAdsResbuf(const CAdsResbuf& src);
  CAdsResbuf& operator=(const CAdsResbuf& src);

public:
  double m_d;
  CString m_s;
};

/////////////////////////////////////////////////////////////////////////////
// CAdsArgs

class CAdsArgs
{
public:
  CAdsArgs();
  ~CAdsArgs();

  void Reset();
  int Count(bool bAll = true);

  bool IsEmpty();
  bool Next();

  bool IsNil();
  bool IsTrue();
  bool GetBoolean(bool& b);
  bool GetBoolean(BOOL& b);

  bool IsNumber();

  bool IsInteger();
  bool GetInteger(int& n);
  bool GetIntegerList(CArray<int, int>& arr);

  bool IsDouble();
  bool GetDouble(double& d);
  bool GetDoubleList(CArray<double, double>& arr);

  bool GetResbuf(CAdsResbuf& res);
  bool GetResbufList(CArray<CAdsResbuf, CAdsResbuf>& arr);

  bool IsString();
  bool GetString(CString& s, bool bAllowNil = false);

  bool IsChar();
  bool GetChar(TCHAR& c, bool bMakeUpper = true);

  bool GetPathString(CString& s, bool bAddBackslash = false);
  bool GetDriveString(CString& s, bool bUseCurrentDrive = true);
  bool GetDriveNumber(int& n, bool bUseCurrentDrive = true);

  bool IsListBegin();
  bool IsListEnd();
  bool IsDot() const;
  bool GetStringList(CStringArray& arr, bool bAllowNil = false);

  bool IsPoint();
  bool Is2dPoint();
  bool Is3dPoint();
  bool GetPoint(ads_point& pt);
  bool Get2dPointList(CDos2dPointArray& arr);
  bool Get3dPointList(CDos3dPointArray& arr);

  bool IsEntity();
  bool GetEntity(AcDbObjectId& e);

  bool IsSelectionSet();
  bool GetEntityList(AcDbObjectIdArray& arr);

  bool GetVariant(COleVariant& va);
  bool GetSafeArray(COleSafeArray& sa, bool bAcad = false);

  struct resbuf* Resbuf();

private:
  CAdsArgs(const CAdsArgs&);
  CAdsArgs& operator=(const CAdsArgs&);
  struct resbuf* m_head;
  struct resbuf* m_rb;
};

/////////////////////////////////////////////////////////////////////////////
// CAdsRetList
class CAdsRetList
{
public:
  CAdsRetList();
  ~CAdsRetList();

  operator const struct resbuf*() const;
  struct resbuf* List();

  bool IsEmpty();

  bool Add(const wchar_t* string, bool bAllowEmpty = false);
  bool Add(double d);
  bool Add(const double* data, int count);
  bool Add(long l);
  bool Add(int n);
  bool Add(short i);
  bool Add(bool b);
  bool Add(const VARIANT& va);
  bool AddLB();
  bool AddLE();
  bool AddDot();

  static bool IsArray(const VARIANT& va);

private:
  CAdsRetList(const CAdsRetList&);
  CAdsRetList& operator=(const CAdsRetList&);
  void AddToTail();
  bool SafeArrayHelper(SAFEARRAY* psa);
  bool VariantArrayHelper(SAFEARRAY* psa);

private:
  struct resbuf* m_rbhead;
  struct resbuf* m_rbtail;
  struct resbuf* m_rbnew;
};

/////////////////////////////////////////////////////////////////////////////
// CAdsWorkingFolder
class CAdsWorkingFolder
{
public:
  CAdsWorkingFolder(const wchar_t* pszFolder);
  ~CAdsWorkingFolder();

private:
  CString m_strSavedFolder;
};

/////////////////////////////////////////////////////////////////////////////
// Utilities

const wchar_t* DOS_PlatformName();

int DOS_FindFile(const wchar_t* fname, wchar_t* result, size_t nBufLen);

void DOS_PrintError(const wchar_t* string);
void DOS_PrintLastError();

bool DOS_SimpleIntegerParse(const wchar_t* string, int& n);
bool DOS_SimpleFloatParse(const wchar_t* string, double& d);

bool DOS_IntegerParse(const wchar_t* string, int& n);
bool DOS_FloatParse(const wchar_t* string, double& d);

int DOS_StringParse(const wchar_t* string, wchar_t delimiter, CStringArray& tokens);

bool DOS_WildCardMatch(const wchar_t* s, const wchar_t* pattern);
bool DOS_WildCardMatchNoCase(const wchar_t* s, const wchar_t* pattern);

int DOS_GetAttributeFlags(int nFlags);
int DOS_SetAttributeFlags(int nAttributes);

double DOS_AcadTime(const CTime& time);
double DOS_AcadTime(const FILETIME& time);

int acedRetNum(double d);

void DOS_Pump();
void DOS_Sleep(DWORD dwMilliseconds = 0);

CString DOS_ReplaceNoCase(const wchar_t* instr, const wchar_t* oldstr, const wchar_t* newstr);

// Description:
//   Ensures a filename is legal.
// Parameters
//	 pszFileName [in] The filename to check. This must be only the filename.
// Returns:
//	0 on success.
//	Non-Zero on failure, where:
//		>0  = The illegal character that was used. If the value is a dot ('.', 46) the file name was nothing but dots.
//		-1  = A NULL or zero length file name was passed, or the file name exceeded 255 characters.
//		<-1	= A device name was used. The value corresponds to the INVALID_FILENAME_... series of enumerations.
int DOS_IsValidFileName(const wchar_t* pszFileName);
const wchar_t* DOS_GetIsValidFileNameErrStr(int err);

/////////////////////////////////////////////////////////////////////////////

class CDosHideCrosshairs
{
public:
  CDosHideCrosshairs();
  ~CDosHideCrosshairs();

private:
  DWORD m_dwModelCrossHairColor;
  DWORD m_dwLayoutCrossHairColor;
  DWORD m_dwParallelCrossHairColor;
  DWORD m_dwPerspectiveCrossHairColor;
  DWORD m_dwBEditCrossHairColor;
};
