/* -*- C -*- */
// Copyright (C) 2009 Johan Hake
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
// First added:  2009-04-27
// Last changed: 2013-08-07

// Enum for comparison functions
enum DolfinCompareType {dolfin_gt, dolfin_ge, dolfin_lt, dolfin_le, dolfin_eq, dolfin_neq};

// Returns the values from a Vector.
// copied_values are true if the returned values are copied and need clean up.
std::vector<double> _get_vector_values(dolfin::GenericVector* self)
{
  std::vector<double> values;
  self->get_local(values);
  return values;
}

// A contains function for Vectors
bool _contains(dolfin::GenericVector* self, double value)
{
  bool contains = false;
  std::vector<double> values = _get_vector_values(self);

  // Check if value is in values
  for (std::size_t i = 0; i < self->size(); i++)
  {
    if (fabs(values[i] - value) < DOLFIN_EPS)
    {
      contains = true;
      break;
    }
  }
  return contains;
}

// A general compare function for Vector vs scalar comparison
// The function returns a boolean numpy array
PyObject* _compare_vector_with_value(dolfin::GenericVector* self, double value,
                                     DolfinCompareType cmp_type)
{
  std::size_t i;
  npy_intp size = self->size();
  const std::size_t n0 = self->local_range().first;

  // Create the Numpy array
  PyArrayObject* return_array = (PyArrayObject*)PyArray_ZEROS(1, &size, NPY_BOOL, 0);

  // Get the data array
  npy_bool* bool_data = (npy_bool *)PyArray_DATA(return_array);

  // Get the values
  std::vector<double> values = _get_vector_values(self);

  switch (cmp_type)
  {
  case dolfin_gt:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] > value)
        bool_data[i+n0] = 1;
    break;
  case dolfin_ge:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] >= value)
        bool_data[i+n0] = 1;
    break;
  case dolfin_lt:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] < value)
        bool_data[i+n0] = 1;
    break;
  case dolfin_le:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] <= value)
        bool_data[i+n0] = 1;
    break;
  case dolfin_eq:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] == value)
        bool_data[i+n0] = 1;
    break;
  case dolfin_neq:
    for ( i = 0; i < self->local_size(); i++)
      if (values[i] != value)
        bool_data[i+n0] = 1;
    break;
  default:
    throw std::runtime_error("invalid compare type");
  }

  return PyArray_Return(return_array);
}

// A general compare function for Vector vs Vector comparison
// The function returns a boolean numpy array
PyObject* _compare_vector_with_vector(dolfin::GenericVector* self,
                                      dolfin::GenericVector* other,
                                      DolfinCompareType cmp_type)
{
  if (self->local_size() != other->local_size())
    throw std::runtime_error("non matching dimensions");

  std::size_t i;
  npy_intp size = self->size();
  const std::size_t n0 = self->local_range().first;

  // Create the Numpy array
  PyArrayObject* return_array = (PyArrayObject*)PyArray_ZEROS(1, &size,
                                                              NPY_BOOL, 0);

  // Get the data array
  npy_bool* bool_data = (npy_bool *)PyArray_DATA(return_array);

  // Get the values
  std::vector<double> self_values = _get_vector_values(self);
  std::vector<double> other_values = _get_vector_values(other);

  switch (cmp_type)
  {
  case dolfin_gt:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] > other_values[i])
        bool_data[i+n0] = 1;
    break;
  case dolfin_ge:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] >= other_values[i])
        bool_data[i+n0] = 1;
    break;
  case dolfin_lt:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] < other_values[i])
        bool_data[i+n0] = 1;
    break;
  case dolfin_le:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] <= other_values[i])
        bool_data[i+n0] = 1;
    break;
  case dolfin_eq:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] == other_values[i])
        bool_data[i+n0] = 1;
    break;
  case dolfin_neq:
    for ( i = 0; i < self->local_size(); i++)
      if (self_values[i] != other_values[i])
        bool_data[i+n0] = 1;
    break;
  default:
    throw std::runtime_error("invalid compare type");
  }

  return PyArray_Return(return_array);
}

// Get single Vector item
double _get_vector_single_item(dolfin::GenericVector* self, int index)
{
  double value;
  dolfin::la_index i(Indices::check_index(index, self->size()));

  // Check that requested index is owned by this process
  if (i < self->local_range().first || i >= self->local_range().second)
    throw std::runtime_error("index must belong to this process, cannot "\
			     "index off-process entries in a GenericVector");

  self->get(&value, 1, &i);
  return value;
}

