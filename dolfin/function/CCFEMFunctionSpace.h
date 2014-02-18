// Copyright (C) 2013 Anders Logg
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
// First added:  2013-08-05
// Last changed: 2013-09-25

#ifndef __CCFEM_FUNCTION_SPACE_H
#define __CCFEM_FUNCTION_SPACE_H

#include <vector>
#include <memory>

namespace dolfin
{

  // Forward declarations
  class FunctionSpace;
  class BoundingBoxTree;
  class CCFEMDofMap;
  class GenericDofMap;

  /// This class represents a cut and composite finite element
  /// function space (CCFEM) defined on one or more possibly
  /// intersecting meshes.
  ///
  /// FIXME: Document usage of class with add() followed by build()

  class CCFEMFunctionSpace
  {
  public:

    /// Create empty CCFEM function space
    CCFEMFunctionSpace();

    /// Destructor
    ~CCFEMFunctionSpace();

    /// Return dimension of the CCFEM function space
    ///
    /// *Returns*
    ///     std::size_t
    ///         The dimension of the CCFEM function space.
    std::size_t dim() const;

    /// Return CCFEM dofmap
    ///
    /// *Returns*
    ///     _CCFEMDofMap_
    ///         The dofmap.
    std::shared_ptr<const CCFEMDofMap> dofmap() const;

    /// Return the number function spaces (parts) of the CCFEM function space
    ///
    /// *Returns*
    ///     std::size_t
    ///         The number of function spaces (parts) of the CCFEM function space.
    std::size_t num_parts() const;

    /// Return function space (part) number i
    ///
    /// *Returns*
    ///     _FunctionSpace_
    ///         Function space (part) number i
    std::shared_ptr<const FunctionSpace> part(std::size_t i) const;

    /// Add function space (shared pointer version)
    ///
    /// *Arguments*
    ///     function_space (_FunctionSpace_)
    ///         The function space.
    void add(std::shared_ptr<const FunctionSpace> function_space);

    /// Add function space (reference version)
    ///
    /// *Arguments*
    ///     function_space (_FunctionSpace_)
    ///         The function space.
    void add(const FunctionSpace& function_space);

    /// Build CCFEM function space
    void build();

    /// Clear CCFEM function space
    void clear();

  private:

    // List of function spaces
    std::vector<std::shared_ptr<const FunctionSpace> > _function_spaces;

    // List of bounding box trees for meshes
    std::vector<std::shared_ptr<BoundingBoxTree> > _trees;

    // CCFEM dofmap
    std::shared_ptr<CCFEMDofMap> _dofmap;

  };

}

#endif
