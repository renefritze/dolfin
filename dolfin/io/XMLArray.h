// Copyright (C) 2009 Ola Skavhaug
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-03-02
// Last changed: 2009-03-04

#ifndef __XMLARRAY_H
#define __XMLARRAY_H

#include <vector>
#include "XMLHandler.h"

namespace dolfin
{

  class XMLArray : public XMLHandler
  {
  public:

    XMLArray(std::vector<int>& ix, NewXMLFile& parser);
    XMLArray(std::vector<uint>& ux, NewXMLFile& parser);
    XMLArray(std::vector<double>& dx, NewXMLFile& parser);
    XMLArray(std::vector<int>& ix, NewXMLFile& parser, uint size);
    XMLArray(std::vector<uint>& ux, NewXMLFile& parser, uint size);
    XMLArray(std::vector<double>& dx, NewXMLFile& parser, uint size);
     
    void start_element (const xmlChar *name, const xmlChar **attrs);
    void end_element   (const xmlChar *name);
    
    /// Write to file
    void write(const std::vector<int>& x, std::string filename);
    void write(const std::vector<uint>& x, std::string filename);
    void write(const std::vector<double>& x, std::string filename);

  private:
    
    enum parser_state { OUTSIDE_ARRAY, INSIDE_ARRAY, ARRAY_DONE };
    enum array_type { INT, UINT, DOUBLE, UNSET };
    
    void start_array(const xmlChar *name, const xmlChar **attrs);
    void read_entry  (const xmlChar *name, const xmlChar **attrs);
    
    std::vector<int>*  ix;
    std::vector<uint>* ux;
    std::vector<double>* dx;
    parser_state state;
    array_type atype;

    uint size;

  };
  
}

#endif
