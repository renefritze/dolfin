// Copyright (C) 2002 [Insert name]
// Licensed under the GNU GPL Version 2.

#ifndef __TEMPLATE_H
#define __TEMPLATE_H

#include <dolfin/PDE.h>

namespace dolfin {

  class Template : public PDE {
  public:
	 
	 Template() : PDE(3) {}
	 
	 real lhs(ShapeFunction &u, ShapeFunction &v) {
		return 1.0;
	 }
	 
	 real rhs(ShapeFunction &v) {
		return 1.0;
	 }
	 
  private:
	 
  };

}
  
#endif
  
