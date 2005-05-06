// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __FINITE_ELEMENT_H
#define __FINITE_ELEMENT_H

#include <dolfin/Mesh.h>
#include <dolfin/Cell.h>
#include <dolfin/Node.h>
#include <dolfin/Point.h>

namespace dolfin
{
  
  /// This is the base class for finite elements automatically
  /// generated by the FEniCS Form Compiler FFC.

  class FiniteElement
  {
  public:
   
    /// Constructor
    FiniteElement();

    /// Destructor
    virtual ~FiniteElement();
    
    /// Return dimension of the finite element space
    virtual unsigned int spacedim() const = 0;

    /// Return dimension of the underlying shape
    virtual unsigned int shapedim() const = 0;

    /// Return vector dimension of the finite element space
    virtual unsigned int tensordim(unsigned int i) const = 0;

    /// Return vector dimension of the finite element space
    virtual unsigned int rank() const = 0;
    
    /// FIXME: Old version, remove
    /// Return map from local to global degree of freedom
    virtual unsigned int dof(unsigned int i, const Cell& cell, const Mesh& mesh) const = 0;

    /// FIXME: New version, should replace dof()
    /// Compute map from local to global degrees of freedom
    virtual void dofmap(int dofs[], const Cell& cell, const Mesh& mesh) const = 0;
    
    /// Return map from local degree of freedom to global coordinate
    virtual const Point coord(unsigned int i, const Cell& cell, const Mesh& mesh) const = 0;

  };

}

#endif
