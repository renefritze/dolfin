// Copyright (C) 2007 Garth N. Wells
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
// Modified by Ola Skavhaug 2007.
// Modified by Anders Logg 2008-2013
//
// First added:  2007-05-24
// Last changed: 2013-09-24

#ifndef __SPARSITY_PATTERN_BUILDER_H
#define __SPARSITY_PATTERN_BUILDER_H

#include <utility>
#include <vector>
#include "dolfin/common/types.h"

namespace dolfin
{

  class GenericDofMap;
  class GenericSparsityPattern;
  class Mesh;
  class CCFEMForm;

  /// This class provides functions to compute the sparsity pattern.

  class SparsityPatternBuilder
  {
  public:

    /// Build sparsity pattern for assembly of given form
    static void build(GenericSparsityPattern& sparsity_pattern,
                      const Mesh& mesh,
                      const std::vector<const GenericDofMap*> dofmaps,
                      bool cells,
                      bool interior_facets,
                      bool exterior_facets,
                      bool diagonal,
                      bool init=true,
                      bool finalize=true);

    /// Build sparsity pattern for assembly of given CCFEM form
    static void build_ccfem(GenericSparsityPattern& sparsity_pattern,
                            const CCFEMForm& form);

  };

}

#endif
