// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2006.
// Modified by Benjamin Kehlet, 2009
//
// First added:  2005-12-19
// Last changed: 2009-02-01

#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include "Parameter.h"
#include <ostream>

namespace dolfin
{

  /// Get value of parameter with given key
  Parameter dolfin_get(std::string key);

  /// Set value of parameter
  void dolfin_set(std::string key, dolfin::Parameter value);

  /// Set special key/value pair
  void dolfin_set(std::string key, std::ostream& ostream);

  /// Add parameter
  void dolfin_add(std::string key, dolfin::Parameter value);

  /// Check if parameter has been changed from default value
  bool dolfin_changed(std::string key);

}

#endif
