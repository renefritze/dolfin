// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-01-17
// Last changed: 2007-03-01

#ifndef __UFC_DATA_H
#define __UFC_DATA_H

#include <ufc.h>
#include <dolfin/UFCMesh.h>
#include <dolfin/UFCCell.h>

namespace dolfin
{

  class Mesh;
  class Cell;
  class DofMaps;

  /// This class is a simple data structure that holds data used
  /// during assembly of a given UFC form.

  class UFC
  {
  public:

    /// Constructor
    UFC(const ufc::form& form, Mesh& mesh, DofMaps& dof_maps);

    /// Destructor
    ~UFC();
    
    /// Update current cell
    inline void update(Cell& cell) { this->cell.update(cell); }

    // Number of arguments in form
    unsigned int num_arguments;

    // Array of finite elements for each argument
    ufc::finite_element** finite_elements;

    // Array of dof maps for each argument
    ufc::dof_map** dof_maps;

    // Cell integral
    ufc::cell_integral* cell_integral;

    // Exterior facet integral
    ufc::exterior_facet_integral* exterior_facet_integral;

    // Interior facet integral
    ufc::interior_facet_integral* interior_facet_integral;

    // Form
    const ufc::form& form;

    // Mesh
    UFCMesh mesh;
    
    // Current cell
    UFCCell cell;

    // Local tensor
    real* A;

    // Array of local dimensions for each argument
    uint* local_dims;

    // Array of global dimensions for each argument
    uint* global_dims;

    // Array of mapped dofs for each argument
    uint** dofs;

  };

}

#endif
