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
// Modified by Garth N. Wells, 2008-2011.
// Modified by Martin Alnes, 2008.
//
// First added:  2007-04-02
// Last changed: 2011-09-13

#ifndef __FORM_H
#define __FORM_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <dolfin/common/Hierarchical.h>
#include <dolfin/common/types.h>
#include "DomainAssigner.h"
#include "Equation.h"

// Forward declaration
namespace ufc
{
  class form;
}

namespace dolfin
{

  class FunctionSpace;
  class GenericFunction;
  class Mesh;
  template <typename T> class MeshFunction;

  /// Base class for UFC code generated by FFC for DOLFIN with option -l.
  ///
  /// A note on the order of trial and test spaces: FEniCS numbers
  /// argument spaces starting with the leading dimension of the
  /// corresponding tensor (matrix). In other words, the test space is
  /// numbered 0 and the trial space is numbered 1. However, in order
  /// to have a notation that agrees with most existing finite element
  /// literature, in particular
  ///
  ///     a = a(u, v)
  ///
  /// the spaces are numbered from right to
  ///
  ///     a: V_1 x V_0 -> R
  ///
  /// .. note::
  ///
  ///     Figure out how to write this in math mode without it getting
  ///     messed up in the Python version.
  ///
  /// This is reflected in the ordering of the spaces that should be
  /// supplied to generated subclasses. In particular, when a bilinear
  /// form is initialized, it should be initialized as
  ///
  /// .. code-block:: c++
  ///
  ///     a(V_1, V_0) = ...
  ///
  /// where ``V_1`` is the trial space and ``V_0`` is the test space.
  /// However, when a form is initialized by a list of argument spaces
  /// (the variable ``function_spaces`` in the constructors below, the
  /// list of spaces should start with space number 0 (the test space)
  /// and then space number 1 (the trial space).

  class Form : public Hierarchical<Form>
  {
  public:

    /// Create form of given rank with given number of coefficients
    ///
    /// *Arguments*
    ///     rank (std::size_t)
    ///         The rank.
    ///     num_coefficients (std::size_t)
    ///         The number of coefficients.
    Form(std::size_t rank, std::size_t num_coefficients);

    /// Create form (shared data)
    ///
    /// *Arguments*
    ///     ufc_form (ufc::form)
    ///         The UFC form.
    ///     function_spaces (std::vector<_FunctionSpace_>)
    ///         Vector of function spaces.
    ///     coefficients (std::vector<_GenericFunction_>)
    ///         Vector of coefficients.
    Form(boost::shared_ptr<const ufc::form> ufc_form,
         std::vector<boost::shared_ptr<const FunctionSpace> > function_spaces,
         std::vector<boost::shared_ptr<const GenericFunction> > coefficients);

    /// Destructor
    virtual ~Form();

    /// Return rank of form (bilinear form = 2, linear form = 1,
    /// functional = 0, etc)
    ///
    /// *Returns*
    ///     std::size_t
    ///         The rank of the form.
    std::size_t rank() const;

    /// Return number of coefficients
    ///
    /// *Returns*
    ///     std::size_t
    ///         The number of coefficients.
    std::size_t num_coefficients() const;

    /// Return coloring type for colored (multi-threaded) assembly of form
    /// over a mesh entity of a given dimension
    ///
    /// *Arguments*
    ///     entity_dim (std::size_t)
    ///         Dimension.
    ///
    /// *Returns*
    ///     std::vector<std::size_t>
    ///         Coloring type.
    std::vector<std::size_t> coloring(std::size_t entity_dim) const;

    /// Set mesh, necessary for functionals when there are no function spaces
    ///
    /// *Arguments*
    ///     mesh (_Mesh_)
    ///         The mesh.
    void set_mesh(boost::shared_ptr<const Mesh> mesh);

    /// Extract common mesh from form
    ///
    /// *Returns*
    ///     _Mesh_
    ///         The mesh.
    const Mesh& mesh() const;

    /// Return mesh shared pointer (if any)
    ///
    /// *Returns*
    ///     _Mesh_
    ///         The mesh shared pointer.
    boost::shared_ptr<const Mesh> mesh_shared_ptr() const;

    /// Return function space for given argument
    ///
    /// *Arguments*
    ///     i (std::size_t)
    ///         Index
    ///
    /// *Returns*
    ///     _FunctionSpace_
    ///         Function space shared pointer.
    boost::shared_ptr<const FunctionSpace> function_space(std::size_t i) const;

    /// Return function spaces for arguments
    ///
    /// *Returns*
    ///     std::vector<_FunctionSpace_>
    ///         Vector of function space shared pointers.
    std::vector<boost::shared_ptr<const FunctionSpace> > function_spaces() const;

    /// Set coefficient with given number (shared pointer version)
    ///
    /// *Arguments*
    ///     i (std::size_t)
    ///         The given number.
    ///     coefficient (_GenericFunction_)
    ///         The coefficient.
    void set_coefficient(std::size_t i,
                         boost::shared_ptr<const GenericFunction> coefficient);

