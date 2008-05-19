// Copyright (C) 2007 Ola Skavhaug.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-11-30
// Last changed: 2007-12-06

#ifndef __LINEAR_ALGEBRA_FACTORY_H
#define __LINEAR_ALGEBRA_FACTORY_H

#include "GenericMatrix.h"
#include "GenericSparsityPattern.h"
#include "GenericVector.h"

namespace dolfin
{

  class LinearAlgebraFactory
  {
    public:

    /// Constructor
    LinearAlgebraFactory() {}

    /// Destructor
    virtual ~LinearAlgebraFactory() {}

    /// Create empty matrix
    virtual dolfin::GenericMatrix* createMatrix() const = 0;

    /// Create empty vector
    virtual dolfin::GenericVector* createVector() const = 0;

    /// Create empty sparsity pattern 
    virtual dolfin::GenericSparsityPattern * createPattern() const = 0;

  };

}

#endif
