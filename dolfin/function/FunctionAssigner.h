// Copyright (C) 2013 Johan Hake
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
// First added:  2013-09-20
// Last changed: 2013-11-08

#ifndef __DOLFIN_FUNCTION_ASSIGNER_H
#define __DOLFIN_FUNCTION_ASSIGNER_H

#include <vector>
#include <boost/shared_ptr.hpp>

namespace dolfin
{

  class Function;
  class FunctionSpace;
  class Mesh;

  /// This class facilitate assignments between Function and sub
  /// Functions. It builds and caches maps between compatible
  /// dofs. These maps are used in the assignment methods which
  /// perform the actual assignment. Optionally can a MeshFunction be
  /// passed together with a label, facilitating FunctionAssignment
  /// over sub domains.
  class FunctionAssigner
  {
  public:

    /// Create a FunctionAssigner between two equally sized functions
    ///
    /// *Arguments*
    ///     receiving_space (_FunctionSpace_)
    ///         The function space of the receiving function
    ///     assigning_space (_FunctionSpace_)
    ///         The function space of the assigning function
    FunctionAssigner(boost::shared_ptr<const FunctionSpace> receiving_space,
		     boost::shared_ptr<const FunctionSpace> assigning_space);

    /// Create a FunctionAssigner between one mixed function
    /// (assigning) and several scalar or scalar sub functions
    /// (receiving). The number of receiving components need to sum up
    /// to the number of sub functions in the assigning mixed
    /// function.
    ///
    /// *Arguments*
    ///     receiving_spaces (std::vector<_FunctionSpace_>)
    ///         The recieving function spaces
    ///     assigning_space (_FunctionSpace_)
    ///         The assigning function space
    FunctionAssigner(std::vector<boost::shared_ptr<const FunctionSpace> > receiving_spaces,
		     boost::shared_ptr<const FunctionSpace> assigning_space);

    /// Create a FunctionAssigner between several scalar or scalar sub
    /// functions (assigning) and one mixed function (receiving). The
    /// number of assigning components need to sum up to the number of
    /// sub functions in the receiving mixed function.
    ///
    /// *Arguments*
    ///     receiving_space (boost::shared_ptr<_FunctionSpace_>)
    ///         The recieving function space
    ///     assigning_spaces (std::vector<boost::shared_ptr<_FunctionSpace_> >)
    ///         The assigning function spaces
    FunctionAssigner(boost::shared_ptr<const FunctionSpace> receiving_space,
		     std::vector<boost::shared_ptr<const FunctionSpace> > assigning_spaces);

    /// Assign one function to another
    ///
    /// *Arguments*
    ///     receiving_func (boost::shared_ptr<_Function_>)
    ///         The recieving function
    ///     assigning_func (boost::shared_ptr<_Function_>)
    ///         The assigning function
    void assign(boost::shared_ptr<Function> receiving_func,
		boost::shared_ptr<const Function> assigning_func) const;

    /// Assign several functions to sub functions of a mixed receiving function
    ///
    /// *Arguments*
    ///     receiving_func (boost::shared_ptr<_Function_>)
    ///         The recieving mixed function
    ///     assigning_funcs (std::vector<boost::shared_ptr<_Function_> >)
    ///         The assigning functions
    void assign(boost::shared_ptr<Function> receiving_func,
		std::vector<boost::shared_ptr<const Function> > assigning_funcs) const;

    /// Assign sub functions of a single mixed function to single
    /// receiving functions
    ///
    /// *Arguments*
    ///     receiving_funcs (std::vector<boost::shared_ptr<_Function_> >)
    ///         The recieving functions
    ///     assigning_func (boost::shared_ptr<_Function_>)
    ///         The assigning mixed function
    void assign(std::vector<boost::shared_ptr<Function> > receiving_funcs,
		boost::shared_ptr<const Function> assigning_func) const;

    /// Destructor
    ~FunctionAssigner();

    /// Return the number of assiging functions
    inline std::size_t num_assigning_functions() const
    { return _assigning_spaces.size(); }

    /// Return the number of receiving functions
    inline std::size_t num_receiving_functions() const
    { return _receiving_spaces.size(); }

  private:

    // Utility function to actually do the assignment
    void _assign(std::vector<boost::shared_ptr<Function> > receiving_funcs,
	std::vector<boost::shared_ptr<const Function> > assigning_funcs) const;

    // Check the compatability of the meshes and return a reference to
    // the mesh
    const Mesh& _get_mesh() const;

    // Check the compatability of the arguments to the constructor and
    // build indices for assignment
    void _check_and_build_indices(const Mesh& mesh,
	  const std::vector<boost::shared_ptr<const FunctionSpace> >& receiving_spaces,
	  const std::vector<boost::shared_ptr<const FunctionSpace> >& assigning_spaces);

    // Shared pointers to the original FunctionSpaces
    std::vector<boost::shared_ptr<const FunctionSpace> > _receiving_spaces;
    std::vector<boost::shared_ptr<const FunctionSpace> > _assigning_spaces;

    // Indices for accessing values to receiving Functions
    std::vector<std::vector<la_index> > _receiving_indices;

    // Indices for accessing values from assigning Functions
    std::vector<std::vector<la_index> > _assigning_indices;

    // Vector for value transfer between assigning and receiving Function
    mutable std::vector<std::vector<double> > _transfer;

  };
}

#endif
