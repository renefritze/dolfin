// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-01-17
// Last changed: 2008-10-26

#ifndef __UFC_DATA_H
#define __UFC_DATA_H

#include <vector>
#include <ufc.h>
#include "UFCMesh.h"
#include "UFCCell.h"

namespace dolfin
{

  class FiniteElement;
  class FunctionSpace;
  class Mesh;
  class Cell;

  /// This class is a simple data structure that holds data used
  /// during assembly of a given UFC form. Data is created for each
  /// primary argument, that is, v_j for j < r. In addition, nodal
  /// basis expansion coefficients and a finite element are created
  /// for each coefficient function.

  class UFC
  {
  public:

    /// Constructor
    UFC(const ufc::form& form, const std::vector<const FunctionSpace*>& V);

    /// Destructor
    ~UFC();
    
    /// Update current cell
    void update(const Cell& cell);

    /// Update current pair of cells for macro element
    void update(const Cell& cell0, const Cell& cell1);

    // Array of finite elements for primary arguments
    FiniteElement** finite_elements;

    // Array of finite elements for coefficients
    FiniteElement** coefficient_elements;

    // Array of cell integrals
    ufc::cell_integral** cell_integrals;

    // Array of exterior facet integrals
    ufc::exterior_facet_integral** exterior_facet_integrals;

    // Array of interior facet integrals
    ufc::interior_facet_integral** interior_facet_integrals;

    // Form
    const ufc::form& form;

    // Mesh
    UFCMesh mesh;
    
    // Current cell
    UFCCell cell;

    // Current pair of cells of macro element
    UFCCell cell0;
    UFCCell cell1;

    // Local tensor
    double* A;

    // Local tensor for macro element
    double* macro_A;

    // Array of local dimensions for each argument
    uint* local_dimensions;

    // Array of local dimensions of macro element for primary arguments
    uint* macro_local_dimensions;

    // Array of global dimensions for primary arguments
    uint* global_dimensions;
    
    // Array of mapped dofs for primary arguments
    uint** dofs;

    // Array of mapped dofs of macro element for primary arguments
    uint** macro_dofs;

    // Array of coefficients
    double** w;

    // Array of coefficients on macro element
    double** macro_w;

  };

}

#endif
