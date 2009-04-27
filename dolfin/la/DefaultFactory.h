// Copyright (C) 2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-05-17
// Last changed: 2007-05-17

#ifndef __DEFAULT_FACTORY_H
#define __DEFAULT_FACTORY_H

#include "LinearAlgebraFactory.h"

namespace dolfin
{

  class DefaultFactory : public LinearAlgebraFactory
  {
    public:

    /// Constructor
    DefaultFactory() {}

    /// Destructor
    virtual ~DefaultFactory() {}

    /// Create empty matrix
    virtual dolfin::GenericMatrix* create_matrix() const;

    /// Create empty vector
    virtual dolfin::GenericVector* create_vector() const;

    /// Create empty sparsity pattern
    virtual dolfin::GenericSparsityPattern * create_pattern() const;

  private:

    // Return instance of default backend
    LinearAlgebraFactory& factory() const;

  };

}

#endif
