#ifndef ENVIRONMENTFUNC_H
#define ENVIRONMENTFUNC_H

#include <string>

namespace ra
{
  namespace environment
  {

    ///<summary>
    ///Returns value of an environment variable.
    ///</summary>
    ///<param name="iName">Name of the variable</param>
    ///<return>Returns the value of the given environment variable.<return>
    std::string getEnvironmentVariable(const char * iName);

  } //namespace environment
} //namespace ra

#endif //ENVIRONMENTFUNC_H