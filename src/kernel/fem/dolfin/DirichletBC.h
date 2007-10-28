// Copyright (C) 2007 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristian Oelgaard, 2007
//
// First added:  2007-04-10
// Last changed: 2007-10-28

#ifndef __DIRICHLET_BC_H
#define __DIRICHLET_BC_H

#include <dolfin/constants.h>
#include <dolfin/SubSystem.h>
#include <dolfin/MeshFunction.h>
#include <dolfin/BoundaryCondition.h>
#include <dolfin/Facet.h>

namespace dolfin
{

  class Function;
  class Mesh;
  class SubDomain;
  class Form;
  class GenericMatrix;
  class GenericVector;
  
  /// The BCMethod variable may be used to specify the type of method
  /// used to identify degrees of freedom on the boundary. Either a
  /// topological approach (default) or a geometrical approach may be
  /// used. The topological approach is faster, but will only identify
  /// degrees of freedom that are located on a facet that is entirely
  /// on the boundary. In particular, the topological approach will
  /// not identify degrees of freedom for discontinuous elements
  /// (which are all internal to the cell).
  enum BCMethod {topological, geometrical};
  
  /// This class specifies the interface for setting (strong)
  /// Dirichlet boundary conditions for partial differential
  /// equations,
  ///
  ///    u = g on G,
  ///
  /// where u is the solution to be computed, g is a function
  /// and G is a sub domain of the mesh.
  ///
  /// A DirichletBC is specified by a Function, a Mesh,
  /// a MeshFunction<uint> over the facets of the mesh and
  /// an integer sub_domain specifying the sub domain on which
  /// the boundary condition is to be applied.
  ///
  /// For mixed systems (vector-valued and mixed elements), an
  /// optional set of parameters may be used to specify for which sub
  /// system the boundary condition should be specified.
  
  class DirichletBC : public BoundaryCondition
  {
  public:

    /// Create boundary condition for sub domain
    DirichletBC(Function& g,
                Mesh& mesh,
                SubDomain& sub_domain,
                BCMethod method = topological);

    /// Create boundary condition for sub domain specified by index
    DirichletBC(Function& g,
                MeshFunction<uint>& sub_domains, uint sub_domain,
                BCMethod method = topological);

    /// Create sub system boundary condition for sub domain
    DirichletBC(Function& g,
                Mesh& mesh,
                SubDomain& sub_domain,
                const SubSystem& sub_system,
                BCMethod method = topological);

    /// Create sub system boundary condition for sub domain specified by index
    DirichletBC(Function& g,
                MeshFunction<uint>& sub_domains,
                uint sub_domain,
                const SubSystem& sub_system,
                BCMethod method = topological);

    /// Destructor
    ~DirichletBC();

    /// Apply boundary condition to linear system
    void apply(GenericMatrix& A, GenericVector& b, const Form& form);

    /// Apply boundary condition to linear system
    void apply(GenericMatrix& A, GenericVector& b, const ufc::form& form);

    /// Apply boundary condition to linear system for a nonlinear problem
    void apply(GenericMatrix& A, GenericVector& b, const GenericVector& x, const Form& form);

    /// Apply boundary condition to linear system for a nonlinear problem
    void apply(GenericMatrix& A, GenericVector& b, const GenericVector& x, const ufc::form& form);

  private:

    // Initialize sub domain markers    
    void init(SubDomain& sub_domain);

    /// Apply boundary conditions
    void apply(GenericMatrix& A, GenericVector& b,
               const GenericVector* x, const ufc::form& form);

    // The function
    Function& g;

    // The mesh
    Mesh& mesh;

    // Sub domain markers (if any)
    MeshFunction<uint>* sub_domains;

    // The sub domain
    uint sub_domain;

    // True if sub domain markers are created locally
    bool sub_domains_local;

    // Sub system
    SubSystem sub_system;

    // Search method
    BCMethod method;

    // Compute boundary values for facet (topological approach)
    void computeBCTopological(std::map<uint, real>& boundary_values,
                              Facet& facet,
                              BoundaryCondition::LocalData& data);
    
    // Compute boundary values for facet (geometrical approach)
    void computeBCGeometrical(std::map<uint, real>& boundary_values,
                              Facet& facet,
                              BoundaryCondition::LocalData& data);
    
    // Check if the point is in the same plane as the given facet
    //    static bool onFacet(const Point& p, Cell& facet);
    static bool onFacet(real* coordinates, Facet& facet);
    
  };

}

#endif
