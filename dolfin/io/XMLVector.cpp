// Copyright (C) 2002-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2002-12-06
// Last changed: 2006-05-23


#include <dolfin/log/dolfin_log.h>
#include <dolfin/la/Vector.h>
#include "XMLVector.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLVector::XMLVector(GenericVector& vector)
  : XMLObject(), x(vector), values(0), size(0)
{
  state = OUTSIDE;
}
//-----------------------------------------------------------------------------
void XMLVector::start_element(const xmlChar *name, const xmlChar **attrs)
{
  switch ( state )
  {
  case OUTSIDE:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "vector") == 0 )
    {
      start_vector(name, attrs);
      state = INSIDE_VECTOR;
    }
    
    break;
    
  case INSIDE_VECTOR:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "entry") == 0 )
      read_entry(name, attrs);
    
    break;
    
  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLVector::end_element(const xmlChar *name)
{
  switch ( state )
  {
  case INSIDE_VECTOR:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "vector") == 0 )
    {
      end_vector();
      state = DONE;
    }
    
    break;
    
  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLVector::start_vector(const xmlChar *name, const xmlChar **attrs)
{
  // Parse size of vector
  size = parseUnsignedInt(name, attrs, "size");
  
  // Initialize vector
  if (values)
    delete [] values;
  values = new double[size];
}
//-----------------------------------------------------------------------------
void XMLVector::end_vector()
{
  // Copy values to vector
  dolfin_assert(values);
  x.resize(size);
  x.set(values);
  delete [] values;
  values = 0;
}
//-----------------------------------------------------------------------------
void XMLVector::read_entry(const xmlChar *name, const xmlChar **attrs)
{
  // Parse values
  uint row   = parseUnsignedInt(name, attrs, "row");
  double value = parse_real(name, attrs, "value");
  
  // Check values
  if (row >= size)
    error("Illegal XML data for Vector: row index %d out of range (0 - %d)",
          row, size - 1);
  
  // Set value
  dolfin_assert(values);
  values[row] = value;
}
//-----------------------------------------------------------------------------