// Get item for slice, list, or numpy array object
std::shared_ptr<dolfin::GenericVector>
_get_vector_sub_vector(const dolfin::GenericVector* self,
                       PyObject* op )
{
  Indices* inds;
  dolfin::la_index* range;
  dolfin::la_index* indices;
  std::shared_ptr<dolfin::GenericVector> return_vec;
  std::size_t m;

  // Get the correct Indices
  if ((inds = indice_chooser(op, self->size())) == 0)
  {
    throw std::runtime_error("index must be either a slice, a list or a "\
			     "Numpy array of integer");
  }

  // Fill the return vector
  try
  {
    indices = inds->indices();
  }

  // We can only throw and catch runtime_errors. These will be caught
  // by swig.
  catch (std::runtime_error e)
  {
    delete inds;
    throw;
  }

  m = inds->size();

  // Create a default Vector
  return_vec = self->factory().create_vector();

  // Initialize the vector to the size of the indices
  return_vec->init(MPI_COMM_SELF, m);
  range = inds->range();

  std::vector<double> values(m);
  self->get(values.data(), m, indices);

  return_vec->set(values.data(), m, range);
  return_vec->apply("insert");

  delete inds;
  return return_vec;
}

// Set items using a GenericVector
void _set_vector_items_vector(dolfin::GenericVector* self, PyObject* op,
			      dolfin::GenericVector& other)
{
  // Get the correct Indices
  Indices* inds;
  dolfin::la_index* range;
  dolfin::la_index* indices;
  std::size_t m;

  if ( (inds = indice_chooser(op, self->size())) == 0 )
    throw std::runtime_error("index must be either a slice, a list or a Numpy array of integer");

  // Check for size of indices
  if ( inds->size() != other.size() )
  {
    delete inds;
    throw std::runtime_error("non matching dimensions on input");
  }

  // Get the indices
  try
  {
    indices = inds->indices();
  }

  // We can only throw and catch runtime_errors. These will be caught by swig.
  catch (std::runtime_error e)
  {
    delete inds;
    throw;
  }

  m = inds->size();
  range = inds->range();
  std::vector<double> values(m);

  // Get and set values
  other.get(&values[0], m, range);
  self->set(&values[0], m, indices);
  self->apply("insert");

  delete inds;
}

// Set items using an array of floats
void _set_vector_items_array_of_float( dolfin::GenericVector* self, PyObject* op, PyObject* other )
{
  Indices* inds;
  double* values;
  dolfin::la_index* indices;
  std::size_t m;
  bool casted = false;

  // Check type of values
  if (!(other != Py_None and PyArray_Check(other)))
    throw std::runtime_error("expected a contiguous 1D numpy array of numbers");

  PyArrayObject* array_other = reinterpret_cast<PyArrayObject*>(other);
  if (!(PyArray_ISNUMBER(array_other) and PyArray_NDIM(array_other) == 1 and PyArray_ISCONTIGUOUS(array_other)))
    throw std::runtime_error("expected a contiguous 1D numpy array of numbers");

  if (PyArray_TYPE(array_other) != NPY_DOUBLE)
  {
    casted = true;
    array_other = reinterpret_cast<PyArrayObject*>(PyArray_Cast(array_other, NPY_DOUBLE));
  }

  // Get the correct Indices
  if ( (inds = indice_chooser(op, self->size())) == 0 )
    throw std::runtime_error("index must be either a slice, a list or a Numpy array of integer");

  // Check for size of indices
  if (inds->size() != static_cast<std::size_t>(PyArray_DIM(array_other,0)))
  {
    delete inds;
    throw std::runtime_error("non matching dimensions on input");
  }

  // Fill the vector using the slice and the provided values
  try
  {
    indices = inds->indices();
  }

  // We can only throw and catch runtime_errors. These will be caught by swig.
  catch (std::runtime_error e)
  {
    delete inds;
    throw;
  }

  m = inds->size();

  // Get the contigous data from the numpy array
  values = static_cast<double*>(PyArray_DATA(array_other));
  self->set(values, m, indices);
  self->apply("insert");

  // Clean casted array
  if (casted)
  {
    Py_DECREF(array_other);
  }
  delete inds;
}

// Set item(s) using single value
void _set_vector_items_value(dolfin::GenericVector* self, PyObject* op, double value)
{
  // Get the correct Indices
  Indices* inds;
  if ( (inds = indice_chooser(op, self->size())) == 0 )
  {
    // If the index is an integer
    int index;
    // FIXME: Add a Py_convert_dolfin_la_index
    if(!Py_convert_int(op, index))
      throw std::runtime_error("index must be either an integer, a slice, a list or a Numpy array of integer");

    self->setitem(Indices::check_index(index, self->size()), value);
  }
  // The op is a Indices
  else
  {
    dolfin::la_index* indices;
    // Fill the vector using the slice
    try
    {
      indices = inds->indices();
    }
    // We can only throw and catch runtime_errors. These will be caught by swig.
    catch (std::runtime_error e)
    {
      delete inds;
      throw;
    }

    // Fill and array with the value and call set()
    std::vector<double> values(inds->size(), value);
    self->set(&values[0], inds->size(), indices);

    delete inds;
  }
  self->apply("insert");
}

// Get single item from Matrix
double _get_matrix_single_item(const dolfin::GenericMatrix* self, int m, int n)
{
  double value;
  dolfin::la_index _m(Indices::check_index(m, self->size(0)));
  dolfin::la_index _n(Indices::check_index(n, self->size(1)));
  self->get(&value, 1, &_m, 1, &_n);
  return value;
 }

