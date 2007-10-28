// Copyright (C) 2007 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristian Oelgaard, 2007
//
// First added:  2007-04-10
// Last changed: 2007-10-28

#include <dolfin/log.h>
#include <dolfin/Mesh.h>
#include <dolfin/Vertex.h>
#include <dolfin/Cell.h>
#include <dolfin/Facet.h>
#include <dolfin/Point.h>
#include <dolfin/SubDomain.h>
#include <dolfin/Form.h>
#include <dolfin/UFCMesh.h>
#include <dolfin/UFCCell.h>
#include <dolfin/GenericMatrix.h>
#include <dolfin/GenericVector.h>
#include <dolfin/SubSystem.h>
#include <dolfin/DirichletBC.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(Function& g,
                         Mesh& mesh,
                         SubDomain& sub_domain,
                         BCMethod method)
  : BoundaryCondition(), g(g), mesh(mesh),
    sub_domains(0), sub_domain(0), sub_domains_local(false), method(method)
{
  // Initialize sub domain markers
  init(sub_domain);
}
//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(Function& g,
                         MeshFunction<uint>& sub_domains,
                         uint sub_domain,
                         BCMethod method)
  : BoundaryCondition(), g(g), mesh(sub_domains.mesh()),
    sub_domains(&sub_domains), sub_domain(sub_domain), sub_domains_local(false),
    method(method)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(Function& g,
                         Mesh& mesh,
                         SubDomain& sub_domain,
                         const SubSystem& sub_system,
                         BCMethod method)
  : BoundaryCondition(), g(g), mesh(mesh),
    sub_domains(0), sub_domain(0), sub_domains_local(false),
    sub_system(sub_system), method(method)
{
  // Set sub domain markers
  init(sub_domain);
}
//-----------------------------------------------------------------------------
DirichletBC::DirichletBC(Function& g,
                         MeshFunction<uint>& sub_domains,
                         uint sub_domain,
                         const SubSystem& sub_system,
                         BCMethod method)
  : g(g), mesh(sub_domains.mesh()),
    sub_domains(&sub_domains), sub_domain(sub_domain), sub_domains_local(false),
    sub_system(sub_system), method(method)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
