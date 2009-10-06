// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-08-09
// Last changed: 2009-10-06

#include <boost/functional/hash.hpp>
#include <cstdlib>
#include <sstream>
#include "types.h"
#include "utils.h"

//-----------------------------------------------------------------------------
std::string dolfin::indent(std::string block)
{
  std::string indentation("  ");
  std::stringstream s;

  s << indentation;
  for (uint i = 0; i < block.size(); ++i)
  {
    s << block[i];
    if (block[i] == '\n' && i < block.size() - 1)
      s << indentation;
  }

  return s.str();
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::hash(std::string signature)
{
  boost::hash<std::string> string_hash;
  std::size_t h = string_hash(signature);

  return h;
}
//-----------------------------------------------------------------------------
