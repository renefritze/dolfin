// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __POISSON_H
#define __POISSON_H

#include <dolfin/Equation.h>

namespace dolfin {

  class Poisson : public Equation {
  public:
	 
	 Poisson(Function &source) : Equation(3) {
		add(f, source);
	 }
	 
	 real lhs(const ShapeFunction &u, const ShapeFunction &v) {
		return 1.0;
	 }
	 
	 real rhs(const ShapeFunction &v) {
		return 1.0;
	 }
	 
  private:
	 
	 ElementFunction f;
	 
  };

}
  
#endif
