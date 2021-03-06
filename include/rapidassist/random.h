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

#ifndef RA_RANDOM_H
#define RA_RANDOM_H

#include <string>

#include "rapidassist/config.h"

namespace ra { namespace random {

  /// <summary>
  /// Returns a random number between 0 (inclusive) and RAND_MAX (inclusive).
  /// </summary>
  /// <returns>Returns a random number between 0 and RAND_MAX.</returns>
  int GetRandomInt();

  /// <summary>
  /// Returns a random number between iMin (inclusive) and iMax (inclusive).
  /// </summary>
  /// <param name="iMin">The minimum value that can be generated</param>
  /// <param name="iMax">The maximum value that can be generated</param>
  /// <returns>Returns a random number between iMin and iMax.</returns>
  int GetRandomInt(int iMin, int iMax);

  /// <summary>
  /// Returns a random number between iMin (inclusive) and iMax (inclusive).
  /// </summary>
  /// <param name="iMin">The minimum value that can be generated</param>
  /// <param name="iMax">The maximum value that can be generated</param>
  /// <returns>Returns a random number between iMin and iMax.</returns>
  double GetRandomDouble(double iMin, double iMax);

  /// <summary>
  /// Returns a random number between iMin (inclusive) and iMax (inclusive).
  /// </summary>
  /// <param name="iMin">The minimum value that can be generated</param>
  /// <param name="iMax">The maximum value that can be generated</param>
  /// <returns>Returns a random number between iMin and iMax.</returns>
  float GetRandomFloat(float iMin, float iMax);

  /// <summary>
  /// Returns a random string.
  /// </summary>
  /// <returns>Returns a random string.</returns>
  std::string GetRandomString();

  /// <summary>
  /// Returns a random string with a maximum length of iMaxLen.
  /// </summary>
  /// <param name="iMaxLen">The maximum length of the generated string.</param>
  /// <param name="oValue">The result of the generated string.</param>
  void GetRandomString(std::string & oValue, size_t iMaxLen);
  std::string GetRandomString(size_t iMaxLen);

  /// <summary>
  /// Returns a random string.
  /// </summary>
  /// <param name="iMaxLen">The maximum length of the generated string.</param>
  /// <param name="iSymbols">The list of allowed character of the generated string.</param>
  /// <param name="oValue">The result of the generated string.</param>
  void GetRandomString(std::string & oValue, size_t iMaxLen, const char* iSymbols);
  std::string GetRandomString(size_t iMaxLen, const char* iSymbols);

  class SymbolsFlags {
  public:
    enum Flags {
      LETTERS_LOWERCASE = 1,  //lowercase letters
      LETTERS_UPPERCASE = 2,  //uppercase letters
      NUMBERS = 4,            //numbers from 0 to 9
      SPECIAL_CHARACTERS = 8, //special characters like @ and !
      ALL = 15,               //all flags
    };
  };

  /// <summary>
  /// Returns a random string.
  /// </summary>
  /// <param name="iMaxLen">The maximum length of the generated string.</param>
  /// <param name="iFlags">The flags that defines the allowed content of the generated string.</param>
  /// <param name="oValue">The result of the generated string.</param>
  void GetRandomString(std::string & oValue, size_t iMaxLen, SymbolsFlags::Flags iFlags);

} //namespace random
} //namespace ra

#endif //RA_RANDOM_H
