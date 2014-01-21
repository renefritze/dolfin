// Copyright (C) 2005-2006 Anders Logg and Garth N. Wells
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Andy R. Terrel 2005
//
// First added:  2005-01-17
// Last changed: 2010-08-28

#ifdef HAS_PETSC

#include <iostream>
#include <petscmat.h>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/types.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/MPI.h>
#include "PETScVector.h"
#include "PETScLinearOperator.h"

using namespace dolfin;

// Callback function for PETSc mult function
namespace dolfin
{
  int usermult(Mat A, Vec x, Vec y)
  {
    // Wrap PETSc Vec as dolfin::PETScVector
    PETScVector _x(x);
    PETScVector _y(y);

    // Extract pointer to PETScLinearOperator
    void* ctx = 0;
    MatShellGetContext(A, &ctx);
    PETScLinearOperator* _A = ((PETScLinearOperator*) ctx);

    // Call user-defined mult function through wrapper
    dolfin_assert(_A);
    GenericLinearOperator* wrapper = _A->wrapper();
    dolfin_assert(wrapper);
    wrapper->mult(_x, _y);

    return 0;
  }
}

//-----------------------------------------------------------------------------
PETScLinearOperator::PETScLinearOperator() : _wrapper(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::size_t PETScLinearOperator::size(std::size_t dim) const
{
  return PETScBaseMatrix::size(dim);
}
//-----------------------------------------------------------------------------
void PETScLinearOperator::mult(const GenericVector& x, GenericVector& y) const
{
  dolfin_not_implemented();
}
//-----------------------------------------------------------------------------
std::string PETScLinearOperator::str(bool verbose) const
{
  std::stringstream s;
  if (verbose)
  {
    warning("Verbose output for PETScLinearOperator not implemented.");
    s << str(false);
  }
  else
  {
    s << "<PETScLinearOperator of size "
      << PETScBaseMatrix::size(0)
      << " x "
      << PETScBaseMatrix::size(1) << ">";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
const GenericLinearOperator* PETScLinearOperator::wrapper() const
{
  return _wrapper;
}
//-----------------------------------------------------------------------------
GenericLinearOperator* PETScLinearOperator::wrapper()
{
  return _wrapper;
}
//-----------------------------------------------------------------------------
void PETScLinearOperator::init_layout(const GenericVector& x,
				      const GenericVector& y,
				      GenericLinearOperator* wrapper)
{
  // Store wrapper
  _wrapper = wrapper;

  // Get global dimension
  const std::size_t M = y.size();
  const std::size_t N = x.size();

  // Get local range
  std::size_t m_local = M;
  std::size_t n_local = N;
  if (MPI::size(MPI_COMM_WORLD) > 1)
  {
    std::pair<std::size_t, std::size_t> local_range_x = x.local_range();
    std::pair<std::size_t, std::size_t> local_range_y = y.local_range();
    m_local = local_range_y.second - local_range_y.first;
    n_local = local_range_x.second - local_range_x.first;
  }

  // Initialize PETSc matrix
  PetscErrorCode ierr;
  if (_A)
    MatDestroy(&_A);

  // Create shell matrix
  ierr = MatCreateShell(PETSC_COMM_WORLD, m_local, n_local, M, N,
                        (void*) this, &_A);
  if (ierr != 0) petsc_error(ierr, __FILE__, "MatCreateShell");

  // Incrase reference count
  PetscObjectReference((PetscObject)_A);

  ierr = MatShellSetOperation(_A, MATOP_MULT, (void (*)()) usermult);
  if (ierr != 0) petsc_error(ierr, __FILE__, "MatShellSetOperation");
}
//-----------------------------------------------------------------------------

#endif
