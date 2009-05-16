// Copyright (C) 2008 Evan Lezar (evanlezar@gmail.com).
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-08-22
// Last changed: 2008-12-17
//
// Modified by Anders Logg, 2008.
//
// This demo demonstrates the calculation of a TM (Transverse Magnetic)
// cutoff wavenumber of a rectangular waveguide with dimensions 1x0.5m.
//
// For more information regarding waveguides, see
//
//   http://www.ee.bilkent.edu.tr/~microwave/programs/magnetic/rect/info.htm
//
// See the pdf in the parent folder and the following reference:
//
//   The Finite Element in Electromagnetics (2nd Ed)
//   Jianming Jin [7.2.1 - 7.2.2]

#include <dolfin.h>
#include "Forms.h"

using namespace dolfin;

#if defined(HAS_PETSC) && defined(HAS_SLEPC)

int main()
{
  // Create mesh
  double width = 1.0;
  double height = 0.5;
  Rectangle mesh(0, 0, width, height, 4, 2);

  // Define the forms - gererates an generalized eigenproblem of the form
  // [S]{h} = k_o^2[T]{h}
  // with the eigenvalues k_o^2 representing the square of the cutoff wavenumber
  // and the corresponding right-eigenvector giving the coefficients of the
  // discrete system used to obtain the approximate field anywhere in the domain
  Forms::FunctionSpace V(mesh);
  Forms::Form_0 s(V, V);
  Forms::Form_1 t(V, V);

  // Assemble the system matrices stiffness (S) and mass matrices (T)
  PETScMatrix S;
  PETScMatrix T;
  assemble(S, s);
  assemble(T, t);

  // Solve the eigen system
  SLEPcEigenSolver esolver;
  esolver.set("eigenvalue spectrum", "smallest real");
  esolver.set("eigenvalue solver", "lapack");
  esolver.solve(S, T);

  // The result should have real eigenvalues but due to rounding errors, some of
  // the resultant eigenvalues may be small complex values.
  // only consider the real part

  // Now, the system contains a number of zero eigenvalues (near zero due to
  // rounding) which are eigenvalues corresponding to the null-space of the curl
  // operator and are a mathematical construct and do not represent physically
  // realizable modes.  These are called spurious modes.
  // So, we need to identify the smallest, non-zero eigenvalue of the system -
  // which corresponds with cutoff wavenumber of the the dominant cutoff mode.
  double cutoff = -1.0;
  double lr, lc;
  for (unsigned int i = 0; i < S.size(1); i++)
  {
    esolver.get_eigenvalue(lr, lc, i);
    if (lr > 1 && lc == 0)
    {
      cutoff = sqrt(lr);
      break;
    }
  }

  if (cutoff == -1.0)
    info("Unable to find dominant mode.");
  else
    info("Cutoff frequency = %g", cutoff);

  return 0;
}

#else

int main()
{
  info("Sorry, this demo is only available when DOLFIN is compiled with PETSc and SLEPc.");
  return 0;
}

#endif
