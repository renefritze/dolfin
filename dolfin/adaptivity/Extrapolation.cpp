// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-12-08
// Last changed: 2010-04-29
//
// Modified by Marie E. Rognes (meg@simula.no) 2010

#include <vector>
#include <boost/scoped_array.hpp>

#include <dolfin/common/Array.h>
#include <dolfin/log/log.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/la/LAPACKMatrix.h>
#include <dolfin/la/LAPACKVector.h>
#include <dolfin/la/LAPACKSolvers.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/BoundaryMesh.h>
#include <dolfin/mesh/FacetCell.h>
#include <dolfin/fem/BasisFunction.h>
#include <dolfin/fem/DofMap.h>
#include <dolfin/fem/DirichletBC.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/common/Timer.h>
#include "Extrapolation.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void Extrapolation::extrapolate(Function& w, const Function& v)
{
  // Using set_local for simplicity here
  not_working_in_parallel("Extrapolation");
  warning("Extrapolation not fully implemented yet.");

  // Check that the meshes are the same
  if (&w.function_space().mesh() != &v.function_space().mesh())
    error("Extrapolation must be computed on the same mesh.");

  // Extract mesh and function spaces
  const FunctionSpace& V(v.function_space());
  const FunctionSpace& W(w.function_space());
  const Mesh& mesh(V.mesh());

  // Initialize cell-cell connectivity
  const uint D = mesh.topology().dim();
  mesh.init(D, D);

  // UFC cell view of center cell
  UFCCell c0(mesh);

  // List of values for each dof of w (multivalued until we average)
  std::vector<std::vector<double> > coefficients;
  coefficients.resize(W.dim());

  // Local array for dof indices
  boost::scoped_array<uint> dofs(new uint[W.dofmap().max_local_dimension()]);

  // Iterate over cells in mesh
  for (CellIterator cell0(mesh); !cell0.end(); ++cell0)
  {
    // Update UFC view
    c0.update(*cell0);

    // Tabulate dofs for w on cell and store values
    W.dofmap().tabulate_dofs(dofs.get(), c0, cell0->index());

    // Compute coefficients on this cell
    uint offset = 0;
    compute_coefficients(coefficients, v, V, W, *cell0, c0, dofs, offset);
  }

  // Average coefficients
  average_coefficients(w, coefficients);
}
//-----------------------------------------------------------------------------
void Extrapolation::compute_coefficients(std::vector<std::vector<double> >& coefficients,
                                         const Function&v,
                                         const FunctionSpace& V,
                                         const FunctionSpace& W,
                                         const Cell& cell0,
                                         const ufc::cell& c0,
                                         const boost::scoped_array<uint>& dofs,
                                         uint& offset)
{
  // Call recursively for mixed elements
  uint num_sub_spaces = V.element().num_sub_elements();
  if (num_sub_spaces > 0)
  {
    for (uint k = 0; k < num_sub_spaces; k++)
      compute_coefficients(coefficients, v[k], *V[k], *W[k], cell0, c0, dofs, offset);
    return;
  }

  // Build data structures for keeping track of unique dofs
  std::map<uint, std::map<uint, uint> > cell2dof2row;
  std::set<uint> unique_dofs;
  build_unique_dofs(unique_dofs, cell2dof2row, cell0, c0, V);

  // Create linear system
  const uint N = W.element().space_dimension();
  const uint M = unique_dofs.size();
  LAPACKMatrix A(M, N);
  LAPACKVector b(M);

  // Add equations on cell and neighboring cells
  add_cell_equations(A, b, cell0, cell0, c0, c0, V, W, v, cell2dof2row[cell0.index()]);
  UFCCell c1(V.mesh());
  for (CellIterator cell1(cell0); !cell1.end(); ++cell1)
  {
    if (cell2dof2row[cell1->index()].size() == 0)
      continue;

    c1.update(*cell1);
    add_cell_equations(A, b, cell0, *cell1, c0, c1, V, W, v, cell2dof2row[cell1->index()]);
  }

  // Solve least squares system
  LAPACKSolvers::solve_least_squares(A, b);

  // Insert resulting coefficients into global coefficient vector
  for (uint i = 0; i < W.dofmap().local_dimension(c0); ++i)
    coefficients[dofs[i + offset]].push_back(b[i]);

  // Increase offset
  offset += W.dofmap().local_dimension(c0);
}
//-----------------------------------------------------------------------------
void Extrapolation::build_unique_dofs(std::set<uint>& unique_dofs,
                                      std::map<uint, std::map<uint, uint> >& cell2dof2row,
                                      const Cell& cell0,
                                      const ufc::cell& c0,
                                      const FunctionSpace& V)
{
  // Counter for matrix row index
  uint row = 0;
  UFCCell c1(V.mesh());

  // Compute unique dofs on center cell
  cell2dof2row[cell0.index()] = compute_unique_dofs(cell0, c0, V, row, unique_dofs);

  // Compute unique dofs on neighbouring cells
  for (CellIterator cell1(cell0); !cell1.end(); ++cell1)
  {
    c1.update(*cell1);
    cell2dof2row[cell1->index()] = compute_unique_dofs(*cell1, c1, V, row, unique_dofs);
  }
}
//-----------------------------------------------------------------------------
void Extrapolation::add_cell_equations(LAPACKMatrix& A,
                                       LAPACKVector& b,
                                       const Cell& cell0,
                                       const Cell& cell1,
                                       const ufc::cell& c0,
                                       const ufc::cell& c1,
                                       const FunctionSpace& V,
                                       const FunctionSpace& W,
                                       const Function& v,
                                       std::map<uint, uint>& dof2row)
{
  // Extract coefficents for v on patch cell
  boost::scoped_array<double> dof_values(new double[V.element().space_dimension()]);
  v.restrict(dof_values.get(), V.element(), cell1, c1, -1);

  // Iterate over given local dofs for V on patch cell
  uint row;
  uint i;
  for (std::map<uint, uint>::iterator it = dof2row.begin(); it!= dof2row.end(); it++) {
    i = it->first;
    row = it->second;

    // Iterate over basis functions for W on center cell
    for (uint j = 0; j < W.element().space_dimension(); ++j) {

      // Create basis function
      const BasisFunction phi(j, W.element(), c0);

      // Evaluate dof on basis function
      const double dof_value = V.element().evaluate_dof(i, phi, c1);

      // Insert dof_value into matrix
      A(row, j) = dof_value;
    }

    // Insert coefficient into vector
    b[row] = dof_values[i];
  }
}
//-----------------------------------------------------------------------------
std::map<dolfin::uint, dolfin::uint>
Extrapolation::compute_unique_dofs(const Cell& cell, const ufc::cell& c,
                                   const FunctionSpace& V,
                                   uint& row,
                                   std::set<uint>& unique_dofs)
{
  boost::scoped_array<uint> dofs(new uint[V.dofmap().local_dimension(c)]);
  V.dofmap().tabulate_dofs(dofs.get(), c, cell.index());

  // Data structure for current cell
  std::map<uint, uint> dof2row;

  for (uint i = 0; i < V.dofmap().local_dimension(c); ++i)
  {
    // Ignore if this degree of freedom is already considered
    if (unique_dofs.find(dofs[i]) != unique_dofs.end())
      continue;

    // Put global index into unique_dofs
    unique_dofs.insert(dofs[i]);

    // Map local dof index to current matrix row-index
    dof2row[i] = row;

    // Increase row index
    row++;
  }

  return dof2row;
}
//-----------------------------------------------------------------------------
void Extrapolation::average_coefficients(Function& w,
                                         std::vector<std::vector<double> >& coefficients)
{
  const FunctionSpace& W = w.function_space();
  Array<double> dof_values(W.dim());

  for (uint i = 0; i < W.dim(); i++)
  {
    double s = 0.0;
    for (uint j = 0; j < coefficients[i].size(); ++j)
      s += coefficients[i][j];
    s /= static_cast<double>(coefficients[i].size());
    dof_values[i] = s;
  }

  // Update dofs for w
  w.vector().set_local(dof_values);
}
//-----------------------------------------------------------------------------
