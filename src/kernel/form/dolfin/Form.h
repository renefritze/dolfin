// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-05-28
// Last changed: 2006-12-12

#ifndef __FORM_H
#define __FORM_H

#include <dolfin/constants.h>
#include <dolfin/Array.h>
#include <dolfin/Function.h>
#include <dolfin/AffineMap.h>
#include <dolfin/FiniteElement.h>
#include <dolfin/BLASFormData.h>

namespace dolfin
{
  
  class Form
  {
  public:

    /// Constructor
    Form(uint num_functions);

    /// Destructor
    virtual ~Form();

    /// Update map to current cell
    void update(Cell& cell, AffineMap& map);

    /// Update form to current cell for exterior facet
    void update(Cell& cell, AffineMap& map, uint facet);

    /// Update map to current pair of cells for interior facet
    void update(Cell& cell0, Cell& cell1, AffineMap& map0, AffineMap& map1, uint facet0, uint facet1);

    Function* function(uint i);
    FiniteElement* element(uint i);

    // Number of functions
    uint num_functions;

    /// Friends
    friend class FEM;

  protected:

    // Initialize function
    void add(Function& f, FiniteElement* element);

    // Initialize function
    void initFunction(uint i, Function& f, FiniteElement* element);

    // Update coefficients
    void updateCoefficients(Cell& cell, AffineMap& map);

    // Update coefficients
    void updateCoefficients(Cell& cell, AffineMap& map, uint facet);

    // Update coefficients
    void updateCoefficients(Cell& cell0, Cell& cell1, AffineMap& map0, AffineMap& map1, uint facet0, uint facet1);

    // Update local data for form
    virtual void updateLocalData() = 0;

    // List of finite elements for functions (coefficients)
    Array<FiniteElement*> elements;

    // List of functions (coefficients)
    Array<Function*> functions;
    
    // Coefficients of functions projected to current element
    real** c;

    // BLAS form data
    BLASFormData blas;
    
    // Block of values used during assembly
    real* block;

  };

}

#endif