DirichletBC::~DirichletBC()
{
  // Delete sub domain markers if created locally
  if ( sub_domains_local )
    delete sub_domains;
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b,
                        const Form& form)
{
  apply(A, b, 0, form.form());
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b,
                        const ufc::form& form)
{
  apply(A, b, 0, form);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b,
                        const GenericVector& x, const Form& form)
{
  apply(A, b, &x, form.form());
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b,
                        const GenericVector& x, const ufc::form& form)
{
  apply(A, b, &x, form);
}
//-----------------------------------------------------------------------------
void DirichletBC::apply(GenericMatrix& A, GenericVector& b,
                        const GenericVector* x, const ufc::form& form)
{
  // FIXME: How do we reuse the dof map for u?
  // FIXME: Perhaps we should make DofMapSet a member of Form?

  if (method == topological)
    message("Applying Dirichlet boundary conditions to linear system.");
  else
    message("Applying Dirichlet boundary conditions to linear system (geometrical approach).");
  
  // Make sure we have the facet - cell connectivity
  const uint D = mesh.topology().dim();
  if (method == topological)
    mesh.init(D - 1, D);

  // Create local data for application of boundary conditions
  BoundaryCondition::LocalData data(form, mesh, sub_system);
  
  // A map to hold the mapping from boundary dofs to boundary values
  std::map<uint, real> boundary_values;

  // Iterate over the facets of the mesh
  Progress p("Applying Dirichlet boundary conditions", mesh.size(D - 1));
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Skip facets not inside the sub domain
    if ((*sub_domains)(*facet) != sub_domain)
    {
      p++;
      continue;
    }

    // Chose strategy
    if (method == topological)
      computeBCTopological(boundary_values, *facet, data);
    else
      computeBCGeometrical(boundary_values, *facet, data);
    
    // Update process
    p++;
  }

  // Copy boundary value data to arrays
  uint* dofs = new uint[boundary_values.size()];
  real* values = new real[boundary_values.size()];
  std::map<uint, real>::const_iterator boundary_value;
  uint i = 0;
  for (boundary_value = boundary_values.begin(); boundary_value != boundary_values.end(); ++boundary_value)
  {
    dofs[i] = boundary_value->first;
    values[i++] = boundary_value->second;
  }

  // Modify boundary values for nonlinear problems
  if (x)
  {
    real* x_values = new real[boundary_values.size()];
    x->get(x_values, boundary_values.size(), dofs);
    for (uint i = 0; i < boundary_values.size(); i++)
      values[i] -= x_values[i];
  }
  
  // Modify RHS vector (b[i] = value)
  b.set(values, boundary_values.size(), dofs);

  // Modify linear system (A_ii = 1)
  A.ident(boundary_values.size(), dofs);

  // Clear temporary arrays
  delete [] dofs;
  delete [] values;

  // Finalise changes to b
  b.apply();
}
//-----------------------------------------------------------------------------
void DirichletBC::init(SubDomain& sub_domain)
{
  cout << "Creating sub domain markers for boundary condition." << endl;

  // Create mesh function for sub domain markers on facets
  mesh.init(mesh.topology().dim() - 1);
  sub_domains = new MeshFunction<uint>(mesh, mesh.topology().dim() - 1);
  sub_domains_local = true;

  // Mark everything as sub domain 1
  (*sub_domains) = 1;
  
  // Mark the sub domain as sub domain 0
  sub_domain.mark(*sub_domains, 0);
}
//-----------------------------------------------------------------------------
void DirichletBC::computeBCTopological(std::map<uint, real>& boundary_values,
                                       Facet& facet,
                                       BoundaryCondition::LocalData& data)
{
  // Get cell to which facet belongs (there may be two, but pick first)
  Cell cell(mesh, facet.entities(facet.dim() + 1)[0]);
  UFCCell ufc_cell(cell);
  
  // Get local index of facet with respect to the cell
  const uint local_facet = cell.index(facet);
  
  // Interpolate function on cell
  g.interpolate(data.w, ufc_cell, *data.finite_element, cell, local_facet);
  
  // Tabulate dofs on cell
  data.dof_map->tabulate_dofs(data.cell_dofs, data.ufc_mesh, ufc_cell);
  
  // Tabulate which dofs are on the facet
  data.dof_map->tabulate_facet_dofs(data.facet_dofs, local_facet);
  
  // Pick values for facet
  for (uint i = 0; i < data.dof_map->num_facet_dofs(); i++)
  {
    const uint dof = data.offset + data.cell_dofs[data.facet_dofs[i]];
    const real value = data.w[data.facet_dofs[i]];
    boundary_values[dof] = value;
  }
}
//-----------------------------------------------------------------------------
void DirichletBC::computeBCGeometrical(std::map<uint, real>& boundary_values,
                                       Facet& facet,
                                       BoundaryCondition::LocalData& data)
{
  // Loop the vertices associated with the facet
  for (VertexIterator vertex(facet); !vertex.end(); ++vertex)
  {
    // Loop the cells associated with the vertex
    for (CellIterator c(*vertex); !c.end(); ++c)
    {
      UFCCell ufc_cell(*c);
      
      // Interpolate function on cell
      g.interpolate(data.w, ufc_cell, *data.finite_element, *c);
      
      // Tabulate dofs on cell, and their coordinates
      data.dof_map->tabulate_dofs(data.cell_dofs, data.ufc_mesh, ufc_cell);
      data.dof_map->tabulate_coordinates(data.coordinates, ufc_cell);
      
      // Loop all dofs on cell
      for (uint i = 0; i < data.dof_map->local_dimension(); ++i)
      {
        // Check if the coordinates are on current facet and thus on boundary
        if (!onFacet(data.coordinates[i], facet))
          continue;
        
        // Set boundary value
        const uint dof = data.offset + data.cell_dofs[i];
        const real value = data.w[i];
        boundary_values[dof] = value;
      }
    }
  }
}
//-----------------------------------------------------------------------------
//bool DirichletBC::onFacet(const Point& p, Cell& facet)
bool DirichletBC::onFacet(real* coordinates, Facet& facet)
{
//std::cout << "calling onFacet" << std::endl;
//std::cout << "facet.dim: " << facet.dim() << std::endl;

  // Get mesh geometry and vertices of facet
//  const MeshGeometry& geometry = facet.mesh().geometry();
//  const uint* vertices = facet.entities(0);

  if (facet.dim() != 2)
    error("Only implemented in 2D, ask Kristian to implement 3D. ;-)");

  if ( facet.dim() == 1 )
  {
    // Check if the coordinates are on the same line as the line segment

    // Create points
    Point p(coordinates[0], coordinates[1]);
    Point v0 = Vertex(facet.mesh(), facet.entities(0)[0]).point();
    Point v1 = Vertex(facet.mesh(), facet.entities(0)[1]).point();

//    dolfin::cout << "p: " << p << dolfin::endl;
//    dolfin::cout << "v0: " << v0 << dolfin::endl;
//    dolfin::cout << "v1: " << v1 << dolfin::endl;

    Point v01 = v1 - v0;
    Point v0p = p - v0;
    // Check if the point is on the line that intersects v0 and v1, and check
    // if the distance from v0 to p is smaller than the distance v0 to v1
    if (v01.cross(v0p).norm() < DOLFIN_EPS and (v0p.norm() - v01.norm()) < DOLFIN_EPS)
      return true;
    else
      return false;
  }
  else if ( facet.dim() == 2 )
  {
    // Check if the coordinates are in the same plane as the triangular facet
    Point p(coordinates[0], coordinates[1], coordinates[2]);
//    Point v0  = geometry.point(vertices[0]);
//    Point v1  = geometry.point(vertices[1]);
//    Point v2  = geometry.point(vertices[2]);
//    Point v01 = v1 - v0;
//    Point v02 = v2 - v0;
//    Point v0p = p - v0;
//    Point n   = v01.cross(v02);
//    return std::abs(n.dot(v0p)) < DOLFIN_EPS;
  return false;
  }

  error("Unable to determine if given point is on facet.");

  return false;
}
//-----------------------------------------------------------------------------