// Get items for slice, list, or numpy array object
std::shared_ptr<dolfin::GenericVector>
_get_matrix_sub_vector(dolfin::GenericMatrix* self, dolfin::la_index single,
                       PyObject* op, bool row )
{
  // Get the correct Indices
  Indices* inds;
  if ( (inds = indice_chooser(op, self->size(row ? 1 : 0))) == 0 )
    throw std::runtime_error("index must be either a slice, a list or a Numpy array of integer");

  dolfin::la_index* indices;
  try
  {
    // Get the indices in a c array
    indices = inds->indices();
  }
  // We can only throw and catch runtime_errors. These will be caught
  // by swig.
  catch (std::runtime_error e)
  {
    delete inds;
    throw;
  }

  // Create the value array and get the values from the matrix
  std::vector<double> values(inds->size());
  if (row)
  {
    // If returning a single row
    self->get(&values[0], 1, &single, inds->size(), indices);
  }
  else
  {
    // If returning a single column
    self->get(&values[0], inds->size(), indices, 1, &single);
  }

  // Create the return vector and set the values
  std::shared_ptr<dolfin::GenericVector> return_vec
    = self->factory().create_vector();
  self->init_vector(*return_vec, 1);

  return_vec->set_local(values);
  return_vec->apply("insert");

  // Clean up
  delete inds;

  return return_vec;
}

/*
// Get items for slice, list, or numpy array object
dolfin::GenericMatrix* _get_matrix_sub_matrix(const dolfin::GenericMatrix* self,
                                              PyObject* row_op, PyObject* col_op )
{
  dolfin::GenericMatrix* return_mat;
  dolfin::uint i, j, k, m, n, nz_i;
  dolfin::uint* col_index_array;
  dolfin::uint* tmp_index_array;
  bool same_indices;
  Indices* row_inds;
  Indices* col_inds;
  double* values;

  // Instantiate the row indices
  if ( (row_inds = indice_chooser(row_op, self->size(0))) == 0 )
    throw std::runtime_error("row indices must be either a slice, a list or a Numpy array of integer");

  // The number of rows
  m = row_inds->size();

  // If None is provided for col_op we assume symmetric indices
  if (col_op == Py_None)
  {
    same_indices = true;

    // Check size of cols
    if (m > self->size(1))
    {
      delete row_inds;
      throw std::runtime_error("num indices excedes the number of columns");
    }

    // Symetric rows and columns yield equal column and row indices
    col_inds = row_inds;
    n = m;

  }
  // Non symetric rows and cols
  else
  {
    same_indices = false;

    // Instantiate the col indices
    if ( (col_inds = indice_chooser(col_op, self->size(1))) == 0 )
    {
      delete row_inds;
      throw std::runtime_error("col indices must be either a slice, a list or a Numpy array of integer");
    }

    // The number of columns
    n = col_inds->size();

  }

  // Access the column indices
  try
  {
    col_index_array = col_inds->indices();
  }

  // We can only throw and catch runtime_errors. These will be caught by swig.
  catch (std::runtime_error e)
  {
    delete row_inds;
    if (!same_indices)
      delete col_inds;
    throw;
  }

  // Create the return matrix
  return_mat = self->factory().create_matrix();

  throw std::runtime_error("Python interface for slices needs to be updated.");
  //return_mat->resize(m, n);

  // Zero the matrix (needed for the uBLASDenseMatrix)
  return_mat->zero();

  // Fill the get the values from me and set non zero values in return matrix
  tmp_index_array = new dolfin::uint[n];
  values = new double[n];
  for (i = 0; i < row_inds->size(); i++)
  {
    // Get all column values
    k = row_inds->index(i);
    self->get(values, 1, &k, n, col_index_array);
    // Collect non zero values
    nz_i = 0;
    for (j = 0; j < col_inds->size(); j++)
    {
      if ( !(fabs(values[j]) < DOLFIN_EPS) )
      {
        tmp_index_array[nz_i] = j;
        values[nz_i] = values[j];
        nz_i++;
      }
    }

    // Set the non zero values to return matrix
    return_mat->set(values, 1, &i, nz_i, tmp_index_array);
  }

  // Clean up
  delete row_inds;
  if ( !same_indices )
    delete col_inds;

  delete[] values;
  delete[] tmp_index_array;

  return_mat->apply("insert");
  return return_mat;
}
*/

// Set single item in Matrix
void _set_matrix_single_item(dolfin::GenericMatrix* self, int m, int n, double value )
{
  dolfin::la_index _m(Indices::check_index(m, self->size(0)));
  dolfin::la_index _n(Indices::check_index(n, self->size(1)));
  self->set(&value, 1, &_m, 1, &_n);
  self->apply("insert");
 }

void _set_matrix_items_array_of_float(dolfin::GenericMatrix* self, PyObject* op,
                                      PyObject* other){}

void _set_matrix_items_matrix(dolfin::GenericMatrix* self, dolfin::GenericMatrix*) {}

void _set_matrix_items_vector(dolfin::GenericMatrix* self, PyObject* op,
                              dolfin::GenericVector& other){}
