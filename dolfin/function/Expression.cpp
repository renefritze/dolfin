// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-09-28
// Last changed: 2009-10-07

// Modified by Johan Hake, 2009

#include <boost/scoped_array.hpp>
#include <dolfin/log/log.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Vertex.h>
#include <dolfin/fem/UFCCell.h>
#include "Data.h"
#include "Expression.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Expression::Expression(uint geometric_dimension)
  : _geometric_dimension(geometric_dimension)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Expression::Expression(uint geometric_dimension, uint dim)
  : _geometric_dimension(geometric_dimension)
{
  value_shape.resize(1);
  value_shape[0] = dim;
}
//-----------------------------------------------------------------------------
Expression::Expression(uint geometric_dimension,
                       std::vector<uint> value_shape)
  : value_shape(value_shape), _geometric_dimension(geometric_dimension)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Expression::Expression(const Expression& expression)
  : value_shape(expression.value_shape),
    _geometric_dimension(expression._geometric_dimension)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Expression::~Expression()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint Expression::geometric_dimension() const
{
  return _geometric_dimension;
}
//-----------------------------------------------------------------------------
dolfin::uint Expression::value_rank() const
{
  return value_shape.size();
}
//-----------------------------------------------------------------------------
dolfin::uint Expression::value_dimension(uint i) const
{
  if (i >= value_shape.size())
    error("Illegal axis %d for value dimension for value of rank %d.",
          i, value_shape.size());
  return value_shape[i];
}
//-----------------------------------------------------------------------------
void Expression::eval(double* values, const double* x) const
{
  assert(values);
  assert(x);

  // Missing eval method if we reach this point
  error("Missing eval() for expression (must be overloaded).");
}
//-----------------------------------------------------------------------------
void Expression::eval(double* values, const Data& data) const
{
  assert(values);

  // Redirect to simple eval
  eval(values, data.x);
}
//-----------------------------------------------------------------------------
void Expression::restrict(double* w,
                          const FiniteElement& element,
                          const Cell& dolfin_cell,
                          const ufc::cell& ufc_cell,
                          int local_facet) const
{
  // Restrict as UFC function (by calling eval)
  restrict_as_ufc_function(w, element, dolfin_cell, ufc_cell, local_facet);
}
//-----------------------------------------------------------------------------
void Expression::compute_vertex_values(double* vertex_values,
                                       const Mesh& mesh) const
{
  assert(vertex_values);

  // Local data for vertex values
  const uint size = value_size();
  boost::scoped_array<double> local_vertex_values(new double[size]);
  Data data;

  // Iterate over cells, overwriting values when repeatedly visiting vertices
  UFCCell ufc_cell(mesh);
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update cell data
    ufc_cell.update(*cell);
    data.update(*cell, ufc_cell, -1);

    // Iterate over cell vertices
    for (VertexIterator vertex(*cell); !vertex.end(); ++vertex)
    {
      // Update coordinate data
      data.x = vertex->x();

      // Evaluate at vertex
      eval(local_vertex_values.get(), data);

      // Copy to array
      for (uint i = 0; i < size; i++)
      {
        const uint global_index = i*mesh.num_vertices() + vertex->index();
        vertex_values[global_index] = local_vertex_values[i];
      }
    }
  }
}
//-----------------------------------------------------------------------------
