// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __XML_MESH_H
#define __XML_MESH_H

#include <dolfin/XMLObject.h>

namespace dolfin {

  class Mesh;
  
  class XMLMesh : public XMLObject {
  public:

    XMLMesh(Mesh& mesh_);
    
    void startElement (const xmlChar *name, const xmlChar **attrs);
    void endElement   (const xmlChar *name);
    
    void reading(std::string filename);
    void done();
    
  private:
    
    enum ParserState { OUTSIDE, INSIDE_MESH, INSIDE_NODES, INSIDE_CELLS, DONE };
    
    void readMesh        (const xmlChar *name, const xmlChar **attrs);
    void readNodes       (const xmlChar *name, const xmlChar **attrs);
    void readCells       (const xmlChar *name, const xmlChar **attrs);
    void readNode        (const xmlChar *name, const xmlChar **attrs);
    void readTriangle    (const xmlChar *name, const xmlChar **attrs);
    void readTetrahedron (const xmlChar *name, const xmlChar **attrs);
    
    void initMesh();

    Mesh& mesh;
    int nodes;
    int cells;
    
    bool _create_edges;
    
    ParserState state;
    
  };
  
}

#endif
