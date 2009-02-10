// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Benjamin Kehlet
//
// First added:  2003-05-06
// Last changed: 2009-01-30

#ifndef __PARAMETER_H
#define __PARAMETER_H

#include <dolfin/log/dolfin_log.h>

namespace dolfin
{

  class ParameterValue;

  /// This class represents a parameter of some given type.
  /// Supported value types are real, int, bool, and string.

  class Parameter
  {
  public:

    /// Supported parameter types
    enum Type {type_real, type_int, type_bool, type_string};

    /// Create int-valued parameter
    Parameter(int value);

    /// Create int-valued parameter
    Parameter(uint value);

    /// Create real-valued parameter
    Parameter(double value);

    /// Create bool-valued parameter
    Parameter(bool value);

    /// Create string-valued parameter
    Parameter(std::string value);

    /// Create string-valued parameter
    Parameter(const char* value);

    /// Copy constructor
    Parameter(const Parameter& parameter);

    /// Destructor
    ~Parameter();

    /// Assignment of int
    const Parameter& operator= (int value);

    /// Assignment of uint
    const Parameter& operator= (uint value);

    /// Assignment of real
    const Parameter& operator= (double value);

    /// Assignment of bool
    const Parameter& operator= (bool value);

    /// Assignment of string
    const Parameter& operator= (std::string value);

    /// Assignment of Parameter
    const Parameter& operator= (const Parameter& parameter);

    /// Cast parameter to int
    operator int() const;

    /// Cast parameter to uint
    operator uint() const;

    /// Cast parameter to real
    operator double() const;
    
    /// Cast parameter to bool
    operator bool() const;

    /// Cast parameter to string
    operator std::string() const;

    /// Return type of parameter
    Type type() const;

    /// Output
    friend LogStream& operator<< (LogStream& stream, const Parameter& parameter);

    /// Friends
    friend class XMLFile;
    friend class ParameterList;

  private:

    // Pointer to parameter value
    ParameterValue* value;

    // Type of parameter
    Type _type;

    // True if parameter has been changed
    bool _changed;
    
  };

  LogStream& operator<< (LogStream& stream, const Parameter& parameter);
  
}

#endif
