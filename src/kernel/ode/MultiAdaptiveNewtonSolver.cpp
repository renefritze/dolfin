// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-27
// Last changed: 2006-05-07

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_math.h>
#include <dolfin/ParameterSystem.h>
#include <dolfin/uBlasSparseMatrix.h>
#include <dolfin/Alloc.h>
#include <dolfin/ODE.h>
#include <dolfin/Method.h>
#include <dolfin/MultiAdaptiveTimeSlab.h>
#include <dolfin/MultiAdaptiveJacobian.h>
#include <dolfin/UpdatedMultiAdaptiveJacobian.h>
#include <dolfin/MultiAdaptiveNewtonSolver.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
MultiAdaptiveNewtonSolver::MultiAdaptiveNewtonSolver
(MultiAdaptiveTimeSlab& timeslab)
  : TimeSlabSolver(timeslab), ts(timeslab), A(0),
    mpc(timeslab, method), f(0), u(0), num_elements(0), num_elements_mono(0),
    updated_jacobian(get("ODE updated jacobian"))
{
  // Initialize local arrays
  f = new real[method.qsize()];
  u = new real[method.nsize()];
  
  // Don't report number of GMRES iteration if not asked to
  solver.set("Krylov report", monitor);
  solver.set("Krylov absolute tolerance", 0.01);
  solver.set("Krylov relative tolerance", 0.01*tol);
  
  // Initialize Jacobian
  if ( updated_jacobian )
    A = new UpdatedMultiAdaptiveJacobian(*this, timeslab);
  else
    A = new MultiAdaptiveJacobian(*this, timeslab);
}
//-----------------------------------------------------------------------------
MultiAdaptiveNewtonSolver::~MultiAdaptiveNewtonSolver()
{
  // Compute multi-adaptive efficiency index
  if ( num_elements > 0 )
  {
    const real alpha = num_elements_mono / static_cast<real>(num_elements);
    dolfin_info("Multi-adaptive efficiency index: %.3f", alpha);
  }
  
  // Delete local arrays
  if ( f ) delete [] f;
  if ( u ) delete [] u;

  // Delete Jacobian
  delete A;
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
  
  // Recompute Jacobian on each time slab
  A->update();

  //debug();
  //A->disp(true, 10);
}
//-----------------------------------------------------------------------------
real MultiAdaptiveNewtonSolver::iteration(uint iter, real tol)
{
  // Evaluate b = -F(x) at current x
  Feval(b);
 
  // FIXME: Scaling needed for PETSc Krylov solver, but maybe not for uBlas?
  
  // Solve linear system
  const real r = b.norm(uBlasVector::linf) + DOLFIN_EPS;
  b /= r;
  num_local_iterations += solver.solve(*A, dx, b, mpc);
  dx *= r;

  // Update solution x -> x + dx (note: b = -F)
  for (uint j = 0; j < ts.nj; j++)
    ts.jx[j] += dx(j);

  // Compute maximum increment
  real max_increment = 0.0;
  for (uint j = 0; j < ts.nj; j++)
  {
    const real increment = fabs(dx(j));
    if ( increment > max_increment )
      max_increment = increment;
  }

  return max_increment;
}
//-----------------------------------------------------------------------------
dolfin::uint MultiAdaptiveNewtonSolver::size() const
{
  return ts.nj;
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::Feval(uBlasVector& F)
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

    // Update values on element using fixed-point iteration
    method.update(x0, f, k, u);
    
    // Subtract current values
    for (uint n = 0; n < method.nsize(); n++)
      F(j + n) = u[j] - ts.jx[j + n];

    // Update dof
    j += method.nsize();
  }
}
//-----------------------------------------------------------------------------
void MultiAdaptiveNewtonSolver::debug()
{
  const uint n = ts.nj;
  uBlasSparseMatrix B(n, n);
  uBlasVector F1(n), F2(n);

  // Iterate over the columns of B
  for (uint j = 0; j < n; j++)
  {
    const real xj = ts.jx[j];
    real dx = std::max(DOLFIN_SQRT_EPS, DOLFIN_SQRT_EPS * std::abs(xj));
		  
    ts.jx[j] -= 0.5*dx;
    Feval(F1);

    ts.jx[j] = xj + 0.5*dx;
    Feval(F2);

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
