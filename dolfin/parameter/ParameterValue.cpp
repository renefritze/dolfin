// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-12-18
// Last changed: 2005-12-21

#include <dolfin/log/dolfin_log.h>
#include "ParameterValue.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
ParameterValue::ParameterValue()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
ParameterValue::~ParameterValue()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
const ParameterValue& ParameterValue::operator= (int value)
{
  error("Cannot assign int value to parameter of type %s.",
  		type().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const ParameterValue& ParameterValue::operator= (double value)
{
  error("Cannot assign double value to parameter of type %s.",
		type().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const ParameterValue& ParameterValue::operator= (bool value)
{
  error("Cannot assign bool value to parameter of type %s.",
		type().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const ParameterValue& ParameterValue::operator= (std::string value)
{
  error("Cannot assign string value to parameter of type %s.",
		type().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
const ParameterValue& ParameterValue::operator= (uint value)
{
  error("Cannot assign uint value to parameter of type %s.",
		type().c_str());
  return *this;
}
//-----------------------------------------------------------------------------
ParameterValue::operator int() const
{
  cout << "Halla eller, fel typ" << endl;
  //error("Unable to convert parameter of type %s to int.",
  //		type().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
ParameterValue::operator double() const
{
  error("Unable to convert parameter of type %s to real.",
		type().c_str());
  return 0.0;
}
//-----------------------------------------------------------------------------
ParameterValue::operator bool() const
{
  error("Unable to convert parameter of type %s to bool.",
		type().c_str());
  return false;
}
//-----------------------------------------------------------------------------
ParameterValue::operator std::string() const
{
  error("Unable to convert parameter of type %s to string.",
		type().c_str());
  return "";
}
//-----------------------------------------------------------------------------
ParameterValue::operator dolfin::uint() const
{
  error("Unable to convert parameter of type %s to uint.",
		type().c_str());
  return 0;
}
//-----------------------------------------------------------------------------
