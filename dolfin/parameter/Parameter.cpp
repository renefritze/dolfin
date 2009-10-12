// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-05-08
// Last changed: 2009-10-12

#include <sstream>
#include <dolfin/log/log.h>
#include "Parameter.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
// class Parameter
//-----------------------------------------------------------------------------
Parameter::Parameter(std::string key)
  : _access_count(0), _change_count(0),
    _key(key), _description("missing description")
{
  // Check that key name is allowed
  check_key(key);
}
//-----------------------------------------------------------------------------
Parameter::~Parameter()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::string Parameter::key() const
{
  return _key;
}
//-----------------------------------------------------------------------------
std::string Parameter::description() const
{
  return _description;
}
//-----------------------------------------------------------------------------
dolfin::uint Parameter::access_count() const
{
  return _access_count;
}
//-----------------------------------------------------------------------------
dolfin::uint Parameter::change_count() const
{
  return _change_count;
}
//-----------------------------------------------------------------------------
void Parameter::set_range(int min_value, int max_value)
{
  error("Cannot set int-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
void Parameter::set_range(real min_value, real max_value)
{
  error("Cannot set real-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
void Parameter::set_range(std::set<std::string> range)
{
  error("Cannot set string-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
void Parameter::get_range(int& min_value, int& max_value) const 
{
  error("Cannot get int-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
void Parameter::get_range(real& min_value, real& max_value) const
{
  error("Cannot get double-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
void Parameter::get_range(std::set<std::string>& range) const
{
  error("Cannot get string-valued range for parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
}
//-----------------------------------------------------------------------------
const Parameter& Parameter::operator= (int value)
{
  error("Cannot assign int-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const Parameter& Parameter::operator= (double value)
{
  error("Cannot assign double-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
#ifdef HAS_GMP
const Parameter& Parameter::operator= (real value)
{
  error("Cannot assign real-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
#endif
//-----------------------------------------------------------------------------
const Parameter& Parameter::operator= (std::string value)
{
  error("Cannot assign string-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const Parameter& Parameter::operator= (const char* value)
{
  error("Cannot assign string-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const Parameter& Parameter::operator= (bool value)
{
  error("Cannot assign bool-value to parameter \"%s\" of type %s.",
        _key.c_str(), type_str().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
Parameter::operator int() const
{
  error("Unable to convert parameter \"%s\" of type %s to int.",
        _key.c_str(), type_str().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
Parameter::operator uint() const
{
  error("Unable to convert parameter \"%s\" of type %s to uint.",
        _key.c_str(), type_str().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
Parameter::operator double() const
{
  error("Unable to convert parameter \"%s\"r of type %s to double.",
        _key.c_str(), type_str().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
Parameter::operator std::string() const
{
  error("Unable to convert parameter \"%s\" of type %s to string.",
        _key.c_str(), type_str().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
Parameter::operator bool() const
{
  error("Unable to convert parameter \"%s\" of type %s to bool.",
        _key.c_str(), type_str().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
real Parameter::get_real() const
{
  error("Unable to convert parameter \"%s\" of type %s to real.",
        _key.c_str(), type_str().c_str());
  return real(0);
}
//-----------------------------------------------------------------------------
void Parameter::check_key(std::string key)
{
  // Space and punctuation not allowed in key names
  for (uint i = 0; i < key.size(); i++)
  {
    if (key[i] == ' ' || key[i] == '.')
      error("Illegal character '%c' in parameter key \"%s\".",
            key[i], key.c_str());
  }
}
//-----------------------------------------------------------------------------
// class IntParameter
//-----------------------------------------------------------------------------
IntParameter::IntParameter(std::string key, int value)
  : Parameter(key), _value(value), _min(0), _max(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
IntParameter::~IntParameter()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void IntParameter::set_range(int min_value, int max_value)
{
  // Check range
  if (min_value > max_value)
    error("Illegal range for int-valued parameter: [%d, %d].",
          min_value, max_value);

  // Set range
  _min  = min_value;
  _max = max_value;
}
//-----------------------------------------------------------------------------
void IntParameter::get_range(int& min_value, int& max_value) const
{
  // Get range
  min_value = _min;
  max_value = _max;
}
//-----------------------------------------------------------------------------
const IntParameter& IntParameter::operator= (int value)
{
  // Check value
  if (_min != _max && (value < _min || value > _max))
    error("Parameter value %d for parameter \"%s\" out of range [%d, %d].",
          value, key().c_str(), _min, _max);

  // Set value
  _value = value;
  _change_count++;

  return *this;
}
//-----------------------------------------------------------------------------
IntParameter::operator int() const
{
  _access_count++;
  return _value;
}
//-----------------------------------------------------------------------------
IntParameter::operator dolfin::uint() const
{
  if (_value < 0)
    error("Unable to convert value %d for parameter \"%s\" to uint (unsigned integer), value is negative.",
          _value, key().c_str());

  _access_count++;
  return _value;
}
//-----------------------------------------------------------------------------
std::string IntParameter::type_str() const
{
  return "int";
}
//-----------------------------------------------------------------------------
std::string IntParameter::value_str() const
{
  std::stringstream s;
  s << _value;
  return s.str();
}
//-----------------------------------------------------------------------------
std::string IntParameter::range_str() const
{
  std::stringstream s;
  if (_min == _max)
    s << "[]";
  else
    s << "[" << _min << ", " << _max << "]";
  return s.str();
}
//-----------------------------------------------------------------------------
std::string IntParameter::str() const
{
  std::stringstream s;
  s << "<int-valued parameter named \""
    << key()
    << "\" with value "
    << _value
    << ">";
  return s.str();
}
//-----------------------------------------------------------------------------
// class RealParameter
//-----------------------------------------------------------------------------
RealParameter::RealParameter(std::string key, real value)
  : Parameter(key), _value(value), _min(0.0), _max(0.0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
RealParameter::~RealParameter()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void RealParameter::set_range(real min_value, real max_value)
{
  // Check range
  if (min_value > max_value)
    error("Illegal range for double-valued parameter: [%g, %g].",
          to_double(min_value), to_double(max_value));

  // Set range
  _min = min_value;
  _max = max_value;
}
//-----------------------------------------------------------------------------
void RealParameter::get_range(real& min_value, real& max_value) const
{
  // Get range
  min_value = _min;
  max_value = _max;
}
//-----------------------------------------------------------------------------
#ifdef HAS_GMP
const RealParameter& RealParameter::operator= (real value) 
{
  // Check value
  if (_min != _max && (value < _min || value > _max))
    error("Parameter value %g for parameter \"%s\" out of range [%g, %g].",
          to_double(value), key().c_str(), to_double(_min), to_double(_max));

  // Set value
  _value = value;
  _change_count++;

  return *this;
}
#endif
//-----------------------------------------------------------------------------
const RealParameter& RealParameter::operator= (double value) 
{
  // Check value
  if (_min != _max && (value < _min || value > _max))
    error("Parameter value %g for parameter \"%s\" out of range [%g, %g].",
          to_double(value), key().c_str(), to_double(_min), to_double(_max));

  // Set value
  _value = value;
  _change_count++;

  return *this;
}
//-----------------------------------------------------------------------------
RealParameter::operator double() const
{
  _access_count++;
  return to_double(_value);
}
//-----------------------------------------------------------------------------
real RealParameter::get_real() const
{
  _access_count++;
  return _value;
}
//-----------------------------------------------------------------------------
std::string RealParameter::type_str() const
{
  return "real";
}
//-----------------------------------------------------------------------------
std::string RealParameter::value_str() const
{
  std::stringstream s;
  s << _value;
  return s.str();
}
//-----------------------------------------------------------------------------
std::string RealParameter::range_str() const
{
  std::stringstream s;
  if (_min == _max)
    s << "[]";
  else
    s << "[" << _min << ", " << _max << "]";
  return s.str();
}
//-----------------------------------------------------------------------------
std::string RealParameter::str() const
{
  std::stringstream s;
  s << "<double-valued parameter named \""
    << key()
    << "\" with value "
    << _value
    << ">";
  return s.str();
}
//-----------------------------------------------------------------------------
// class StringParameter
//-----------------------------------------------------------------------------
StringParameter::StringParameter(std::string key, std::string value)
  : Parameter(key), _value(value)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
StringParameter::~StringParameter()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void StringParameter::set_range(std::set<std::string> range)
{
  _range = range;
}
//-----------------------------------------------------------------------------
void StringParameter::get_range(std::set<std::string>& range) const
{
  // Get range
  range = _range;
}
//-----------------------------------------------------------------------------
const StringParameter& StringParameter::operator= (std::string value)
{
  // Check value
  if (_range.size() > 0 && _range.find(value) == _range.end())
  {
    std::stringstream s;
    s << "Illegal value for parameter. Allowed values are: " << range_str();
    error(s.str());
  }

  // Set value
  _value = value;
  _change_count++;

  return *this;
}
//-----------------------------------------------------------------------------
const StringParameter& StringParameter::operator= (const char* value)
{
  std::string s(value);

  // Check value
  if (_range.size() > 0 && _range.find(s) == _range.end())
  {
    std::stringstream s;
    s << "Illegal value for parameter. Allowed values are: " << range_str();
    error(s.str());
  }

  // Set value
  _value = s;
  _change_count++;

  return *this;
}
//-----------------------------------------------------------------------------
StringParameter::operator std::string() const
{
  _access_count++;
  return _value;
}
//-----------------------------------------------------------------------------
std::string StringParameter::type_str() const
{
  return "string";
}
//-----------------------------------------------------------------------------
std::string StringParameter::value_str() const
{
  return _value;
}
//-----------------------------------------------------------------------------
std::string StringParameter::range_str() const
{
  std::stringstream s;
  s << "[";
  uint i = 0;
  for (std::set<std::string>::const_iterator it = _range.begin();
       it != _range.end(); ++it)
  {
    s << *it;
    if (i++ < _range.size() - 1)
      s << ", ";
  }
  s << "]";

  return s.str();
}
//-----------------------------------------------------------------------------
std::string StringParameter::str() const
{
  std::stringstream s;
  s << "<string-valued parameter named \""
    << key()
    << "\" with value "
    << _value
    << ">";
  return s.str();
}
//-----------------------------------------------------------------------------
// class BoolParameter
//-----------------------------------------------------------------------------
BoolParameter::BoolParameter(std::string key, bool value)
  : Parameter(key), _value(value)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
BoolParameter::~BoolParameter()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
const BoolParameter& BoolParameter::operator= (bool value)
{
  // Set value
  _value = value;
  _change_count++;

  return *this;
}
//-----------------------------------------------------------------------------
BoolParameter::operator bool() const
{
  _access_count++;
  return _value;
}
//-----------------------------------------------------------------------------
std::string BoolParameter::type_str() const
{
  return "bool";
}
//-----------------------------------------------------------------------------
std::string BoolParameter::value_str() const
{
  if (_value)
    return "true";
  else
    return "false";
}
//-----------------------------------------------------------------------------
std::string BoolParameter::range_str() const
{
  return "{true, false}";
}
//-----------------------------------------------------------------------------
std::string BoolParameter::str() const
{
  std::stringstream s;
  s << "<bool-valued parameter named \""
    << key()
    << "\" with value "
    << _value
    << ">";
  return s.str();
}
//-----------------------------------------------------------------------------
