// Copyright (C) 2007-2008 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristian Oelgaard, 2008
// Modified by Martin Sandve Alnes, 2008
//
// First added:  2007-04-10
// Last changed: 2009-02-26

#include <boost/assign/list_of.hpp>

#include <dolfin/common/constants.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/log/log.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshData.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/mesh/Vertex.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/Point.h>
#include <dolfin/mesh/SubDomain.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/GenericVector.h>
#include "DofMap.h"
#include "FiniteElement.h"
#include "UFCMesh.h"
#include "UFCCell.h"
#include "DirichletBC.h"

using namespace dolfin;

const std::set<std::string> DirichletBC::methods = boost::assign::list_of("topological")("geometric")("pointwise");

//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(const FunctionSpace& V,
                         const Function& g,
                         const SubDomain& sub_domain,
                         std::string method)
  : BoundaryCondition(V),
    g(g),
    method(method), user_sub_domain(&sub_domain)
{
  check();
  init_from_sub_domain(sub_domain);
}
//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(const FunctionSpace& V,
                         const Function& g,
                         const MeshFunction<uint>& sub_domains,
                         uint sub_domain,
                         std::string method)
  : BoundaryCondition(V),
    g(g),
    method(method), user_sub_domain(0)
{
  check();
  init_from_mesh_function(sub_domains, sub_domain);
}
//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(const FunctionSpace& V,
                         const Function& g,
                         uint sub_domain,
                         std::string method)
  : BoundaryCondition(V),
    g(g),
    method(method), user_sub_domain(0)
{
  check();
  init_from_mesh(sub_domain);
}
//-----------------------------------------------------------------------------
DirichletBC::~DirichletBC()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A) const
{
  apply(&A, 0, 0);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericVector& b) const
{
  apply(0, &b, 0);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b) const
{
  apply(&A, &b, 0);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericVector& b, const GenericVector& x) const
{
  apply(0, &b, &x);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A,
                        GenericVector& b,
                        const GenericVector& x) const
{
  apply(&A, &b, &x);
}
//-----------------------------------------------------------------------------
void DirichletBC::zero(GenericMatrix& A) const
{
  // A map to hold the mapping from boundary dofs to boundary values
  std::map<uint, double> boundary_values;

  // Create local data for application of boundary conditions
  BoundaryCondition::LocalData data(*V);

  // Compute dofs and values
  compute_bc(boundary_values, data);

  // Copy boundary value data to arrays
  uint* dofs = new uint[boundary_values.size()];
  std::map<uint, double>::const_iterator boundary_value;
  uint i = 0;
  for (boundary_value = boundary_values.begin(); boundary_value != boundary_values.end(); ++boundary_value)
    dofs[i++] = boundary_value->first;

  // Modify linear system (A_ii = 1)
  A.zero(boundary_values.size(), dofs);

  // Finalise changes to A
  A.apply();

  // Clear temporary arrays
  delete [] dofs;
}
//-----------------------------------------------------------------------------
bool DirichletBC::is_compatible(Function& v) const
{
  // This function only checks the values at vertices when it should
  // really check that the dof functionals agree. The check here is
  // neither necessary nor sufficient to guarantee compatible boundary
  // boundary conditions but a more robust test requires access to the
  // function space.

  // Compute value size
  uint size = 1;
  const uint rank = g.function_space().element().value_rank();
  for (uint i = 0; i < rank ; i++)
    size *= g.function_space().element().value_dimension(i);
  double* g_values = new double[size];
  double* v_values = new double[size];

  // Get mesh
  const Mesh& mesh = V->mesh();

  // Iterate over facets
  for (uint f = 0; f < facets.size(); f++)
  {
    // Create cell and facet
    uint cell_number  = facets[f].first;
    uint facet_number = facets[f].second;
    Cell cell(mesh, cell_number);
    Facet facet(mesh, facet_number);

    // Make cell and facet available to user-defined function
    error("Does the new Function class need an 'update' function?");
    //g.update(cell, facet_number);
    //v.update(cell, facet_number);

    // Iterate over facet vertices
    for (VertexIterator vertex(facet); !vertex.end(); ++vertex)
    {
      // Evaluate g and v at vertex
      g.eval(g_values, vertex->x());
      v.eval(v_values, vertex->x());

      // Check values
      for (uint i = 0; i < size; i++)
      {
        if (std::abs(g_values[i] - v_values[i]) > DOLFIN_EPS)
        {
          Point p(mesh.geometry().dim(), vertex->x());
          cout << "Incompatible function value " << v_values[i] << " at x = " << p << ", should be " << g_values[i] << "." << endl;
          delete [] g_values;
          delete [] v_values;
          return false;
        }
      }
    }
  }

  delete [] g_values;
  delete [] v_values;
  return true;
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix* A,
                        GenericVector* b,
                        const GenericVector* x) const
{
  // Check arguments
  check(A, b, x);

  // A map to hold the mapping from boundary dofs to boundary values
  std::map<uint, double> boundary_values;

  // Create local data for application of boundary conditions
  BoundaryCondition::LocalData data(*V);

  // Compute dofs and values
  compute_bc(boundary_values, data);

  // Copy boundary value data to arrays
  uint* dofs   = new uint[boundary_values.size()];
  double* values = new double[boundary_values.size()];
  std::map<uint, double>::const_iterator boundary_value;
  uint i = 0;
  for (boundary_value = boundary_values.begin(); boundary_value != boundary_values.end(); ++boundary_value)
  {
    dofs[i]     = boundary_value->first;
    values[i++] = boundary_value->second;
  }

  // Modify boundary values for nonlinear problems
  if (x)
  {
    double* x_values = new double[boundary_values.size()];
    x->get(x_values, boundary_values.size(), dofs);
    for (uint i = 0; i < boundary_values.size(); i++)
      values[i] = x_values[i] - values[i];
    delete [] x_values;
  }

  info("Applying boundary conditions to linear system.");

  // Modify RHS vector (b[i] = value) and apply changes
  if (b)
  {
    b->set(values, boundary_values.size(), dofs);
    b->apply();
  }

  // Modify linear system (A_ii = 1) and apply changes
  if (A)
  {
    A->ident(boundary_values.size(), dofs);
    A->apply();
  }

  // Clear temporary arrays
  delete [] dofs;
  delete [] values;
}
//-----------------------------------------------------------------------------
void DirichletBC::check() const
{
  // Check that function is in function space
  //if (!g.in(*V))
  //  error("Unable to create boundary condition, boundary value function is not in trial space.");

  // Check that boundary condition method is known
  if (methods.count(method) == 0)
    error("Unknown method for applying Dirichlet boundary condtions."); 


  // Check that the mesh is ordered
  if (!V->mesh().ordered())
    error("Unable to create boundary condition, mesh is not correctly ordered (consider calling mesh.order()).");
}
//-----------------------------------------------------------------------------
void DirichletBC::check(GenericMatrix* A,
                        GenericVector* b,
                        const GenericVector* x) const
{
  dolfin_assert(V);

  // Check matrix and vector dimensions
  if (A && x && A->size(0) != x->size())
    error("Matrix dimension (%d rows) does not match vector dimension (%d) for application of boundary conditions.",
          A->size(0), x->size());
  if (A && b && A->size(0) != b->size())
    error("Matrix dimension (%d rows) does not match vector dimension (%d) for application of boundary conditions.",
          A->size(0), b->size());
  if (x && b && x->size() != b->size())
    error("Vector dimension (%d rows) does not match vector dimension (%d) for application of boundary conditions.",
          x->size(), b->size());

  // Check dimension of function space
  if (A && A->size(0) < V->dim())
    error("Dimension of function space (%d) too large for application to linear system (%d rows).",
          V->dim(), A->size(0));
  if (x && x->size() < V->dim())
    error("Dimension of function space (%d) too large for application to linear system (%d rows).",
          V->dim(), x->size());
  if (b && b->size() < V->dim())
    error("Dimension of function space (%d) too large for application to linear system (%d rows).",
          V->dim(), b->size());

  // FIXME: Check case A.size() > V->dim() for subspaces
}
//-----------------------------------------------------------------------------
void DirichletBC::init_from_sub_domain(const SubDomain& sub_domain)
{
  dolfin_assert(facets.size() == 0);

  // FIXME: This can be made more efficient, we should be able to
  // FIXME: extract the facets without first creating a MeshFunction on
  // FIXME: the entire mesh and then extracting the subset. This is done
  // FIXME: mainly for convenience (we may reuse mark() in SubDomain).

  // Create mesh function for sub domain markers on facets
  const uint dim = V->mesh().topology().dim();
  V->mesh().init(dim - 1);
  MeshFunction<uint> sub_domains(const_cast<Mesh&>(V->mesh()), dim - 1);

  // Set geometric dimension (needed for SWIG interface)
  sub_domain._geometric_dimension = V->mesh().geometry().dim();

  // Mark everything as sub domain 1
  sub_domains = 1;

  // Mark the sub domain as sub domain 0
  sub_domain.mark(sub_domains, 0);

  // Initialize from mesh function
  init_from_mesh_function(sub_domains, 0);
}
//-----------------------------------------------------------------------------
void DirichletBC::init_from_mesh_function(const MeshFunction<uint>& sub_domains,
                                          uint sub_domain)
{
  dolfin_assert(facets.size() == 0);

  // Make sure we have the facet - cell connectivity
  const uint dim = V->mesh().topology().dim();
  V->mesh().init(dim - 1, dim);

  // Build set of boundary facets
  for (FacetIterator facet(V->mesh()); !facet.end(); ++facet)
  {
    // Skip facets not on this boundary
    if (sub_domains(*facet) != sub_domain)
      continue;

    // Get cell to which facet belongs (there may be two, but pick first)
    Cell cell(V->mesh(), facet->entities(dim)[0]);

    // Get local index of facet with respect to the cell
    const uint facet_number = cell.index(*facet);

    // Copy data
    facets.push_back(std::pair<uint, uint>(cell.index(), facet_number));
  }
}
//-----------------------------------------------------------------------------
void DirichletBC::init_from_mesh(uint sub_domain)
{
  dolfin_assert(facets.size() == 0);

  // For this to work, the mesh *needs* to be ordered according to
  // the UFC ordering before it gets here. So reordering the mesh
  // here will either have no effect (if the mesh is already ordered
  // or it won't do anything good (since the markers are wrong anyway).
  // In conclusion: we don't need to order the mesh here.

  cout << "Creating sub domain markers for boundary condition." << endl;

  // Get data
  std::vector<uint>* facet_cells   = const_cast<Mesh&>(V->mesh()).data().array("boundary facet cells");
  std::vector<uint>* facet_numbers = const_cast<Mesh&>(V->mesh()).data().array("boundary facet numbers");
  std::vector<uint>* indicators    = const_cast<Mesh&>(V->mesh()).data().array("boundary indicators");

  // Check data
  if (!facet_cells)
    error("Mesh data \"boundary facet cells\" not available.");
  if (!facet_numbers)
    error("Mesh data \"boundary facet numbers\" not available.");
  if (!indicators)
    error("Mesh data \"boundary indicators\" not available.");

  // Get size
  const uint size = facet_cells->size();
  dolfin_assert(size == facet_numbers->size());
  dolfin_assert(size == indicators->size());

  // Build set of boundary facets
  for (uint i = 0; i < size; i++)
  {
    // Skip facets not on this boundary
    if ((*indicators)[i] != sub_domain)
      continue;

    // Copy data
    facets.push_back(std::pair<uint, uint>((*facet_cells)[i], (*facet_numbers)[i]));
  }
}
//-----------------------------------------------------------------------------
void DirichletBC::compute_bc(std::map<uint, double>& boundary_values,
                             BoundaryCondition::LocalData& data) const
{
  // Choose strategy
  if (method == "topological")
    compute_bc_topological(boundary_values, data);
  else if (method == "geometric")
    compute_bc_geometric(boundary_values, data);
  else if (method == "pointwise")
    compute_bc_pointwise(boundary_values, data);
  else
    error("Unknown method for application of boundary conditions.");
}
//-----------------------------------------------------------------------------
void DirichletBC::compute_bc_topological(std::map<uint, double>& boundary_values,
                                         BoundaryCondition::LocalData& data) const
{
  // Special case
  if (facets.size() == 0)
  {
    warning("Found no facets matching domain for boundary condition.");
    return;
  }

  // Get mesh and dofmap
  const Mesh& mesh = V->mesh();
  const DofMap& dofmap = V->dofmap();

  // Iterate over facets
  Progress p("Computing Dirichlet boundary values, topological search", facets.size());
  for (uint f = 0; f < facets.size(); f++)
  {
    // Get cell number and local facet number
    uint cell_number = facets[f].first;
    uint facet_number = facets[f].second;

    // Create cell
    Cell cell(mesh, cell_number);
    UFCCell ufc_cell(cell);

    // Interpolate function on cell
    g.interpolate(data.w, *V, ufc_cell, cell.index(), facet_number);

    // Tabulate dofs on cell
    dofmap.tabulate_dofs(data.cell_dofs, ufc_cell, cell_number);

    // Tabulate which dofs are on the facet
    dofmap.tabulate_facet_dofs(data.facet_dofs, facet_number);

    // Debugging print:
    /*
       cout << endl << "Handling BC's for:" << endl;
       cout << "Cell:  " << facet.entities(facet.dim() + 1)[0] << endl;
       cout << "Facet: " << local_facet << endl;
    */

    // Pick values for facet
    for (uint i = 0; i < dofmap.num_facet_dofs(); i++)
    {
      const uint dof = dofmap.offset() + data.cell_dofs[data.facet_dofs[i]];
      const double value = data.w[data.facet_dofs[i]];
      boundary_values[dof] = value;
      //cout << "Setting BC value: i = " << i << ", dof = " << dof << ", value = " << value << endl;
    }

    p++;
  }
}
//-----------------------------------------------------------------------------
void DirichletBC::compute_bc_geometric(std::map<uint, double>& boundary_values,
                                       BoundaryCondition::LocalData& data) const
{
  // Special case
  if (facets.size() == 0)
  {
    warning("Found no facets matching domain for boundary condition.");
    return;
  }

  // Get mesh and dofmap
  const Mesh& mesh = V->mesh();
  const DofMap& dofmap = V->dofmap();

  // Initialize facets, needed for geometric search
  info("Computing facets, needed for geometric application of boundary conditions.");
  mesh.init(mesh.topology().dim() - 1);

  // Iterate over facets
  Progress p("Computing Dirichlet boundary values, geometric search", facets.size());
  for (uint f = 0; f < facets.size(); f++)
  {
    // Get cell number and local facet number
    uint cell_number = facets[f].first;
    uint facet_number = facets[f].second;

    // Create facet
    Cell cell(mesh, cell_number);
    Facet facet(mesh, cell.entities(mesh.topology().dim() - 1)[facet_number]);

    // Loop the vertices associated with the facet
    for (VertexIterator vertex(facet); !vertex.end(); ++vertex)
    {
      // Loop the cells associated with the vertex
      for (CellIterator c(*vertex); !c.end(); ++c)
      {
        UFCCell ufc_cell(*c);

        bool interpolated = false;

        // Tabulate coordinates of dofs on cell
        dofmap.tabulate_coordinates(data.coordinates, ufc_cell);

        // Loop over all dofs on cell
        for (uint i = 0; i < dofmap.local_dimension(ufc_cell); ++i)
        {
          // Check if the coordinates are on current facet and thus on boundary
          if (!on_facet(data.coordinates[i], facet))
            continue;

          if(!interpolated)
          {
            // Tabulate dofs on cell
            dofmap.tabulate_dofs(data.cell_dofs, ufc_cell, c->index());

            // Interpolate function on cell
            g.interpolate(data.w, *V, ufc_cell, c->index());
          }

          // Set boundary value
          const uint dof = dofmap.offset() + data.cell_dofs[i];
          const double value = data.w[i];
          boundary_values[dof] = value;
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
void DirichletBC::compute_bc_pointwise(std::map<uint, double>& boundary_values,
                                       BoundaryCondition::LocalData& data) const
{
  dolfin_assert(user_sub_domain);

  // Get mesh and dofmap
  const Mesh& mesh = V->mesh();
  const DofMap& dofmap = V->dofmap();

  // Iterate over cells
  Progress p("Computing Dirichlet boundary values, pointwise search", mesh.num_cells());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    UFCCell ufc_cell(*cell);

    // Tabulate coordinates of dofs on cell
    dofmap.tabulate_coordinates(data.coordinates, ufc_cell);

    // Interpolate function only once and only on cells where necessary
    bool interpolated = false;

    // Loop all dofs on cell
    for (uint i = 0; i < dofmap.local_dimension(ufc_cell); ++i)
    {
      // Check if the coordinates are part of the sub domain
      if ( !user_sub_domain->inside(data.coordinates[i], false) )
        continue;

      if (!interpolated)
      {
        interpolated = true;

        // Tabulate dofs on cell
        dofmap.tabulate_dofs(data.cell_dofs, ufc_cell, cell->index());

        // Interpolate function on cell
        g.interpolate(data.w, *V, ufc_cell, cell->index());
      }

      // Set boundary value
      const uint dof = dofmap.offset() + data.cell_dofs[i];
      const double value = data.w[i];
      boundary_values[dof] = value;
    }

    p++;
  }
}
//-----------------------------------------------------------------------------
bool DirichletBC::on_facet(double* coordinates, Facet& facet) const
{
  // Check if the coordinates are on the same line as the line segment
  if ( facet.dim() == 1 )
  {
    // Create points
    Point p(coordinates[0], coordinates[1]);
    Point v0 = Vertex(facet.mesh(), facet.entities(0)[0]).point();
    Point v1 = Vertex(facet.mesh(), facet.entities(0)[1]).point();

    // Create vectors
    Point v01 = v1 - v0;
    Point vp0 = v0 - p;
    Point vp1 = v1 - p;

    // Check if the length of the sum of the two line segments vp0 and vp1 is
    // equal to the total length of the facet
    if ( std::abs(v01.norm() - vp0.norm() - vp1.norm()) < DOLFIN_EPS )
      return true;
    else
      return false;
  }
  // Check if the coordinates are in the same plane as the triangular facet
  else if ( facet.dim() == 2 )
  {
    // Create points
    Point p(coordinates[0], coordinates[1], coordinates[2]);
    Point v0 = Vertex(facet.mesh(), facet.entities(0)[0]).point();
    Point v1 = Vertex(facet.mesh(), facet.entities(0)[1]).point();
    Point v2 = Vertex(facet.mesh(), facet.entities(0)[2]).point();

    // Create vectors
    Point v01 = v1 - v0;
    Point v02 = v2 - v0;
    Point vp0 = v0 - p;
    Point vp1 = v1 - p;
    Point vp2 = v2 - p;

    // Check if the sum of the area of the sub triangles is equal to the total
    // area of the facet
    if ( std::abs(v01.cross(v02).norm() - vp0.cross(vp1).norm() - vp1.cross(vp2).norm()
        - vp2.cross(vp0).norm()) < DOLFIN_EPS )
      return true;
    else
      return false;
  }

  error("Unable to determine if given point is on facet (not implemented for given facet dimension).");

  return false;
}
//-----------------------------------------------------------------------------
void DirichletBC::get_bc(uint* indicators, double* values) const
{
  // A map to hold the mapping from boundary dofs to boundary values
  std::map<uint, double> boundary_values;

  // Create local data for application of boundary conditions
  BoundaryCondition::LocalData data(*V);

  // Compute dofs and values
  compute_bc(boundary_values, data);

  std::map<uint, double>::const_iterator boundary_value;
  uint i = 0;
  for (boundary_value = boundary_values.begin(); boundary_value != boundary_values.end(); ++boundary_value)
  {
    i = boundary_value->first;
    indicators[i] = 1;
    values[i] = boundary_value->second;
  }
}
//-----------------------------------------------------------------------------
