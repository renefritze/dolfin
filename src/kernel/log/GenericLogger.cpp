// Copyright (C) 2003-2007 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-03-13
// Last changed: 2007-05-11

#include <dolfin/GenericLogger.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
GenericLogger::GenericLogger()
{
  level = 0;
}
//-----------------------------------------------------------------------------
void GenericLogger::begin()
{
  level++;
}
//-----------------------------------------------------------------------------
void GenericLogger::end()
{
  level--;

  if ( level < 0 )
    level = 0;
}
//-----------------------------------------------------------------------------
