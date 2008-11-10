// Copyright (C) 2003-2008 Johan Hoffman and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-07-15
// Last changed: 2008-03-29

#ifndef __M_FILE_H
#define __M_FILE_H

#include <dolfin/common/types.h>
#include <dolfin/common/real.h>
#include "GenericFile.h"

/// Note: This M file  generator will save the values only with double precision, even if 
/// dolfin is working with GMP types (extended precision)

namespace dolfin
{
  
  class MFile : public GenericFile
  {
  public:
    
    MFile(const std::string filename);
    virtual ~MFile();

    // Input
    
    // Output

    void operator<< (const GenericVector& x);
    virtual void operator<< (const GenericMatrix& A) = 0;
    void operator<< (const Mesh& mesh);
    void operator<< (const Function& u);
    void operator<< (Sample& sample);

  };
  
}

#endif
