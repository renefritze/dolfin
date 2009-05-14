// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2004-03-31
// Last changed: 2006-05-23

#ifndef __XML_PARAMETER_LIST_H
#define __XML_PARAMETER_LIST_H

#include "XMLObject.h"

namespace dolfin
{

  class ParameterList;

  class XMLParameterList : public XMLObject
  {
  public:

    XMLParameterList(ParameterList& parameters);

    void start_element(const xmlChar *name, const xmlChar **attrs);
    void end_element  (const xmlChar *name);

    void open(std::string filename);
    bool close();

  private:

    enum ParserState { OUTSIDE, INSIDE_PARAMETERS, DONE };

    void read_parameter(const xmlChar *name, const xmlChar **attrs);

    ParameterList& parameters;
    ParserState state;

  };

}

#endif
