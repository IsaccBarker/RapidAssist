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

#ifndef RA_PROPERTIESFILE_H
#define RA_PROPERTIESFILE_H

#include <map>

#include "rapidassist/config.h"
#include "rapidassist/strings.h"

namespace ra { namespace filesystem {

  class PropertiesFile {
  public:
    PropertiesFile();
    virtual ~PropertiesFile();

    virtual bool Load(const std::string & file_path);
    virtual bool Save(const std::string & file_path);
    virtual bool LoadUtf8(const std::string & file_path);
    virtual bool SaveUtf8(const std::string & file_path);

    virtual bool Clear();
    virtual bool HasKey(const std::string & key) const;
    virtual bool DeleteKey(const std::string & key);
    virtual bool GetValue(const std::string & key, std::string & value) const;
    virtual bool SetValue(const std::string & key, const std::string & value);

  private:
    bool Load(const ra::strings::StringVector & lines);
    bool Save(FILE * f);

  private:
    typedef std::map<std::string /*keyname*/, std::string /*value*/> PropertyMap;
    PropertyMap properties_;
  };

} //namespace filesystem
} //namespace ra

#endif //RA_PROPERTIESFILE_H
