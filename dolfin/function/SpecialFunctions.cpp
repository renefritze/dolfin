// Copyright (C) 2006-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristian B. Oelgaard, 2007, 2008.
// Modified by Martin Sandve Alnes, 2008.
// Modified by Garth N. Wells, 2008.
//
// First added:  2008-07-17
// Last changed: 2008-10-28

#include <dolfin/common/constants.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/fem/Form.h>
#include <dolfin/fem/UFC.h>
#include "FunctionSpace.h"
#include "SpecialFunctions.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
MeshSize::MeshSize(const FunctionSpace& V) : Function(V)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
double MeshSize::eval(const double* x) const
{
  return cell().diameter();
}
//-----------------------------------------------------------------------------
double MeshSize::min() const
{
  CellIterator c(function_space().mesh());
  double hmin = c->diameter();
  for (; !c.end(); ++c)
    hmin = std::min(hmin, c->diameter());
  return hmin;
}
//-----------------------------------------------------------------------------
double MeshSize::max() const
{
  CellIterator c(function_space().mesh());
  double hmax = c->diameter();
  for (; !c.end(); ++c)
    hmax = std::max(hmax, c->diameter());
  return hmax;
}
//-----------------------------------------------------------------------------
InvMeshSize::InvMeshSize(Mesh& mesh) : Function(), mesh(mesh) 
{
  // Do nothing
}
//-----------------------------------------------------------------------------
double InvMeshSize::eval(const double* x) const
{
  return 1.0 / cell().diameter();
}
//-----------------------------------------------------------------------------
AvgMeshSize::AvgMeshSize(const FunctionSpace& V) : Function(V)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
double AvgMeshSize::eval(const double* x) const
{
  // If there is no facet (assembling on interior), return cell diameter
  if (!on_facet())
    return cell().diameter();
  else
  {
    // Create facet from the global facet number
    Facet facet0(function_space().mesh(), cell().entities(cell().mesh().topology().dim() - 1)[facet()]);

    // If there are two cells connected to the facet
    if (facet0.numEntities(cell().mesh().topology().dim()) == 2)
    {
      // Create the two connected cells and return the average of their diameter
      Cell cell0(function_space().mesh(), facet0.entities(cell().mesh().topology().dim())[0]);
      Cell cell1(function_space().mesh(), facet0.entities(cell().mesh().topology().dim())[1]);

      return (cell0.diameter() + cell1.diameter())/2.0;
    }
    // Else there is only one cell connected to the facet and the average is 
    // the cell diameter
    else
      return cell().diameter();
  }
}
//-----------------------------------------------------------------------------
FacetNormal::FacetNormal(const FunctionSpace& V) : Function(V)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void FacetNormal::eval(double* values, const double* x) const
{
  if (on_facet())
  {
    for (uint i = 0; i < cell().dim(); i++)
      values[i] = cell().normal(facet(), i);
  }
  else
  {
    for (uint i = 0; i < cell().dim(); i++)
      values[i] = 0.0;
  }
}
//-----------------------------------------------------------------------------
dolfin::uint FacetNormal::rank() const
{
  return 1;
}
//-----------------------------------------------------------------------------    
dolfin::uint FacetNormal::dim(uint i) const
{
  if(i > 0)
    error("Invalid dimension %d in FacetNormal::dim.", i);
  return function_space().mesh().geometry().dim();
}
//-----------------------------------------------------------------------------
FacetArea::FacetArea(Mesh& mesh) : Function(), mesh(mesh)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void FacetArea::eval(double* values, const double* x) const
{
  if (facet() >= 0)
    values[0] = cell().facetArea(facet());
  else
    values[0] = 0.0;
}
//-----------------------------------------------------------------------------
InvFacetArea::InvFacetArea(Mesh& mesh) : Function(), mesh(mesh)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void InvFacetArea::eval(double* values, const double* x) const
{
  if (facet() >= 0)
    values[0] = 1.0 / cell().facetArea(facet());
  else
    values[0] = 0.0;
}
//-----------------------------------------------------------------------------
OutflowFacet::OutflowFacet(Mesh& mesh, Form& form) : Function(), form(form), mesh(mesh)                           
{
  // Some simple sanity checks on form
  if (!(form.rank() == 0 && form.ufc_form().num_coefficients() == 2))
    error("Invalid form: rank = %d, number of coefficients = %d. Must be rank 0 form with 2 coefficients.", 
              form.rank(), form.ufc_form().num_coefficients());
  if (!(form.ufc_form().num_cell_integrals() == 0 && form.ufc_form().num_exterior_facet_integrals() == 1 
        && form.ufc_form().num_interior_facet_integrals() == 0))
    error("Invalid form: Must have exactly 1 exterior facet integral");

   error("Need to fix special function OutflowFacet::OutflowFacet for new interface.");
  //form.updateDofMaps(mesh);
  //ufc = new UFC(form.form(), mesh, form.dofMaps());
}
//-----------------------------------------------------------------------------
OutflowFacet::~OutflowFacet()
{
  delete ufc;
}
//-----------------------------------------------------------------------------
double OutflowFacet::eval(const double* x) const
{
  // If there is no facet (assembling on interior), return 0.0
  if (facet() < 0)
    return 0.0;
  else
  {
    // Copy cell, cannot call interpolate with const cell()
    Cell cell0(cell());
    ufc->update(cell0);

    // Interpolate coefficients on cell and current facet
    error("Need to extend Function interface.");
    //for (dolfin::uint i = 0; i < form.coefficients().size(); i++)
    //  form.coefficients()[i]->interpolate(ufc->w[i], ufc->cell, 
    //                            *ufc->coefficient_elements[i], cell0, facet());

    // Get exterior facet integral (we need to be able to tabulate ALL facets 
    // of a given cell)
    ufc::exterior_facet_integral* integral = ufc->exterior_facet_integrals[0];

    // Call tabulate_tensor on exterior facet integral, 
    // dot(velocity, facet_normal)
    integral->tabulate_tensor(ufc->A, ufc->w, ufc->cell, facet());
  }

   // If dot product is positive, the current facet is an outflow facet
  if (ufc->A[0] > DOLFIN_EPS)
    return 1.0;
  else
    return 0.0;
}
//-----------------------------------------------------------------------------











