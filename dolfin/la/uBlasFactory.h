// Copyright (C) 2007 Ola Skavhaug.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
//
// First added:  2007-12-06
// Last changed: 2008-05-18

#ifndef __UBLAS_FACTORY_H
#define __UBLAS_FACTORY_H

#include "uBlasMatrix.h"
#include "uBlasVector.h"
#include "SparsityPattern.h"
#include "LinearAlgebraFactory.h"


namespace dolfin
{
  // Forward declaration
  template< class T> class uBlasMatrix;

  template<class Mat = ublas_sparse_matrix>
  class uBlasFactory: public LinearAlgebraFactory
  {
  public:

    /// Destructor
    virtual ~uBlasFactory() {}

    /// Create empty matrix
    uBlasMatrix<Mat>* createMatrix() const
    { return new uBlasMatrix<Mat>(); }

    /// Create empty sparsity pattern 
    SparsityPattern* createPattern() const
    { return new SparsityPattern(); }

    /// Create empty vector
    uBlasVector* createVector() const
    { return new uBlasVector(); }

    static uBlasFactory<Mat>& instance() 
    { return factory; }

  private:

    // Private Constructor
    uBlasFactory() {}

    // Singleton instance
    static uBlasFactory<Mat> factory;
  };
}

// Initialise static data
template<class Mat> dolfin::uBlasFactory<Mat> dolfin::uBlasFactory<Mat>::factory;


#endif
