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

#include "TestLogger.h"
#include "rapidassist/logger.h"

namespace ra { namespace logger { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestLogger::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestLogger::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogger, testLoggerLevels)
  {
    logger::setQuietMode(false);
    logger::log(logger::LOG_INFO,     "This is information at line=%d.", __LINE__);
    logger::log(logger::LOG_WARNING,  "This is a warning at line=%d.", __LINE__);
    logger::log(logger::LOG_ERROR,    "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestLogger, testQuietMode)
  {
    logger::setQuietMode(true);
    logger::log(logger::LOG_INFO,     "This is information at line=%d.", __LINE__);
    logger::log(logger::LOG_WARNING,  "This is a warning at line=%d.", __LINE__);
    logger::log(logger::LOG_ERROR,    "This is an error at line=%d.", __LINE__);
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra
