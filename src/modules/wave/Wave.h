// Copyright (C) 2003 Johan Jansson.
// Licensed under the GNU GPL Version 2.

#ifndef __WAVE_H
#define __WAVE_H

#include <dolfin/PDE.h>

namespace dolfin {
  
  class Wave : public PDE {
  public:
    
    Wave(Function& source,
	 Function& uprevious,
	 Function& wprevious) : PDE(2)
    {
      add(f,  source);
      add(up, uprevious);
      add(wp, wprevious);
    }
    
    real lhs(const ShapeFunction& u, const ShapeFunction& v)
    {
      /*
      // dG(0) in time

      return (1 / k * u*v + k *
	      (u.ddx() * v.ddx() + u.ddy() * v.ddy())) * dx;
      //*/

      ///*
      // cG(1) in time
      
      return
        (2 / k * u * v + k / 2 * (u.ddx() * v.ddx() + u.ddy() * v.ddy())) * dx;
      //*/
    }
    
    real rhs(const ShapeFunction& v)
    {
      /*
      // dG(0) in time

      return (k * f * v + wp * v + 1 / k * up * v) * dx;
      */

      ///*
      // cG(1) in time
      
      return
	(k * f * v +
	 2 / k * up * v -
	 k / 2 * (up.ddx() * v.ddx() + up.ddy() * v.ddy()) +
	 2 * wp * v) * dx;
      //*/
    }
    
  private:    

    ElementFunction f;  // Source term
    ElementFunction up; // Position value at left end-point
    ElementFunction wp; // Velocity value at left end-point

  };

}

#endif
