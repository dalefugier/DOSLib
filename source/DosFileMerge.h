/////////////////////////////////////////////////////////////////////////////
// DosFileMerge.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#define BYTES_PER_READ 65536

class CDosFileMerge
{
public:
  CDosFileMerge(const CString& output, const CStringArray& input);
  virtual ~CDosFileMerge();

  bool Merge();

protected:
  CString m_output;
  CStringArray m_input;
  CFile* m_pInFile;
  CFile* m_pOutFile;
};
