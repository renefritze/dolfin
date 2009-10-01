// Copyright (C) 2003-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2005-2009.
// Modified by Martin Sandve Alnes, 2008.
//
// First added:  2003-11-28
// Last changed: 2009-10-01

#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <dolfin/log/log.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/io/File.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/la/DefaultFactory.h>
#include <dolfin/fem/FiniteElement.h>
#include <dolfin/fem/DofMap.h>
#include <dolfin/fem/UFC.h>
#include <dolfin/mesh/IntersectionDetector.h>
#include "Data.h"
#include "Expression.h"
#include "FunctionSpace.h"
#include "Function.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Function::Function(const FunctionSpace& V)
  : Variable("v", "unnamed function"),
    _function_space(reference_to_no_delete_pointer(V)),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V.element())
{
  // Initialize vector
  init_vector();

  // FIXME: Should we resize the vector immediately?

}
//-----------------------------------------------------------------------------
Function::Function(boost::shared_ptr<const FunctionSpace> V)
  : Variable("v", "unnamed function"),
    _function_space(V),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V->element())
{
  // Initialize vector
  init_vector();
}
//-----------------------------------------------------------------------------
Function::Function(boost::shared_ptr<const FunctionSpace> V,
                   GenericVector& x)
  : Variable("v", "unnamed function"),
    _function_space(V),
    _vector(reference_to_no_delete_pointer(x)),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V->element())
{
  assert(V->dofmap().global_dimension() == x.size());
}
//-----------------------------------------------------------------------------
Function::Function(boost::shared_ptr<const FunctionSpace> V,
                   boost::shared_ptr<GenericVector> x)
  : Variable("v", "unnamed function"),
    _function_space(V),
    _vector(x),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V->element())
{
  assert(V->dofmap().global_dimension() <= x->size());
}
//-----------------------------------------------------------------------------
Function::Function(const FunctionSpace& V, GenericVector& x)
  : Variable("v", "unnamed function"),
    _function_space(reference_to_no_delete_pointer(V)),
    _vector(reference_to_no_delete_pointer(x)),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V.element())
{
  assert(V.dofmap().global_dimension() == x.size());
}
//-----------------------------------------------------------------------------
Function::Function(const FunctionSpace& V, std::string filename)
  : Variable("v", "unnamed function"),
    _function_space(reference_to_no_delete_pointer(V)),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V.element())
{
  // Initialize vector
  init_vector();

  // Read vector from file
  File file(filename);
  file >> *_vector;

  // Check size of vector
  if (_vector->size() != _function_space->dim())
    error("Unable to read Function from file, number of degrees of freedom (%d) does not match dimension of function space (%d).", _vector->size(), _function_space->dim());
}
//-----------------------------------------------------------------------------
Function::Function(boost::shared_ptr<const FunctionSpace> V, std::string filename)
  : Variable("v", "unnamed function"),
    _function_space(V),
    _off_process_vector(static_cast<GenericVector*>(0)), scratch0(V->element())
{
  // Create vector
  DefaultFactory factory;
  _vector.reset(factory.create_vector());

  // Initialize vector
  init_vector();

  // Read vector from file
  File file(filename);
  file >> *_vector;

  // Check size of vector
  if (_vector->size() != _function_space->dim())
    error("Unable to read Function from file, number of degrees of freedom (%d) does not match dimension of function space (%d).", _vector->size(), _function_space->dim());
}
//-----------------------------------------------------------------------------
Function::Function(const Function& v)
  : Variable("v", "unnamed function"),
    _function_space(static_cast<FunctionSpace*>(0)),
    _vector(static_cast<GenericVector*>(0)),
    _off_process_vector(static_cast<GenericVector*>(0))
{
  *this = v;
}
//-----------------------------------------------------------------------------
Function::~Function()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
const Function& Function::operator= (const Function& v)
{
  // Make a copy of all the data, or if v is a sub-function, then we collapse
  // the dof map and copy only the relevant entries from the vector of v.
  if (v._vector->size() == v._function_space->dim())
  {
    // Copy function space
    _function_space = v._function_space;

    // Copy vector
    *_vector = *v._vector;
  }
  else
  {
    // Create collapsed dof map
    std::map<uint, uint> collapsed_map;
    boost::shared_ptr<DofMap> collapsed_dof_map(v._function_space->dofmap().collapse(collapsed_map));

    // Create new FunctionsSpapce
    _function_space = v._function_space->collapse_sub_space(collapsed_dof_map);

    assert(collapsed_map.size() ==  _function_space->dofmap().global_dimension());

    // Create new vector
    const uint size = collapsed_dof_map->global_dimension();
    _vector.reset(v.vector().factory().create_vector());
    _vector->resize(size);

    // Get rows of original and new vectors
    std::map<uint, uint>::const_iterator entry;
    std::vector<uint> new_rows(size);
    std::vector<uint> old_rows(size);
    uint i = 0;
    for (entry = collapsed_map.begin(); entry != collapsed_map.end(); ++entry)
    {
      new_rows[i] = entry->first;
      old_rows[i++] = entry->second;
    }

    // Get old values and set new values
    v.gather();
    std::vector<double> values(size);
    v.get(&values[0], size, &old_rows[0]);
    this->_vector->set(&values[0], size, &new_rows[0]);
  }
  scratch0.init(this->_function_space->element());

  return *this;
}
//-----------------------------------------------------------------------------
const Function& Function::operator= (const Expression& v)
{
  // The below is copied from Function& Function::operator=

  // Resize vector if required
  init_vector();

  info("Assignment from expression, interpolating.");
  function_space().interpolate(*_vector, v);
  return *this;
}
//-----------------------------------------------------------------------------
Function& Function::operator[] (uint i)
{
  // Check if sub-Function is in the cache, otherwise create and add to cache
  boost::ptr_map<uint, Function>::iterator sub_function = sub_functions.find(i);
  if (sub_function != sub_functions.end())
    return *(sub_function->second);
  else
  {
    // Extract function subspace
    std::vector<uint> component = boost::assign::list_of(i);
    boost::shared_ptr<const FunctionSpace> sub_space(this->function_space().extract_sub_space(component));

    // Insert sub-Function into map and return reference
    sub_functions.insert(i, new Function(sub_space, this->_vector));
    return *(sub_functions.find(i)->second);
  }
}
//-----------------------------------------------------------------------------
const FunctionSpace& Function::function_space() const
{
  return *_function_space;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const FunctionSpace> Function::function_space_ptr() const
{
  return _function_space;
}
//-----------------------------------------------------------------------------
GenericVector& Function::vector()
{
  // Check that this is not a sub function.
  if (_vector->size() != _function_space->dofmap().global_dimension())
    error("You are attempting to access a non-const vector from a sub-Function.");
  return *_vector;
}
//-----------------------------------------------------------------------------
const GenericVector& Function::vector() const
{
  return *_vector;
}
//-----------------------------------------------------------------------------
bool Function::in(const FunctionSpace& V) const
{
  return _function_space.get() == &V;
}
//-----------------------------------------------------------------------------
dolfin::uint Function::geometric_dimension() const
{
  assert(_function_space);
  return _function_space->mesh().geometry().dim();
}
//-----------------------------------------------------------------------------
void Function::eval(double* values, const double* x) const
{
  assert(values);

  // Initialize intersection detector if not done before
  if (!intersection_detector)
    intersection_detector.reset(new IntersectionDetector(_function_space->mesh()));

  // Find the cell that contains x
  Point point(_function_space->mesh().geometry().dim(), x);
  std::vector<uint> cells;
  intersection_detector->intersection(point, cells);
  if (cells.size() < 1)
    error("Unable to evaluate function at given point (not inside domain).");
  Cell cell(_function_space->mesh(), cells[0]);
  UFCCell ufc_cell(cell);

  // Evaluate
  eval(values, x, ufc_cell, cell.index());
}
//-----------------------------------------------------------------------------
void Function::eval(double* values, const Data& data) const
{
  assert(values);
  assert(data.x);

  // FIXME: Dangerous since we can' be sure this cell originates from the
  // FIXME: same mesh!

  // Use UFC cell if available
  if (data._ufc_cell)
  {
    const uint cell_index = data._ufc_cell->entity_indices[data._ufc_cell->topological_dimension][0];
   eval(values, data.x, *data._ufc_cell, cell_index);
  }
  else
    _function_space->eval(values, data.x, *this);
}
//-----------------------------------------------------------------------------
void Function::eval(double* values,
                    const double* x,
                    const ufc::cell& ufc_cell,
                    uint cell_index) const
{
  assert(values);
  assert(x);

  // Restrict function to cell
  Cell cell(_function_space->mesh(), cell_index);
  restrict(scratch0.coefficients, _function_space->element(), cell, ufc_cell, -1);

  // Compute linear combination
  for (uint j = 0; j < scratch0.size; j++)
    values[j] = 0.0;
  for (uint i = 0; i < _function_space->element().space_dimension(); i++)
  {
    _function_space->element().evaluate_basis(i, scratch0.values, x, ufc_cell);
    for (uint j = 0; j < scratch0.size; j++)
      values[j] += (scratch0.coefficients[i])*(scratch0.values[j]);
  }
}
//-----------------------------------------------------------------------------
void Function::interpolate(const Function& v)
{
  function_space().interpolate(this->vector(), v, "non-matching");
}
//-----------------------------------------------------------------------------
void Function::interpolate_vertex_values(double* vertex_values) const
{
  assert(vertex_values);
  assert(_function_space);
  _function_space->interpolate_vertex_values(vertex_values, *this);
}
//-----------------------------------------------------------------------------
void Function::interpolate(const Expression& v)
{
  // Interpolate to vector
  DefaultFactory factory;
  boost::shared_ptr<GenericVector> coefficients(factory.create_vector());
  function_space().interpolate(*coefficients, *this);

  // Set values
  init_vector();
  *_vector = *coefficients;
}
//-----------------------------------------------------------------------------
void Function::compute_off_process_dofs() const
{
  // Clear data
  _off_process_dofs.clear();
  global_to_local.clear();

  // Get mesh
  assert(_function_space);
  const Mesh& mesh = _function_space->mesh();

  // Storage for each cell dofs
  const DofMap& dofmap = _function_space->dofmap();
  const uint num_dofs_per_cell = _function_space->element().space_dimension();
  const uint num_dofs_global = vector().size();
  uint* dofs = new uint[num_dofs_per_cell];

  // Iterate over mesh and check which dofs are needed
  UFCCell ufc_cell(mesh);
  uint i = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update to current cell
    ufc_cell.update(*cell);

    // Tabulate dofs on cell
    dofmap.tabulate_dofs(dofs, ufc_cell, cell->index());

    for (uint d = 0; d < num_dofs_per_cell; ++d)
    {
      const uint dof = dofs[d];
      const uint index_owner = MPI::index_owner(dof, num_dofs_global);
      if (index_owner != MPI::process_number())
      {
        if (std::find(_off_process_dofs.begin(), _off_process_dofs.end(), dof) == _off_process_dofs.end())
        {
          _off_process_dofs.push_back(dof);
          global_to_local[dof] = i++;
        }
      }
    }
  }

  delete [] dofs;
}
//-----------------------------------------------------------------------------
void Function::init_vector()
{
  // Get size
  const uint N = _function_space->dofmap().global_dimension();

  // Create vector of dofs
  if (!_vector)
  {
    DefaultFactory factory;
    _vector.reset(factory.create_vector());
  }

  // Initialize vector of dofs
  assert(_vector);
  _vector->resize(N);
  _vector->zero();
}
//-----------------------------------------------------------------------------
void Function::get(double* block, uint m, const uint* rows) const
{
  // Get local ownership range
  const std::pair<uint, uint> range = _vector->local_range();

  if (range.first == 0 && range.second == _vector->size())
    _vector->get(block, m, rows);
  else
  {
    if (!_off_process_vector.get())
      error("Function has not been prepared with off-process data. Did you forget to call Function::gather()?");

    // FIXME: Perform some more sanity checks

    // Build lists of local and nonlocal coefficients
    uint n_local = 0;
    uint n_nonlocal = 0;
    for (uint i = 0; i < m; ++i)
    {
      if (rows[i] >= range.first && rows[i] < range.second)
      {
        scratch.local_index[n_local]  = i;
        scratch.local_rows[n_local++] = rows[i];
     }
      else
      {
        scratch.nonlocal_index[n_nonlocal]  = i;
        scratch.nonlocal_rows[n_nonlocal++] = global_to_local[rows[i]];
      }
    }

    // Get local coefficients
    _vector->get_local(scratch.local_block, n_local, scratch.local_rows);

    // Get off process coefficients
    _off_process_vector->get_local(scratch.nonlocal_block, n_nonlocal, scratch.nonlocal_rows);

    // Copy result into block
    for (uint i = 0; i < n_local; ++i)
      block[scratch.local_index[i]] = scratch.local_block[i];
    for (uint i = 0; i < n_nonlocal; ++i)
      block[scratch.nonlocal_index[i]] = scratch.nonlocal_block[i];
  }
}
//-----------------------------------------------------------------------------
void Function::restrict(double* w,
                        const FiniteElement& element,
                        const Cell& dolfin_cell,
                        const ufc::cell& ufc_cell,
                        int local_facet) const
{
  assert(w);

  // FIXME: Avoid new/delete in this function

  // Get dofmap
  const DofMap& dofmap = _function_space->dofmap();

  // Tabulate dofs
  uint* dofs = new uint[dofmap.local_dimension(ufc_cell)];
  dofmap.tabulate_dofs(dofs, ufc_cell, dolfin_cell.index());

  // Pick values from vector(s)
  get(w, dofmap.local_dimension(ufc_cell), dofs);

  // Clean up
  delete [] dofs;
}
//-----------------------------------------------------------------------------
void Function::gather() const
{
  // Gather off-process coefficients if running in parallel and function has a vector
  if (MPI::num_processes() > 1)
  {
    assert(_function_space);

    // Initialise scratch space
    scratch.init(_function_space->dofmap().max_local_dimension());

    // Compute lists of off-process dofs
    compute_off_process_dofs();

    // Create off process vector if it doesn't exist
    if (!_off_process_vector.get())
      _off_process_vector.reset(_vector->factory().create_local_vector());

    // Gather off process coefficients
    _vector->gather(*_off_process_vector, _off_process_dofs);
  }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Function::Scratch0::Scratch0(const FiniteElement& element)
  : size(0), dofs(0), coefficients(0), values(0)
{
  init(element);
}
//-----------------------------------------------------------------------------
Function::Scratch0::Scratch0()
  : size(0), dofs(0), coefficients(0), values(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Function::Scratch0::~Scratch0()
{
  delete [] dofs;
  delete [] coefficients;
  delete [] values;
}
//-----------------------------------------------------------------------------
void Function::Scratch0::init(const FiniteElement& element)
{
  // Compute size of value (number of entries in tensor value)
  size = 1;
  for (uint i = 0; i < element.value_rank(); i++)
    size *= element.value_dimension(i);

  // Initialize local array for mapping of dofs
  delete [] dofs;
  dofs = new uint[element.space_dimension()];
  for (uint i = 0; i < element.space_dimension(); i++)
    dofs[i] = 0;

  // Initialize local array for expansion coefficients
  delete [] coefficients;
  coefficients = new double[element.space_dimension()];
  for (uint i = 0; i < element.space_dimension(); i++)
    coefficients[i] = 0.0;

  // Initialize local array for values
  delete [] values;
  values = new double[size];
  for (uint i = 0; i < size; i++)
    values[i] = 0.0;
}
//-----------------------------------------------------------------------------

