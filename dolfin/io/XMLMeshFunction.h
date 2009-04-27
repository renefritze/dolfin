// Copyright (C) 2006 Ola Skavhaug.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-11-29
// Last changed: 2006-11-29

#ifndef __NEW_XML_MESHFUNCTION_H
#define __NEW_XML_MESHFUNCTION_H

#include <dolfin/mesh/MeshFunction.h>
#include "XMLObject.h"

namespace dolfin
{
  
  class XMLMeshFunction : public XMLObject
  {
  public:

    XMLMeshFunction(MeshFunction<int>& meshfunction);
    XMLMeshFunction(MeshFunction<unsigned int>& meshfunction);
    XMLMeshFunction(MeshFunction<double>& meshfunction);
    XMLMeshFunction(MeshFunction<bool>& meshfunction);
    ~XMLMeshFunction();
    
    void start_element (const xmlChar* name, const xmlChar** attrs);
    void end_element   (const xmlChar* name);
    
    void open(std::string filename);
    bool close();
    
  private:
    
    enum ParserState { OUTSIDE, INSIDE_MESHFUNCTION, INSIDE_ENTITY, DONE };
    enum MeshFunctionType { INT, UINT, DOUBLE, BOOL, UNSET };
    
    void readMeshFunction(const xmlChar* name, const xmlChar** attrs);
    void read_entities    (const xmlChar* name, const xmlChar** attrs);

    ParserState state;
    MeshFunctionType mf_type;
    MeshFunction<int>* _imeshfunction;
    MeshFunction<unsigned int>* _uimeshfunction;
    MeshFunction<double>* _dmeshfunction;
    MeshFunction<bool>* _bmeshfunction;

  };
  
}

#endif
