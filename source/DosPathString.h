///////////////////////////////////////////////////////////////////////////////
// PathString.h
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosPathString : public CString
{
public:
  CDosPathString() : CString() {}
  CDosPathString(const CString& stringSrc) : CString(stringSrc) {}
  CDosPathString(const CDosPathString& stringSrc) : CString(stringSrc) {}
  CDosPathString(TCHAR ch, int nRepeat = 1) : CString(ch, nRepeat) {}
  CDosPathString(LPCTSTR lpch, int nLength) : CString(lpch, nLength) {}
  CDosPathString(const unsigned char* psz) : CString(psz) {}
  CDosPathString(LPCWSTR lpsz) : CString(lpsz) {}
  CDosPathString(LPCSTR lpsz) : CString(lpsz) {}

  // Get stuff
  CString GetDrive(bool bAddBackslash = false);
  CString GetPath(bool bAddBackslash = false);
  CString GetFileName();
  CString GetExtension();

  // Adds a backslash to the end of the path to create the correct syntax for a
  // path. If the path already has a trailing backslash, no backslash will be added.
  void AddBackslash();

  // Adds a file extension to the path.
  BOOL AddExtension(LPCTSTR pszExt);

  // Appends one path to the end of another. 
  BOOL Append(LPCTSTR pszMore);

  // Creates a root path from a given drive number. 
  BOOL BuildRoot(int nDrive);

  // Canonicalizes a path. 
  BOOL Canonicalize();

  // Concatenates two strings that represent properly formed paths into one path,
  // as well as any relative path pieces.
  BOOL Combine(LPCTSTR pszDir, LPCTSTR pszFile);

  // Truncates the path to fit within a given pixel width by replacing path
  // components with ellipses.
  BOOL CompactPath(HDC hDC, UINT dx);

  // Truncates a path to fit within a certain number of characters by replacing
  // path components with ellipses.
  BOOL CompactPathEx(UINT cchMax);

  // Takes a file URL and converts it to a DOS path. 
  BOOL CreateFromUrl(LPCTSTR pszUrl);

  // Determines if a file exists. 
  BOOL FileExists();

  // Create an absolute or full path name from a relative path name.
  BOOL FullPath();

  // Converts a short path to long path
  BOOL LongPath();

  // Converts a long path to a short path
  BOOL ShortPath();

  // Verifies that a path is a valid directory. 
  BOOL IsDirectory();

  // Determines whether or not a specified path is an empty directory. 
  BOOL IsDirectoryEmpty();

  // Searches a path for any path delimiting characters (for example, ':' or '\' ).
  // If there are no path delimiting characters present, the path is considered
  // to be a File Spec path. 
  BOOL IsFileSpec();

  // Determines if a file is an HTML file. The determination is made based on
  // the content type that is registered for the file's extension. 
  BOOL IsHTMLFile();

  // Determines whether or not a file name is in long format. 
  BOOL IsLFNFileSpec();

  //Determines whether a path string represents a network resource.
  BOOL IsNetworkPath();

  // Searches the path to determine if it contains a valid prefix
  // A prefix is one of these types: "C:\\", ".", "..", "..\\". 
  BOOL IsPrefix(LPCTSTR  pszPrefix);

  // Searches the path and determines if it is relative.
  BOOL IsRelative();

  // Parses the path to determine if it is a directory root. 
  BOOL IsRoot();

  // Compares two paths to determine if they have a common root component. 
  BOOL IsSameRoot(LPCTSTR pszPath);

  // Determines if an existing folder contains the attributes that make it a system folder.
  BOOL IsSystemFolder();

  // Determines if the string is a valid UNC (universal naming convention)
  // for a server and share path. 
  BOOL IsUNC();

  // Determines if a string is a valid UNC (universal naming convention)
  // for a server path only. 
  BOOL IsUNCServer();

  // Determines if a string is a valid universal naming convention (UNC)
  // share path (e.g \\server\share ).
  BOOL IsUNCServerShare();

  // Tests the path to determine if it conforms to a valid URL format.
  BOOL IsURL();

  // Converts the path to all lowercase characters to give the path
  // a consistent appearance. 
  BOOL MakePretty();

  // Searches the path using a DOS wild card match type.
  BOOL MatchSpec(LPCTSTR pszSpec);

  //Parses a file location string containing a file location and icon index,
  // and returns separate values. 
  int ParseIconLocation();

  // Searches the path for spaces. If spaces are found, the entire path
  // is enclosed in quotation marks.
  void QuoteSpaces();

  // Creates a relative path from one file or folder to another.
  BOOL RelativePathTo(LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo);

  // Removes any command-line arguments from the path. 
  void RemoveArgs();

  // Removes the trailing backslash from the path. 
  void RemoveBackslash();

  // Removes all leading and trailing spaces from the path. 
  void RemoveBlanks();

  // Removes the file extension from the path, if there is one.
  void RemoveExtension();

  // Removes the trailing file name and backslash from the path, if it has them.
  BOOL RemoveFileSpec();

  // Replaces the extension of a file name with a new extension.
  // If the file name does not contain an extension,
  // the extension will be attached to the end of the string. 
  BOOL RenameExtension(LPCTSTR pszExt);

  // Parses the path, ignoring the drive letter or UNC server/share path parts.
  void SkipRoot();

  // Removes all parts of the path except for the root information. 
  BOOL StripToRoot();

  // Removes quotes from the beginning and end of the path. 
  void UnquoteSpaces();

  void RemoveRoot();
};
