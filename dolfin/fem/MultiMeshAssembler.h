// Copyright (C) 2013-2014 Anders Logg
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
// First added:  2013-09-12
// Last changed: 2014-05-22

#ifndef __MultiMesh_ASSEMBLER_H
#define __MultiMesh_ASSEMBLER_H

#include "AssemblerBase.h"
#include "Assembler.h"

namespace dolfin
{

  // Forward declarations
  class GenericTensor;
  class MultiMeshForm;

  /// This class implements functionality for finite element assembly
  /// over cut and composite finite element (MultiMesh) function spaces.

  class MultiMeshAssembler : public AssemblerBase
  {
  public:

    /// Constructor
    MultiMeshAssembler();

    /// Assemble tensor from given form
    ///
    /// *Arguments*
    ///     A (_GenericTensor_)
    ///         The tensor to assemble.
    ///     a (_Form_)
    ///         The form to assemble the tensor from.
    void assemble(GenericTensor& A, const MultiMeshForm& a);

  private:

    // Assemble over uncut cells
    void assemble_uncut_cells(GenericTensor& A, const MultiMeshForm& a);

    // Assemble over cut cells
    void assemble_cut_cells(GenericTensor& A, const MultiMeshForm& a);

    // Assemble over interface
    void assemble_interface(GenericTensor& A, const MultiMeshForm& a);

    // Initialize global tensor
    void init_global_tensor(GenericTensor& A, const MultiMeshForm& a);

  };

}

#endif
