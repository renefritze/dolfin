// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __CGQ_ELEMENT_H
#define __CGQ_ELEMENT_H

#include <dolfin/constants.h>
#include <dolfin/Element.h>

namespace dolfin {

  class RHS;

  class cGqElement : public Element {
  public:
    
    cGqElement(unsigned int q, unsigned int index, real t0, real t1);
    ~cGqElement();

    Type type() const;

    unsigned int size() const;

    real value(real t) const;
    real value(unsigned int node, real t) const;
    real initval() const;
    real dx() const;

    void update(real u0);
    real update(RHS& f);
    real update(RHS& f, real* values);
    real update(RHS& f, real alpha);
    real update(RHS& f, real alpha, real* values);
    
    real updateLocalNewton(RHS& f);
    real updateLocalNewton(RHS& f, real* values);

    void set(real u0);
    void set(const real* const values);
    void get(real* const values) const;

    bool accept(real TOL, real r);

    real computeTimeStep(real TOL, real r, real kmax) const;
    real computeDiscreteResidual(RHS& f);
    real computeElementResidual(RHS& f);

  private:
    
    void feval(RHS& f);
    real integral(unsigned int i) const;
    
  };   
    
}

#endif
