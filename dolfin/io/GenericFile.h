// Copyright (C) 2003-2008 Johan Hoffman and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-07-15
// Last changed: 2008-03-29

#ifndef __GENERIC_FILE_H
#define __GENERIC_FILE_H

#include <string>

#include <dolfin/la/GenericVector.h>
#include <dolfin/la/GenericMatrix.h>

namespace dolfin
{
  
  class Mesh;
  class Graph;
  template <class T> class MeshFunction;
  class Function;
  class Sample;
  class FiniteElementSpec;
  class ParameterList;
  class BLASFormData;

  class FiniteElement;
  
  class GenericFile
  {
  public:
    
    GenericFile(const std::string filename);
    virtual ~GenericFile();
    
    // Input

    virtual void operator>> (GenericVector& x);
    virtual void operator>> (GenericMatrix& A);
    virtual void operator>> (Mesh& mesh);
    virtual void operator>> (MeshFunction<int>& meshfunction);
    virtual void operator>> (MeshFunction<unsigned int>& meshfunction);
    virtual void operator>> (MeshFunction<double>& meshfunction);
    virtual void operator>> (MeshFunction<bool>& meshfunction);
    virtual void operator>> (Function& mesh);
    virtual void operator>> (Sample& sample);
    virtual void operator>> (FiniteElementSpec& spec);
    virtual void operator>> (ParameterList& parameters);
    virtual void operator>> (BLASFormData& blas);
    virtual void operator>> (Graph& graph);
    
    // Output
    
    virtual void operator<< (const GenericVector& x);
    virtual void operator<< (const GenericMatrix& A);
    virtual void operator<< (const Mesh& mesh);
    virtual void operator<< (const MeshFunction<int>& meshfunction);
    virtual void operator<< (const MeshFunction<unsigned int>& meshfunction);
    virtual void operator<< (const MeshFunction<double>& meshfunction);
    virtual void operator<< (const MeshFunction<bool>& meshfunction);
    virtual void operator<< (const Function& u);
    virtual void operator<< (const Sample& sample);
    virtual void operator<< (const FiniteElementSpec& spec);
    virtual void operator<< (const ParameterList& parameters);
    virtual void operator<< (const BLASFormData& blas);
    virtual void operator<< (const Graph& graph);
    
    void read();
    void write();
    
  protected:
    
    void read_not_impl(const std::string object);
    void write_not_impl(const std::string object);

    std::string filename;
    std::string type;
    
    bool opened_read;
    bool opened_write;

    bool check_header; // True if we have written a header

    // Counters for the number of times various data has been written
    uint counter;
    uint counter1;
    uint counter2;

  };
  
}

#endif
