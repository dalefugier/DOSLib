/////////////////////////////////////////////////////////////////////////////
// DosUnicodeTextFile.h
//
// Copyright © 1992-2015 Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosUnicodeTextFile
{
public:
  CDosUnicodeTextFile();
  ~CDosUnicodeTextFile();

  enum endian
  {
    little_endian, // least significant byte first or reverse byte order - Intel x86, ...
    big_endian     // most significant byte first - Motorola, Sparc, MIPS, ...
  };
  static endian Endian(int); // convert integer to endian enum
  static endian Endian();    // returns endian-ness of current CPU

  enum filetype
  {
    unknown_filetype,
    ansi_filetype,
    utf8_filetype,
    utf16_filetype,
  };
  static filetype FileType(int); // convert integer to filetype enum

  enum status
  {
    no_status,
    read_status,
    write_status,
  };

  bool OpenToRead(const wchar_t* pszFileName);
  bool OpenToWrite(const wchar_t* pszFileName, filetype ft = ansi_filetype);
  bool OpenToAppend(const wchar_t* pszFileName);

  filetype GetFileType() const;
  bool ReadString(CString& string);
  bool WriteString(const wchar_t* pszString);

  void Close();

private:
  FILE* m_fp;
  endian m_cpu_endian;
  endian m_file_endian;
  status m_status;
  filetype m_filetype;
  bool m_bWriteBOM;

private:
  CDosUnicodeTextFile(const CDosUnicodeTextFile&);
  CDosUnicodeTextFile& operator=(const CDosUnicodeTextFile&);

  bool ValidateToRead();
  bool RewindFilePointer(fpos_t pos_distance);
  bool ToggleByteOrder(int count, int sizeof_element, const void* src, void* dst);
  bool WriteBOM();
};
