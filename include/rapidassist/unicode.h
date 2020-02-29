/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef RA_UNICODE_H
#define RA_UNICODE_H

#include <string>

namespace ra { namespace unicode {

  /// <summary>
  /// Returns true if the given string is encoded in ASCII.
  /// </summary>
  /// <param name="str">The buffer of the given string.</param>
  /// <returns>Returns true if the given string is encoded in ASCII. Returns false otherwise</returns>
  bool IsAscii(const char * str);

  /// <summary>
  /// Returns true if the given string is compatible with Windows CP 1252 encoding.
  /// </summary>
  /// <param name="str">The buffer of the given string.</param>
  /// <returns>Returns true if the given string is compatible with Windows CP 1252 encoding. Returns false otherwise</returns>
  bool IsValidCp1252(const char * str);

  /// <summary>
  /// Returns true if the given string is compatible with ISO-8859-1 encoding.
  /// </summary>
  /// <param name="str">The buffer of the given string.</param>
  /// <returns>Returns true if the given string is compatible with ISO-8859-1 encoding. Returns false otherwise</returns>
  /// <remarks>
  /// A buffer that is compatible with ISO-8859-1 encoding will always be compatible with Windows CP 1252 encoding.
  /// In other words, all characters of ISO-8859-1 encoding are also available in Windows CP 1252 encoding.
  /// See https://en.wikipedia.org/wiki/Windows-1252#Code_page_layout and
  /// https://en.wikipedia.org/wiki/ISO/IEC_8859-1#Code_page_layout for details.
  /// </remarks>
  bool IsValidIso8859_1(const char * str);

  /// <summary>
  /// Returns true if the given string is compatible with UTF-8 encoding.
  /// </summary>
  /// <param name="str">The buffer of the given string.</param>
  /// <returns>Returns true if the given string is compatible with UTF-8 encoding. Returns false otherwise</returns>
  /// <remarks>A buffer that is pure ASCII will always be compatible with UTF-8 encoding.</remarks>
  bool IsValidUtf8(const char * str);



//--------------------------------------------------------------------------------------------------
//The following functions are Windows helper functions to provide conversion between different encoding.
//--------------------------------------------------------------------------------------------------
#ifdef _WIN32

  /// <summary>
  /// Convert a wide unicode string to an utf8 string
  /// </summary>
  /// <param name="wstr">The unicode string that must be converted.</param>
  /// <returns>Returns an utf8 encoded string.</returns>
  std::string UnicodeToUtf8(const std::wstring & wstr);

  /// <summary>
  /// Convert an utf8 string to a wide unicode string
  /// </summary>
  /// <param name="str">The utf8 string that must be converted.</param>
  /// <returns>Returns an unicode encoded string.</returns>
  std::wstring Utf8ToUnicode(const std::string & str);

  /// <summary>
  /// Convert an wide unicode string to ansi string
  /// </summary>
  /// <param name="str">The unicode string that must be converted.</param>
  /// <returns>Returns an ansi encoded string.</returns>
  std::string UnicodeToAnsi(const std::wstring & wstr);

  /// <summary>
  /// Convert an ansi string to a wide unicode string
  /// </summary>
  /// <param name="str">The ansi string that must be converted.</param>
  /// <returns>Returns an unicode encoded string.</returns>
  std::wstring AnsiToUnicode(const std::string & str);

  /// <summary>
  /// Convert an utf8 string to an ansi string
  /// </summary>
  /// <param name="str">The utf8 string that must be converted.</param>
  /// <returns>Returns an ansi encoded string.</returns>
  std::string Utf8ToAnsi(const std::string & str);

  /// <summary>
  /// Convert an ansi string to an utf8 string
  /// </summary>
  /// <param name="str">The ansi string that must be converted.</param>
  /// <returns>Returns a utf8 encoded string.</returns>
  std::string AnsiToUtf8(const std::string & str);

//--------------------------------------------------------------------------------------------------
#endif //_WIN32
//--------------------------------------------------------------------------------------------------

} //namespace unicode
} //namespace ra

#endif //RA_UNICODE_H
