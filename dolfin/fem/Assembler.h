// Copyright (C) 2007-2011 Anders Logg
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
// Modified by Garth N. Wells 2007-2008
// Modified by Ola Skavhaug 2008
// Modified by Joachim B Haga 2012
//
// First added:  2007-01-17
// Last changed: 2013-09-19

#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include <vector>
#include "AssemblerBase.h"

namespace dolfin
{

  // Forward declarations
  class GenericTensor;
  class Form;
  class UFC;
  template<typename T> class MeshFunction;

  /// This class provides automated assembly of linear systems, or
  /// more generally, assembly of a sparse tensor from a given
  /// variational form.
  ///
  /// Subdomains for cells and facets may be specified by assigning
  /// subdomain indicators specified by _MeshFunction_ to the _Form_
  /// being assembled:
  ///
  ///    .. code-block:: c++
  ///
  ///        form.dx = cell_domains
  ///        form.ds = exterior_facet_domains
  ///        form.dS = interior_facet_domains

  class Assembler : public AssemblerBase
  {
  public:

    /// Constructor
    Assembler() {}

    /// Assemble tensor from given form
    ///
    /// *Arguments*
    ///     A (_GenericTensor_)
    ///         The tensor to assemble.
    ///     a (_Form_)
    ///         The form to assemble the tensor from.
    void assemble(GenericTensor& A, const Form& a);

    /// Assemble tensor from given form over cells. This function is
    /// provided for users who wish to build a customized assembler.
    void assemble_cells(GenericTensor& A, const Form& a, UFC& ufc,
                        std::shared_ptr<const MeshFunction<std::size_t> > domains,
                        std::vector<double>* values);

    /// Assemble tensor from given form over exterior facets. This
    /// function is provided for users who wish to build a customized
    /// assembler.
    void assemble_exterior_facets(GenericTensor& A, const Form& a,
                                  UFC& ufc,
                                  std::shared_ptr<const MeshFunction<std::size_t> > domains,
                                  std::vector<double>* values);

    /// Assemble tensor from given form over interior facets. This
    /// function is provided for users who wish to build a customized
    /// assembler.
    void assemble_interior_facets(GenericTensor& A, const Form& a,
                                  UFC& ufc,
                                  std::shared_ptr<const MeshFunction<std::size_t> > domains,
                                  std::vector<double>* values);

    /// Assemble tensor from given form over vertices. This function is
    /// provided for users who wish to build a customized assembler.
    void assemble_vertices(GenericTensor& A, const Form& a, UFC& ufc,
                           std::shared_ptr<const MeshFunction<std::size_t> > domains);

  };

}

#endif
