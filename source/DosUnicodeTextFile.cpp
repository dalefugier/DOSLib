/////////////////////////////////////////////////////////////////////////////
// DosUnicodeTextFile.cpp
//
// Copyright © 1992-2018, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosUnicodeTextFile.h"

CDosUnicodeTextFile::CDosUnicodeTextFile()
  : m_fp(0)
{
  m_cpu_endian = Endian();
  Close();
}

CDosUnicodeTextFile::~CDosUnicodeTextFile()
{
  Close();
}

void CDosUnicodeTextFile::Close()
{
  if (m_fp)
    fclose(m_fp);
  m_fp = 0;
  m_file_endian = m_cpu_endian;
  m_status = no_status;
  m_filetype = unknown_filetype;
  m_bWriteBOM = false;
}

bool CDosUnicodeTextFile::OpenToRead(const wchar_t* pszFileName)
{
  bool rc = false;
  Close();
  if (pszFileName && pszFileName[0])
  {
    errno_t err = _wfopen_s(&m_fp, pszFileName, L"rb");
    if (0 == err && m_fp)
      rc = ValidateToRead();
  }
  return rc;
}

bool CDosUnicodeTextFile::OpenToWrite(const wchar_t* pszFileName, filetype ft)
{
  bool rc = false;
  Close();
  if (pszFileName && pszFileName[0])
  {
    errno_t err = _wfopen_s(&m_fp, pszFileName, L"wb");
    if (0 == err && m_fp)
    {
      m_filetype = ft;
      m_status = write_status;
      rc = true;
    }
  }
  return rc;
}

bool CDosUnicodeTextFile::OpenToAppend(const wchar_t* pszFileName)
{
  bool rc = false;
  Close();
  if (pszFileName && pszFileName[0])
  {
    // Try opening the file for reading
    // so the file type can be determined
    rc = OpenToRead(pszFileName);
    if (rc)
    {
      // Calling Close() will reset the file type, so save it first.
      filetype ft = m_filetype;
      Close();

      // Now open file file for appending
      errno_t err = _wfopen_s(&m_fp, pszFileName, L"ab");
      rc = (0 == err && m_fp);
      if (rc)
      {
        m_filetype = ft; // Set file type
        m_status = write_status;
        m_bWriteBOM = true; // 11-Dec-2009 Dale Fugier
      }
    }
  }
  return rc;
}

bool CDosUnicodeTextFile::ValidateToRead()
{
  bool rc = false;
  if (m_fp)
  {
    if (m_status == no_status)
    {
      unsigned char c[3];
      if (3 != fread(c, 1, 3, m_fp))
      {
        Close();
        return false;
      }
      if ((c[0] == 0xFF) && (c[1] == 0xFE))
      {
        m_file_endian = little_endian;
        m_filetype = utf16_filetype;
        rc = RewindFilePointer(1);
      }
      else if ((c[0] == 0xFE) && (c[1] == 0xFF))
      {
        m_file_endian = big_endian;
        m_filetype = utf16_filetype;
        rc = RewindFilePointer(1);
      }
      else if ((c[0] == 0xEF) && (c[1] == 0xBB) && (c[2] == 0xBF))
      {
        m_filetype = utf8_filetype;
        rc = true;
      }
      else
      {
        m_filetype = ansi_filetype;
        rc = RewindFilePointer(3);
      }

      m_status = rc ? read_status : no_status;
    }
  }

  if (!rc)
    Close();

  return rc;
}

static wchar_t toWchar(const unsigned char c)
{
  wchar_t w;
  if (1 != ::MultiByteToWideChar(CP_ACP, 0, (const char*)&c, 1, &w, 1))
    w = c;
  return w;
}

