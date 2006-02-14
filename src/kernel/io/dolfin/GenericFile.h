// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-07-15
// Last changed: 2005-10-03

#ifndef __GENERIC_FILE_H
#define __GENERIC_FILE_H

#include <string>

namespace dolfin
{
  
  class Vector;
  class Matrix;
  class Mesh;
  class Function;
  class Sample;
  class ParameterList;
  class BLASFormData;

  class FiniteElement;
  
  class GenericFile
  {
  public:
    
    GenericFile(const std::string filename);
    virtual ~GenericFile();
    
    // Input
    
    virtual void operator>> (Vector& x);
    virtual void operator>> (Matrix& A);
    virtual void operator>> (Mesh& mesh);
    virtual void operator>> (Sample& sample);
    virtual void operator>> (ParameterList& parameters);
    virtual void operator>> (BLASFormData& blas);
    virtual void parse(Function& u, FiniteElement& element);
    
    // Output
    
    virtual void operator<< (Vector& x);
    virtual void operator<< (Matrix& A);
    virtual void operator<< (Mesh& mesh);
    virtual void operator<< (Function& u);
    virtual void operator<< (Sample& sample);
    virtual void operator<< (ParameterList& parameters);
    virtual void operator<< (BLASFormData& blas);
    
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
