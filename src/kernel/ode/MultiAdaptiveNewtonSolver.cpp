// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-27
// Last changed: 2005-10-24

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/Alloc.h>
#include <dolfin/ODE.h>
#include <dolfin/Matrix.h>
#include <dolfin/Method.h>
#include <dolfin/MultiAdaptiveTimeSlab.h>
#include <dolfin/MultiAdaptiveNewtonSolver.h>

#include <petscpc.h>
#include <src/ksp/pc/pcimpl.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
MultiAdaptiveNewtonSolver::MultiAdaptiveNewtonSolver
(MultiAdaptiveTimeSlab& timeslab)
  : TimeSlabSolver(timeslab), ts(timeslab),
    A(*this, timeslab), B(*this, timeslab),
    f(0), mpc(A)
{
  // Initialize local array
  f = new real[method.qsize()];
  
  // Don't report number of GMRES iteration if not asked to
  if ( !monitor )
    solver.setReport(false);

  solver.setRtol(0.01);
  solver.setAtol(0.01*tol);

  use_new_jacobian = dolfin_get("use new jacobian");
  
  // Set preconditioner
  //solver.setPreconditioner(mpc);
}
//-----------------------------------------------------------------------------
MultiAdaptiveNewtonSolver::~MultiAdaptiveNewtonSolver()
{
  // Compute multi-adaptive efficiency index
  const real alpha = num_elements_mono / static_cast<real>(num_elements);
  dolfin_info("Multi-adaptive efficiency index: %.3f", alpha);
  
  // Delete local array
  if ( f ) delete [] f;
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::end()
{
  num_elements += ts.ne;
  num_elements_mono += ts.length() / ts.kmin * static_cast<real>(ts.ode.size());
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::start()
{
  // Get size of system
  int nj = static_cast<int>(ts.nj);

  // Initialize increment vector
  dx.init(nj);

  // Initialize right-hand side
  b.init(nj);

  if ( use_new_jacobian )
  {
    // Initialize Jacobian matrix
    B.init(dx, dx);
    
    // Recompute Jacobian on each time slab
    B.update();
  }
  else
  {
    // Initialize Jacobian matrix
    A.init(dx, dx);
    
    // Recompute Jacobian on each time slab
    A.update();
  }

  //debug();
  //A.disp(true, 10);
}
//-----------------------------------------------------------------------------
real MultiAdaptiveNewtonSolver::iteration(uint iter, real tol)
{
  // Evaluate b = -F(x) at current x
  real* bb = b.array(); // Assumes uniprocessor case
  Feval(bb);
  b.restore(bb);
  
  // Solve linear system, seems like we need to scale the right-hand
  // side to make it work with the PETSc GMRES solver
  
  if ( use_new_jacobian )
  {
    solver.solve(B, dx, b);
  }
  else
  {
    //const real r = b.norm(Vector::linf) + DOLFIN_EPS;
    //b /= r;
    solver.solve(A, dx, b);
    //dx *= r;
  }

  //cout << "A = "; A.disp(true, 10);
  //cout << "dx = "; dx.disp();
  //cout << "b = "; b.disp();
   
  // Get array containing the increments (assumes uniprocessor case)
  real* dxx = dx.array();

  // Update solution x -> x + dx
  for (uint j = 0; j < ts.nj; j++)
    ts.jx[j] += dxx[j];

  // Compute maximum increment
  real max_increment = 0.0;
  for (uint j = 0; j < ts.nj; j++)
  {
    const real increment = fabs(dxx[j]);
    if ( increment > max_increment )
      max_increment = increment;
  }

  // Restore array
  dx.restore(dxx);

  return max_increment;
}
//-----------------------------------------------------------------------------
dolfin::uint MultiAdaptiveNewtonSolver::size() const
{
  return ts.nj;
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::Feval(real F[])
{
  // Reset dof
  uint j = 0;

  // Reset current sub slab
  int s = -1;

  // Reset elast
  for (uint i = 0; i < ts.N; i++)
    ts.elast[i] = -1;

  // Iterate over all elements
  for (uint e = 0; e < ts.ne; e++)
  {
    // Cover all elements in current sub slab
    s = ts.coverNext(s, e);

    // Get element data
    const uint i = ts.ei[e];
    const real a = ts.sa[s];
    const real b = ts.sb[s];
    const real k = b - a;

    // Get initial value for element
    const int ep = ts.ee[e];
    const real x0 = ( ep != -1 ? ts.jx[ep*method.nsize() + method.nsize() - 1] : ts.u0[i] );

    // Evaluate right-hand side at quadrature points of element
    if ( method.type() == Method::cG )
      ts.cGfeval(f, s, e, i, a, b, k);
    else
      ts.dGfeval(f, s, e, i, a, b, k);  
    //cout << "f = "; Alloc::disp(f, method.qsize());

    // Update values on element using fixed point iteration
    method.update(x0, f, k, F + j);
    
    // Subtract current values
    for (uint n = 0; n < method.nsize(); n++)
      F[j + n] -= ts.jx[j + n];

    // Update dof
    j += method.nsize();
  }
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::debug()
{
  const uint n = ts.nj;
  Matrix B(n, n);
  Vector F1(n), F2(n);

  // Iterate over the columns of B
  for (uint j = 0; j < n; j++)
  {
    const real xj = ts.jx[j];
    real dx = std::max(DOLFIN_SQRT_EPS, DOLFIN_SQRT_EPS * std::abs(xj));
		  
    ts.jx[j] -= 0.5*dx;
    real* F = b.array();
    Feval(F);
    b.restore(F);
    F1 = b; // Should be -b

    ts.jx[j] = xj + 0.5*dx;
    F = b.array();
    Feval(F);
    b.restore(F);
    F2 = b; // Should be -b

    ts.jx[j] = xj;

    for (uint i = 0; i < n; i++)
    {
      real dFdx = (F1(i) - F2(i)) / dx;
      if ( fabs(dFdx) > DOLFIN_EPS )
	B(i, j) = dFdx;
    }
  }

  B.disp();
}
//-----------------------------------------------------------------------------
