// Copyright (C) 2006-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Martin Sandve Alnes, 2008.
// Modified by Garth N. Wells, 2009.
//
// First added:  2006-02-09
// Last changed: 2009-10-05

#include <dolfin/log/log.h>
#include "Constant.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Constant::Constant(double value)
{
  _values.resize(1);
  _values[0] = value;
}
//-----------------------------------------------------------------------------
Constant::Constant(double value0, double value1)
  : Expression(2)
{
  _values.resize(2);
  _values[0] = value0;
  _values[1] = value1;
}
//-----------------------------------------------------------------------------
Constant::Constant(double value0, double value1, double value2)
  : Expression(3)
{
  _values.resize(3);
  _values[0] = value0;
  _values[1] = value1;
  _values[2] = value2;
}
//-----------------------------------------------------------------------------
Constant::Constant(std::vector<double> values)
  : Expression(values.size()), _values(values)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Constant::Constant(std::vector<uint> value_shape,
                   std::vector<double> values)
  : Expression(value_shape), _values(values)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Constant::Constant(const Constant& constant)
  : Expression(constant)
{
  *this = constant;
}
//-----------------------------------------------------------------------------
Constant::~Constant()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
const Constant& Constant::operator= (const Constant& constant)
{
  // Check value shape
  if (constant.value_shape != value_shape)
    error("Unable to assign value to constant, value shape mismatch.");

  // Assign values
  _values = constant._values;

  return *this;
}
//-----------------------------------------------------------------------------
const Constant& Constant::operator= (double constant)
{
  // Check value shape
  if (value_shape.size() != 0)
    error("Unable to assign value to constant, not a scalar.");

  // Assign value
  assert(_values.size() == 1);
  _values[0] = constant;

  return *this;
}
//-----------------------------------------------------------------------------
Constant::operator double() const
{
  // Check value shape
  if (value_shape.size() != 0)
    error("Unable to convert constant to double, not a scalar.");

  // Return value
  assert(_values.size() == 1);
  return _values[0];
}
//-----------------------------------------------------------------------------
void Constant::eval(Array<double>& values, const Array<double>& x,
                    const ufc::cell& cell) const
{
  // Copy values
  for (uint j = 0; j < _values.size(); j++)
    values[j] = _values[j];
}
//-----------------------------------------------------------------------------
