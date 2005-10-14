// Copyright (C) 2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2004-2005.
// Modified by Garth N. Wells 2005.
//
// First added:  2005
// Last changed: 2005-09-16

//#include <iostream>
#include <sstream>
#include <iomanip>

#include "dolfin/timeinfo.h"
#include "dolfin/ElasticityUpdatedSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
ElasticityUpdatedSolver::ElasticityUpdatedSolver(Mesh& mesh, 
						 Function& f,
						 Function& v0,
						 Function& rho,
						 real& E, real& nu, real& nuv,
						 real& nuplast,
						 BoundaryCondition& bc,
						 real& k, real& T)
  : mesh(mesh), f(f), v0(v0), rho(rho), E(E), nu(nu), nuv(nuv),
    nuplast(nuplast), bc(bc), k(k),
    T(T), counter(0), lastsample(0),
    lambda(E * nu / ((1 + nu) * (1 - 2 * nu))),
    mu(E / (2 * (1 + nu))),
    t(0.0), rtol(1.0e-4), maxiters(10), do_plasticity(false), yield(0.0),
    savesamplefreq(33.0),
    v1(x2_1, mesh, element1),
    u0(x1_0, mesh, element1),
    u1(x1_1, mesh, element1),
    sigma0(xsigma0, mesh, element2),
    sigma1(xsigma1, mesh, element2),
    epsilon1(xepsilon1, mesh, element2),
    sigmanorm(xsigmanorm, mesh, element3),
    Lv(f, sigma1, epsilon1, nuv),
    Lsigma(v1, sigma1, sigmanorm, lambda, mu, nuplast)
{
  cout << "nuv: " << nuv << endl;
  cout << "lambda: " << lambda << endl;
  cout << "mu: " << mu << endl;

  init();
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::init()
{
  Matrix M;

  int Nv = FEM::size(mesh, element1);
  int Nsigma = FEM::size(mesh, element2);
  int Nsigmanorm = FEM::size(mesh, element3);

  x1_0.init(Nv);
  x1_1.init(Nv);
  x2_0.init(Nv);
  x2_1.init(Nv);

  x1ode.init(Nv);
  x2ode.init(Nv);

  xtmp1.init(Nv);
  xtmp2.init(Nv);

  fcontact.init(Nv);

  Dummy.init(Nv, Nv);

  for(int i = 0; i < Nv; i++)
  {
    Dummy(i, i) = 1.0;
  }


  msigma.init(Nsigma);

  xsigmatmp1.init(Nsigma);
  xsigmatmp2.init(Nsigma);

  xsigma0.init(Nsigma);
  xsigma1.init(Nsigma);

  xsigmaode.init(Nsigma);

  xjaumann1.init(Nsigma);

  xepsilon1.init(Nsigma);

  xsigmanorm.init(Nsigmanorm);

  stepresidual.init(Nv);

  // Set initial velocities

  AffineMap map;
  v0.set(element1);

  {
  int *dofs = new int[element1.spacedim()];
  real *coefficients = new real[element1.spacedim()];
  for(CellIterator c(&mesh); !c.end(); ++c)
  {
    Cell& cell = *c;

    // Use DOLFIN's interpolation

    map.update(cell);
    v0.interpolate(coefficients, map);
    element1.dofmap(dofs, cell, mesh);

    for(uint i = 0; i < element1.spacedim(); i++)
      x2_1(dofs[i]) = coefficients[i];
  }
  delete [] dofs;
  delete [] coefficients;
  }

  FEM::applyBC(Dummy, x2_1, mesh, element1, bc);

  cout << "x2_1:" << endl;
  x2_1.disp();

  // Set initial stress

  xsigma1 = 0.0;

  cout << "xsigma1:" << endl;
  xsigma1.disp();

  xsigmanorm = 1.0;

  ElasticityUpdatedMass::BilinearForm amass(rho);

  // Assemble mass matrix
  FEM::assemble(amass, M, mesh);

  // Lump mass matrix
  FEM::lump(M, m);

  cout << "m:" << endl;
  m.disp();


  // Compute mass vector (sigma)
  {
  int *dofs = new int[element2.spacedim()];
  for (CellIterator c(mesh); !c.end(); ++c)
  {
    Cell& cell = *c;

    element2.dofmap(dofs, cell, mesh);

    real factor = 1.0 / cell.volume(); 

    for(uint i = 0; i < element2.spacedim(); i++)
      msigma(dofs[i]) = factor;
  }
  delete [] dofs;
  }

  // The mesh points are the initial values of u
  int offset = mesh.noNodes();
  for (NodeIterator n(&mesh); !n.end(); ++n)
  {
    Node& node = *n;
    int nid = node.id();

    x1_1(0 * offset + nid) = node.coord().x;
    x1_1(1 * offset + nid) = node.coord().y;
    x1_1(2 * offset + nid) = node.coord().z;
  }

}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::preparestep()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::prepareiteration()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::step()
{


  // Make time step
  x1_0 = x1_1;
  x2_0 = x2_1;
  xsigma0 = xsigma1;

  t += k;
  cout << "t: " << t << endl;
  
  for(int iter = 0; iter < maxiters; iter++)
  {
    prepareiteration();

    // Compute RHS of ODE system
    fu();


    // Time step method (dG(0)), position

    x1_1 = x1_0;
    x1_1.axpy(k, x1ode);

    // Time step method (dG(0)), velocity (also compute residual)

    stepresidual = x2_0;
    stepresidual.axpy(k, x2ode);
    stepresidual.axpy(-1, x2_1);
    x2_1 += stepresidual;

//     cout << "x2_1:" << endl;
//     x2_1.disp();

    
    cout << "stepresidual(j): " << stepresidual.norm(Vector::linf) << endl;

    // Time step method (dG(0)), stress

    xsigma1 = xsigma0;
    xsigma1.axpy(k, xsigmaode);


//     cout << "x2_1:" << endl;
//     x2_1.disp();

    if(stepresidual.norm(Vector::linf) <= rtol && iter >= 0)
    {
      cout << "converged" << endl;
      break;
    }
    else if(iter == maxiters - 1)
    {
      cout << "did not converge" << endl;
    }
  }




  /*
  // Make time step
  x1_0 = x1_1;
  x2_0 = x2_1;
  xsigma0 = xsigma1;

  t += k;
  cout << "t: " << t << endl;
  
  for(int iter = 0; iter < maxiters; iter++)
  {
    prepareiteration();

    // Update the mesh
    for (NodeIterator n(&mesh); !n.end(); ++n)
    {
      Node& node = *n;

      node.coord().x = u1(node, 0);
      node.coord().y = u1(node, 1);
      node.coord().z = u1(node, 2);
    }



    // Compute norm of stress (sigmanorm)
    if(do_plasticity)
    {
      {
      int *dofs = new int[element2.spacedim()];
      for (CellIterator c(mesh); !c.end(); ++c)
      {
	Cell& cell = *c;

	element2.dofmap(dofs, cell, mesh);

	real proj = 1;
	real norm = 0;
	for(uint i = 0; i < element2.spacedim(); i++)
	{
	  norm = std::max(norm, fabs(xsigma0(dofs[i])));
	}
	
	if(norm > yield)
	{
	  cout << "sigmanorm(" << cell.id() << "): " << norm << endl;
	  proj = 1.0 / norm;
	}
	
	xsigmanorm(dofs[0]) = proj;
      }
      delete [] dofs;
      }
    }
    
    //       dolfin_debug("Assembling sigma vectors");
    //       tic();
    
    // Assemble sigma1 vectors
    FEM::assemble(Lsigma, xsigmatmp2, mesh);
//     FEM::assemble(Ljaumann, xjaumann1, mesh);
    
    VecPointwiseMult(xsigmatmp1.vec(), xsigmatmp2.vec(), msigma.vec());
    
    xsigmatmp1.apply();
    
    xepsilon1 = xsigmatmp1;
    xepsilon1 *= 1.0 / lambda;
    //    xepsilon1 *= 1.0 / (3 * lambda + 2 * mu);

//     cout << "epsilon:" << endl;
//     xepsilon1.disp();

    xsigma1 = xsigma0;
    xsigma1.axpy(k, xsigmatmp1);
    


//     // Print dot(sigma)
    
//     for (CellIterator c(mesh); !c.end(); ++c)
//     {
//       Cell& cell = *c;
//       if(cell.id() == 0)
//       {
	
//   	cout << "t: " << t << endl;
//   	cout << "cell: " << cell.id() << endl;
//   	cout << cell.midpoint() << endl;
	
//   	int dofs[element2.spacedim()];
//   	element2.dofmap(dofs, cell, mesh);
	
//   	for(uint i = 0; i < element2.spacedim(); i++)
//   	{
//   	  cout << "dot(sigma)(:, " << i << "): " << xsigmatmp1(dofs[i]) << endl;
//   	}
//   	for(uint i = 0; i < element2.spacedim(); i++)
//   	{
//   	  cout << "sigma(:, " << i << "): " << xsigma1(dofs[i]) << endl;
//   	}
//    	for(uint i = 0; i < element2.spacedim(); i++)
//   	{
//   	  cout << "jaumann(:, " << i << "): " << xjaumann1(dofs[i]) << endl;
//   	}
//       }
//     }
	
    // Assemble v vector
    FEM::assemble(Lv, xtmp1, mesh);

    // Add contact forces
    xtmp1.axpy(1, fcontact);

    FEM::applyBC(Dummy, xtmp1, mesh, element1, bc);
//     FEM::applyBC(Dummy, fcontact, mesh, element1, bc);

//     cout << "fcontact: " << endl;
//     fcontact.disp();
//     cout << "xtmp1: " << endl;
//     xtmp1.disp();

    
    VecPointwiseDivide(stepresidual.vec(), xtmp1.vec(), m.vec());
    stepresidual *= k;
    stepresidual.axpy(-1, x2_1);
    stepresidual.axpy(1, x2_0);
    
    x2_1 += stepresidual;

//     cout << "x2_1:" << endl;
//     x2_1.disp();


    x1_1 = x1_0;
    x1_1.axpy(k, x2_0);
    
    cout << "stepresidual(j): " << stepresidual.norm(Vector::linf) << endl;

//     cout << "x2_1:" << endl;
//     x2_1.disp();

    if(stepresidual.norm(Vector::linf) <= rtol && iter >= 0)
    {
      cout << "converged" << endl;
      break;
    }
    else if(iter == maxiters - 1)
    {
      cout << "did not converge" << endl;
    }
  }
  */  
  
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::solve()
{
  cout << "lambda: " << lambda << endl;
  cout << "mu: " << mu << endl;
  
  File         file("elasticity.m");

  // Synchronize f with time t
  f.sync(t);
  
  // Save the solution
  condsave(mesh, file, t);

  // Start a progress session
  Progress p("Time-stepping");
  
  // Start time-stepping
  while ( true && t < T ) {
  
    preparestep();
    step();
    
    // Save the solution
    condsave(mesh, file, t);

    // Benchmark
//     FEM::assemble(Lsigma0, xsigma0_1, mesh);
    
    // Update progress
    p = t / T;
  }
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::save(Mesh& mesh, File& solutionfile, real t)
{
    std::ostringstream fileid, filename;
    fileid.fill('0');
    fileid.width(6);
    
    fileid << counter;
    
    filename << "mesh" << fileid.str() << ".xml.gz";
    
    cout << "writing: " << filename.str() << " at t: " << t << endl;
    
    std::string foo = filename.str();
    const char *fname = foo.c_str();
    
    File meshfile(fname);
    
    meshfile << mesh;

}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::condsave(Mesh& mesh, File& solutionfile, real t)
{
//   real samplefreq = 1.0 / 33.0;
  real sampleperiod = 1.0 / savesamplefreq;

  while(lastsample + sampleperiod < t || t == 0.0)
  {
    save(mesh, solutionfile, t);

    counter++;

    lastsample = std::min(t, lastsample + sampleperiod);
    cout << "lastsample: " << lastsample << " t: " << t << endl;

    if(t == 0.0)
    {
      break;
    }
  }
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::solve(Mesh& mesh,
				    Function& f,
				    Function& v0,
				    Function& rho,
				    real& E, real& nu, real& nuv,
				    real& nuplast,
				    BoundaryCondition& bc,
				    real& k, real& T)
{
  ElasticityUpdatedSolver solver(mesh, f, v0, rho, E, nu, nuv, nuplast,
				 bc, k, T);
  solver.solve();
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::fu()
{
  cout << "fu()" << endl;

  // Compute x1ode, x2ode and xsigmaode based on x1_1, x2_1 and xsigma1

  // Update the mesh
  for (NodeIterator n(&mesh); !n.end(); ++n)
  {
    Node& node = *n;
    
    node.coord().x = u1(node, 0);
    node.coord().y = u1(node, 1);
    node.coord().z = u1(node, 2);
  }

  // x1ode
  x1ode = x2_1;

  // xsigma1
  FEM::assemble(Lsigma, xsigmatmp1, mesh);
  VecPointwiseMult(xsigmaode.vec(), xsigmatmp1.vec(), msigma.vec());
    
  xsigmaode.apply();
    
  // xepsilon1 (needed for x2ode)

  xepsilon1 = xsigmaode;
  xepsilon1 *= 1.0 / lambda;

  // x2ode

  // Assemble v vector
  FEM::assemble(Lv, xtmp1, mesh);
  
  // Add contact forces
  xtmp1.axpy(1, fcontact);
  
  FEM::applyBC(Dummy, xtmp1, mesh, element1, bc);
  VecPointwiseDivide(x2ode.vec(), xtmp1.vec(), m.vec());

  x2ode.apply();
}
//-----------------------------------------------------------------------------