    /// Set coefficient with given name (shared pointer version)
    ///
    /// *Arguments*
    ///     name (std::string)
    ///         The name.
    ///     coefficient (_GenericFunction_)
    ///         The coefficient.
    void set_coefficient(std::string name,
                         boost::shared_ptr<const GenericFunction> coefficient);

    /// Set all coefficients in given map, possibly a subset (shared
    /// pointer version)
    ///
    /// *Arguments*
    ///     coefficients (std::map<std::string, _GenericFunction_>)
    ///         The map of coefficients.
    void set_coefficients(std::map<std::string, boost::shared_ptr<const GenericFunction> > coefficients);

    /// Return coefficient with given number
    ///
    /// *Arguments*
    ///     i (std::size_t)
    ///         Index
    ///
    /// *Returns*
    ///     _GenericFunction_
    ///         The coefficient.
    boost::shared_ptr<const GenericFunction> coefficient(std::size_t i) const;

    /// Return coefficient with given name
    ///
    /// *Arguments*
    ///     name (std::string)
    ///         The name.
    ///
    /// *Returns*
    ///     _GenericFunction_
    ///         The coefficient.
    boost::shared_ptr<const GenericFunction> coefficient(std::string name) const;

    /// Return all coefficients
    ///
    /// *Returns*
    ///     std::vector<_GenericFunction_>
    ///         All coefficients.
    std::vector<boost::shared_ptr<const GenericFunction> > coefficients() const;

    /// Return the number of the coefficient with this name
    ///
    /// *Arguments*
    ///     name (std::string)
    ///         The name.
    ///
    /// *Returns*
    ///     std::size_t
    ///         The number of the coefficient with the given name.
    virtual std::size_t coefficient_number(const std::string & name) const;

    /// Return the name of the coefficient with this number
    ///
    /// *Arguments*
    ///     i (std::size_t)
    ///         The number
    ///
    /// *Returns*
    ///     std::string
    ///         The name of the coefficient with the given number.
    virtual std::string coefficient_name(std::size_t i) const;

    /// Return cell domains (zero pointer if no domains have been
    /// specified)
    ///
    /// *Returns*
    ///     _MeshFunction_ <std::size_t>
    ///         The cell domains.
    boost::shared_ptr<const MeshFunction<std::size_t> > cell_domains_shared_ptr() const;

    /// Return exterior facet domains (zero pointer if no domains have
    /// been specified)
    ///
    /// *Returns*
    ///     boost::shared_ptr<_MeshFunction_ <std::size_t> >
    ///         The exterior facet domains.
    boost::shared_ptr<const MeshFunction<std::size_t> > exterior_facet_domains_shared_ptr() const;

    /// Return interior facet domains (zero pointer if no domains have
    /// been specified)
    ///
    /// *Returns*
    ///     _MeshFunction_ <std::size_t>
    ///         The interior facet domains.
    boost::shared_ptr<const MeshFunction<std::size_t> > interior_facet_domains_shared_ptr() const;

    /// Set cell domains
    ///
    /// *Arguments*
    ///     cell_domains (_MeshFunction_ <std::size_t>)
    ///         The cell domains.
    void set_cell_domains(boost::shared_ptr<const MeshFunction<std::size_t> > cell_domains);

    /// Set exterior facet domains
    ///
    /// *Arguments*
    ///     exterior_facet_domains (_MeshFunction_ <std::size_t>)
    ///         The exterior facet domains.
    void set_exterior_facet_domains(boost::shared_ptr<const MeshFunction<std::size_t> > exterior_facet_domains);

    /// Set interior facet domains
    ///
    /// *Arguments*
    ///     interior_facet_domains (_MeshFunction_ <std::size_t>)
    ///         The interior facet domains.
    void set_interior_facet_domains(boost::shared_ptr<const MeshFunction<std::size_t> > interior_facet_domains);

    /// Return UFC form shared pointer
    ///
    /// *Returns*
    ///     ufc::form
    ///         The UFC form.
    boost::shared_ptr<const ufc::form> ufc_form() const;

    /// Check function spaces and coefficients
    void check() const;

    /// Comparison operator, returning equation lhs == rhs
    Equation operator==(const Form& rhs) const;

    /// Comparison operator, returning equation lhs == 0
    Equation operator==(int rhs) const;

    // Assignment of domains
    CellDomainAssigner dx;
    ExteriorFacetDomainAssigner ds;
    InteriorFacetDomainAssigner dS;

  protected:

    // The UFC form
    boost::shared_ptr<const ufc::form> _ufc_form;

    // Function spaces (one for each argument)
    std::vector<boost::shared_ptr<const FunctionSpace> > _function_spaces;

    // Coefficients
    std::vector<boost::shared_ptr<const GenericFunction> > _coefficients;

    // The mesh (needed for functionals when we don't have any spaces)
    boost::shared_ptr<const Mesh> _mesh;

    // Markers for cell domains
    boost::shared_ptr<const MeshFunction<std::size_t> > _cell_domains;

    // Markers for exterior facet domains
    boost::shared_ptr<const MeshFunction<std::size_t> > _exterior_facet_domains;

    // Markers for interior facet domains
    boost::shared_ptr<const MeshFunction<std::size_t> > _interior_facet_domains;

  private:

    const std::size_t _rank;

  };

}

#endif
