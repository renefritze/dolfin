// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <petsc/petscmat.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/NewPDE.h>
#include <dolfin/BilinearForm.h>
#include <dolfin/LinearForm.h>
#include <dolfin/Mesh.h>
#include <dolfin/Matrix.h>
#include <dolfin/NewMatrix.h>
#include <dolfin/NewVector.h>
#include <dolfin/Vector.h>
#include <dolfin/Boundary.h>
#include <dolfin/NewFiniteElement.h>
#include <dolfin/NewFEM.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
real** NewFEM::allocElementMatrix(const NewFiniteElement& element)
{
  unsigned int ncomponents = 0;

  if(element.rank() == 0)
  {
    ncomponents = 1;
  }
  else if(element.rank() == 1)
  {
    ncomponents = element.tensordim(0);
  }

  // Allocate element matrix
  real** AK = new real*[element.spacedim() * ncomponents];
  for (unsigned int i = 0; i < element.spacedim() * ncomponents; i++)
    AK[i] = new real [element.spacedim() * ncomponents];
  
  // Set all entries to zero                                                    
  for (unsigned int i = 0; i < element.spacedim() * ncomponents; i++)
    for (unsigned int j = 0; j < element.spacedim() * ncomponents; j++)
      AK[i][j] = 0.0;

  return AK;
}
//-----------------------------------------------------------------------------
real* NewFEM::allocElementVector(const NewFiniteElement& element)
{
  // Allocate element vector
  real* bK = new real[element.spacedim()];

  // Set all entries to zero
  for (unsigned int i = 0; i < element.spacedim(); i++)
    bK[i] = 0.0;

  return bK;
}
//-----------------------------------------------------------------------------
void NewFEM::freeElementMatrix(real**& AK, const NewFiniteElement& element)
{
  for (unsigned int i = 0; i < element.spacedim(); i++)
    delete [] AK[i];
  delete [] AK;
  AK = 0;
}
//-----------------------------------------------------------------------------
void NewFEM::freeElementVector(real*& bK, const NewFiniteElement& element)
{
  delete [] bK;
  bK = 0;
}
//-----------------------------------------------------------------------------
void NewFEM::assemble(BilinearForm& a, LinearForm& L, Mesh& mesh, 
		      NewMatrix& A, NewVector& b) 
{

  // Start a progress session
  Progress p("Assembling matrix and vector (interior contributions)", mesh.noCells());

  // Initialize finite element and element matrix and vector
  const NewFiniteElement& element = a.element;
  real** AK = allocElementMatrix(element);
  real*  bK = allocElementVector(element);

  unsigned int n = element.spacedim();
  real* block = new real[n*n];
  int* dofs = new int[n];

  // Initialize global matrix 
  // Max connectivity in NewMatrix::init() is assumed to 
  // be 50, alternatively use connectivity information to
  // minimize memory requirements. 
  unsigned int N = size(mesh, element);
  A.init(N, N, element.spacedim());
  A = 0.0;
  b.init(N);
  b = 0.0;

  // Iterate over all cells in the mesh
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update form
    a.update(*cell);
    L.update(*cell);
   
    // Compute element matrix and vector 
    a.interior(AK);
    L.interior(bK);

    // Copy values to one array
    unsigned int pos = 0;
    for (unsigned int i = 0; i < n; i++)
      for (unsigned int j = 0; j < n; j++)
	block[pos++] = AK[i][j];

    // Copy values to one array
    pos = 0;
    for (unsigned int i = 0; i < n; i++)
      block[pos++] = bK[i];

    // Compute mapping from local to global degrees of freedom
    for (unsigned int i = 0; i < n; i++)
      dofs[i] = element.dof(i, *cell);
    
    // Add element matrix to global matrix
    A.add(block, dofs, n, dofs, n);
    
    // Add element matrix to global matrix
    b.add(block, dofs, n);

    // Update progress
    p++;
  }
  
  // Complete assembly
  A.apply();
  b.apply();

  delete [] block;
  delete [] dofs;

  // Delete element matrix
  freeElementMatrix(AK, element);
  freeElementVector(bK, element);

}
//-----------------------------------------------------------------------------
void NewFEM::assemble(BilinearForm& a, Mesh& mesh, NewMatrix& A)
{

  // Start a progress session
  Progress p("Assembling matrix (interior contribution)", mesh.noCells());

  // Initialize finite element and element matrix
  const NewFiniteElement& element = a.element;
  real** AK = allocElementMatrix(element);

  unsigned int n = element.spacedim();
  real* block = new real[n*n];
  int* dofs = new int[n];

  // Initialize global matrix 
  // Max connectivity in NewMatrix::init() is assumed to 
  // be 50, alternatively use connectivity information to
  // minimize memory requirements. 
  unsigned int N = size(mesh, element);
  A.init(N, N, element.spacedim());
  A = 0.0;

  // Iterate over all cells in the mesh
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update form
    a.update(*cell);
    
    // Compute element matrix
    a.interior(AK);

    // Copy values to one array
    unsigned int pos = 0;
    for (unsigned int i = 0; i < n; i++)
      for (unsigned int j = 0; j < n; j++)
	block[pos++] = AK[i][j];

    // Compute mapping from local to global degrees of freedom
    for (unsigned int i = 0; i < n; i++)
      dofs[i] = element.dof(i, *cell);
    
    // Add element matrix to global matrix
    A.add(block, dofs, n, dofs, n);

    // Update progress
    p++;
  }
  
  // Complete assembly
  A.apply();

  delete [] block;
  delete [] dofs;

  // Delete element matrix
  freeElementMatrix(AK, element);

}
//-----------------------------------------------------------------------------
void NewFEM::assemble(LinearForm& L, Mesh& mesh, NewVector& b)
{
  // Start a progress session
  Progress p("Assembling vector (interior contribution)", mesh.noCells());

  // Initialize finite element and element matrix
  const NewFiniteElement& element = L.element;
  real* bK = allocElementVector(element);

  unsigned int n = element.spacedim();
  real* block = new real[n];
  int* dofs = new int[n];

  // Initialize global vector 
  unsigned int N = size(mesh, element);
  b.init(N);
  b = 0.0;

  // Iterate over all cells in the mesh
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update form
    L.update(*cell);
    
    // Compute element matrix
    L.interior(bK);

    // Copy values to one array
    unsigned int pos = 0;
    for (unsigned int i = 0; i < n; i++)
      block[pos++] = bK[i];

    // Compute mapping from local to global degrees of freedom
    for (unsigned int i = 0; i < n; i++)
      dofs[i] = element.dof(i, *cell);
    
    // Add element matrix to global matrix
    b.add(block, dofs, n);

    // Update progress
    p++;
  }
  
  // Complete assembly
  b.apply();

  delete [] block;
  delete [] dofs;

  // Delete element matrix
  freeElementVector(bK, element);
}
//-----------------------------------------------------------------------------
dolfin::uint NewFEM::size(Mesh& mesh, const NewFiniteElement& element)
{
  // Count the degrees of freedom (check maximum index)
  uint dofmax = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    for (uint i = 0; i < element.spacedim(); i++)
    {
      uint dof = element.dof(i, *cell);
      if ( dof > dofmax )
	dofmax = dof;
    }
  }
  return dofmax + 1;
}
//-----------------------------------------------------------------------------
/*
void NewFEM::assemble(NewPDE& pde, Mesh& mesh, Matrix& A, Vector& b)
{
  // Assemble matrix
  assemble(pde.a(), mesh, A);

  // Assemble vector
  assemble(pde.L(), mesh, b);
}
//-----------------------------------------------------------------------------
void NewFEM::assemble(BilinearForm& a, Mesh& mesh, Matrix& A)
{
  // Allocate and reset matrix
  alloc(A, a.element, mesh);

  // Assemble interior contribution
  assembleInterior(a, mesh, A);
  
  // Assemble boundary contribution
  //assembleBoundary(a, mesh, A);
  
  // Clear unused elements
  A.resize();

  // Write a message
  cout << "Assembled: " << A << endl; 
}
//-----------------------------------------------------------------------------
void NewFEM::assemble(LinearForm& L, Mesh& mesh, Vector& b)
{
  // Allocate and reset vector
  alloc(b, L.element, mesh);
  
  // Assemble interior contribution
  assembleInterior(L, mesh, b);

  // Assemble boundary contribution
  //assembleBoundary(L, mesh, b);

  // Write a message
  cout << "Assembled: " << b << endl;
}
//-----------------------------------------------------------------------------
void NewFEM::assembleInterior(BilinearForm& a, Mesh& mesh, Matrix& A)
{
  // Start a progress session
  Progress p("Assembling matrix (interior contribution)", mesh.noCells());
  
  // Initialize finite element and element matrix
  const NewFiniteElement& element = a.element;
  real** AK = allocElementMatrix(element);

  unsigned int ncomponents;

  if(element.rank() == 0)
  {
    ncomponents = 1;
  }
  else if(element.rank() == 1)
  {
    ncomponents = element.tensordim(0);
  }

  // Iterate over all cells in the mesh
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update form
    a.update(*cell);
    
    // Compute element matrix
    bool result = a.interior(AK);

    // Check if we should skip the remaining cells
    if ( !result )
    {
      dolfin_info("Form does not contain a contribution from interior of domain.");
      dolfin_info("Skipping remaining cells.");
      p = 1.0;
      break;
    }

    // Add nonzero entries to global matrix
    for (unsigned int n = 0; n < a.nonzero.size(); n++)
    {
      const IndexPair& index = a.nonzero[n];

      for (unsigned int k = 0; k < ncomponents; k++)
      {
	for (unsigned int l = 0; l < ncomponents; l++)
	{
	  A(element.dof(index.i, *cell) * ncomponents + k,
	    element.dof(index.j, *cell) * ncomponents + l) +=
	    AK[index.i * ncomponents + k][index.j * ncomponents + l];
	}
      }
    }
    // Update progress
    p++;
  }
  
  // Delete element matrix
  freeElementMatrix(AK, element);
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundary(BilinearForm& a, Mesh& mesh, Matrix& A)
{
  // Check mesh type
  switch (mesh.type()) {
  case Mesh::triangles:
    assembleBoundaryTri(a, mesh, A);
    break;
  case Mesh::tetrahedrons:
    assembleBoundaryTet(a, mesh, A);
    break;
  default:
    dolfin_error("Unknown mesh type.");
  }
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundaryTri(BilinearForm& a, Mesh& mesh, Matrix& A)
{
  // FIXME: Not implemented
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundaryTet(BilinearForm& a, Mesh& mesh, Matrix& A)
{
  // FIXME: Not implemented
}
//-----------------------------------------------------------------------------
void NewFEM::assembleInterior(LinearForm& L, Mesh& mesh, Vector& b)
{
  // Start a progress session
  Progress p("Assembling matrix (interior contribution)", mesh.noCells());
  
  // Initialize finite element and element vector
  const NewFiniteElement& element = L.element;
  real* bK = allocElementVector(element);

  // Iterate over all cells in the mesh
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update form
    L.update(*cell);
    
    // Compute element vector
    bool result = L.interior(bK);

    // Check if we should skip the remaining cells
    if ( !result )
    {
      dolfin_info("Form does not contain a contribution from interior of domain.");
      dolfin_info("Skipping remaining cells.");
      p = 1.0;
      break;
    }

    // Add entries to global vector
    for (unsigned int i = 0; i < element.spacedim(); i++)
      b(element.dof(i, *cell)) += bK[i];
  }
  
  // Delete element vector
  freeElementVector(bK, element);
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundary(LinearForm& L, Mesh& mesh, Vector& b)
{
  // Check mesh type
  switch (mesh.type()) {
  case Mesh::triangles:
    assembleBoundaryTri(L, mesh, b);
    break;
  case Mesh::tetrahedrons:
    assembleBoundaryTet(L, mesh, b);
    break;
  default:
    dolfin_error("Unknown mesh type.");
  }
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundaryTri(LinearForm& L, Mesh& mesh, Vector& b)
{
  // FIXME: Not implemented
}
//-----------------------------------------------------------------------------
void NewFEM::assembleBoundaryTet(LinearForm& L, Mesh& mesh, Vector& b)
{
  // FIXME: Not implemented
}
//-----------------------------------------------------------------------------
void NewFEM::alloc(Matrix& A, const NewFiniteElement& element, Mesh& mesh)
{
  unsigned int ncomponents;

  if(element.rank() == 0)
  {
    ncomponents = 1;
  }
  else if(element.rank() == 1)
  {
    ncomponents = element.tensordim(0);
  }

  // Count the degrees of freedom (check maximum index)
  unsigned int dofmax = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    for (unsigned int i = 0; i < element.spacedim(); i++)
    {
      for (unsigned int k = 0; k < ncomponents; k++)
      {
	unsigned int dof = element.dof(i, *cell) * ncomponents + k;
	if ( dof > dofmax )
	  dofmax = dof;
      }
    }
  }
  
  // Initialise matrix
  dofmax++;
  if ( A.size(0) != dofmax || A.size(1) != dofmax )
    A.init(dofmax, dofmax);

  // Set all entries to zero (for repeated assembly)
  A = 0.0;
}
//-----------------------------------------------------------------------------
void NewFEM::alloc(Vector& b, const NewFiniteElement& element, Mesh& mesh)
{
  // Count the degrees of freedom (check maximum index)
  unsigned int dofmax = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    for (unsigned int i = 0; i < element.spacedim(); i++)
    {
      unsigned int dof = element.dof(i, *cell);
      if ( dof > dofmax )
	dofmax = dof;
    }
  }
  
  // Initialise vector
  dofmax++;
  if ( b.size() != dofmax )
    b.init(dofmax);

  // Set all entries to zero (for repeated assembly)
  b = 0.0;
}
//-----------------------------------------------------------------------------
*/
