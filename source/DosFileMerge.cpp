/////////////////////////////////////////////////////////////////////////////
// DosFileMerge.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosFileMerge.h"

CDosFileMerge::CDosFileMerge(const CString& output, const CStringArray& input)
  : m_pInFile(0)
  , m_pOutFile(0)
{
  m_output = output;
  m_input.Append(input);
}

CDosFileMerge::~CDosFileMerge()
{
  if (m_pInFile)
  {
    delete m_pInFile;
    m_pInFile = 0;
  }

  if (m_pOutFile)
  {
    delete m_pOutFile;
    m_pOutFile = 0;
  }
}

bool CDosFileMerge::Merge()
{
  if (m_pInFile)
  {
    delete m_pInFile;
    m_pInFile = 0;
  }

  if (m_pOutFile)
  {
    delete m_pOutFile;
    m_pOutFile = 0;
  }

  if (m_input.GetCount() == 0 || m_output.IsEmpty())
    return false;

  try
  {
    m_pOutFile = new CFile();
    if (PathFileExists(m_output))
    {
      if (!m_pOutFile->Open(m_output, CFile::typeBinary | CFile::modeWrite))
        return false;
      m_pOutFile->SeekToEnd();
    }
    else
    {
      if (!m_pOutFile->Open(m_output, CFile::modeCreate | CFile::typeBinary | CFile::modeWrite))
        return false;
    }

    m_pInFile = new CStdioFile();

    int i;
    for (i = 0; i < m_input.GetCount(); i++)
    {
      if (!m_pInFile->Open(m_input[i], CFile::modeRead | CFile::typeBinary))
      {
        m_pOutFile->Close();
        return false;
      }

      int nBytesRead = 0;
      char buffer[BYTES_PER_READ];

      do
      {
        nBytesRead = m_pInFile->Read(buffer, BYTES_PER_READ);
        m_pOutFile->Write(buffer, nBytesRead);
      } while (nBytesRead == BYTES_PER_READ);

      m_pInFile->Close();
    }

    m_pOutFile->Close();

    if (m_pInFile)
    {
      delete m_pInFile;
      m_pInFile = 0;
    }

    if (m_pOutFile)
    {
      delete m_pOutFile;
      m_pOutFile = 0;
    }
  }

  catch (...)
  {
    return false;
  }

  return true;
}
