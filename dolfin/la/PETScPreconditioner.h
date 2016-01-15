// Copyright (C) 2010 Garth N. Wells
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Anders Logg 2011
//
// First added:  2010-02-25
// Last changed: 2014-07-09

#ifndef __DOLFIN_PETSC_PRECONDITIONER_H
#define __DOLFIN_PETSC_PRECONDITIONER_H

#ifdef HAS_PETSC

#include <string>
#include <vector>
#include <memory>
#include <petscpc.h>

#include <dolfin/common/types.h>
#include <dolfin/common/Variable.h>
#include <dolfin/la/PETScObject.h>
#include <dolfin/parameter/Parameters.h>
#include "GenericPreconditioner.h"

namespace dolfin
{

  // Forward declarations
  class PETScKrylovSolver;
  class PETScSNESSolver;
  class VectorSpaceBasis;

  /// This class is a wrapper for configuring PETSc
  /// preconditioners. It does not own a preconditioner. It can take a
  /// PETScKrylovSolver and set the preconditioner type and
  /// parameters.

  class PETScPreconditioner : public PETScObject,
    public GenericPreconditioner, public Variable
  {
  public:

    /// Create a particular preconditioner object
    explicit PETScPreconditioner(std::string type = "default");

    /// Destructor
    virtual ~PETScPreconditioner();

    /// Set the preconditioner type and parameters
    virtual void set(PETScKrylovSolver& solver);

    /// Set the (near) null space of the preconditioner operator
    /// (matrix). This is required for certain preconditioner types,
    /// e.g. smoothed aggregation multigrid
    void set_nullspace(const VectorSpaceBasis& near_nullspace);

    /// Return the PETSc null space
    MatNullSpace near_nullspace() const
    { return petsc_near_nullspace; }

    /// Set the coordinates of the operator (matrix) rows and
    /// geometric dimension d. This is can be used by required for
    /// certain preconditioners, e.g. ML. The input for this function
    /// can be generated using GenericDofMap::tabulate_all_dofs.
    void set_coordinates(const std::vector<double>& x, std::size_t dim);

    // FIXME: Document
    // FIXME: Use shared pointers for the fields, so we can use
    // PETSC_USE_POINTER instead of PETSC_COPY_VALUES
    static void
      set_fieldsplit(PETScKrylovSolver& solver,
                     const std::vector<std::vector<dolfin::la_index>>& fields,
                     const std::vector<std::string>& split_names);

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

    /// Return a list of available preconditioners
    static std::map<std::string, std::string> preconditioners();

    /// Default parameter values
    static Parameters default_parameters();

    friend class PETScSNESSolver;

    friend class PETScTAOSolver;

  private:

    /// Named preconditioner
    std::string _type;

    // Available names preconditioners
    static const std::map<std::string, const PCType> _methods;

    // Available preconditioner descriptions
    static const std::map<std::string, std::string>
      _methods_descr;

    // Near null space vectors
    std::vector<PETScVector> _near_nullspace;

    // PETSc near null space.
    MatNullSpace petsc_near_nullspace;

    // Operator row coordinates
    std::vector<double> _coordinates;

    // Geometric dimension associates with coordinates
    std::size_t gdim;

  };

}

#endif

#endif
