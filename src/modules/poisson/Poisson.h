// This file was automatically generated by FFC, the FEniCS Form Compiler.
// Licensed under the GNU GPL Version 2.

#ifndef __POISSON_H
#define __POISSON_H

#include <dolfin/NewFiniteElement.h>
#include <dolfin/LinearForm.h>
#include <dolfin/BilinearForm.h>

namespace dolfin { namespace Poisson {

/// This is the finite element for which the form is generated,
/// providing the information neccessary to do assembly.

class FiniteElement : public dolfin::NewFiniteElement
{
public:

  FiniteElement() : dolfin::NewFiniteElement(), tensordims(0)
  {
    // Do nothing
  }

  ~FiniteElement()
  {
    if ( tensordims ) delete [] tensordims;
  }

  inline unsigned int spacedim() const
  {
    return 3;
  }

  inline unsigned int shapedim() const
  {
    return 2;
  }

  inline unsigned int tensordim(unsigned int i) const
  {
    dolfin_error("Element is scalar.");
    return 0;
  }

  inline unsigned int rank() const
  {
    return 0;
  }

  // FIXME: Only works for nodal basis
  inline unsigned int dof(unsigned int i, const Cell& cell, const Mesh& mesh) const
  {
    return cell.nodeID(i);
  }

  // FIXME: Only works for nodal basis
  inline const Point coord(unsigned int i, const Cell& cell, const Mesh& mesh) const
  {
    Point p;
    return p;
  }

private:

  unsigned int* tensordims;

};

/// This class contains the form to be evaluated, including
/// contributions from the interior and boundary of the domain.

class BilinearForm : public dolfin::BilinearForm
{
public:

  BilinearForm() : dolfin::BilinearForm()
  {
  }

  bool interior(real* block) const
  {
    // Compute geometry tensors
    real G0_00 = det*(g00*g00 + g01*g01);
    real G0_01 = det*(g00*g10 + g01*g11);
    real G0_10 = det*(g10*g00 + g11*g01);
    real G0_11 = det*(g10*g10 + g11*g11);

    // Compute element tensor
    block[0] = 0.5*G0_00 + 0.5*G0_01 + 0.5*G0_10 + 0.5*G0_11;
    block[1] = -0.5*G0_00 - 0.5*G0_10;
    block[2] = -0.5*G0_01 - 0.5*G0_11;
    block[3] = -0.5*G0_00 - 0.5*G0_01;
    block[4] = 0.5*G0_00;
    block[5] = 0.5*G0_01;
    block[6] = -0.5*G0_10 - 0.5*G0_11;
    block[7] = 0.5*G0_10;
    block[8] = 0.5*G0_11;

    return true;
  }

};

/// This class contains the form to be evaluated, including
/// contributions from the interior and boundary of the domain.

class LinearForm : public dolfin::LinearForm
{
public:

  LinearForm(const NewFunction& w0) : dolfin::LinearForm()
  {
    // Add functions
    init(1, 3);
    add(w0);
  }

  bool interior(real* block) const
  {
    // Compute geometry tensors
    real G0_0 = det*c[0][0];
    real G0_1 = det*c[0][1];
    real G0_2 = det*c[0][2];

    // Compute element tensor
    block[0] = 0.0833333333333*G0_0 + 0.0416666666667*G0_1 + 0.0416666666667*G0_2;
    block[1] = 0.0416666666667*G0_0 + 0.0833333333333*G0_1 + 0.0416666666667*G0_2;
    block[2] = 0.0416666666667*G0_0 + 0.0416666666667*G0_1 + 0.0833333333333*G0_2;

    return true;
  }

};


} }

#endif
