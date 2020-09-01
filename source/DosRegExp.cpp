/////////////////////////////////////////////////////////////////////////////
// DosRegExp.cpp
//
// Copyright (c) 1992-2020, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "atlrx.h"
#include "DosRegExp.h"

CDosRegExp::CDosRegExp()
  : m_error(REPARSE_ERROR_OK)
  , m_szInput(0)
  , m_szNext(0)
{
  m_bCaseSensitive = TRUE;
}

CDosRegExp::CDosRegExp(const wchar_t* szRegExp, BOOL bCaseSensitive)
  : m_error(REPARSE_ERROR_OK)
  , m_szInput(0)
  , m_szNext(0)
{
  m_error = Parse(szRegExp, bCaseSensitive);
}

CDosRegExp::CDosRegExp(const CDosRegExp& src)
{
  *this = src;
}

CDosRegExp::~CDosRegExp()
{
}

const CDosRegExp& CDosRegExp::operator=(const CDosRegExp& src)
{
  if (this != &src)
  {
    m_strRegExp = src.m_strRegExp;
    m_bCaseSensitive = src.m_bCaseSensitive;
    if (!m_strRegExp.IsEmpty())
      m_error = Parse(m_strRegExp, m_bCaseSensitive);
  }
  return *this;
}

CDosRegExp::operator const wchar_t*() const
{
  return m_strRegExp;
}

REParseError CDosRegExp::Parse(const wchar_t* szRegExp, BOOL bCaseSensitive)
{
  m_strRegExp = szRegExp;
  m_szInput = m_szNext = 0;
  return CAtlRegExp<>::Parse(szRegExp, bCaseSensitive);
}

void CDosRegExp::SetInput(const wchar_t* szInput)
{
  m_szInput = m_szNext = szInput;
}

BOOL CDosRegExp::Match(const wchar_t* szInput, const wchar_t** ppszEnd)
{
  if (0 == szInput || 0 == *szInput)
    return FALSE;

  SetInput(szInput);

  return CAtlRegExp<>::Match(szInput, &m_mc, ppszEnd);
}

BOOL CDosRegExp::NextMatch()
{
  return CAtlRegExp<>::Match(m_szNext, &m_mc, &m_szNext);
}

CString CDosRegExp::GetMatch(int* pOffset) const
{
  if (pOffset)
    *pOffset = (int)(m_mc.m_Match.szStart - m_szInput);
  return GetMGString(m_mc.m_Match);
}

UINT CDosRegExp::GetNumGroups() const
{
  return m_mc.m_uNumGroups;
}

CString CDosRegExp::GetGroup(int nIndex, int* pOffset) const
{
  CAtlREMatchContext<>::MatchGroup mg;
  const_cast<CDosRegExp*>(this)->m_mc.GetMatch(nIndex, &mg);
  if (pOffset)
    *pOffset = (int)(mg.szStart - m_szInput);
  return GetMGString(mg);
}

CString CDosRegExp::Replace(const wchar_t* szInput, const wchar_t* szVal, BOOL bAll)
{
  CString s;
  const wchar_t* szRest = szInput;
  while (Match(szInput, &szRest))
  {
    int offset = 0;
    GetMatch(&offset);
    CString temp = szInput;
    s += temp.Left(offset);
    s += szVal;
    szInput = szRest;
    if (!bAll)
      break;
  }
  s += szInput;
  return s;
}

CString CDosRegExp::Replace(const wchar_t* szInput, const wchar_t* szRegExp, const wchar_t* szVal, BOOL bAll, BOOL bIgnoreCase)
{
  CDosRegExp re(szRegExp, !bIgnoreCase);
  return re.Replace(szInput, szVal, bAll);
}

CString CDosRegExp::Replace(const wchar_t* szInput, REREPLACEPROC replfn, void* pParam)
{
  CString s;
  const wchar_t* szRest = szInput;
  while (Match(szInput, &szRest))
  {
    int offset = 0;
    CString match = GetMatch(&offset);
    CString temp = szInput;
    s += temp.Left(offset);
    CString szVal = (*replfn)(*this, match, pParam);
    s += szVal;
    szInput = szRest;
  }
  s += szInput;
  return s;
}

CString CDosRegExp::Replace(const wchar_t* szInput, const wchar_t* szRegExp, REREPLACEPROC replfn, void* pParam, BOOL bIgnoreCase)
{
  CDosRegExp re(szRegExp, !bIgnoreCase);
  return re.Replace(szInput, replfn, pParam);
}

INT_PTR CDosRegExp::Split(const wchar_t* szInput, CStringArray& arr, INT_PTR nMax)
{
  arr.SetSize(nMax > 0 ? nMax : 10);
  if (0 == nMax)
    nMax = 0xFFFF;

  int count = 0;
  const wchar_t* szRest = szInput;
  while (Match(szInput, &szRest))
  {
    int offset = 0;
    GetMatch(&offset);
    CString str = szInput;
    str = str.Left(offset);
    arr.Add(str);
    szInput = szRest;
    if (++count >= nMax)
      break;
  }

  if (*szInput)
    arr.Add(szInput);

  return arr.GetSize();
}

INT_PTR CDosRegExp::Split(const wchar_t* szInput, const wchar_t* szSep, CStringArray& arr, BOOL bCaseSensitive, INT_PTR nMax)
{
  CDosRegExp re(szSep, bCaseSensitive);
  return re.Split(szInput, arr, nMax);
}

CString CDosRegExp::GetMGString(const CAtlREMatchContext<>::MatchGroup& mg) const
{
  CString strMatch = mg.szStart;
  return strMatch.Left((int)(mg.szEnd - mg.szStart));
}

CString CDosRegExp::GetErrorName(REParseError err)
{
  CString str;
  switch (err)
  {
  case REPARSE_ERROR_OK:
    str = L"no error occurred.";
    break;
  case REPARSE_ERROR_OUTOFMEMORY:
    str = L"out of memory.";
    break;
  case REPARSE_ERROR_BRACE_EXPECTED:
    str = L"a closing brace was expected.";
    break;
  case REPARSE_ERROR_PAREN_EXPECTED:
    str = L"a closing parenthesis was expected.";
    break;
  case REPARSE_ERROR_BRACKET_EXPECTED:
    str = L"a closing bracket was expected.";
    break;
  case REPARSE_ERROR_UNEXPECTED:
    str = L"an unspecified fatal error occurred.";
    break;
  case REPARSE_ERROR_EMPTY_RANGE:
    str = L"a range expression was empty.";
    break;
  case REPARSE_ERROR_INVALID_GROUP:
    str = L"a back reference was made to a group that did not exist.";
    break;
  case REPARSE_ERROR_INVALID_RANGE:
    str = L"an invalid range was specified.";
    break;
  case REPARSE_ERROR_EMPTY_REPEATOP:
    str = L"a repeat operator (* or +) was applied to an expression that could be empty.";
    break;
  case REPARSE_ERROR_INVALID_INPUT:
    str = L"the input string was invalid.";
    break;
  default:
    str = L"unknown error.";
    break;
  }
  return str;
}
