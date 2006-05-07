// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-03-31
// Last changed: 2006-05-07

#ifndef __MASS_MATRIX_H
#define __MASS_MATRIX_H

#ifdef HAVE_PETSC_H

#include <dolfin/Matrix.h>

namespace dolfin
{

  /// The standard mass matrix on a given mesh.

  class MassMatrix : public Matrix
  {
  public:
  
    /// Construct a mass matrix for a given mesh
    MassMatrix(Mesh& mesh);

  };

}

#endif

#endif
