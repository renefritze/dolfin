// Copyright (C) 2006 Johan Jansson and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-02-15
// Last changed: 2006-02-16

#include <dolfin/dolfin_log.h>
#include <dolfin/Function.h>
#include <dolfin/XMLFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLFunction::XMLFunction(Function& f) : XMLObject(), f(f)
{
  state = OUTSIDE;
}
//-----------------------------------------------------------------------------
void XMLFunction::startElement(const xmlChar* name, const xmlChar** attrs)
{
  switch ( state )
  {
  case OUTSIDE:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "function") == 0 )
    {
      readFunction(name,attrs);
      state = INSIDE_FUNCTION;
    }
    
    break;
    
  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLFunction::endElement(const xmlChar* name)
{
  switch ( state )
  {
  case INSIDE_FUNCTION:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "function") == 0 )
    {
      ok = true;
      state = DONE;
    }
    
    break;
    
  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLFunction::readFunction(const xmlChar* name, const xmlChar** attrs)
{
  // Set default values
  std::string element;

  // Parse values
  parseStringRequired(name, attrs, "element", element);
}
//-----------------------------------------------------------------------------
