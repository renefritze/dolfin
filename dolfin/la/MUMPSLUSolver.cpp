// Copyright (C) 2011 Garth N. Wells
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
// First added:  2011-10-16
// Last changed:


#include <vector>

#include "dolfin/common/NoDeleter.h"
#include "dolfin/common/MPI.h"
#include "dolfin/log/log.h"
#include "CoordinateMatrix.h"
#include "GenericVector.h"
#include "LUSolver.h"
#include "MUMPSLUSolver.h"

#ifdef PETSC_HAVE_MUMPS

using namespace dolfin;

// Macros to match MUMPS documentation (C/Fortran indexing issue)
#define ICNTL(I) icntl[(I)-1]
#define CNTL(I) cntl[(I)-1]
#define INFOG(I) infog[(I)-1]
#define INFO(I) info[(I)-1]
#define RINFOG(I) rinfog[(I)-1]
#define RINFO(I) rinfo[(I)-1]

//-----------------------------------------------------------------------------
Parameters MUMPSLUSolver::default_parameters()
{
  Parameters p(LUSolver::default_parameters());
  p.rename("mumps_lu_solver");

  return p;
}
//-----------------------------------------------------------------------------
MUMPSLUSolver::MUMPSLUSolver(const CoordinateMatrix& A)
  : _A(reference_to_no_delete_pointer(A))
{
  // Set parameter values
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
MUMPSLUSolver::MUMPSLUSolver(std::shared_ptr<const CoordinateMatrix> A)
  : _A(A)
{
  // Set parameter values
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
MUMPSLUSolver::~MUMPSLUSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::size_t MUMPSLUSolver::solve(GenericVector& x, const GenericVector& b)
{
  assert(_A);

  DMUMPS_STRUC_C data;

  data.comm_fortran = -987654;

  // Initialise
  data.job = -1;

  // Host participates in solve
  data.par = 1;

  // Output related paramters
  //data.ICNTL(1) = 6; // error messages
  //data.ICNTL(2) = 0;
  //data.ICNTL(3) = 6; // Global information
  //data.ICNTL(3) = 6; // Global information
  if (parameters["verbose"])
    data.ICNTL(4) = 2;
  else
    data.ICNTL(4) = 1;

  // Matrix symmetry (0=non-symmetric, 2=symmetric postitve defn, 2=symmetric)
  data.sym = 0;
  if (parameters["symmetric"])
    data.sym = 2;

  // Initialise MUMPS
  dmumps_c(&data);

  // Related to use of ScaLAPACK (+/-. Negative is faster?)
  //data.ICNTL(13) = -1;

  // Solve transpose (1: A x = b, otherwise A^T x = b)
  data.ICNTL(9) = 1;

  // FIXME (20=default)
  data.ICNTL(14) = 20;

  // Reordering (7=automatic)
  data.ICNTL(7) = 7;

  // Control solution vector (0=solution on root, 1=solution distributed)
  data.ICNTL(21) = 1;

  // Distributed matrix
  data.ICNTL(18) = 3;

  // Parallel/serial analysis (0=auto, 1=serial, 2=parallel)
  if (MPI::size(_A->mpi_comm()) > 1)
    data.ICNTL(28) = 2;
  else
    data.ICNTL(28) = 0;

  // Parallel graph partitioning library (0=auto, 1=pt-scotch, 2=parmetis)
  data.ICNTL(29) = 0;

  // Global size
  assert(_A->size(0) == _A->size(1));
  data.n = _A->size(0);

  if (!_A->base_one())
    error("MUMPS requires a CoordinateMatrix with Fortran-style base 1 indexing.");

  // Get matrix coordindate and value data
  const std::vector<std::size_t>& rows = _A->rows();
  const std::vector<std::size_t>& cols = _A->columns();
  const std::vector<double>& vals = _A->values();

  // Number of non-zero entries on this process
  data.nz_loc = rows.size();

  // Pass matrix data to MUMPS. Trust MUMPS not to change it
  data.irn_loc = const_cast<int*>(reinterpret_cast<const int*>(rows.data()));
  data.jcn_loc = const_cast<int*>(reinterpret_cast<const int*>(cols.data()));
  data.a_loc   = const_cast<double*>(&vals[0]);

  // Analyse and factorize
  data.job = 4;
  dmumps_c(&data);
  if (data.INFOG(1) < 0)
    error("MUMPS reported an error during the analysis and factorisation.");

  cout << "Factorisation finished" << endl;

  // Gather RHS on root process and attach
  std::vector<double> _b;
  b.gather_on_zero(_b);
  data.rhs = &_b[0];

  // Scaling strategy (77 is default)
  data.ICNTL(8) = 77;

  // Get size of local solution vector x and create objects to hold solution
  const std::size_t local_x_size = data.INFO(23);
  std::vector<int> x_local_indices(local_x_size);
  std::vector<double> x_local_vals(local_x_size);

  // Attach solution data to MUMPS object
  data.lsol_loc = local_x_size;
  data.sol_loc  = x_local_vals.data();
  data.isol_loc = x_local_indices.data();

  // Solve problem
  data.job = 3;
  dmumps_c(&data);
  if (data.INFOG(1) < 0)
    error("MUMPS reported an error during the solve.");

  // Shift indices by -1
  for (std::size_t i = 0; i < local_x_size ; ++i)
    x_local_indices[i]--;

  // Set x values
  x.set(x_local_vals.data(), x_local_indices.size(),
        x_local_indices.data());
  x.apply("insert");

  // Clean up
  data.job = -2;
  dmumps_c(&data);

  return 1;
}
//-----------------------------------------------------------------------------
#endif
