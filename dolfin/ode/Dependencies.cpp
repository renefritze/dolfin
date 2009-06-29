// Copyright (C) 2003-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-01-06
// Last changed: 2008-10-06

#include <dolfin/common/real.h>
#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/math/dolfin_math.h>
#include <dolfin/parameter/parameters.h>
#include "ODE.h"
#include "Dependencies.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Dependencies::Dependencies(uint N) :
  N(N), increment(0), _sparse(false)
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
  make_sparse();

  // Set size of row
  sdep[i].reserve(size);
}
//-----------------------------------------------------------------------------
void Dependencies::set(uint i, uint j, bool checknew)
{
  // Prepare sparse pattern if necessary
  make_sparse();

  // Check if the size has been specified
  if ( sdep[i].size() == sdep[i].capacity() )
    warning("Specify number of dependencies first for better performance.");

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
/*
void Dependencies::set(const uBLASSparseMatrix& A)
{
  // Prepare sparse pattern if necessary
  make_sparse();

  // Check dimension of matrix
  if ( A.size(0) != N )
    error("Incorrect matrix dimensions for dependency pattern.");

  // Get data from matrix
  for (uint i = 0; i < N; i++)
  {
    // FIXME: Could add function to return sparsity pattern
    std::vector<uint> columns;
    std::vector<real> values;
    A.getrow(i, columns, values);
    setsize(i, columns.size());
    for (uint j = 0; j < columns.size(); j++)
      set(i, columns[j]);
  }
}
*/
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
  make_sparse();

  // Count the number of dependencies
  std::vector<uint> rowsizes(N);
  std::fill(rowsizes.begin(), rowsizes.end(), 0);
  for (uint i = 0; i < N; i++)
  {
    const std::vector<uint>& row(dependencies.sdep[i]);
    for (uint pos = 0; pos < row.size(); ++pos)
      rowsizes[row[pos]]++;
  }

  // Set row sizes
  for (uint i = 0; i < N; i++)
    setsize(i, rowsizes[i]);

  // Set dependencies
  for (uint i = 0; i < N; i++)
  {
    const std::vector<uint>& row(dependencies.sdep[i]);
    for (uint pos = 0; pos < row.size(); ++pos)
      set(row[pos], i);
  }
}
//-----------------------------------------------------------------------------
void Dependencies::detect(ODE& ode)
{
  // Get size of increment
  double tmp = ode.get("ODE sparsity check increment");
  increment = tmp;

  // Prepare sparse pattern if necessary
  make_sparse();

  // Randomize solution vector
  real* u = new real[N];
  for (uint i = 0; i < N; i++)
    u[i] = rand();

  // Check dependencies for all components
  Progress p("Computing sparsity", N);
  uint sum = 0;
  for (uint i = 0; i < N; i++)
  {
    // Count the number of dependencies
    uint size = 0;
    real f0 = ode.f(u, 0.0, i);
    for (uint j = 0; j < N; j++)
      if (check_dependency(ode, u, f0, i, j))
        size++;

    // Compute total number of dependencies
    sum += size;

    // Set size of row
    setsize(i, size);

    // Set the dependencies
    for (uint j = 0; j < N; j++)
      if ( check_dependency(ode, u, f0, i, j) )
	set(i, j);

    // Update progress
    p = i;
  }

  // Clean up
  delete [] u;

  info("Automatically detected %d dependencies.", sum);
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
    info("Dependency pattern:");
    for (uint i = 0; i < N; i++)
    {
      cout << i << ":";
      for (uint pos = 0; pos < sdep[i].size(); ++pos)
        cout << " " << sdep[i][pos];
      cout << endl;
    }
  }
  else
    info("Dependency pattern: dense");
}
//-----------------------------------------------------------------------------
bool Dependencies::check_dependency(ODE& ode, real* u, real f0,
                                    uint i, uint j)
{
  // Save original value
  real uj = u[j];

  // Change value and compute new value for f_i
  u[j] += increment;
  real f = ode.f(u, 0.0, i);

  // Restore the value
  u[j] = uj;

  // Compare function values
  return real_abs(f - f0) > real_epsilon();
}
//-----------------------------------------------------------------------------
void Dependencies::make_sparse()
{
  if ( _sparse )
    return;

  sdep.reserve(N);
  sdep.resize(N);

  ddep.clear();

  _sparse = true;
}
//-----------------------------------------------------------------------------
