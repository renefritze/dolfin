// Copyright (C) 2003-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Magnus Vikstrom 2007.
//
// First added:  2003-07-15
// Last changed: 2009-01-14

#ifndef __XML_FILE_H
#define __XML_FILE_H

#include <libxml/parser.h>

#include <dolfin/common/types.h>
#include <dolfin/la/Vector.h>
#include <dolfin/la/GenericMatrix.h>
#include "GenericFile.h"

namespace dolfin
{
  
  class Mesh;
  class Graph;
  template <class T> class MeshFunction;
  class ParameterList;
  class BLASFormData;
  class LocalMeshData;

  class XMLObject;
  
  class XMLFile : public GenericFile
  {
  public:
    
    XMLFile(const std::string filename, bool gzip);
    ~XMLFile();
    
    // Input

    void operator>> (GenericVector& x);
    void operator>> (GenericMatrix& A);
    void operator>> (Mesh& mesh);
    void operator>> (LocalMeshData& meshdata);
    void operator>> (MeshFunction<int>& meshfunction);
    void operator>> (MeshFunction<unsigned int>& meshfunction);
    void operator>> (MeshFunction<double>& meshfunction);
    void operator>> (MeshFunction<bool>& meshfunction);
    void operator>> (ParameterList& parameters);
    void operator>> (BLASFormData& blas);
    void operator>> (Graph& graph);
    
    // Output

    void operator<< (const GenericVector& x);
    void operator<< (const GenericMatrix& A);
    void operator<< (const Mesh& mesh);
    void operator<< (const Graph& graph);
    void operator<< (const MeshFunction<int>& mesh);
    void operator<< (const MeshFunction<unsigned int>& mesh);
    void operator<< (const MeshFunction<double>& mesh);
    void operator<< (const MeshFunction<bool>& mesh);
    void operator<< (const ParameterList& parameters);
    
    // Friends
    friend void sax_start_element (void *ctx, const xmlChar *name, const xmlChar **attrs);
    friend void sax_end_element   (void *ctx, const xmlChar *name);
    
  private:
    
    void parseFile();
    void parseSAX();

    FILE* openFile();
    void  closeFile(FILE* fp);
    
    // Implementation for specific class (output)
    XMLObject* xmlObject;

    // True if header is written (need to close)
    bool header_written;

    // Most recent position in file
    long mark;

    // True if file is/should be gzipped
    bool gzip;

  };
  
  // Callback functions for the SAX interface
  
  void sax_start_document (void *ctx);
  void sax_end_document   (void *ctx);
  void sax_start_element  (void *ctx, const xmlChar *name, const xmlChar **attrs);
  void sax_end_element    (void *ctx, const xmlChar *name);

  void sax_warning     (void *ctx, const char *msg, ...);
  void sax_error       (void *ctx, const char *msg, ...);
  void sax_fatal_error (void *ctx, const char *msg, ...);
  
}

#endif