bool CDosUnicodeTextFile::ReadString(CString& string)
{
  bool rc = false;
  string.Empty();
  if (m_status == read_status)
  {
    switch (m_filetype)
    {
    case unknown_filetype:
      rc = false;
      break;

    case ansi_filetype:
    {
      CStringA str_a;
      char c;
      while (1 == fread(&c, 1, 1, m_fp))
      {
        rc = true;
        str_a += c;
        if (c == 0x0A)
          break; // stop reading at \n characters
      }

      if (rc)
      {
        CStringW str_w;
        wchar_t* psz = str_w.GetBufferSetLength(str_a.GetLength());
        int res = ::MultiByteToWideChar(CP_ACP, 0, str_a, str_a.GetLength(), psz, str_w.GetLength());
        str_w.ReleaseBuffer();
        if (res)
          string = str_w;
        else
          string = str_a;
      }
    }
    break;

    case utf8_filetype:
    {
      wchar_t w;
      unsigned char c;
      unsigned char c2[3];
      unsigned int u[4], wu;
      while (1 == fread(&c, 1, 1, m_fp))
      {
        rc = true;
        w = 0L;
        if ((c & 0xF8) == 0xF0) // 11110...
        {
          // 4-byte UTF8 character encountered
          if (3 == fread(&c2[0], 1, 3, m_fp))
          {
            // Make sure that the next three characters have leading binary "10......" in the byte.
            if (((c2[0] & 0xC0) == 0x80) && ((c2[1] & 0xC0) == 0x80) && ((c2[2] & 0xC0) == 0x80))
            {
              // concatenate appropriate bytes of c and c2 into w.
              u[0] = c & 0x07;
              u[1] = c2[0] & 0x3F;
              u[2] = c2[1] & 0x3F;
              u[3] = c2[2] & 0x3F;
              wu = (u[0] << 18) | (u[1] << 12) | (u[2] << 6) | (u[3]);
              w = wu;
            }
            else
            {
              // interpret c as illegal ASCII character and read it as such,
              // and rewind file pointer to unread last three characters.
              w = toWchar(c);
              RewindFilePointer(3);
            }
          }
          else
          {
            // there aren't 3 more characters to complete this sequence.
            // Assume this is an illegal ASCII character and press on...
            w = toWchar(c);
          }
        }
        else if ((c & 0xF0) == 0xE0) // 1110....
        {
          // 3-byte UTF8 character encountered
          if (2 == fread(&c2[0], 1, 2, m_fp))
          {
            // Make sure that the next two characters have leading binary "10......" in the byte.
            if (((c2[0] & 0xC0) == 0x80) && ((c2[1] & 0xC0) == 0x80))
            {
              // concatenate appropriate bytes of c and c2 into w.
              u[0] = c & 0xF;
              u[1] = c2[0] & 0x3F;
              u[2] = c2[1] & 0x3F;
              wu = (u[0] << 12) | (u[1] << 6) | u[2];
              w = wu;
            }
            else
            {
              // interpret c as illegal ASCII character and read it as such,
              // and rewind file pointer to unread last two characters.
              w = toWchar(c);
              RewindFilePointer(2);
            }
          }
          else
          {
            // there aren't 2 more characters to complete this sequence.
            // Assume this is an illegal ASCII character and press on...
            w = toWchar(c);
          }
        }
        else if ((c & 0xE0) == 0xC0) // 110.....
        {
          // 2-byte UTF8 character encountered
          if (1 == fread(&c2[0], 1, 1, m_fp))
          {
            // Make sure that the next character has a leading binary "10......" in the byte.
            if ((c2[0] & 0xC0) == 0x80)
            {
              // concatenate appropriate bytes of c and c2 into w.
              u[0] = c & 0x1F;
              u[1] = c2[0] & 0x3F;
              wu = (u[0] << 6) | u[1];
              w = wu;
            }
            else
            {
              // interpret c as illegal ASCII character and read it as such,
              // and rewind file pointer to unread last character.
              w = toWchar(c);
              RewindFilePointer(1);
            }
          }
          else
          {
            // there isn't 1 more characters to complete this sequence.
            // Assume this is an illegal ASCII character and press on...
            w = toWchar(c);
          }
        }
        else
        {
          // 1-byte UTF8 character encountered
          w = toWchar(c);
        }

        if (c == 0x0D)
          continue; // skip \r characters

        string += w;

        if (c == 0x0A)
          break; // stop reading at \n characters
      }
      break;
    }

    case utf16_filetype:
    {
      wchar_t c;
      while (2 == fread(&c, 1, 2, m_fp))
      {
        rc = true;
        string += c;
        if (c == 0x0A)
          break; // stop reading at \n characters
      }
      break;
    }
    } // end switch

    if (rc && (m_file_endian != m_cpu_endian))
    {
      wchar_t* psz = string.GetBuffer();
      ToggleByteOrder(string.GetLength(), 2, psz, psz);
      string.ReleaseBuffer();
    }
  }

  return rc;
}

bool CDosUnicodeTextFile::WriteBOM()
{
  bool rc = false;
  switch (m_filetype)
  {
  case unknown_filetype:
    rc = false;
    break;
  case ansi_filetype:
    rc = true;
    break;
  case utf8_filetype:
  {
    unsigned char c[] = { 0xEF, 0xBB, 0xBF };
    if (3 == fwrite(c, 1, 3, m_fp))
      rc = true;
  }
  break;
  case utf16_filetype:
  {
    unsigned char c[] = { 0xFF, 0xFE };
    if (2 == fwrite(c, 1, 2, m_fp))
      rc = true;
  }
  }
  return rc;
}

