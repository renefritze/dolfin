// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_math.h>
#include <dolfin/cGqMethods.h>

using namespace dolfin;

/// Global table of cG(q) methods
cGqMethods dolfin::cG;

//-----------------------------------------------------------------------------
cGqMethods::cGqMethods() : methods(0), size(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
cGqMethods::~cGqMethods()
{
  for (unsigned int i = 0; i < size; i++)
  {
    if ( methods[i] )
      delete methods[i];
    methods[i] = 0;
  }
}
//-----------------------------------------------------------------------------
void cGqMethods::init(unsigned int q)
{
  // Check if we need to increase the size of the list
  if ( (q+1) > size )
  {
    cGqMethod** new_methods = new cGqMethod*[q+1];
    for (unsigned int i = 0; i < (q+1); i++)
      new_methods[i] = 0;
    for (unsigned int i = 0; i < size; i++)
      new_methods[i] = methods[i];
    delete [] methods;
    methods = new_methods;
    size = q+1;
  }

  // Check if the method has already been initialized
  if ( methods[q] )
    return;

  // Initialize the method
  methods[q] = new cGqMethod(q);
}
//-----------------------------------------------------------------------------
