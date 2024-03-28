//////////////////////////////////////////////////////////////////////////////////////////
// func_strings.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosStringArray.h"
#include "DosRegExp.h"

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strlength
int CDOSLibApp::ads_dos_strlength()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  acedRetInt(str.GetLength());

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strchar
int CDOSLibApp::ads_dos_strchar()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int pos = 0;
  if (!args.GetInteger(pos))
    return RSERR;

  if (pos < 0)
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  if (pos >= str.GetLength())
  {
    acutPrintf(L"DOSLib error: string length exceeded.\n");
    return RSERR;
  }

  args.Next();

  if (args.IsEmpty())
  {
    CString ch = str[pos];
    acedRetStr(ch);
  }
  else
  {
    CString ch;
    if (!args.GetString(ch))
      return RSERR;

    str.SetAt(pos, ch[0]);
    acedRetStr(str);
  }

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strcompare
int CDOSLibApp::ads_dos_strcompare()
{
  CAdsArgs args;

  CString str1;
  if (!args.GetString(str1))
    return RSERR;

  args.Next();

  CString str2;
  if (!args.GetString(str2))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
    acedRetInt(str1.Compare(str2));
  else
    acedRetInt(str1.CompareNoCase(str2));

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strcmplogical
int CDOSLibApp::ads_dos_strcmplogical()
{
  CAdsArgs args;

  CString str1;
  if (!args.GetString(str1))
    return RSERR;

  args.Next();

  CString str2;
  if (!args.GetString(str2))
    return RSERR;

  acedRetInt(StrCmpLogicalW(str1, str2));

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strmid
int CDOSLibApp::ads_dos_strmid()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int first = 0;
  if (!args.GetInteger(first))
    return RSERR;

  if (first < 0)
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  args.Next();

  if (args.IsEmpty())
  {
    acedRetStr(str.Mid(first));
  }
  else
  {
    int count = 0;
    if (!args.GetInteger(count))
      return RSERR;

    if (count < 0)
    {
      acutPrintf(L"DOSLib error: invalid integer argument.\n");
      return RSERR;
    }

    acedRetStr(str.Mid(first, count));
  }

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strleft
int CDOSLibApp::ads_dos_strleft()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int count = 0;
  if (!args.GetInteger(count))
    return RSERR;

  if (count < 0)
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  acedRetStr(str.Left(count));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strright
int CDOSLibApp::ads_dos_strright()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int count = 0;
  if (!args.GetInteger(count))
    return RSERR;

  if (count < 0)
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  acedRetStr(str.Right(count));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strincludingc
int CDOSLibApp::ads_dos_strincluding()
{
  CAdsArgs args;

  CString str1;
  if (!args.GetString(str1))
    return RSERR;

  args.Next();

  CString str2;
  if (!args.GetString(str2))
    return RSERR;

  acedRetStr(str1.SpanIncluding(str2));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strexcluding
//
int CDOSLibApp::ads_dos_strexcluding()
{
  CAdsArgs args;

  CString str1;
  if (!args.GetString(str1))
    return RSERR;

  args.Next();

  CString str2;
  if (!args.GetString(str2))
    return RSERR;

  acedRetStr(str1.SpanExcluding(str2));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strcase
int CDOSLibApp::ads_dos_strcase()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();
  if (args.IsEmpty())
    str.MakeUpper();
  else
    str.MakeLower();
  acedRetStr(str);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strreverse
int CDOSLibApp::ads_dos_strreverse()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  str.MakeReverse();
  acedRetStr(str);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strreplace

int CDOSLibApp::ads_dos_strreplace()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString strOld;
  if (!args.GetString(strOld))
    return RSERR;

  args.Next();

  CString strNew;
  if (!args.GetString(strNew))
    return RSERR;

  args.Next();

  // 28-Mar-2024, https://github.com/dalefugier/DOSLib/issues/34

  if (args.IsEmpty())
  {
    if (strOld.Compare(strNew) != 0)
      str.Replace(strOld, strNew);
    acedRetStr(str);
  }
  else
  {
    if (strOld.CompareNoCase(strNew) != 0)
      str = DOS_ReplaceNoCase(str, strOld, strNew);
    acedRetStr(str);
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strremove
int CDOSLibApp::ads_dos_strremove()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString ch;
  if (!args.GetString(ch))
    return RSERR;

  str.Remove(ch[0]);
  acedRetStr(str);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strinsert
int CDOSLibApp::ads_dos_strinsert()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int index = 0;
  if (!args.GetInteger(index))
    return RSERR;

  if (index < 0 || index >= str.GetLength())
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  args.Next();

  CString chars;
  if (!args.GetString(chars))
    return RSERR;

  str.Insert(index, chars);
  acedRetStr(str);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strdelete
int CDOSLibApp::ads_dos_strdelete()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  int index = 0;
  if (!args.GetInteger(index))
    return RSERR;

  args.Next();

  int count = 1;
  if (!args.IsEmpty())
  {
    if (!args.GetInteger(count))
      return RSERR;
  }

  if (index < 0 || count < 0)
  {
    acutPrintf(L"DOSLib error: invalid integer argument.\n");
    return RSERR;
  }

  if (index >= str.GetLength())
  {
    acedRetStr(str);
  }
  else
  {
    if (count > str.GetLength() - index)
      count = str.GetLength() - index;

    str.Delete(index, count);
    acedRetStr(str);
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strtrim
int CDOSLibApp::ads_dos_strtrim()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    str.Trim();
    acedRetStr(str);
  }
  else
  {
    CString chars;
    if (!args.GetString(chars))
      return RSERR;

    str.Trim(chars);
    acedRetStr(str);
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strtrimleft
int CDOSLibApp::ads_dos_strtrimleft()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    str.TrimLeft();
    acedRetStr(str);
  }
  else
  {
    CString chars;
    if (!args.GetString(chars))
      return RSERR;

    str.TrimLeft(chars);
    acedRetStr(str);
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strtrimright
int CDOSLibApp::ads_dos_strtrimright()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  if (args.IsEmpty())
  {
    str.TrimRight();
    acedRetStr(str);
  }
  else
  {
    CString chars;
    if (!args.GetString(chars))
      return RSERR;

    str.TrimRight(chars);
    acedRetStr(str);
  }

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strfind
int CDOSLibApp::ads_dos_strfind()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString sub;
  if (!args.GetString(sub))
    return RSERR;

  args.Next();
  int index = 0;

  if (!args.IsEmpty())
  {
    if (!args.GetInteger(index))
      return RSERR;

    if (index < 0)
    {
      acutPrintf(L"DOSLib error: invalid integer argument.\n");
      return RSERR;
    }
  }

  acedRetInt(str.Find(sub, index));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strreversefind
int CDOSLibApp::ads_dos_strreversefind()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString ch;
  if (!args.GetString(ch))
    return RSERR;

  acedRetInt(str.ReverseFind(ch[0]));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strfindoneof
int CDOSLibApp::ads_dos_strfindoneof()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString chars;
  if (!args.GetString(chars))
    return RSERR;

  acedRetInt(str.FindOneOf(chars));

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strcat
int CDOSLibApp::ads_dos_strcat()
{
  CAdsArgs args;

  CString str, buffer;

  while (!args.IsEmpty())
  {
    if (!args.GetString(str))
      return RSERR;
    buffer += str;
    args.Next();
  }

  acedRetStr(buffer);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strtokens
class CDosStrToken
{
public:
  CDosStrToken(const CString& str, const CString& seps);

public:
  CString GetNextToken();
  BOOL IsTokens();

private:
  CString m_str;
  CString m_seps;
};

CDosStrToken::CDosStrToken(const CString& str, const CString& seps)
{
  m_str = str;
  m_seps = seps;
}

BOOL CDosStrToken::IsTokens()
{
  return (m_str.GetLength() > 0) ? TRUE : FALSE;
}

CString CDosStrToken::GetNextToken()
{
  CString str = m_str;
  int nLen = str.GetLength();
  int nPos = str.FindOneOf(m_seps);
  if (nPos != -1)
  {
    str = str.Left(nPos);
    nPos++;
    m_str = m_str.Right(nLen - nPos);
  }
  else
  {
    m_str.Empty();
  }
  return str;
}

static int DosTokenString(
  LPCTSTR lpszString,
  LPCTSTR lpszDelimiters,
  CStringArray& saTokens,
  int nMaxTokens = 0,
  BOOL bTrimToken = FALSE,
  BOOL bEnableEscapedChars = FALSE,
  BOOL bEnableDoubleQuote = FALSE,
  BOOL bReturnEmptyToken = TRUE
  )
{
  saTokens.RemoveAll();

  if ((lpszString == NULL) || (lpszString[0] == _T('\0')))
    return 0;

  if ((lpszDelimiters == NULL) || (lpszDelimiters[0] == _T('\0')))
    return 0;

  BOOL bEndedWithDelim = FALSE;
  const TCHAR* cp = lpszString;

  if (!bReturnEmptyToken)
    cp += _tcsspn(cp, lpszDelimiters);			// skip leading delimiters

  // loop to find all tokens

  while (*cp)
  {
    const TCHAR * pos = NULL;
    CString strToken = _T("");

    // skip leading whitespace;  normally this will not cause a problem,
    // since it is unlikely that you would want to return empty tokens
    // when whitespace is the delimiter
    while ((*cp == _T(' ')) || (*cp == _T('\t')))
      strToken += *cp++;

    BOOL bInQuote = FALSE;

    if (bEnableDoubleQuote && (*cp == _T('"')))
    {
      bInQuote = TRUE;
      ++cp;							// skip " char
    }

    // loop to accumulate token
    for (pos = cp; *pos; pos++)
    {
      // check for end of token - either a quote or a delimiter
      if (bInQuote)
      {
        if (*pos == _T('"'))
          break;
      }
      else
      {
        if (_tcsspn(pos, lpszDelimiters) != 0)
          break;
      }

      if (bEnableEscapedChars && (*pos == _T('\\')))
      {
        pos++;						// skip '\'
        if (*pos == _T('\0'))
          break;
      }
      strToken += *pos;
    }
    cp = pos;

    // check if string ended with delimiter
    if (_tcsspn(cp, lpszDelimiters))
      bEndedWithDelim = TRUE;
    else
      bEndedWithDelim = FALSE;

    // skip ending delimiter/quote
    if (*cp)
      cp++;

    if (bInQuote)
    {
      // if quote ended with delimiter, skip it
      if (_tcsspn(cp, lpszDelimiters))
        cp++;
    }

    if (bTrimToken)
    {
      strToken.TrimLeft();
      strToken.TrimRight();
    }

    if ((!strToken.IsEmpty()) || bReturnEmptyToken)
    {
      // either token is not empty OR caller wants empty tokens

      if ((nMaxTokens > 0) && (saTokens.GetSize() >= nMaxTokens))
        break;

      saTokens.Add(strToken);
    }
  }

  // add empty token if caller wants it
  if (bReturnEmptyToken && bEndedWithDelim)
    saTokens.Add(_T(""));

  return (int)saTokens.GetSize();
}

int CDOSLibApp::ads_dos_strtokens()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString tokens;
  if (!args.GetString(tokens))
    return RSERR;

  args.Next();

  CAdsRetList result;
  if (args.IsEmpty())
  {
    CStringArray saTokens;
    if (DosTokenString(str, tokens, saTokens))
    {
      for (int i = 0; i < saTokens.GetSize(); i++)
        result.Add(saTokens[i], true);
    }
    //CDosStrToken tokenizer( str, tokens );
    //while( tokenizer.IsTokens() )
    //  result.Add( tokenizer.GetNextToken(), true );
  }
  else
  {
    int pos = 0;
    CString res = str.Tokenize(tokens, pos);
    while (res != L"")
    {
      result.Add(res);
      res = str.Tokenize(tokens, pos);
    }
  }

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strmatch
bool DOS_WildCardMatch(const wchar_t* s, const wchar_t* pattern)
{
  if (!pattern || !pattern[0])
  {
    return (!s || !s[0]) ? true : false;
  }

  if (*pattern == '*')
  {
    pattern++;
    while (*pattern == '*')
      pattern++;

    if (!pattern[0])
      return true;

    while (*s)
    {
      if (DOS_WildCardMatch(s, pattern))
        return true;
      s++;
    }

    return false;
  }

  while (*pattern != '*')
  {
    if (*pattern == '?')
    {
      if (*s)
      {
        pattern++;
        s++;
        continue;
      }
      return false;
    }

    if (*pattern == '\\')
    {
      switch (pattern[1])
      {
      case '*':
      case '?':
        pattern++;
        break;
      }
    }

    if (*pattern != *s)
    {
      return false;
    }

    if (*s == 0)
      return true;

    pattern++;
    s++;
  }

  return DOS_WildCardMatch(s, pattern);
}

bool DOS_WildCardMatchNoCase(const wchar_t* s, const wchar_t* pattern)
{
  if (!pattern || !pattern[0])
  {
    return (!s || !s[0]) ? true : false;
  }

  if (*pattern == '*')
  {
    pattern++;
    while (*pattern == '*')
      pattern++;

    if (!pattern[0])
      return true;

    while (*s)
    {
      if (DOS_WildCardMatchNoCase(s, pattern))
        return true;
      s++;
    }

    return false;
  }

  while (*pattern != '*')
  {
    if (*pattern == '?')
    {
      if (*s)
      {
        pattern++;
        s++;
        continue;
      }
      return false;
    }

    if (*pattern == '\\')
    {
      switch (pattern[1])
      {
      case '*':
      case '?':
        pattern++;
        break;
      }
    }
    if (towupper(*pattern) != towupper(*s))
    {
      return false;
    }

    if (*s == 0)
      return true;

    pattern++;
    s++;
  }

  return DOS_WildCardMatchNoCase(s, pattern);
}

int CDOSLibApp::ads_dos_strmatch()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  args.Next();

  CString pattern;
  if (!args.GetString(pattern))
    return RSERR;

  args.Next();

  bool rc = false;
  if (args.IsEmpty())
    rc = DOS_WildCardMatch(str, pattern);
  else
    rc = DOS_WildCardMatchNoCase(str, pattern);

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strischar
int CDOSLibApp::ads_dos_strischar()
{
  CAdsArgs args;

  CString str;
  if (!args.GetString(str))
    return RSERR;

  TCHAR ch = str[0];

  args.Next();

  int test = 0;
  if (!args.GetInteger(test))
    return RSERR;

  if (test < 1 || test > 1024)
    return RSERR;

  BOOL rc = TRUE;
  if (test & 1) // Alphanumeric
    rc = rc && (_istalnum(ch) == 0) ? FALSE : TRUE;
  if (test & 2) // Alphabetic
    rc = rc && (_istalpha(ch) == 0) ? FALSE : TRUE;
  if (test & 4) // Decimal digit
    rc = rc && (_istdigit(ch) == 0) ? FALSE : TRUE;
  if (test & 8) // Printable
    rc = rc && (_istprint(ch) == 0) ? FALSE : TRUE;
  if (test & 16) // Punctuation
    rc = rc && (_istpunct(ch) == 0) ? FALSE : TRUE;
  if (test & 32) // White-space
    rc = rc && (_istspace(ch) == 0) ? FALSE : TRUE;
  if (test & 64) // Lowercase
    rc = rc && (_istlower(ch) == 0) ? FALSE : TRUE;
  if (test & 128) // Uppercase
    rc = rc && (_istupper(ch) == 0) ? FALSE : TRUE;
  if (test & 256) // ASCII
    rc = rc && (_istascii(ch) == 0) ? FALSE : TRUE;
  if (test & 512) // Control
    rc = rc && (_istcntrl(ch) == 0) ? FALSE : TRUE;

  if (rc)
    acedRetT();
  else
    acedRetNil();

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strformat
int CDOSLibApp::ads_dos_strformat()
{
  CAdsArgs args;

  // Total number of arguments
  int arg_count = args.Count();

  // Get format string
  CString str;
  if (!args.GetString(str))
    return RSERR;

  // Count the number of '%' tokens.
  // If "%%" sequence is found, skip
  // to next character.
  const int char_count = str.GetLength();
  int i, token_count = 0;
  for (i = 0; i < char_count; i++)
  {
    if (str[i] == '%')
    {
      if (i + 1 < char_count && str[i + 1] == '%')
        i++;
      else
        token_count++;
    }
  }

  // If no tokens found, just return format string.
  if (token_count == 0)
  {
    acedRetStr(str);
    return RSRSLT;
  }

  // Make sure there are at least enough arguments
  // to cover all of the tokens.
  if (token_count > arg_count - 1)
    return RSERR;

  // Tokenize the format string. The first
  // string will not have any tokens in it.
  int first = 0;
  CStringArray strarray;
  for (i = 0; i < char_count; i++)
  {
    if (str[i] != '%')
      continue;

    if (i + 1 < char_count && str[i + 1] == '%')
    {
      i++;
      continue;
    }

    if (i == 0)
    {
      strarray.Add(CString());
      continue;
    }

    CString temp = str.Mid(first, i - first);
    strarray.Add(temp);
    first = i;
  }

  // Add the end of the string
  CString temp = str.Mid(first, char_count - first);
  strarray.Add(temp);

  // One more confirmation
  if (token_count != strarray.GetCount() - 1)
    return RSERR;

  // Format it
  CString strResult = strarray[0];
  for (i = 1; i < strarray.GetCount(); i++)
  {
    CString strTemp;
    CString strFormat = strarray[i];
    args.Next();

    if (args.IsChar())
    {
      wchar_t c;
      args.GetChar(c, false);
      strTemp.Format(strFormat, c);
    }
    else if (args.IsString())
    {
      CString s;
      args.GetString(s);
      strTemp.Format(strFormat, s);
    }
    else if (args.IsDouble())
    {
      double d = 0.0;
      args.GetDouble(d);
      strTemp.Format(strFormat, d);
    }
    else if (args.IsInteger())
    {
      int n = 0;
      args.GetInteger(n);
      strTemp.Format(strFormat, n);
    }
    else
    {
      acedRetNil();
      return RSRSLT;
    }

    strResult += strTemp;
  }

  acedRetStr(strResult);

  return RSRSLT;
}

#define	DIM(a)	(sizeof(a)/sizeof(a[0]))

/////////////////////////////////////////////////////////////////////////////
// dos_strformatnumber

class CDosNumberFormatStruct
{
public:
  CDosNumberFormatStruct(LCID lcid = CP_ACP);
  int GetDefaultFormat(LCID lcid);
  NUMBERFMT m_nf;
public:
  wchar_t m_decimalSep[10];
  wchar_t m_thousandSep[10];
};

CDosNumberFormatStruct::CDosNumberFormatStruct(LCID lcid)
{
  GetDefaultFormat(lcid);
}

int CDosNumberFormatStruct::GetDefaultFormat(LCID lcid)
{
  memset(&m_nf, 0, sizeof(m_nf));

  wchar_t	buff[100];
  int	rc = 1;

  // number of fractional digits
  rc &= GetLocaleInfo(lcid, LOCALE_IDIGITS, buff, DIM(buff));
  m_nf.NumDigits = wcstoul(buff, NULL, 0);

  // leading zeros for decimal
  rc &= GetLocaleInfo(lcid, LOCALE_ILZERO, buff, DIM(buff));
  m_nf.LeadingZero = wcstoul(buff, NULL, 0);

  // decimal separator
  rc &= GetLocaleInfo(lcid, LOCALE_SDECIMAL, m_decimalSep, DIM(m_decimalSep));
  m_nf.lpDecimalSep = m_decimalSep;

  // monetary thousand separator
  rc &= GetLocaleInfo(lcid, LOCALE_SMONTHOUSANDSEP, m_thousandSep, DIM(m_thousandSep));
  m_nf.lpThousandSep = m_thousandSep;

  // negative number mode
  rc &= GetLocaleInfo(lcid, LOCALE_INEGNUMBER, buff, DIM(buff));
  m_nf.NegativeOrder = wcstoul(buff, NULL, 0);

  // digit grouping
  rc &= GetLocaleInfo(lcid, LOCALE_SGROUPING, buff, DIM(buff));
  wchar_t* parseGrp = buff;
  while (*parseGrp)
  {
    if (*parseGrp >= '1' && *parseGrp <= '9')
      m_nf.Grouping = m_nf.Grouping * 10 + (*parseGrp - '0');
    parseGrp++;
  }

  return rc;
}

int CDOSLibApp::ads_dos_strformatnumber()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  //CDosNumberFormatStruct nf(LOCALE_INVARIANT);
  CDosNumberFormatStruct nf(LOCALE_USER_DEFAULT);

  args.Next();

  if (!args.IsEmpty())
  {
    int NumDigits = nf.m_nf.NumDigits;
    if (args.GetInteger(NumDigits))
    {
      if (NumDigits >= 0 && NumDigits <= 9)
        nf.m_nf.NumDigits = NumDigits;
    }

    args.Next();

    if (!args.IsEmpty())
    {
      int LeadingZero = nf.m_nf.LeadingZero;
      if (args.GetInteger(LeadingZero))
      {
        if (LeadingZero >= 0 && LeadingZero <= 1)
          nf.m_nf.LeadingZero = LeadingZero;
      }

      args.Next();

      if (!args.IsEmpty())
      {
        CString ds;
        if (args.GetString(ds) && ds.GetLength() < 4)
          wcscpy(nf.m_decimalSep, ds);

        args.Next();

        if (!args.IsEmpty())
        {
          CString ts;
          if (args.GetString(ts) && ts.GetLength() < 4)
            wcscpy(nf.m_thousandSep, ts);
        }
      }
    }
  }

  CString str;
  str.Format(L"%.16f", d);

  wchar_t szValue[1024];
  int cchNumber = sizeof(szValue) / sizeof(szValue[0]);
  int count = ::GetNumberFormat(LOCALE_USER_DEFAULT, 0, str, &nf.m_nf, szValue, cchNumber);
  if (count > 0)
    acedRetStr(szValue);
  else
    acedRetNil();

  return RSRSLT;
}

/////////////////////////////////////////////////////////////////////////////
// dos_strformatcurrency

class CDosCurrencyFormatStruct
{
public:
  CDosCurrencyFormatStruct(LCID lcid = CP_ACP);
  int GetDefaultFormat(LCID lcid);
  CURRENCYFMT m_cf;
public:
  wchar_t m_decimalSep[10];
  wchar_t m_thousandSep[10];
  wchar_t m_currencySymbol[10];
};

CDosCurrencyFormatStruct::CDosCurrencyFormatStruct(LCID lcid)
{
  GetDefaultFormat(lcid);
}

int CDosCurrencyFormatStruct::GetDefaultFormat(LCID lcid)
{
  memset(&m_cf, 0, sizeof(m_cf));

  wchar_t buff[100];
  int	rc = 1;

  // # local monetary digits
  rc &= GetLocaleInfo(lcid, LOCALE_ICURRDIGITS, buff, DIM(buff));
  m_cf.NumDigits = wcstoul(buff, NULL, 0);

  // leading zeros for decimal
  rc &= GetLocaleInfo(lcid, LOCALE_ILZERO, buff, DIM(buff));
  m_cf.LeadingZero = wcstoul(buff, NULL, 0);

  // monetary decimal separator
  rc &= GetLocaleInfo(lcid, LOCALE_SMONDECIMALSEP, m_decimalSep, DIM(m_decimalSep));
  m_cf.lpDecimalSep = m_decimalSep;

  // monetary thousand separator
  rc &= GetLocaleInfo(lcid, LOCALE_SMONTHOUSANDSEP, m_thousandSep, DIM(m_thousandSep));
  m_cf.lpThousandSep = m_thousandSep;

  // local monetary symbol
  rc &= GetLocaleInfo(lcid, LOCALE_SCURRENCY, m_currencySymbol, DIM(m_currencySymbol));
  m_cf.lpCurrencySymbol = m_currencySymbol;

  // negative currency mode
  rc &= GetLocaleInfo(lcid, LOCALE_INEGCURR, buff, DIM(buff));
  m_cf.NegativeOrder = wcstoul(buff, NULL, 0);

  // positive currency mode
  rc &= GetLocaleInfo(lcid, LOCALE_ICURRENCY, buff, DIM(buff));
  m_cf.PositiveOrder = wcstoul(buff, NULL, 0);

  // monetary grouping
  rc &= GetLocaleInfo(lcid, LOCALE_SMONGROUPING, buff, DIM(buff));
  wchar_t* parseGrp = buff;
  while (*parseGrp)
  {
    if (*parseGrp >= '1' && *parseGrp <= '9')
      m_cf.Grouping = m_cf.Grouping * 10 + (*parseGrp - '0');
    parseGrp++;
  }

  return rc;
}

int CDOSLibApp::ads_dos_strformatcurrency()
{
  CAdsArgs args;

  double d = 0.0;
  if (!args.GetDouble(d))
    return RSERR;

  CDosCurrencyFormatStruct cf(LOCALE_USER_DEFAULT);

  args.Next();

  if (!args.IsEmpty())
  {
    int NumDigits = cf.m_cf.NumDigits;
    if (args.GetInteger(NumDigits))
    {
      if (NumDigits >= 0 && NumDigits <= 9)
        cf.m_cf.NumDigits = NumDigits;
    }

    args.Next();

    if (!args.IsEmpty())
    {
      int LeadingZero = cf.m_cf.LeadingZero;
      if (args.GetInteger(LeadingZero))
      {
        if (LeadingZero >= 0 && LeadingZero <= 1)
          cf.m_cf.LeadingZero = LeadingZero;
      }

      args.Next();

      if (!args.IsEmpty())
      {
        CString ds;
        if (args.GetString(ds) && ds.GetLength() < 4)
          wcscpy(cf.m_decimalSep, ds);

        args.Next();

        if (!args.IsEmpty())
        {
          CString ts;
          if (args.GetString(ts) && ts.GetLength() < 4)
            wcscpy(cf.m_thousandSep, ts);

          args.Next();

          if (!args.IsEmpty())
          {
            CString cs;
            if (args.GetString(cs) && cs.GetLength() < 6)
              wcscpy(cf.m_currencySymbol, cs);
          }
        }
      }
    }
  }

  CString str;
  str.Format(L"%.16f", d);

  wchar_t szValue[1024];
  int cchNumber = sizeof(szValue) / sizeof(szValue[0]);
  int count = ::GetCurrencyFormat(LOCALE_USER_DEFAULT, 0, str, &cf.m_cf, szValue, cchNumber);
  if (count > 0)
    acedRetStr(szValue);
  else
    acedRetNil();

  return RSRSLT;
}

int CDOSLibApp::ads_dos_strformatdate()
{
  CAdsArgs args;

  CString strFormat;
  if (!args.GetString(strFormat))
    return RSERR;

  args.Next();

  CArray<int, int> t;
  if (!args.GetIntegerList(t))
    return RSERR;

  int c = (int)t.GetCount();
  if (c < 3)
    return RSERR;

  CTime time(
    t[0], // year
    t[1], // month
    t[2], // day
    (c > 3) ? t[3] : 0, // hours
    (c > 4) ? t[4] : 0, // minutes
    (c > 5) ? t[5] : 0  // seconds
    );

  CString strTime = time.Format(strFormat);

  if (strTime.IsEmpty())
    acedRetNil();
  else
    acedRetStr(strTime);

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strsort
int CDOSLibApp::ads_dos_strsort()
{
  CAdsArgs args;

  CDosStringArray strlist;
  if (!args.GetStringList(strlist))
    return RSERR;

  // -1 = logical
  //  0 = not case sensitive
  //  1 = case sensitive
  int case_sensitive = 0;
  bool bAscending = true;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetInteger(case_sensitive))
      return RSERR;

    case_sensitive = DOS_CLAMP(case_sensitive, -1, 1);

    args.Next();

    if (!args.IsEmpty())
    {
      if (args.IsTrue())
        bAscending = true;
      else if (args.IsNil())
        bAscending = false;
      else
        return RSERR;
    }
  }

  if (-1 == case_sensitive)
    strlist.SortLogical(bAscending);
  else
    strlist.Sort((1 == case_sensitive), bAscending);

  CAdsRetList result;

  int i;
  for (i = 0; i < strlist.GetCount(); i++)
    result.Add(strlist[i]);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strcull
int CDOSLibApp::ads_dos_strcull()
{
  CAdsArgs args;

  CDosStringArray strlist;
  if (!args.GetStringList(strlist))
    return RSERR;

  args.Next();

  bool bCaseSensitive = (args.IsEmpty() ? true : false);
  strlist.Cull(bCaseSensitive);

  CAdsRetList result;

  int i;
  for (i = 0; i < strlist.GetCount(); i++)
    result.Add(strlist[i]);

  if (result.IsEmpty())
    acedRetNil();
  else
    acedRetList(result);

  return RSRSLT;
}

//////////////////////////////////////////////////////////////////////////////////////////
// dos_strregexp
int CDOSLibApp::ads_dos_strregexp()
{
  CAdsArgs args;

  CString strRegExp, strInput, strValue;
  BOOL bReplaceAll = TRUE;
  BOOL bIgnoreCase = FALSE;

  if (!args.GetString(strRegExp))
    return RSERR;

  args.Next();

  if (!args.GetString(strInput))
    return RSERR;

  args.Next();

  if (!args.IsEmpty())
  {
    if (!args.GetBoolean(bIgnoreCase))
      return RSERR;

    args.Next();

    if (!args.IsEmpty())
    {
      if (!args.GetString(strValue))
        return RSERR;

      args.Next();

      if (!args.IsEmpty())
      {
        if (!args.GetBoolean(bReplaceAll))
          return RSERR;
      }
    }
  }

  if (!strValue.IsEmpty())
  {
    CString strOutput = CDosRegExp::Replace(strInput, strRegExp, strValue, bReplaceAll, bIgnoreCase);
    if (strOutput.IsEmpty())
      acedRetNil();
    else
      acedRetStr(strOutput);
  }
  else
  {
    CDosRegExp re(strRegExp, bIgnoreCase);
    if (re.m_error != REPARSE_ERROR_OK)
    {
      CString strError;
      strError.Format(L"DOSLib error: %s\n", re.GetErrorName(re.m_error));
      acutPrintf(strError);
      acedRetNil();
      return RSRSLT;
    }

    CAdsRetList result;

    re.SetInput(strInput);
    int count = 0;
    while (re.NextMatch())
    {
      int offset = 0;
      CString strMatch = re.GetMatch(&offset);
      result.AddLB();
      result.Add(offset);
      result.Add(strMatch);

      UINT nGroups = re.GetNumGroups();
      for (UINT i = 0; i < nGroups; i++)
      {
        CString strGroup = re.GetGroup(i, &offset);
        result.AddLB();
        result.Add(offset);
        result.Add(strGroup);
        result.AddLE();
      }

      result.AddLE();
      count++;
    }

    if (result.IsEmpty())
      acedRetNil();
    else
      acedRetList(result);
  }

  return RSRSLT;
}
