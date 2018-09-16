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

#include "TestProcess.h"
#include "rapidassist/process.h"
#include "rapidassist/filesystem.h"

namespace ra { namespace process { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestProcess::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestProcess::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessPath)
  {
    std::string path = ra::process::getCurrentProcessPath();
    printf("Process path: %s\n", path.c_str());
    ASSERT_TRUE(!path.empty());
    ASSERT_TRUE( ra::filesystem::fileExists(path.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestProcess, testGetCurrentProcessDir)
  {
    std::string dir = ra::process::getCurrentProcessDir();
    printf("Process dir: %s\n", dir.c_str());
    ASSERT_TRUE(!dir.empty());
    ASSERT_TRUE( ra::filesystem::folderExists(dir.c_str()) );
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace process
} //namespace ra