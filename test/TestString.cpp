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

#include "TestString.h"
#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/random.h"
#include "rapidassist/generics.h"
#include <stdint.h>
#include <float.h>

namespace ra { namespace strings { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestString::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestString::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testIsNumeric) {
    //lazy test
    for (int i = -100; i <= 100; i++) {
      for (int j = 0; j <= 100; j++) {
        //build a string from i and j
        static const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        //integers
        sprintf(buffer, "%d", i);
        ASSERT_TRUE(strings::isNumeric(buffer)) << "isNumeric(\"" << buffer << "\") returned false.";

        //floating point
        sprintf(buffer, "%d.%03d", i, j);
        ASSERT_TRUE(strings::isNumeric(buffer)) << "isNumeric(\"" << buffer << "\") returned false.";
      }
    }

    //test special case
    ASSERT_TRUE(strings::isNumeric("+12"));

    //plus or minus sign error
    ASSERT_FALSE(strings::isNumeric("12+123"));
    ASSERT_FALSE(strings::isNumeric("12-123"));
    ASSERT_FALSE(strings::isNumeric("+12+123"));
    ASSERT_FALSE(strings::isNumeric("+12-123"));

    //multiple dots
    ASSERT_FALSE(strings::isNumeric("12.345.67"));
    ASSERT_FALSE(strings::isNumeric("+12.345.67"));

    //alpha characters
    ASSERT_FALSE(strings::isNumeric("+12.34a"));
    ASSERT_FALSE(strings::isNumeric("+12.34!"));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testReplace) {
    //at the beginning
    {
      const std::string EXPECTED = "DeaDbeef";
      std::string str = "deadbeef";
      int numReplacements = replace(str, "d", "D");
      ASSERT_EQ(2, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }

    //at the end
    {
      const std::string EXPECTED = "beefDeaD";
      std::string str = "beefdead";
      int numReplacements = replace(str, "d", "D");
      ASSERT_EQ(2, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }

    //replace everything
    {
      const std::string EXPECTED = "aabbccddeeff";
      std::string str = "deadbeef";
      int numReplacements = replace(str, "deadbeef", "aabbccddeeff");
      ASSERT_EQ(1, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }

    //replace nothing
    {
      const std::string EXPECTED = "deadbeef";
      std::string str = "deadbeef";
      int numReplacements = replace(str, "notfound", "error");
      ASSERT_EQ(0, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }

    //replace short string by longer string
    {
      const std::string EXPECTED = "super deadbeef";
      std::string str = "deadbeef";
      int numReplacements = replace(str, "dead", "super dead");
      ASSERT_EQ(1, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }

    //replace long string by short string
    {
      const std::string EXPECTED = "dead!";
      std::string str = "deadbeef";
      int numReplacements = replace(str, "beef", "!");
      ASSERT_EQ(1, numReplacements);
      ASSERT_EQ(EXPECTED, str);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringParseValue) {
    //uint64_t
    struct UIint64Test {
      const char * EXPECTED_STR;
      uint64_t EXPECTED_VALUE;
    };
    static const UIint64Test tests[] = {
      {"0", 0ull},
      {"54321", 54321ull},
      {"1234567890123456789", 1234567890123456789ull},
      {"576460752303423488", 0x800000000000000ull},
      {"1152921504606846975", 0xFFFFFFFFFFFFFFFull},
    };
    size_t numTests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < numTests; i++) {
      const char * EXPECTED_STR = tests[i].EXPECTED_STR;
      const uint64_t & EXPECTED_VALUE = tests[i].EXPECTED_VALUE;

      //assert toString()
      std::string actualStr = toString(EXPECTED_VALUE);
      ASSERT_EQ(EXPECTED_STR, actualStr);

      //assert parseValue()
      uint64_t actualValue = 0;
      bool parsed = parse(EXPECTED_STR, actualValue);
      ASSERT_TRUE(parsed);
      ASSERT_EQ(EXPECTED_VALUE, actualValue);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testCapitalizeFirstCharacter) {
    {
      //normal
      const std::string EXPECTED = "Deadbeef";
      const std::string value = "deadbeef";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //empty
      const std::string EXPECTED = "";
      const std::string value = "";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //single letter
      const std::string EXPECTED = "A";
      const std::string value = "a";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //special character
      const std::string EXPECTED = "#!/bin/bash";
      const std::string value = "#!/bin/bash";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //spaces
      const std::string EXPECTED = " foo";
      const std::string value = " foo";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //numbers
      const std::string EXPECTED = "5foo";
      const std::string value = "5foo";
      std::string actual = capitalizeFirstCharacter(value);
      ASSERT_EQ(EXPECTED, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testUppercase) {
    {
      //normal
      const std::string EXPECTED = "DEADBEEF";
      const std::string value = "deadbeef";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //empty
      const std::string EXPECTED = "";
      const std::string value = "";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //single letter
      const std::string EXPECTED = "A";
      const std::string value = "a";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //special character
      const std::string EXPECTED = "#!/BIN/BASH";
      const std::string value = "#!/bin/bash";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //spaces
      const std::string EXPECTED = " FOO";
      const std::string value = " foo";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //numbers
      const std::string EXPECTED = "5FOO";
      const std::string value = "5foo";
      std::string actual = uppercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testLowercase) {
    {
      //normal
      const std::string EXPECTED = "deadbeef";
      const std::string value = "DeAdBeEf";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //empty
      const std::string EXPECTED = "";
      const std::string value = "";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //single letter
      const std::string EXPECTED = "a";
      const std::string value = "A";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //special character
      const std::string EXPECTED = "#!/bin/bash";
      const std::string value = "#!/bIn/BaSh";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //spaces
      const std::string EXPECTED = " foo";
      const std::string value = " fOO";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //numbers
      const std::string EXPECTED = "5foo";
      const std::string value = "5fOO";
      std::string actual = lowercase(value);
      ASSERT_EQ(EXPECTED, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testStreamOperators) {
    {
      //const void * value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = (environment::isProcess32Bit() ? "fooBAR0x12345678" : "fooBAR0x0000000012345678");
      const void * value = (const void *)0x12345678;
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const std::string & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBARtest";
      const std::string value = "test";
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const char * value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBARtest";
      const char * value = "test";
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const int8_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR-23";
      const int8_t value = -23;
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const uint8_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR200";
      const uint8_t value = 200;
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const int16_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR-31234";
      const int16_t value = -31234;
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const uint16_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR51234";
      const uint16_t value = 51234;
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const int32_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR-2147483600";
      const int32_t value = -2147483600l; //min is -2147483648
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const uint32_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR4123456789";
      const uint32_t value = 4123456789ul; //max is 4294967295
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const int64_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR-9123456789012345678";
      const int64_t value = -9123456789012345678ll; //min is -9223372036854775808
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
    {
      //const uint64_t & value
      const std::string HEADER = "fooBAR";
      const std::string EXPECTED = "fooBAR18123456789012345678";
      const uint64_t value = 18123456789012345678ull; //max is 18446744073709551615
      std::string actual = HEADER;
      actual << value;
      ASSERT_EQ(EXPECTED, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testRemoveEOL) {
    //test NULL
    {
      removeEOL(NULL);
    }

    //test empty string
    {
      char empty[] = "";
      removeEOL(empty);
    }

    //test windows
    {
      const std::string EXPECTED = "fooBAR";
      char buffer[] = "fooBAR\r\n";
      removeEOL(buffer);
      ASSERT_EQ(EXPECTED, buffer);
    }

    //test unix
    {
      const std::string EXPECTED = "fooBAR";
      char buffer[] = "fooBAR\n";
      removeEOL(buffer);
      ASSERT_EQ(EXPECTED, buffer);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testSplit) {
    //test NULL
    {
      static const std::string INPUT = "Aa.Bb.Cc";
      StringVector list = split(INPUT, (const char *)NULL); //same as not found
      ASSERT_EQ(1, list.size());
      ASSERT_EQ(INPUT, list[0]);
    }

    //test no split found
    {
      static const std::string INPUT = "Aa.Bb.Cc";
      StringVector list = split(INPUT, "!");
      ASSERT_EQ(1, list.size());
      ASSERT_EQ(INPUT, list[0]);
    }

    //test found
    {
      static const std::string INPUT = "Aa.Bb.Cc";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(3, list.size());
      ASSERT_EQ("Aa", list[0]);
      ASSERT_EQ("Bb", list[1]);
      ASSERT_EQ("Cc", list[2]);
    }

    //test last character is separator
    {
      static const std::string INPUT = "Aa.Bb.Cc.";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(4, list.size());
      ASSERT_EQ("Aa", list[0]);
      ASSERT_EQ("Bb", list[1]);
      ASSERT_EQ("Cc", list[2]);
      ASSERT_EQ("", list[3]);
    }

    //test first character is separator
    {
      static const std::string INPUT = ".Aa.Bb.Cc";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(4, list.size());
      ASSERT_EQ("", list[0]);
      ASSERT_EQ("Aa", list[1]);
      ASSERT_EQ("Bb", list[2]);
      ASSERT_EQ("Cc", list[3]);
    }

    //test with only a separator
    {
      static const std::string INPUT = ".";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(2, list.size());
      ASSERT_EQ("", list[0]);
      ASSERT_EQ("", list[1]);
    }

    //test consecutive separators
    {
      static const std::string INPUT = "Aa..Bb";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(3, list.size());
      ASSERT_EQ("Aa", list[0]);
      ASSERT_EQ("", list[1]);
      ASSERT_EQ("Bb", list[2]);
    }

    //test only separators
    {
      static const std::string INPUT = "...";
      StringVector list = split(INPUT, ".");
      ASSERT_EQ(4, list.size());
      ASSERT_EQ("", list[0]);
      ASSERT_EQ("", list[1]);
      ASSERT_EQ("", list[2]);
      ASSERT_EQ("", list[3]);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testJoin) {
    //test NULL
    {
      static const std::string EXPECTED = "Aa.Bb.Cc";
      StringVector list = split(EXPECTED, ".");
      std::string joinStr = join(list, (const char *)NULL); //same as empty separator
      ASSERT_EQ(std::string("AaBbCc"), joinStr);
    }

    //test empty separator
    {
      static const std::string EXPECTED = "Aa.Bb.Cc";
      StringVector list = split(EXPECTED, ".");
      std::string joinStr = join(list, "");
      ASSERT_EQ(std::string("AaBbCc"), joinStr);
    }

    //test normal
    {
      static const std::string EXPECTED = "Aa.Bb.Cc";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }

    //test last character is separator
    {
      static const std::string EXPECTED = "Aa.Bb.Cc.";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }

    //test first character is separator
    {
      static const std::string EXPECTED = ".Aa.Bb.Cc";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }

    //test first and last character are separator
    {
      static const std::string EXPECTED = ".Aa.Bb.Cc.";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }

    //test consecutive separators
    {
      static const std::string EXPECTED = "Aa..Bb";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }

    //test only separators
    {
      static const std::string EXPECTED = "...";
      static const char * SEPARATOR = ".";
      StringVector list = split(EXPECTED, SEPARATOR);
      std::string joinStr = join(list, SEPARATOR);
      ASSERT_EQ(EXPECTED, joinStr);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testTrim) {
    {
      static const std::string value = "   abc   ";
      ASSERT_EQ("abc", trim(value));
      ASSERT_EQ("abc   ", trimLeft(value));
      ASSERT_EQ("   abc", trimRight(value));
    }

    {
      static const std::string value = "***abc***";
      ASSERT_EQ("abc", trim(value, '*'));
      ASSERT_EQ("abc***", trimLeft(value, '*'));
      ASSERT_EQ("***abc", trimRight(value, '*'));
    }

    ASSERT_EQ("", trim(""));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testReverse) {
    //assert even number of characters
    ASSERT_EQ("abc", reverse("cba"));

    //assert odd number of characters
    ASSERT_EQ("abcd", reverse("dcba"));

    ASSERT_EQ("", reverse(""));
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testFormat) {
    std::string text = ra::strings::format("%d %s %c %3.2f", 23, "this is a string", 'e', 4.234);
    ASSERT_EQ("23 this is a string e 4.23", text);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testStreamOperatorMatchesToString) {
    //assert that 'operator<<' is identical to toString()

    {
      const float value = 1.0f;
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
    {
      const double value = 1.0;
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
    {
      const float value = 1.0f / 7.0f;  // 0.142857 15
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
    {
      const double value = 1.0 / 7.0;  // 0.142857 142857 14285
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
    {
      const float value = 1234.0f / 9999.0f;  // 0.1234 1234
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
    {
      const double value = 1234.0 / 9999.0;  // 0.1234 1234 1234 1234 1
      std::string s1, s2;
      s1 << value;
      s2 = toString(value);
      ASSERT_EQ(s1, s2);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringUInt8_t) {
    typedef uint8_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)0; //UINT8_MIN
      ASSERT_EQ("0", toString(value));
    }
    {
      test_type value = UINT8_MAX;
      ASSERT_EQ("255", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringSInt8_t) {
    typedef int8_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)INT8_MIN;
      ASSERT_EQ("-128", toString(value));
    }
    {
      test_type value = INT8_MAX;
      ASSERT_EQ("127", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringUInt16_t) {
    typedef uint16_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)0; //UINT16_MIN
      ASSERT_EQ("0", toString(value));
    }
    {
      test_type value = UINT16_MAX;
      ASSERT_EQ("65535", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringSInt16_t) {
    typedef int16_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)INT16_MIN;
      ASSERT_EQ("-32768", toString(value));
    }
    {
      test_type value = INT16_MAX;
      ASSERT_EQ("32767", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringUInt32_t) {
    typedef uint32_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)0; //UINT32_MIN
      ASSERT_EQ("0", toString(value));
    }
    {
      test_type value = UINT32_MAX;
      ASSERT_EQ("4294967295", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringSInt32_t) {
    typedef int32_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)INT32_MIN;
      ASSERT_EQ("-2147483648", toString(value));
    }
    {
      test_type value = INT32_MAX;
      ASSERT_EQ("2147483647", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringUInt64_t) {
    typedef uint64_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)0; //UINT64_MIN
      ASSERT_EQ("0", toString(value));
    }
    {
      test_type value = UINT64_MAX;
      ASSERT_EQ("18446744073709551615", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringSInt64_t) {
    typedef int64_t test_type;
    {
      test_type value = (test_type)32;
      ASSERT_EQ("32", toString(value));
    }
    {
      test_type value = (test_type)INT64_MIN;
      ASSERT_EQ("-9223372036854775808", toString(value));
    }
    {
      test_type value = INT64_MAX;
      ASSERT_EQ("9223372036854775807", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringBoolean) {
    typedef bool test_type;
    {
      test_type value = true;
      ASSERT_EQ("true", toString(value));
    }
    {
      test_type value = false;
      ASSERT_EQ("false", toString(value));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringLosslessFloat) {
    //try with a few random guess
    for (size_t i = 0; i < 20000; i++) {
      //compute the fraction
      float value = ra::random::getRandomFloat(-1000000.0f, +1000000.0f);
      SCOPED_TRACE(value);

      //used for debugging
      int32_t value_raw = ra::generics::readAs<int32_t>(value);
      SCOPED_TRACE(value_raw);

      //convert to string
      std::string str = ra::strings::toStringLossless(value);
      SCOPED_TRACE(str);

      //convert back to float
      float parsed_value = 0.0f;
      bool success = ra::strings::parse(str, parsed_value);
      ASSERT_TRUE(success);

      //we truly wants a bitwise compare
      ASSERT_EQ(value, parsed_value);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringLosslessDouble) {
    //try again with a few random guess
    for (size_t i = 0; i < 20000; i++) {
      //compute the fraction
      double value = ra::random::getRandomDouble(-100000000000.0f, +100000000000.0f);
      SCOPED_TRACE(value);

      //used for debugging.
      int64_t value_raw = ra::generics::readAs<int64_t>(value);
      SCOPED_TRACE(value_raw);

      //convert to string
      std::string str = ra::strings::toStringLossless(value);
      SCOPED_TRACE(str);

      //convert back to double
      double parsed_value = 0.0;
      bool success = ra::strings::parse(str, parsed_value);
      ASSERT_TRUE(success);

      //we truly wants a bitwise compare
      ASSERT_EQ(value, parsed_value);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringFormattedFloat) {
    //test default behavior
    {
      const float value = 0.876543210f;
      ASSERT_EQ("1", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("0.9", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("0.88", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("0.877", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("0.8765", ra::strings::toStringFormatted(value, 4));
      ASSERT_EQ("0.87654", ra::strings::toStringFormatted(value, 5));
      ASSERT_EQ("0.876543", ra::strings::toStringFormatted(value, 6));
      ASSERT_EQ("0.8765432", ra::strings::toStringFormatted(value, 7));
      ASSERT_EQ("0.87654322", ra::strings::toStringFormatted(value, 8)); //that's an exception
    }

    //test non significant zeros
    {
      const float value = 0.1f;
      ASSERT_EQ("0", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("0.1", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("0.10", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("0.100", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("0.1000", ra::strings::toStringFormatted(value, 4));
    }

    //test big value
    {
      const float value = 1000.0f;
      ASSERT_EQ("1000", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("1000.0", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("1000.00", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("1000.000", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("1000.0000", ra::strings::toStringFormatted(value, 4));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringFormattedDouble) {
    //test default behavior
    {
      const double value = 0.876543210;
      ASSERT_EQ("1", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("0.9", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("0.88", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("0.877", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("0.8765", ra::strings::toStringFormatted(value, 4));
      ASSERT_EQ("0.87654", ra::strings::toStringFormatted(value, 5));
      ASSERT_EQ("0.876543", ra::strings::toStringFormatted(value, 6));
      ASSERT_EQ("0.8765432", ra::strings::toStringFormatted(value, 7));
      ASSERT_EQ("0.87654321", ra::strings::toStringFormatted(value, 8));
    }

    //test non significant zeros
    {
      const double value = 0.1;
      ASSERT_EQ("0", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("0.1", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("0.10", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("0.100", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("0.1000", ra::strings::toStringFormatted(value, 4));
    }

    //test big value
    {
      const double value = 1000.0;
      ASSERT_EQ("1000", ra::strings::toStringFormatted(value, 0));
      ASSERT_EQ("1000.0", ra::strings::toStringFormatted(value, 1));
      ASSERT_EQ("1000.00", ra::strings::toStringFormatted(value, 2));
      ASSERT_EQ("1000.000", ra::strings::toStringFormatted(value, 3));
      ASSERT_EQ("1000.0000", ra::strings::toStringFormatted(value, 4));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringLossy) {
    //float
    {
      ASSERT_EQ("1.2", ra::strings::toStringLossy(1.2f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.22", ra::strings::toStringLossy(1.22f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.222", ra::strings::toStringLossy(1.222f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.2222", ra::strings::toStringLossy(1.2222f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.22223", ra::strings::toStringLossy(1.22223f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.222233", ra::strings::toStringLossy(1.222233f, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON));
    }

    //double
    {
      ASSERT_EQ("1.2", ra::strings::toStringLossy(1.2, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.22", ra::strings::toStringLossy(1.22, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.222", ra::strings::toStringLossy(1.222, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.2222", ra::strings::toStringLossy(1.2222, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.22223", ra::strings::toStringLossy(1.22223, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
      ASSERT_EQ("1.222233", ra::strings::toStringLossy(1.222233, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testToStringLossyVsLossLess) {
    //there should be a difference between lossless and lossy conversions.

    //float
    {
      const float value = 0.45f; // 0.44999999
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = 1.0f / 7.0f; // 0.142857 15, theorical value of 0.142857...
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = 1234.0f / 9999.0f; // 0.12341234, theorical value of 0.1234...
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = 112704.88f;
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = (((float)1234 / 9999) + 1000.0f); // 1000.1234, theorical value of 1000.1234...
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = (998877654321.0f / 1000000000.0f); // 998.87762
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const float value = 5.3f; // 5.3000002
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::FLOAT_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }


    //double
    {
      const double value = 0.3; // 0.29999999999999999
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const double value = 1.0 / 7.0; // 0.14285714285714285, theorical value of 0.142857...
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }
    {
      const double value = 1234.0 / 9999.0; // 0.12341234123412341, theorical value of 0.1234...
      std::string lossless = toStringLossless(value);
      std::string lossy = toStringLossy(value, ra::strings::DOUBLE_TOSTRING_LOSSY_EPSILON);
      ASSERT_NE(lossless, lossy);
    }

  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestString, testParseBoolean) {
    ASSERT_TRUE(ra::strings::parseBoolean("true"));
    ASSERT_TRUE(ra::strings::parseBoolean("tRuE"));
    ASSERT_TRUE(ra::strings::parseBoolean("yes"));
    ASSERT_TRUE(ra::strings::parseBoolean("yEs"));
    ASSERT_TRUE(ra::strings::parseBoolean("y"));
    ASSERT_TRUE(ra::strings::parseBoolean("Y"));
    ASSERT_TRUE(ra::strings::parseBoolean("on"));
    ASSERT_TRUE(ra::strings::parseBoolean("oN"));

    ASSERT_FALSE(ra::strings::parseBoolean("false"));
    ASSERT_FALSE(ra::strings::parseBoolean("fALsE"));
    ASSERT_FALSE(ra::strings::parseBoolean("no"));
    ASSERT_FALSE(ra::strings::parseBoolean("nO"));
    ASSERT_FALSE(ra::strings::parseBoolean("n"));
    ASSERT_FALSE(ra::strings::parseBoolean("N"));
    ASSERT_FALSE(ra::strings::parseBoolean("off"));
    ASSERT_FALSE(ra::strings::parseBoolean("oFF"));

    ASSERT_FALSE(ra::strings::parseBoolean("anythingelse"));

    bool b = false;
    ASSERT_TRUE(ra::strings::parse("true", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("tRuE", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("yes", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("yEs", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("y", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("Y", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("on", b)); ASSERT_TRUE(b);
    ASSERT_TRUE(ra::strings::parse("oN", b)); ASSERT_TRUE(b);

    ASSERT_TRUE(ra::strings::parse("false", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("fALsE", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("no", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("nO", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("n", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("N", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("off", b)); ASSERT_FALSE(b);
    ASSERT_TRUE(ra::strings::parse("oFF", b)); ASSERT_FALSE(b);

    //assert cannot parse
    ASSERT_FALSE(ra::strings::parse("anythingelse", b));
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace strings
} //namespace ra
