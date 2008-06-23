// Copyright (C) 2005-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-09-02
// Last changed: 2008-06-23

#include "TimeDependent.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeDependent::TimeDependent() : t(0)
{
 // Do nothing
}
//-----------------------------------------------------------------------------
TimeDependent::TimeDependent(const real* t) : t(t)
{
 // Do nothing
}
//-----------------------------------------------------------------------------
TimeDependent::~TimeDependent() 
{
 // Do nothing
}
//-----------------------------------------------------------------------------
void TimeDependent::sync(const real* t)
{
  this->t  = t;
}
//-----------------------------------------------------------------------------
