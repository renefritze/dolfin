// Copyright (C) 2013 Garth N. Wells
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
// Modified by Steven Vandekerckhove, 2014.
//
// First added:  2013-02-12
// Last changed: 2014-11-18

#include <Eigen/Dense>

#include <dolfin/la/GenericVector.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/Timer.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/la/GenericLinearAlgebraFactory.h>
#include <dolfin/function/GenericFunction.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/function/Function.h>
#include "assemble.h"
#include "GenericDofMap.h"
#include "Form.h"
#include "UFC.h"

#include "LocalSolver.h"

using namespace dolfin;

LocalSolver::LocalSolver()
{
}
//----------------------------------------------------------------------------
LocalSolver::LocalSolver(const Form& a, const Form& L) :
  _a(reference_to_no_delete_pointer(a)), _l(reference_to_no_delete_pointer(L))
{
  init();
}
//----------------------------------------------------------------------------
LocalSolver::LocalSolver(std::shared_ptr<const Form> a,
                         std::shared_ptr<const Form> L) :
  _a(a), _l(L)
{
  init();
}
//----------------------------------------------------------------------------
void LocalSolver::init()
{
  UFC ufc_a(*this->_a);
  UFC ufc_L(*this->_l);

  // Raise error for Point integrals
  if (ufc_a.form.has_point_integrals() || ufc_L.form.has_point_integrals())
  {
    dolfin_error("LocalSolver.cpp",
                 "assemble system",
                 "Point integrals are not supported (yet)");
  }

  // Set timer
  Timer timer("Prepare Local solver");

  // Extract mesh
  const Mesh& mesh = _a->mesh();

  // Form ranks
  const std::size_t rank_a = ufc_a.form.rank();
  const std::size_t rank_L = ufc_L.form.rank();

  // Check form ranks
  dolfin_assert(rank_a == 2);
  dolfin_assert(rank_L == 1);

  // Collect pointers to dof maps
  std::shared_ptr<const GenericDofMap> dofmap_a0
    = _a->function_space(0)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_a1
    = _a->function_space(1)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_L
    = _a->function_space(0)->dofmap();
  dolfin_assert(dofmap_a0);
  dolfin_assert(dofmap_a1);
  dolfin_assert(dofmap_L);

  // Cell integrals
  ufc::cell_integral* integral_a = ufc_a.default_cell_integral.get();

  // Eigen data structures
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> A;

  // Assemble over cells
  Progress p("Performing local (cell-wise) solve", mesh.num_cells());
  ufc::cell ufc_cell;
  std::vector<double> vertex_coordinates;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update to current cell
    cell->get_vertex_coordinates(vertex_coordinates);
    cell->get_cell_data(ufc_cell);
    ufc_a.update(*cell, vertex_coordinates, ufc_cell,
                 integral_a->enabled_coefficients());

    // Get local-to-global dof maps for cell
    const std::vector<dolfin::la_index>& dofs_a0
      = dofmap_a0->cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs_a1
      = dofmap_a1->cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs_L
      = dofmap_L->cell_dofs(cell->index());

    // Check that local problem is square and a and L match
    dolfin_assert(dofs_a0.size() == dofs_a1.size());
    dolfin_assert(dofs_a1.size() == dofs_L.size());

    // Resize A and b SVDK: just set size
    A.resize(dofs_a0.size(), dofs_a1.size());

    // Tabulate A on cell
    integral_a->tabulate_tensor(A.data(),
                                ufc_a.w(),
                                vertex_coordinates.data(),
                                ufc_cell.orientation);

     // Solve local problem
    _lus.push_back(A.partialPivLu());

    p++;
  }
}
//----------------------------------------------------------------------------
void LocalSolver::solve(GenericVector& x) const
{
  UFC ufc_a(*this->_a);
  UFC ufc_L(*this->_l);
  std::shared_ptr<const GenericVector> _x(reference_to_no_delete_pointer(x));

  // Set timer
  Timer timer("Local solver");

  // Extract mesh
  const Mesh& mesh = (*this->_a).mesh();

  // Form ranks
  const std::size_t rank_a = ufc_a.form.rank();
  const std::size_t rank_L = ufc_L.form.rank();

  // Check form ranks
  dolfin_assert(rank_a == 2);
  dolfin_assert(rank_L == 1);

  // Collect pointers to dof maps
  std::shared_ptr<const GenericDofMap> dofmap_a0
    = (*this->_a).function_space(0)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_a1
    = (*this->_a).function_space(1)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_L
    = (*this->_a).function_space(0)->dofmap();
  dolfin_assert(dofmap_a0);
  dolfin_assert(dofmap_a1);
  dolfin_assert(dofmap_L);

  // Initialise vector
  if (x.empty())
  {
    std::pair<std::size_t, std::size_t> local_range
      = dofmap_L->ownership_range();
    x.init(mesh.mpi_comm(), local_range);
  }

  // Eigen data structures
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> A;
  Eigen::VectorXd b, x_local;;
  std::shared_ptr<GenericVector> Gb = _x->factory().create_vector();

  // Globally assemble RHS
  if (this->_l->ufc_form())
    assemble(*Gb, *this->_l);

  // Assemble over cells
  Progress p("Performing local (cell-wise) solve", mesh.num_cells());
  ufc::cell ufc_cell;
  std::vector<double> vertex_coordinates;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update to current cell
    cell->get_vertex_coordinates(vertex_coordinates);
    cell->get_cell_data(ufc_cell);

    // Get local-to-global dof maps for cell
    const std::vector<dolfin::la_index>& dofs_a0
      = dofmap_a0->cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs_L
      = dofmap_L->cell_dofs(cell->index());

    // Resize b
    b.resize(dofs_L.size());

    // Extract local RHS info from global RHS
    (*Gb).get_local(b.data(), dofs_a0.size(), dofs_a0.data());

     // Solve local problem
    x_local = _lus[cell->index()].solve(b);

    // Set solution in global vector
    x.set_local(x_local.data(), dofs_a0.size(), dofs_a0.data());

    p++;
  }
  // Finalise vector
  x.apply("insert");
}
//----------------------------------------------------------------------------
void LocalSolver::solve(GenericVector& x, const Form& a, const Form& L,
                        bool symmetric) const
{
  UFC ufc_a(a);
  UFC ufc_L(L);

  // Raise error for Point integrals
  if (ufc_a.form.has_point_integrals() || ufc_L.form.has_point_integrals())
  {
    dolfin_error("LocalSolver.cpp",
                 "assemble system",
                 "Point integrals are not supported (yet)");
  }

  // Raise error for Point integrals
  if (ufc_L.form.has_interior_facet_integrals() ||
      ufc_L.form.has_exterior_facet_integrals())
  {
    dolfin_error("LocalSolver.cpp",
                 "assemble system",
                 "Facet integrals are not supported by this function. \r\n \
                 Use the local solver which reuses factorization in stead");
  }

  // Set timer
  Timer timer("Local solver");

  // Extract mesh
  const Mesh& mesh = a.mesh();

  // Form ranks
  const std::size_t rank_a = ufc_a.form.rank();
  const std::size_t rank_L = ufc_L.form.rank();

  // Check form ranks
  dolfin_assert(rank_a == 2);
  dolfin_assert(rank_L == 1);

  // Collect pointers to dof maps
  std::shared_ptr<const GenericDofMap> dofmap_a0
    = a.function_space(0)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_a1
    = a.function_space(1)->dofmap();
  std::shared_ptr<const GenericDofMap> dofmap_L
    = a.function_space(0)->dofmap();
  dolfin_assert(dofmap_a0);
  dolfin_assert(dofmap_a1);
  dolfin_assert(dofmap_L);

  // Initialise vector
  if (x.empty())
  {
    std::pair<std::size_t, std::size_t> local_range
      = dofmap_L->ownership_range();
    x.init(mesh.mpi_comm(), local_range);
  }

  // Cell integrals
  ufc::cell_integral* integral_a = ufc_a.default_cell_integral.get();
  ufc::cell_integral* integral_L = ufc_L.default_cell_integral.get();

  // Eigen data structures
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> A;
  Eigen::VectorXd b, x_local;

  // Assemble over cells
  Progress p("Performing local (cell-wise) solve", mesh.num_cells());
  ufc::cell ufc_cell;
  std::vector<double> vertex_coordinates;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update to current cell
    cell->get_vertex_coordinates(vertex_coordinates);
    cell->get_cell_data(ufc_cell);
    ufc_a.update(*cell, vertex_coordinates, ufc_cell,
                 integral_a->enabled_coefficients());
    ufc_L.update(*cell, vertex_coordinates, ufc_cell,
                 integral_L->enabled_coefficients());

    // Get local-to-global dof maps for cell
    const std::vector<dolfin::la_index>& dofs_a0
      = dofmap_a0->cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs_a1
      = dofmap_a1->cell_dofs(cell->index());
    const std::vector<dolfin::la_index>& dofs_L
      = dofmap_L->cell_dofs(cell->index());

    // Check that local problem is square and a and L match
    dolfin_assert(dofs_a0.size() == dofs_a1.size());
    dolfin_assert(dofs_a1.size() == dofs_L.size());

    // Resize A and b
    A.resize(dofs_a0.size(), dofs_a1.size());
    b.resize(dofs_L.size());

    // Tabulate A and b on cell
    integral_a->tabulate_tensor(A.data(),
                                ufc_a.w(),
                                vertex_coordinates.data(),
                                ufc_cell.orientation);
    integral_L->tabulate_tensor(b.data(),
                                ufc_L.w(),
                                vertex_coordinates.data(),
                                ufc_cell.orientation);

    // Solve local problem
    x_local = A.partialPivLu().solve(b);

    // Set solution in global vector
    x.set_local(x_local.data(), dofs_a0.size(), dofs_a0.data());

    p++;
  }

  // Finalise vector
  x.apply("insert");
}
//-----------------------------------------------------------------------------