bool CDosUnicodeTextFile::WriteString(const wchar_t* pszString)
{
  bool rc = false;
  if (m_status != write_status)
    return rc;

  if (0 == pszString || 0 == pszString[0])
    return rc;

  if (!m_bWriteBOM)
  {
    rc = WriteBOM();
    if (rc)
      m_bWriteBOM = true;
    else
      return rc;
  }

  CString str_w(pszString);
  // Write \n as a \r\n pair.
  str_w.Remove('\r');
  str_w.Replace(L"\n", L"\r\n");

  if (m_file_endian != m_cpu_endian)
  {
    wchar_t* psz = str_w.GetBuffer();
    ToggleByteOrder(str_w.GetLength(), 2, psz, psz);
    str_w.ReleaseBuffer();
  }

  switch (m_filetype)
  {
  case unknown_filetype:
    break;

  case ansi_filetype:
  {
    int len_w = str_w.GetLength();
    int len_a = ::WideCharToMultiByte(CP_ACP, 0, str_w, len_w, 0, 0, 0, 0);
    if (len_a)
    {
      CStringA str_a;
      char* psz = str_a.GetBufferSetLength(len_a);
      int res = ::WideCharToMultiByte(CP_ACP, 0, str_w, len_w, psz, len_a, 0, 0);
      str_a.ReleaseBuffer();
      if (res)
      {
        if (len_a == fwrite(str_a, 1, len_a, m_fp))
          rc = true;
      }
    }
  }
  break;

  case utf8_filetype:
  {
    int len_w = str_w.GetLength();
    int len_a = ::WideCharToMultiByte(CP_UTF8, 0, str_w, len_w, 0, 0, 0, 0);
    if (len_a)
    {
      CStringA str_a;
      char* psz = str_a.GetBufferSetLength(len_a);
      int res = ::WideCharToMultiByte(CP_UTF8, 0, str_w, len_w, psz, len_a, 0, 0);
      str_a.ReleaseBuffer();
      if (res)
      {
        if (len_a == fwrite(str_a, 1, len_a, m_fp))
          rc = true;
      }
    }
  }
  break;

  case utf16_filetype:
  {
    int len_w = str_w.GetLength();
    if (len_w == fwrite(str_w, 2, len_w, m_fp))
      rc = true;
  }
  break;
  }
  return rc;
}

bool CDosUnicodeTextFile::RewindFilePointer(fpos_t pos_distance)
{
  bool rc = false;
  if (m_fp)
  {
    if (pos_distance < 0)
    {
      rewind(m_fp);
      rc = true;
    }
    else
    {
      fpos_t pos;
      if (0 == fgetpos(m_fp, &pos))
      {
        pos -= pos_distance;
        if (0 == fsetpos(m_fp, &pos))
          rc = true;
      }
    }
  }
  return rc;
}

CDosUnicodeTextFile::filetype CDosUnicodeTextFile::GetFileType() const
{
  return m_filetype;
}

CDosUnicodeTextFile::filetype CDosUnicodeTextFile::FileType(int i)
{
  filetype ft = ansi_filetype;
  switch (i)
  {
  case 1:
    ft = ansi_filetype;
    break;
  case 2:
    ft = utf8_filetype;
    break;
  case 3:
    ft = utf16_filetype;
    break;
  }
  return ft;
}

CDosUnicodeTextFile::endian CDosUnicodeTextFile::Endian(int i)
{ // convert integer to endian enum
  endian e = (i <= 0) ? little_endian : big_endian;
  return e;
}

CDosUnicodeTextFile::endian CDosUnicodeTextFile::Endian()
{
  // returns endian-ness of cpu.
  union {
    int i;
    unsigned char b[sizeof(int)];
  } u;
  u.i = 1;
  return (u.b[0] == 1) ? little_endian : big_endian;
}

bool CDosUnicodeTextFile::ToggleByteOrder(
  int count,          // number of elements
  int sizeof_element, // size of element (2,4, or 8)
  const void* src,    // source buffer
  void* dst           // destination buffer (can be same as source buffer)
  )
{
  unsigned char c[32];
  const unsigned char* a = (const unsigned char*)src;
  unsigned char* b = (unsigned char*)dst;

  bool rc = (count == 0 || (count > 0 && src&&dst));
  if (rc)
  {
    // loops are unrolled and a switch is used
    // to speed things up a bit.
    switch (sizeof_element)
    {
    case 2:
      while (count--)
      {
        c[0] = *a++;
        c[1] = *a++;
        *b++ = c[1];
        *b++ = c[0];
      }
      break;

    case 4:
      while (count--)
      {
        c[0] = *a++;
        c[1] = *a++;
        c[2] = *a++;
        c[3] = *a++;
        *b++ = c[3];
        *b++ = c[2];
        *b++ = c[1];
        *b++ = c[0];
      }
      break;

    case 8:
      while (count--)
      {
        c[0] = *a++;
        c[1] = *a++;
        c[2] = *a++;
        c[3] = *a++;
        c[4] = *a++;
        c[5] = *a++;
        c[6] = *a++;
        c[7] = *a++;
        *b++ = c[7];
        *b++ = c[6];
        *b++ = c[5];
        *b++ = c[4];
        *b++ = c[3];
        *b++ = c[2];
        *b++ = c[1];
        *b++ = c[0];
      }
      break;

    default:
      if (sizeof_element > 0 && sizeof_element < 32)
      {
        // As of 2 May 2003, this case is never used
        // by core opennurbs objects.
        //
        // This is here so that future code will work
        // if and when 128 bit "ints"/"doubles" become common
        // enough that they can be stored in 3dm files.
        // It may also happen that third party applications
        // on specialized CPUs need to toggle byte order
        // for 128 bit ints/doubles stored as user data.
        int i;
        while (count--)
        {
          for (i = 0; i < sizeof_element; i++)
            c[i] = *a++;
          while (i--)
            *b++ = c[i];
        }
      }
      else
      {
        rc = false;
      }
      break;
    }
  }
  return rc;
}