// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells 2007
//
// First added:  2007-07-08
// Last changed: 2008-10-30

#ifndef __PERIODIC_BC_H
#define __PERIODIC_BC_H

#include <dolfin/common/types.h>
#include "SubSystem.h"
#include "BoundaryCondition.h"

namespace dolfin
{

  class DofMap;
  class Mesh;
  class SubDomain;
  class Form;
  class GenericMatrix;
  class GenericVector;

  /// This class specifies the interface for setting periodic boundary
  /// conditions for partial differential equations,
  ///
  ///    u(x) = u(F^{-1}(x)) on G,
  ///    u(x) = u(F(x))      on H,
  ///
  /// where F : H --> G is a map from a subdomain H to a subdomain G.
  ///
  /// A PeriodicBC is specified by a Mesh and a SubDomain. The given
  /// subdomain must overload both the inside() function, which
  /// specifies the points of G, and the map() function, which
  /// specifies the map from the points of H to the points of G.
  ///
  /// For mixed systems (vector-valued and mixed elements), an
  /// optional set of parameters may be used to specify for which sub
  /// system the boundary condition should be specified.
  
  class PeriodicBC : public BoundaryCondition
  {
  public:

    /// Create periodic boundary condition for sub domain
    PeriodicBC(const FunctionSpace& V,
               const SubDomain& sub_domain);

    /// Create sub system boundary condition for sub domain
    PeriodicBC(const FunctionSpace& V,
               const SubDomain& sub_domain,
               const SubSystem& sub_system);
    
    /// Destructor
    ~PeriodicBC();

    /// Apply boundary condition to a matrix
    void apply(GenericMatrix& A);

    /// Apply boundary condition to a vector
    void apply(GenericVector& b);

    /// Apply boundary condition to a linear system
    void apply(GenericMatrix& A, GenericVector& b);

    /// Apply boundary condition to a vector for a nonlinear problem
    void apply(GenericVector& b, const GenericVector& x);

    /// Apply boundary condition to a linear system for a nonlinear problem
    void apply(GenericMatrix& A, GenericVector& b, const GenericVector& x);

  private:

    // The function space (possibly a sub function space)
    std::tr1::shared_ptr<const FunctionSpace> V;

    // The subdomain
    const SubDomain& sub_domain;

  };

}

#endif
