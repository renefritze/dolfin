// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __DOF_FUNCTION_H
#define __DOF_FUNCTION_H

#include <dolfin/ElementFunction.h>
#include <dolfin/GenericFunction.h>

namespace dolfin {

  class Cell;
  class FiniteElement;
  class Vector;
  
  class DofFunction : public GenericFunction {
  public:

	 DofFunction(Vector& dofs);
	 
	 // Update values of element function
	 void update(FunctionSpace::ElementFunction &v,
					 const FiniteElement &element,
					 const Cell &cell,
					 real t) const;

	 // Evaluation of function
	 real operator() (const Node&  n, real t) const;
	 real operator() (const Point& p, real t) const;

  private:

	 Vector& x;
	 
  };

}

#endif
