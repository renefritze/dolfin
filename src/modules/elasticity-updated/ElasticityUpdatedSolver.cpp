// Copyright (C) 2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2004-2005.
//
// First added:  2005
// Last changed: 2005

//#include <iostream>
#include <sstream>
#include <iomanip>

#include "dolfin/timeinfo.h"
#include "dolfin/ElasticityUpdatedSolver.h"
#include "dolfin/ElasticityUpdated.h"
#include "dolfin/ElasticityUpdatedSigma.h"
#include "dolfin/ElasticityUpdatedProj.h"
#include "dolfin/ElasticityUpdatedMass.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
ElasticityUpdatedSolver::ElasticityUpdatedSolver(Mesh& mesh, 
						 Function& f,
						 Function& v0,
						 Function& rho,
						 real E, real nu, real nuv,
						 real nuplast,
						 BoundaryCondition& bc,
						 real k, real T)
  : mesh(mesh), f(f), v0(v0), rho(rho), E(E), nu(nu), nuv(nuv),
    nuplast(nuplast), bc(bc), k(k),
    T(T), counter(0), lastsample(0),
    lambda(E * nu / ((1 + nu) * (1 - 2 * nu))),
    mu(E / (2 * (1 + nu))),
    t(0.0), rtol(1.0e-4), maxiters(10), do_plasticity(false), yield(0.0),
    v1(x2_1, mesh, element1),
    u0(x1_0, mesh, element1),
    u1(x1_1, mesh, element1),
    sigma0(xsigma0, mesh, element2),
    sigma1(xsigma1, mesh, element2),
    epsilon1(xepsilon1, mesh, element2),
    sigmanorm(xsigmanorm, mesh, element2),
    Lv(f, sigma1, epsilon1, nuv),
    Lsigma(v1, sigma1, sigmanorm, lambda, mu, nuplast)
{
  // Do nothing
  init();
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::init()
{
  Matrix M;

  int Nv = FEM::size(mesh, element1);
  int Nsigma = FEM::size(mesh, element2);
  int Nmesh = 3 * mesh.noNodes();

  x1_0.init(Nv);
  x1_1.init(Nv);
  x2_0.init(Nv);
  x2_1.init(Nv);

  xtmp1.init(Nv);
  xtmp2.init(Nv);

  Dummy.init(Nv, Nv);

  for(int i = 0; i < Nv; i++)
  {
    Dummy(i, i) = 1.0;
  }


  msigma.init(Nsigma);

  xsigmatmp1.init(Nsigma);

  xsigma0.init(Nsigma);
  xsigma1.init(Nsigma);

  xepsilon1.init(Nsigma);

  xsigmanorm.init(Nsigma);

  mesh0.init(Nmesh);

  xsigma1 = 0;

  stepresidual.init(Nv);

  // Set initial velocities

  AffineMap map;
  v0.set(element1);

  for(CellIterator c(&mesh); !c.end(); ++c)
  {
    Cell& cell = *c;

    // Use DOLFIN's interpolation

    real coefficients[element1.spacedim()];
    int dofs[element1.spacedim()];

    map.update(cell);
    v0.interpolate(coefficients, map);
    element1.dofmap(dofs, cell, mesh);

    for(uint i = 0; i < element1.spacedim(); i++)
      x2_1(dofs[i]) = coefficients[i];
  }

  FEM::applyBC(Dummy, x2_1, mesh, element1, bc);


//   cout << "x2_1:" << endl;
//   x2_1.disp();

  xsigmanorm = 1.0;

  ElasticityUpdatedMass::BilinearForm amass(rho);

  // Assemble mass matrix
  FEM::assemble(amass, M, mesh);

  // Lump mass matrix
  FEM::lump(M, m);

  // Compute mass vector (sigma)
  for (CellIterator c(mesh); !c.end(); ++c)
  {
    Cell& cell = *c;

    int dofs[element2.spacedim()];
    element2.dofmap(dofs, cell, mesh);

    real factor = 1.0 / cell.volume(); 

    for(uint i = 0; i < element2.spacedim(); i++)
      msigma(dofs[i]) = factor;
  }
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::preparestep()
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

  // Copy mesh values

  for (NodeIterator n(&mesh); !n.end(); ++n)
  {
    Node& node = *n;
    int nid = node.id();

    mesh0(3 * nid + 0) = node.coord().x;
    mesh0(3 * nid + 1) = node.coord().y;
    mesh0(3 * nid + 2) = node.coord().z;
  }
  
  t += k;
  f.set(t);
  //     cout << "t: " << t << endl;
  
  for(int iter = 0; iter < maxiters; iter++)
  {
    // Compute norm of stress (sigmanorm)
    if(do_plasticity)
    {
      for (CellIterator c(mesh); !c.end(); ++c)
      {
	Cell& cell = *c;

	int dofs[element2.spacedim()];
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
    }
    
    //       dolfin_debug("Assembling sigma vectors");
    //       tic();
    
    // Assemble sigma0 vectors
    FEM::assemble(Lsigma, xsigma1, mesh);
    
    VecPointwiseMult(xsigmatmp1.vec(), xsigma1.vec(), msigma.vec());
    
    xsigmatmp1.apply();
    
    xsigma1 = xsigma0;
    xsigma1.axpy(k, xsigmatmp1);
    
    xepsilon1 *= 1.0 / lambda;
    
    // Assemble v vector
    FEM::assemble(Lv, xtmp1, mesh);

    FEM::applyBC(Dummy, xtmp1, mesh, element1, bc);
    
    b = xtmp1;
    b *= k;
    
    VecPointwiseDivide(stepresidual.vec(), xtmp1.vec(), m.vec());
    stepresidual *= k;
    stepresidual.axpy(-1, x2_1);
    stepresidual.axpy(1, x2_0);
    
    
    x2_1 += stepresidual;
    
    x1_1 = x1_0;
    x1_1.axpy(k, x2_0);
    
    cout << "stepresidual: " << stepresidual.norm(Vector::linf) << endl;

    // Update the mesh
    for (NodeIterator n(&mesh); !n.end(); ++n)
    {
      Node& node = *n;
      int nid = node.id();
      
      node.coord().x = mesh0(3 * nid + 0) + k * v1(node, 0);
      node.coord().y = mesh0(3 * nid + 1) + k * v1(node, 1);
      node.coord().z = mesh0(3 * nid + 2) + k * v1(node, 2);
    }
    
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
  
  //     // Print dot(sigma)
  
  //     for (CellIterator c(mesh); !c.end(); ++c)
  //     {
  //       Cell& cell = *c;
  //       if(cell.id() == 0)
  //       {
  
  // 	cout << "t: " << t << endl;
  // 	cout << "cell: " << cell.id() << endl;
  // 	cout << cell.midpoint() << endl;
  
  // 	int dofs[element2_0.spacedim()];
  // 	element2_0.dofmap(dofs, cell, mesh);
  
  // 	for(uint i = 0; i < element2_0.spacedim(); i++)
  // 	{
  // 	  cout << "dot(sigma)(:, 0): " << xtmp0_1(dofs[i]) << " " << xtmp1_1(dofs[i]) << " " << xtmp2_1(dofs[i]) << endl;
  // 	}
  // 	for(uint i = 0; i < element2_0.spacedim(); i++)
  // 	{
  // 	  cout << "sigma(:, 0): " << xsigma0_1(dofs[i]) << " " << xsigma1_1(dofs[i]) << " " << xsigma2_1(dofs[i]) << endl;
  // 	}
  
}
//-----------------------------------------------------------------------------
void ElasticityUpdatedSolver::solve()
{
  cout << "lambda: " << lambda << endl;
  cout << "mu: " << mu << endl;
  
  File         file("elasticity.m");

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
  real samplefreq = 1.0 / 33.0;

  while(lastsample + samplefreq < t || t == 0.0)
  {
    save(mesh, solutionfile, t);

    counter++;

    lastsample = std::min(t, lastsample + samplefreq);
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
				    real E, real nu, real nuv,
				    real nuplast,
				    BoundaryCondition& bc,
				    real k, real T)
{
  ElasticityUpdatedSolver solver(mesh, f, v0, rho, E, nu, nuv, nuplast,
				 bc, k, T);
  solver.solve();
}
//-----------------------------------------------------------------------------
