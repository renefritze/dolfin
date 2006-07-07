// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-01-06
// Last changed: 2006-07-05

#include <cmath>
#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_math.h>
#include <dolfin/ParameterSystem.h>
#include <dolfin/DenseVector.h>
#include <dolfin/ODE.h>
#include <dolfin/Dependencies.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Dependencies::Dependencies(uint N) :
  N(N), increment(get("ODE sparsity check increment")), _sparse(false)
{
  // Use dense dependency pattern by default
  ddep.reserve(N);
  ddep.resize(N);
  for (uint i = 0; i < N; i++)
    ddep[i] = i;
}
//-----------------------------------------------------------------------------
Dependencies::~Dependencies()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void Dependencies::setsize(uint i, uint size)
{
  // Prepare sparse pattern if necessary
  makeSparse();
  
  // Set size of row
  sdep[i].reserve(size);
}
//-----------------------------------------------------------------------------
void Dependencies::set(uint i, uint j, bool checknew)
{
  // Prepare sparse pattern if necessary
  makeSparse();

  // Check if the size has been specified
  if ( sdep[i].size() == sdep[i].capacity() )
    dolfin_warning("Specify number of dependencies first for better performance.");

  // Check if the dependency already exists before inserting it
  if ( checknew )
  {
    for (uint k = 0; k < sdep[i].size(); k++)
      if ( sdep[i][k] == j )
	return;
  }

  // Add the dependency
  sdep[i].push_back(j);
}
//-----------------------------------------------------------------------------
void Dependencies::set(const uBlasSparseMatrix& A)
{
  // Prepare sparse pattern if necessary
  makeSparse();

  // Check dimension of matrix
  if ( A.size(0) != N )
    dolfin_error("Incorrect matrix dimensions for dependency pattern.");

  // Get data from matrix
  for (uint i = 0; i < N; i++)
  {
    // FIXME: Could add function to return sparsity pattern
    Array<int> columns;
    Array<real> values;
    int ncols = 0;
    A.getRow(i, ncols, columns, values); 
    setsize(i, ncols);
    for (uint j = 0; j < static_cast<uint>(ncols); j++)
      set(i, columns[j]);
  }
}
//-----------------------------------------------------------------------------
void Dependencies::transp(const Dependencies& dependencies)
{
  // Don't compute dependency pattern is full
  if ( !dependencies._sparse )
  {
    if ( _sparse )
    {
      for (uint i = 0; i < N; i++)
        sdep[i].clear();
      sdep.clear();
      
      _sparse = false;
    }
    
    return;
  }

  // Prepare sparse pattern if necessary
  makeSparse();

  // Count the number of dependencies
  Array<uint> rowsizes(N);
  rowsizes = 0;
  for (uint i = 0; i < N; i++)
  {
    const Array<uint>& row(dependencies.sdep[i]);
    for (uint pos = 0; pos < row.size(); ++pos)
      rowsizes[row[pos]]++;
  }

  // Set row sizes
  for (uint i = 0; i < N; i++)
    setsize(i, rowsizes[i]);

  // Set dependencies
  for (uint i = 0; i < N; i++)
  {
    const Array<uint>& row(dependencies.sdep[i]);
    for (uint pos = 0; pos < row.size(); ++pos)
      set(row[pos], i);
  }
}
//-----------------------------------------------------------------------------
void Dependencies::detect(ODE& ode)
{
  // Prepare sparse pattern if necessary
  makeSparse();

  // Randomize solution vector
  DenseVector u(N);
  for (uint i = 0; i < N; i++)
    u(i) = rand();
  
  // Check dependencies for all components
  Progress p("Computing sparsity", N);
  uint sum = 0;
  for (uint i = 0; i < N; i++)
  {
    // Count the number of dependencies
    uint size = 0;
    real f0 = ode.f(u, 0.0, i);
    for (uint j = 0; j < N; j++)
      if ( checkDependency(ode, u, f0, i, j) )
        size++;
    
    // Compute total number of dependencies
    sum += size;
    
    // Set size of row
    setsize(i, size);

    // Set the dependencies
    for (uint j = 0; j < N; j++)
      if ( checkDependency(ode, u, f0, i, j) )
	set(i, j);
    
    // Update progress
    p = i;
  }

  dolfin_info("Automatically detected %d dependencies.", sum);
}
//-----------------------------------------------------------------------------
bool Dependencies::sparse() const
{
  return _sparse;
}
//-----------------------------------------------------------------------------
void Dependencies::disp() const
{
  if ( _sparse )
  {
    dolfin_info("Dependency pattern:");
    for (uint i = 0; i < N; i++)
    {
      cout << i << ":";
      for (uint pos = 0; pos < sdep[i].size(); ++pos)
        cout << " " << sdep[i][pos];
      cout << endl;
    }
  }
  else  
    dolfin_info("Dependency pattern: dense");
}
//-----------------------------------------------------------------------------
bool Dependencies::checkDependency(ODE& ode, DenseVector& u, real f0,
				   uint i, uint j)
{
  // Save original value
  real uj = u(j);

  // Change value and compute new value for f_i
  u(j) += increment;
  real f = ode.f(u, 0.0, i);

  // Restore the value
  u(j) = uj;

  // Compare function values
  return fabs(f - f0) > DOLFIN_EPS;
}
//-----------------------------------------------------------------------------
void Dependencies::makeSparse()
{
  if ( _sparse )
    return;
  
  sdep.reserve(N);
  sdep.resize(N);
  
  ddep.clear();
    
    _sparse = true;
}
//-----------------------------------------------------------------------------
