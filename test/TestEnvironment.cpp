#include "TestEnvironment.h"
#include "rapidassist/environment.h"

namespace ra { namespace environment { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestEnvironment::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetEnvironmentVariable)
  {
    //test NULL
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable(NULL);
      ASSERT_EQ(EXPECTED, actual);
    }

    //test empty string
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("");
      ASSERT_EQ(EXPECTED, actual);
    }

    //test not found
    {
      static const std::string EXPECTED = "";
      std::string actual = environment::getEnvironmentVariable("FOO_BAR_BIG_BANG");
      ASSERT_EQ(EXPECTED, actual);
    }

#ifdef WIN32
    //test TEMP
    {
      std::string actual = environment::getEnvironmentVariable("TEMP");
      ASSERT_NE("", actual);
    }
#elif __linux__
    //test SHELL
    {
      std::string actual = environment::getEnvironmentVariable("SHELL");
      ASSERT_NE("", actual);
    }
#endif
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testProcessXXBit)
  {
    if (environment::isProcess32Bit())
    {
      ASSERT_FALSE(environment::isProcess64Bit());
      ASSERT_EQ(4, sizeof(void*));
    }
    else if (environment::isProcess64Bit())
    {
      ASSERT_FALSE(environment::isProcess32Bit());
      ASSERT_EQ(8, sizeof(void*));
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestEnvironment, testGetLineSeparator)
  {
    const char * separator = environment::getLineSeparator();
    ASSERT_TRUE( separator != NULL );
    ASSERT_TRUE( !std::string(separator).empty() );
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace environment
} //namespace ra