// Copyright (C) 2012 Fredrik Valdmanis
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
// Modified by Joachim B Haga 2012
// Modified by Benjamin Kehlet 2012
//
// First added:  2012-06-21
// Last changed: 2012-11-12

#ifdef HAS_VTK

#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <boost/static_assert.hpp>

#include <dolfin/mesh/MeshFunction.h>
#include "VTKPlottableMeshFunction.h"

using namespace dolfin;

//---------------------------------------------------------------------------
template <typename T> VTKPlottableMeshFunction<T>::VTKPlottableMeshFunction(
  std::shared_ptr<const MeshFunction<T>> mesh_function)
  : VTKPlottableMesh(mesh_function->mesh(), mesh_function->dim()),
    _mesh_function(mesh_function)
{
  // Do nothing
}
//----------------------------------------------------------------------------
template <typename T>
void VTKPlottableMeshFunction<T>::update(std::shared_ptr<const Variable> var,
                                         const Parameters& parameters,
                                         int frame_counter)
{
  if (var)
    _mesh_function = std::dynamic_pointer_cast<const MeshFunction<T>>(var);
  dolfin_assert(_mesh_function);

  VTKPlottableMesh::update(reference_to_no_delete_pointer(*_mesh_function->mesh()),
                           parameters, frame_counter);

  setCellValues(_mesh_function->size(), _mesh_function->values(), parameters);
}

//---------------------------------------------------------------------------
// Instantiate valid types for VTKPlottableMeshFunction
//---------------------------------------------------------------------------

template class dolfin::VTKPlottableMeshFunction<bool>;
template class dolfin::VTKPlottableMeshFunction<double>;
template class dolfin::VTKPlottableMeshFunction<float>;
template class dolfin::VTKPlottableMeshFunction<int>;
template class dolfin::VTKPlottableMeshFunction<std::size_t>;

#endif
