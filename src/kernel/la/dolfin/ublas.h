// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-06-23
// Last changed: 

#ifndef __UBLAS_H
#define __UBLAS_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_of_vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

// These two files must be included due to a bug in Boost version < 1.33.
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/operation.hpp>

#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace dolfin
{
  /// Various typedefs for uBlas data types 

  namespace ublas = boost::numeric::ublas;

  // uBlas vector
  typedef ublas::vector<double> ublas_vector;
  typedef ublas::vector_range<ublas_vector> ublas_vector_range;

  // uBlas dense matrix
  typedef ublas::matrix< double, ublas::row_major, ublas::unbounded_array<double> > ublas_dense_matrix;
  typedef ublas::matrix_range<ublas_dense_matrix> ublas_matrix_range;

  // uBlas dense matrix (column major format)
  typedef ublas::matrix<double,ublas::column_major> ublas_matrix_cmajor;
  typedef ublas::matrix_range<ublas_matrix_cmajor> ublas_matrix_cmajor_range;

  // uBlas sparse matrix
  typedef ublas::compressed_matrix<double> ublas_sparse_matrix;

  // uBlas sparse matrix (column major format) 
  typedef ublas::compressed_matrix<double> ublas_sparse_matrix_cmajor;

  // uBlas sparse matrix for temporoary assembly
  typedef ublas::generalized_vector_of_vector< double, ublas::row_major, 
            ublas::vector<ublas::compressed_vector<double> > > ublas_assembly_matrix;

  // uBlas upper triangular matrix (column major format)
  typedef ublas::triangular_matrix<double, ublas::upper, ublas::column_major> ublas_matrix_cmajor_tri;
  typedef ublas::matrix_range<ublas_matrix_cmajor_tri> ublas_matrix_cmajor_tri_range;
  typedef ublas::matrix_column<ublas_matrix_cmajor_tri> ublas_matrix_cmajor_tri_column;

}

#endif
