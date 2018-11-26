/////////////////////////////////////////////////////////////////////////////
// DosRegExp.h
//
// Copyright © 1992-2012 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Assoicates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

// CDosRegExp encapsulates CAtlRegExp<> to make ATL regular expressions a little
// more usable. CDosRegExp hides CAtlRegExp and CAtlREMatchContext and also
// implements useful functions like Replace and Split.
#include "atlrx.h"

class CDosRegExp : public CAtlRegExp<> 
{
public:
	CDosRegExp();
	CDosRegExp( const wchar_t* szRegExp, BOOL bCaseSensitive = TRUE );
	CDosRegExp( const CDosRegExp& src );
	~CDosRegExp();

	const CDosRegExp& operator=( const CDosRegExp& src );
	operator const wchar_t*() const;

  // Parse szRegExp: reset internal state and return error code
	REParseError Parse( const wchar_t* szRegExp, BOOL bCaseSensitive = TRUE );

  // Set input string. Use this with NextMatch to find all matches
	void SetInput( const wchar_t* szInput );

	// Find a single match in input string.
	// Optional argument returns pointer to next input char to search
  // after match. Used internally to find successive matches.
	BOOL Match( const wchar_t* szInput, const wchar_t** ppszEnd = 0 );

  // Find next match after calling SetInput
	BOOL NextMatch();

	// Get current match
  // Optional argument returns offset into input string.
	CString GetMatch( int* pOffset = 0) const;

	// Get number of groups. In ATL syntax, groups are marked with {}.
	UINT GetNumGroups() const;

	// Get nth match group. 
  // Optional argument returns offset into input string.
	CString GetGroup( int nIndex, int* pOffset = 0) const;

	// Callback for dynamic replace
	typedef CString (CALLBACK* REREPLACEPROC)( const CDosRegExp& src, const CString& strMatch, void* pParam );

  // Global replace. This is not implemented by ATL.
  // Does not support $1 $2 variables (replace with subgroup match).
	CString Replace( const wchar_t* szInput, const wchar_t* szValue, BOOL bAll = TRUE );
	static CString Replace( const wchar_t* szInput, const wchar_t* szRegExp, const wchar_t* szValue, BOOL bAll = TRUE, BOOL bIgnoreCase = FALSE );

	CString Replace( const wchar_t* szInput, REREPLACEPROC replfn, void* pParam );
	static CString Replace( const wchar_t* szInput, const wchar_t* szRegExp, REREPLACEPROC replfn, void* pParam, BOOL bIgnoreCase = FALSE );

  // / Split function implements Perl-style split.
	INT_PTR Split( const wchar_t* szInput, CStringArray& arr, INT_PTR maximum = 0 );
	static INT_PTR Split( const wchar_t* szInput, const wchar_t* szSep, CStringArray& arr, BOOL bCaseSensitive = TRUE, INT_PTR nMax = 0 );

  // Helper functions to get error name from error code
	static CString GetErrorName( REParseError err );

  // Current ATL parse error, if any
	REParseError m_error;			

protected:
  // helper to extract string from ATL MatchGroup
	CString GetMGString( const CAtlREMatchContext<>::MatchGroup& mg ) const;

protected:
 	CString m_strRegExp;       // the regular expression
	BOOL m_bCaseSensitive;     // case sensitivity
	CAtlREMatchContext<> m_mc; // internal match context
	const wchar_t* m_szInput;  // original input string
	const wchar_t* m_szNext;   // next character to search
};