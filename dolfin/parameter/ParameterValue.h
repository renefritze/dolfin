// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Benjamin Kehlet 2009
//
// First added:  2005-12-18
// Last changed: 2009-03-21

#ifndef __PARAMETER_VALUE_H
#define __PARAMETER_VALUE_H

#include <string>
#include <dolfin/common/types.h>

namespace dolfin
{

  /// Base class for parameter values
  class ParameterValue
  {
  public:

    /// Constructor
    ParameterValue();

    /// Destructor
    virtual ~ParameterValue();

    /// Assignment of int
    virtual const ParameterValue& operator= (int value);

    /// Assignment of uint
    virtual const ParameterValue& operator= (uint value);

    /// Assignment of double
    virtual const ParameterValue& operator= (double value);

#ifdef HAS_GMP
    /// Assignment of real (GMP type)
    virtual const ParameterValue& operator= (real value);
#endif

    /// Assignment of bool
    virtual const ParameterValue& operator= (bool value);

    /// Assignment of string
    virtual const ParameterValue& operator= (std::string value);
    
    /// Cast to int
    virtual operator int() const;

    /// Cast to uint
    virtual operator uint() const;

    /// Cast to real
    virtual operator double() const;

    /// Cast to bool
    virtual operator bool() const;

    /// Cast to string
    virtual operator std::string() const;

    /// Get (possibly extended precision) real value
    virtual real get_real() const;

    /// Name of value type
    virtual std::string type() const = 0;


  };

  /// int-valued parameter value
  class IntValue : public ParameterValue
  {
  public:

    /// Constructor
    IntValue(int value) : ParameterValue(), value(value) {}
    
    /// Destructor
    ~IntValue() {}

    /// Assignment of int
    const ParameterValue& operator= (int value) { this->value = value; return *this; }

    /// Assignment of uint
    const ParameterValue& operator= (uint value) { this->value = static_cast<int>(value); return *this; }

    /// Cast to int
    operator int() const { return value; }

    /// Cast to uint
    operator uint() const
    { 
      if ( value < 0 )
	error("Unable to convert negative int parameter to uint.");
      return static_cast<uint>(value);
    }

    /// Name of value type
    std::string type() const { return "int"; }

  private:

    int value;

  };

  /// real-valued parameter value
  class RealValue : public ParameterValue
  {
  public:

    /// Constructor
    RealValue(double value) : ParameterValue(), value(value) {}
    
#ifdef HAS_GMP
    RealValue(real& value) : ParameterValue(), value(value) {}
#endif

    /// Destructor
    ~RealValue() {}

    /// Assignment of real
    const ParameterValue& operator= (double value) { this->value = value; return *this; }
#ifdef HAS_GMP
    const ParameterValue& operator= (real value) { this->value = value; return *this; }
#endif

    /// Cast to real
    operator double() const { return to_double(value); }
    real get_real() const { return value;}

    /// Name of value type
    std::string type() const { return "real"; }

  private:

    real value;

  };

  /// bool-valued parameter value
  class BoolValue : public ParameterValue
  {
  public:

    /// Constructor
    BoolValue(bool value) : ParameterValue(), value(value) {}

    /// Destructor
    ~BoolValue() {}

    /// Assignment of bool
    const ParameterValue& operator= (bool value) { this->value = value; return *this; }
    
    /// Cast to int
    operator bool() const { return value; }

    /// Name of value type
    std::string type() const { return "bool"; }

  private:

    bool value;

  };

  /// string-valued parameter value
  class StringValue : public ParameterValue
  {
  public:

    /// Constructor
    StringValue(std::string value) : ParameterValue(), value(value) {}
    
    /// Destructor
    ~StringValue() {}

    /// Assignment of string
    const ParameterValue& operator= (std::string value) { this->value = value; return *this; }

    /// Cast to string
    operator std::string() const { return value; }

    /// Name of value type
    std::string type() const { return "string"; }

  private:

    std::string value;

  };

}

#endif
