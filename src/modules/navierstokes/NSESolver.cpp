// Copyright (C) 2005 Johan Hoffman.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells 2005.
// Modified by Anders Logg 2005.
//
// First added:  2005
// Last changed: 2005-12-21

#include <dolfin/timing.h>
#include <dolfin/NSEMomentum.h>
#include <dolfin/NSEContinuity.h>
#include <dolfin/NSESolver.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
NSESolver::NSESolver(Mesh& mesh, Function& f, BoundaryCondition& bc_mom, 
		     BoundaryCondition& bc_con)
  : mesh(mesh), f(f), bc_mom(bc_mom), bc_con(bc_con) 
{
  // Declare parameters
  add("velocity file name", "velocity.pvd");
  add("pressure file name", "pressure.pvd");
}
//-----------------------------------------------------------------------------
void NSESolver::solve()
{
  real T0 = 0.0;   // start time 
  real t  = 0.0;   // current time
  real k  = 0.01;   // time step
  real T  = 8.0;   // final time
  real nu = 0.001; // viscosity 

  // Set time step
  real hmin;
  GetMinimumCellSize(mesh, hmin);  
  k = 0.25*hmin;

  // Create matrices and vectors 
  Matrix Amom, Acon;
  //  Vector x0vel, xvel, xcvel, xpre, bmom, bcon;
  Vector bmom, bcon;

  int nsd = 3;

  // Initialize velocity;
  Vector x0vel(nsd*mesh.noVertices());
  Vector xcvel(nsd*mesh.noVertices());
  //Vector xmvel(nsd*mesh.noCells());
  Vector xvel(nsd*mesh.noVertices());
  x0vel = 0.0;
  xcvel = 0.0;
  //xmvel = 0.0;
  xvel = 0.0;

  Vector xpre(mesh.noVertices());
  xpre = 0.0;

  Vector residual_mom(nsd*mesh.noVertices());
  Vector residual_con(mesh.noVertices());
  residual_mom = 1.0e3;
  residual_con = 1.0e3;

  GMRES solver_con;
  GMRES solver_mom;

  KSP ksp_con = solver_con.solver();
  PC pc;
  KSPGetPC(ksp_con,&pc);

  // Check that PETSc was compiled with HYPRE
  #ifdef PETSC_HAVE_HYPRE
    PCSetType(pc,PCHYPRE);
    PCHYPRESetType(pc,"boomeramg");
  #else
    dolfin_warning("PETSc has not been compiled with the HYPRE library for "
                   "algerbraic multigrid. Navier Stokes module will use the "
                   "GMRES solver for the continuity equation. For performance " 
                   "installation of HYPRE is recommended. See the DOLFIN user " 
                   "manual. ");
  #endif
    
  /*
  solver_con.setRtol(1.0e-10);
  solver_con.setAtol(1.0e-10);

  solver_mom.setRtol(1.0e-10);
  solver_mom.setAtol(1.0e-10);
  */

  cout << "Create functions" << endl;
  
  // Create functions
  //  Function u(xvel, mesh);   // Velocity
  Function u0(x0vel, mesh); // Velocity from previous time step 
  Function uc(xcvel, mesh); // Velocity linearized convection 
  //  Function um(xmvel, mesh); // Cell mean velocity  
  Function p(xpre, mesh);   // Pressure

  
  
  // vectors for functions for stabilization 
  Vector d1vector, d2vector;
  Function delta1(d1vector), delta2(d2vector);

  cout << "Create bilinear form: momentum" << endl;

  // Define the bilinear and linear forms
  NSEMomentum::BilinearForm amom(uc,delta1,delta2,k,nu);
  NSEMomentum::LinearForm Lmom(uc,u0,f,p,delta1,delta2,k,nu);

  cout << "Create bilinear form: continuity" << endl;

  NSEContinuity::BilinearForm acon(delta1);
  NSEContinuity::LinearForm Lcon(uc,f,delta1);

  cout << "Create file" << endl;

  Function u(xvel, mesh, uc.element());   // Velocity

  // Synchronise functions and boundary conditions with time
  u.sync(t);
  p.sync(t);
  bc_con.sync(t);
  bc_mom.sync(t);
    
  File file_u(get("velocity file name")); // file for saving velocity 
  File file_p(get("pressure file name"));  // file for saving pressure

  cout << "Assemble form: continuity" << endl;

  ComputeStabilization(mesh,u0,nu,k,d1vector,d2vector);

  // Discretize Continuity equation 
  FEM::assemble(acon, Acon, mesh);

  int time_step = 0;
  int sample = 0;
  int no_samples = 10;

  // Start time-stepping
  Progress prog("Time-stepping");
  while (t<T) 
  {

    time_step++;
    cout << "Starting time step " << time_step << endl;
    
    x0vel = xvel;

    ComputeStabilization(mesh,u0,nu,k,d1vector,d2vector);

    residual_mom = 1.0e3;
    residual_con = 1.0e3;

    //for (int i=0;i<10;i++){
    while (sqrt(sqr(residual_mom.norm()) + sqr(residual_con.norm())) > 1.0e-2){

      cout << "Assemble form: continuity" << endl;

      // Discretize Continuity equation 
      FEM::assemble(Lcon, bcon, mesh);

      cout << "Set boundary conditions: continuity" << endl;

      // Set boundary conditions
      FEM::applyBC(Acon, bcon, mesh, acon.trial(), bc_con);

      cout << "Solve linear system" << endl;

      // Solve the linear system
      tic();
      solver_con.solve(Acon, xpre, bcon);
      cout << "Linear solve took: " << toc() << " seconds" << endl; 

      cout << "Assemble form: momentum" << endl;      

      // Discretize Momentum equations
      tic();
      FEM::assemble(amom, Lmom, Amom, bmom, mesh, bc_mom);
      cout << "Assembly took: " << toc() << " seconds" << endl; 

      cout << "Set boundary conditions: momentum" << endl;

      // Set boundary conditions
      FEM::applyBC(Amom, bmom, mesh, amom.trial(),bc_mom);

      cout << "Solve linear system" << endl;

      // Solve the linear system
      tic();
      solver_mom.solve(Amom, xvel, bmom);
      cout << "Linear solve took: " << toc() << " seconds" << endl; 
      
      xcvel = xvel;
      //ComputeMeanVelocity(xcvel,xmvel);
    
      FEM::assemble(amom, Lmom, Amom, bmom, mesh, bc_mom);
      FEM::applyBC(Amom, bmom, mesh, amom.trial(),bc_mom);
      Amom.mult(xvel,residual_mom);
      residual_mom -= bmom;
      
      FEM::assemble(Lcon, bcon, mesh);
      FEM::applyBC(Acon, bcon, mesh, acon.trial(),bc_con);
      Acon.mult(xpre,residual_con);
      residual_con -= bcon;
      
      cout << "Momentum residual  : l2 norm = " << residual_mom.norm() << endl;
      cout << "Continuity residual: l2 norm = " << residual_con.norm() << endl;
      cout << "Total NSE residual : l2 norm = " << sqrt(sqr(residual_mom.norm()) + sqr(residual_con.norm())) << endl;

      file_p << p;
      file_u << u;

    }

    //cout << "Save solution" << endl;

    // Save the solution
    if ( (time_step == 1) || (t > (T-T0)*(real(sample)/real(no_samples))) ){
      file_p << p;
      file_u << u;
      sample++;
    }

    t = t + k;

    // Update progress
    prog = t / T;
  }

    cout << "Save solution" << endl;

    /*
    // Save the solution
    file_p << p;
    file_u << u;
    */

}
//-----------------------------------------------------------------------------
void NSESolver::solve(Mesh& mesh, Function& f, BoundaryCondition& bc_mom, 
		      BoundaryCondition& bc_con)
{
  NSESolver solver(mesh, f, bc_mom, bc_con);
  solver.solve();
}
//-----------------------------------------------------------------------------
void NSESolver::ComputeCellSize(Mesh& mesh, Vector& hvector)
{
  // Compute cell size h
  hvector.init(mesh.noCells());	
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      hvector((*cell).id()) = (*cell).diameter();
    }
}
//-----------------------------------------------------------------------------
void NSESolver::GetMinimumCellSize(Mesh& mesh, real& hmin)
{
  // Get minimum cell diameter
  hmin = 1.0e6;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      if ((*cell).diameter() < hmin) hmin = (*cell).diameter();
    }
}
//-----------------------------------------------------------------------------
void NSESolver::ComputeStabilization(Mesh& mesh, Function& w, real nu, real k, 
				     Vector& d1vector, Vector& d2vector)
{
  // Stabilization parameters 
  real C1 = 4.0;   
  real C2 = 2.0;   

  d1vector.init(mesh.noCells());	
  d2vector.init(mesh.noCells());	

  real normw; 

  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    //normw = sqrt(sqr(w((*cell).midpoint(),0)) + sqr(w((*cell).midpoint(),1)));
    normw = 0.0;
    for (VertexIterator n(cell); !n.end(); ++n)
      normw += sqrt( sqr((w.vector())((*n).id()*2)) + sqr((w.vector())((*n).id()*2+1)) );
    normw /= (*cell).noVertices();
    if ( (((*cell).diameter()/nu) > 1.0) || (nu < 1.0e-10) ){
      d1vector((*cell).id()) = C1 * (0.5 / sqrt( 1.0/sqr(k) + sqr(normw/(*cell).diameter()) ) );
      d2vector((*cell).id()) = C2 * (*cell).diameter();
    } else{
      d1vector((*cell).id()) = C1 * sqr((*cell).diameter());
      d2vector((*cell).id()) = C2 * sqr((*cell).diameter());
    }	
  }
}
//-----------------------------------------------------------------------------
void NSESolver::ComputeMeanVelocity(Vector& xnodal, Vector& xcell)
{
  // Compute cell mean 
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    xcell((*cell).id()) = 0.0;
    for (VertexIterator vertex(cell); !vertex.end(); ++vertex)
      xcell((*cell).id()) += xnodal((*vertex).id());
    xcell((*cell).id()) = xcell((*cell).id()) / real(cell->noVertices());
  }
}
//-----------------------------------------------------------------------------
