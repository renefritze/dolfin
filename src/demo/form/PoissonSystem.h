// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __POISSONSYSTEM_H
#define __POISSONSYSTEM_H

#include <dolfin/NewPDE.h>

namespace dolfin
{

  /// EXPERIMENTAL: Redesign of the evaluation of variational forms
  
  class PoissonSystem : public NewPDE
  {
  public:

    PoissonSystem(Function& f) : NewPDE(8, true, false), w0(8)
    {
      // Add functions
      add(w0, f);

      // Set nonzero pattern (add the rest of the entries)
      nonzero.push_back(IndexPair(0,0));
    }

    unsigned int dim() const
    {
      return 3;
    }

    unsigned int dof(unsigned int i, const Cell& cell) const
    {
      // Something needs to be done here for systems

      return cell.nodeID(i);
    }
    
    void interiorElementMatrix(NewArray< NewArray<real> >& A) const
    {
      real tmp0 = det / 6.0;
      
      real G00 = tmp0*(g00*g00 + g01*g01 + g02*g02);
      real G01 = tmp0*(g00*g10 + g01*g11 + g02*g12);
      real G02 = tmp0*(g00*g20 + g01*g21 + g02*g22);
      real G11 = tmp0*(g10*g10 + g11*g11 + g12*g12);
      real G12 = tmp0*(g10*g20 + g11*g21 + g12*g22);
      real G22 = tmp0*(g20*g20 + g21*g21 + g22*g22);
      
      A[1][1] = G00;
      A[1][2] = G01;
      A[1][3] = G02;
      A[2][2] = G11;
      A[2][3] = G12;
      A[3][3] = G22;
      A[0][1] = - A[1][1] - A[1][2] - A[1][3];
      A[0][2] = - A[1][2] - A[2][2] - A[2][3];
      A[0][3] = - A[1][3] - A[2][3] - A[3][3];
      A[0][0] = - A[0][1] - A[0][2] - A[0][3];
      A[1][0] = A[0][1];
      A[2][0] = A[0][2];
      A[2][1] = A[1][2];
      A[3][0] = A[0][3];
      A[3][1] = A[1][3];
      A[3][2] = A[2][3];

      A[4][4] = A[0][0];
      A[4][5] = A[0][1];
      A[4][6] = A[0][2];
      A[4][7] = A[0][3];
      A[5][4] = A[1][0];
      A[5][5] = A[1][1];
      A[5][6] = A[1][2];
      A[5][7] = A[1][3];
      A[6][4] = A[2][0];
      A[6][5] = A[2][1];
      A[6][6] = A[2][2];
      A[6][7] = A[2][3];
      A[7][4] = A[3][0];
      A[7][5] = A[3][1];
      A[7][6] = A[3][2];
      A[7][7] = A[3][3];
    }
    
    void interiorElementVector(NewArray<real>& b) const
    {
      real tmp0 = det / 120.0;

      real G0 = tmp0*w0[0];
      real G1 = tmp0*w0[1];
      real G2 = tmp0*w0[2];
      real G3 = tmp0*w0[3];
      real G4 = tmp0*w0[0];
      real G5 = tmp0*w0[1];
      real G6 = tmp0*w0[2];
      real G7 = tmp0*w0[3];
      
      real tmp1 = G0 + G1 + G2 + G3;

      b[0] = tmp1 + G0;
      b[1] = tmp1 + G1;
      b[2] = tmp1 + G2;
      b[3] = tmp1 + G3;

      real tmp2 = G4 + G5 + G6 + G7;

      b[4] = tmp2 + G4;
      b[5] = tmp2 + G5;
      b[6] = tmp2 + G6;
      b[7] = tmp2 + G7;
    }

  private:

    NewArray<real> w0;
    
  };
  
}

#endif
