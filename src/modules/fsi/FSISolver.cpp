// Copyright (C) 2005 Johan Hoffman.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells 2005.
// Modified by Anders Logg 2005-2006.
//
// First added:  2005
// Last changed: 2006-08-08


#include <dolfin/timing.h>
#include <dolfin/FSISolver.h>
#include <dolfin/FSIMomentum3D.h>
#include <dolfin/FSIMomentum2D.h>
#include <dolfin/FSIContinuity3D.h>
#include <dolfin/FSIContinuity2D.h>
#include <dolfin/FSIDotSigma2D.h>
#include <dolfin/FSIDotSigma3D.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void FSISolver::ComputePhi(Vector& phi)
{
  phi.init(mesh.numCells());

  for (CellIterator cell(mesh); !cell.end(); ++cell) {
   
    phi(cell->index()) = 0;
    
    for (VertexIterator n(cell); !n.end(); ++n) {
      if ((phi(cell->index()) = bisect(n->point(), 0)) > 0) {
	phi(cell->index()) = 1;
	continue;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void FSISolver::ComputeAlpha(Vector& alpha, Vector& phi, real& nu)
{
  alpha.init(mesh.numCells());
  
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    alpha(cell->index()) = phi(cell->index())*2*nu + (1-phi(cell->index()))*k*E;
}
//-----------------------------------------------------------------------------
void FSISolver::ComputeDensity(Vector& rho, Vector& phi)
{
  rho.init(mesh.numCells());

  for (CellIterator cell(mesh); !cell.end(); ++cell)
    rho(cell->index()) = phi(cell->index())*rhof + (1-phi(cell->index()))*rhos;
}
//-----------------------------------------------------------------------------
FSISolver::FSISolver(Mesh& mesh, 
		     Function& f, 
		     ALEBoundaryCondition& bc_mom, 
		     ALEBoundaryCondition& bc_con, 
		     Function& bisect, 
		     real& rhof, 
		     real& rhos, 
		     real& E, 
		     real& elnu, 
		     real& k)
  : mesh(mesh), f(f), bc_mom(bc_mom), bc_con(bc_con), 
    bisect(bisect), rhof(rhof), rhos(rhos), E(E), elnu(elnu), k(k)
{
  // Declare parameters
  add("velocity file name", "velocity.pvd");
  add("pressure file name", "pressure.pvd");

  boundary = new BoundaryMesh(mesh, boundary_vertex_map, boundary_cell_map);
}
//-----------------------------------------------------------------------------
FSISolver::~FSISolver() 
{
  delete boundary;
}
//-----------------------------------------------------------------------------
void FSISolver::solve()
{
  real T0 = 0.0;        // start time 
  real t  = 0.0;        // current time
  real T  = 8.0;        // final time
  real nu = 1.0/3900.0; // viscosity 
 
  // Lamé variable - lambda is not required
  real mu = E / (2 * (1 + elnu));

  // Set time step (proportional to the minimum cell diameter) 
  real hmin;
  GetMinimumCellSize(mesh, hmin); 

  if (k == 0) k = 0.25*hmin;

  dolfin_info("FSIc: time step size k: %e", k);

  // Create matrices and vectors 
  Matrix Amom, Acon;
  Vector bmom, bcon;

  // Get the number of space dimensions of the problem 
  int nsd = mesh.topology().dim();
  dolfin_info("Number of space dimensions: %d",nsd);

  // Initialize vectors for velocity and pressure 
  Vector x0vel(nsd*mesh.numVertices()); // x0vel: velocity from previous time step 
  Vector xcvel(nsd*mesh.numVertices()); // xcvel: linearized velocity 
  Vector xvel(nsd*mesh.numVertices());  // xvel:  current velocity 
  Vector xpre(mesh.numVertices());      // pvel:  current pressure 
  x0vel = 0.0;
  xcvel = 0.0;
  xvel  = 0.0;
  xpre  = 0.0;

  // Initialize mesh velocity vector 
  Vector mvel_vec(nsd*mesh.numVertices());
  mvel_vec = 0.0;
  
  // Set initial velocity
  SetInitialVelocity(xvel);

  // Initialize vectors for the residuals of 
  // the momentum and continuity equations  
  Vector residual_mom(nsd*mesh.numVertices());
  Vector residual_con(mesh.numVertices());
  residual_mom = 1.0e3;
  residual_con = 1.0e3;

  // Initialize algebraic solvers 
  KrylovSolver solver_con(gmres, amg);
  KrylovSolver solver_mom(gmres);
 
  // Create functions for the velocity and pressure 
  // (needed for the initialization of the forms)
  Function u0(x0vel, mesh);   // velocity from previous time step 
  Function uc(xcvel, mesh);   // velocity linearized convection 
  Function p(xpre,   mesh);   // current pressure

  // Create function for mesh velocity
  Function w(mvel_vec, mesh);
  
  // Initialize stabilization parameters 
  Vector d1vector, d2vector;
  Function delta1(d1vector), delta2(d2vector);

  // Initialize the bilinear and linear forms
  BilinearForm* amom = 0;
  BilinearForm* acon = 0;
  LinearForm*   Lmom = 0;
  LinearForm*   Lcon = 0;

  // Define FSI variables
  Vector phi_vec;      // material marker
  Vector rho_vec;      // density of material 
  Vector alpha_vec;    // stress component
  Vector dsig;         // dot sigma, change of stress deviatoric

  // Define temporary variables
  Matrix A_tmp;        // temp matrix for dsig updating
  Vector m_tmp;        // temp vector for dsig updating

  // Create functions corresponding to FSI variables
  Function phi(phi_vec);
  Function rho(rho_vec);
  Function alpha(alpha_vec);

  // Initialization of variables
  ComputePhi(phi_vec);
  ComputeAlpha(alpha_vec, phi_vec, nu);
  ComputeDensity(rho_vec, phi_vec);

  real dummy = 1;      // required for Bilinear form, Bug??
  
  BilinearForm* adsig = 0;
  LinearForm*   Ldsig = 0;
  
  // Create forms for dotsigma updating
  if (nsd == 3) {
    
    adsig = new FSIDotSigma3D::BilinearForm(dummy);
    Ldsig = new FSIDotSigma3D::LinearForm(uc, mu);
    
  } else if (nsd == 2) {

    adsig = new FSIDotSigma2D::BilinearForm(dummy);
    Ldsig = new FSIDotSigma2D::LinearForm(uc, mu);
  
  } else {
    dolfin_error("FSI solver only implemented for 2 and 3 space dimensions.");
  }

  UpdateDotSigma(dsig, *adsig, *Ldsig, A_tmp, m_tmp);

  Vector   sigma_vec(dsig.size());
  Function sigma(sigma_vec);

  sigma_vec = 0.0;

  if (nsd == 3) {       // working in 3D

    amom = new FSIMomentum3D::BilinearForm(uc,delta1,delta2,w,phi,rho,alpha,k);
    Lmom = new FSIMomentum3D::LinearForm(uc,u0,f,p,delta1,delta2,w,phi,rho,sigma,k);
    acon = new FSIContinuity3D::BilinearForm(delta1);
    Lcon = new FSIContinuity3D::LinearForm(uc,f,delta1);   

  } else if (nsd == 2) {    // working in 2D   

    amom = new FSIMomentum2D::BilinearForm(uc,delta1,delta2,w,phi,rho,alpha,k);
    Lmom = new FSIMomentum2D::LinearForm(uc,u0,f,p,delta1,delta2,w,phi,rho,sigma,k);
    acon = new FSIContinuity2D::BilinearForm(delta1);
    Lcon = new FSIContinuity2D::LinearForm(uc,f,delta1);

  } else {
    dolfin_error("FSI solver only implemented for 2 and 3 space dimensions.");
  }

  // Create function for velocity 
  // (must be done after initialization of forms)
  Function u(xvel, mesh, uc.element());   // current velocity

  // Synchronise functions and boundary conditions with time
  u.sync(t);
  p.sync(t);
  bc_con.sync(t);
  bc_mom.sync(t);

  // Initialize output files 
  File file_u(get("velocity file name"));  // file for saving velocity 
  File file_p(get("pressure file name"));  // file for saving pressure

  // Compute stabilization parameters
  ComputeStabilization(mesh,u0,nu,k,d1vector,d2vector);

  dolfin_info("Assembling matrix: continuity");

  // Assembling matrices 
  FEM::assemble(*amom, Amom, mesh);
  FEM::assemble(*acon, Acon, mesh);

  // Initialize time-stepping parameters
  int time_step  = 0;
  int sample     = 0;
  int no_samples = 100;

  int iteration;
  int max_iteration = 50;

  // Residual, tolerance and maxmimum number of fixed point iterations
  real residual;
  real rtol = 1.0e-2;

  // Start time-stepping
  Progress prog("Time-stepping");
  
  cout << "FSIc: time \t itn \t mom_res \t con_res \t tot_res" << endl;
  
  while (t < T) 
  {		
    time_step++;
    dolfin_info("Time step %d",time_step);

    // Set current velocity to velocity at previous time step 
    x0vel = xvel;

    // Compute stabilization parameters
    ComputeStabilization(mesh,u0,nu,k,d1vector,d2vector);

    // Initialize residual 
    residual = 2*rtol;
    iteration = 0;
		
    // Assemble continuity matrix
    FEM::assemble(*acon, Acon, mesh);
		
    // Fix-point iteration for non-linear problem 
    while (residual > rtol && iteration < max_iteration) {
      
      dolfin_info("Assemble vector: continuity");

      // Assemble continuity vector 
      FEM::assemble(*Lcon, bcon, mesh);

      // Set boundary conditions for continuity equation 
      FEM::applyBC(Acon, bcon, mesh, acon->trial(), bc_con);

      // Stop recording boundary points for reference frame
      bc_con.endRecording();

      dolfin_info("Solve linear system: continuity");

      // Solve the linear system for the continuity equation 
      tic();
      solver_con.solve(Acon, xpre, bcon);
      dolfin_info("Linear solve took %g seconds",toc());

      dolfin_info("Assemble vector: momentum");

      // Assemble momentum matrix
      FEM::assemble(*amom, Amom, mesh);
     			
      // Assemble momentum vector 
      tic();
      FEM::assemble(*Lmom, bmom, mesh);
      dolfin_info("Assemble took %g seconds",toc());
			
      // Set boundary conditions for the momentum equation 
      FEM::applyBC(Amom, bmom, mesh, amom->trial(), bc_mom);

      // Stop recording boundary points for reference frame
      bc_mom.endRecording();
      
      dolfin_info("Solve linear system: momentum");
			
      // Solve the linear system for the momentum equation 
      tic();
      solver_mom.solve(Amom, xvel, bmom);
      dolfin_info("Linear solve took %g seconds",toc());
      
      // Set linearized velocity to current velocity 
      xcvel = xvel;
    
      dolfin_info("Assemble matrix and vector: momentum");
      FEM::assemble(*amom, *Lmom, Amom, bmom, mesh, bc_mom);
      FEM::applyBC(Amom, bmom, mesh, amom->trial(),bc_mom);

      // Compute residual for momentum equation 
      Amom.mult(xvel,residual_mom);
      residual_mom -= bmom;
      
      dolfin_info("Assemble vector: continuity");
      FEM::assemble(*Lcon, bcon, mesh);
      FEM::applyBC(Acon, bcon, mesh, acon->trial(),bc_con);

      // Compute residual for continuity equation 
      Acon.mult(xpre,residual_con);
      residual_con -= bcon;

      residual = sqrt(sqr(residual_mom.norm()) + sqr(residual_con.norm()));

      cout << "FSIc: " << t << " \t " << (iteration+1) << " \t " 
	   << residual_mom.norm() << " \t " 
	   << residual_con.norm() << " \t " 
	   << residual
	   << endl;
      
      dolfin_info("FSI: Momentum residual  : l2 norm = %e",residual_mom.norm());
      dolfin_info("FSI: continuity residual: l2 norm = %e",residual_con.norm());
      dolfin_info("FSI: Total FSI residual : l2 norm = %e",residual);

      dolfin_info("FSI: iterations completed: %d", (iteration+1));
      dolfin_info("FSI: time now: %g", t);

      iteration++;
    }

    // Move the structure and smooth the fluid mesh
    mvel_vec = 0.0;
    UpdateStructure(mvel_vec, phi_vec, uc);  // move the structure according to uc
    UpdateFluid(mvel_vec, phi_vec);          // smooth the fluid mesh
    mvel_vec /= k;
   
    if (residual > rtol)
      dolfin_warning("FSI fixed point iteration did not converge"); 
		
    if ( (time_step == 1) || (t > (T-T0)*(real(sample)/real(no_samples))) ) {
      dolfin_info("FSI: save solution to file");
      file_p << p;
      file_u << u;
      sample++;
    }
    
    // Increase time with timestep
    t += k;

    // Update stress deviatoric
    sigma_vec += k * dsig;
    UpdateDotSigma(dsig, *adsig, *Ldsig, A_tmp, m_tmp);
        
    // Update progress
    prog = t / T;    
  }

  dolfin_info("save solution to file");
  file_p << p;
  file_u << u;

  delete amom;
  delete Lmom;
  delete acon;
  delete Lcon;
  delete adsig;
  delete Ldsig;
}
//-----------------------------------------------------------------------------
void FSISolver::solve(Mesh& mesh, 
		      Function& f, 
		      ALEBoundaryCondition& bc_mom, 
		      ALEBoundaryCondition& bc_con, 
		      Function& bisect, 
		      real& rhof,
		      real& rhos, 
		      real& E, 
		      real& elnu, 
		      real& k)
{
  FSISolver solver(mesh, f, bc_mom, bc_con, bisect, rhof, rhos, E, elnu, k);
  solver.solve();
}
//-----------------------------------------------------------------------------
void FSISolver::ComputeCellSize(Mesh& mesh, Vector& hvector)
{
  // Compute cell size h
  hvector.init(mesh.numCells());	
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      hvector((*cell).index()) = (*cell).diameter();
    }
}
//-----------------------------------------------------------------------------
void FSISolver::GetMinimumCellSize(Mesh& mesh, real& hmin)
{
  // Get minimum cell diameter
  hmin = 1.0e6;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      if ((*cell).diameter() < hmin) hmin = (*cell).diameter();
    }
}
//-----------------------------------------------------------------------------
void FSISolver::ComputeStabilization(Mesh& mesh, Function& w, real nu, real k, 
				     Vector& d1vector, Vector& d2vector)
{
  // Compute least-squares stabilizing terms: 
  //
  // if  h/nu > 1 or ny < 10^-10
  //   d1 = C1 * ( 0.5 / sqrt( 1/k^2 + |U|^2/h^2 ) )   
  //   d2 = C2 * h 
  // else 
  //   d1 = C1 * h^2  
  //   d2 = C2 * h^2  

  real C1 = 4.0;   
  real C2 = 2.0;   

  d1vector.init(mesh.numCells());	
  d2vector.init(mesh.numCells());	

  real normw; 

  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    normw = 0.0;
    for (VertexIterator n(cell); !n.end(); ++n)
      normw += sqrt( sqr((w.vector())((*n).index()*2)) + sqr((w.vector())((*n).index()*2+1)) );
    normw /= (*cell).numEntities(0);
    if ( (((*cell).diameter()/nu) > 1.0) || (nu < 1.0e-10) ){
      d1vector((*cell).index()) = C1 * (0.5 / sqrt( 1.0/sqr(k) + sqr(normw/(*cell).diameter()) ) );
      d2vector((*cell).index()) = C2 * (*cell).diameter();
    } else{
      d1vector((*cell).index()) = C1 * sqr((*cell).diameter());
      d2vector((*cell).index()) = C2 * sqr((*cell).diameter());
    }	
  }
}
//-----------------------------------------------------------------------------
void FSISolver::SetInitialVelocity(Vector& xvel)
{
  // Function for setting initial velocity, 
  // given as a function of the coordinates (x,y,z).
  //
  // This function is only temporary: initial velocity 
  // should be possible to set in the main-file. 


//  real x,y,z;

  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
  {
    // Get coordinates of the vertex 
//    real x = (*vertex).coord().x;
//    real y = (*vertex).coord().y;
//    real z = (*vertex).coord().z;
    
    // Specify the initial velocity using (x,y,z) 
    //
    // Example: 
    // 
    // xvel((*vertex).index()) = sin(x)*cos(y)*sqrt(z);
    
    xvel((*vertex).index()) = 0.0;
  }
}
//-----------------------------------------------------------------------------
