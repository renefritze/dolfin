// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modifications by Georgios Foufas 2002, 2003

#ifndef __MATRIX_HH
#define __MATRIX_HH

#include <iostream>

#include <dolfin/constants.h>

namespace dolfin {
  
  class Vector;
  
  class Matrix {
  public:
	 
	 Matrix  ();
	 Matrix  (int m, int n);
	 ~Matrix ();

	 // Enable different indexing for copy and write (thanks Jim)
	 class Reference {
	 public:

		Reference(Matrix& matrix, int i, int j) : A(matrix)
		{
		  this->i = i;
		  this->j = j;
		}
		
		operator real() const {
		  return A.readElement(i, j);
		}

		void operator= (real value) {
		  A.writeElement(i, j, value);
		}

		void operator= (const Reference& r) {
		  A.writeElement(i, j, r.A(r.i, r.j));
		}
		
		void operator+= (real value) {
		  A.addtoElement(i, j, value);
		}
		
	 protected:
		Matrix& A;
		int i;
		int j;
	 };

	 friend class Reference;

	 // Operators
	 void operator=  (Matrix& A);
	 void operator+= (Matrix& A);
	 void operator*= (real a);
	 
	 /// Resize to empty matrix of given size
	 void init(int m, int n);
	 /// Resize matrix (clear unused elements)
	 void resize();
	 /// Clear matrix
	 void clear();
	 /// Returns size (0 for rows, 1 for columns)
	 int size(int dim) const;
	 /// Returns number of nonzero elements
	 int size();
	 /// Returns size of matrix in bytes (approximately)
	 int bytes();
	 /// Set number of nonzero entries in a row (clearing old values)
	 void initRow(int i, int rowsize);
	 /// Set number of nonzero entries in a row (keeping old values)
	 void resizeRow(int i, int rowsize);
	 /// Returns size of row i
	 int rowSize(int i);
	 
	 /// Indexing: fast alternative
	 real operator()(int i, int *j, int pos) const;
	 /// Indexing: slow alternative
	 Reference operator()(int i, int j);
	 real      operator()(int i, int j) const;
		
	 /// Returns maximum norm
	 real norm();
	 /// Set all elements 0 on this row except (i,i) = 1
	 void ident(int i);
	 /// Returns element i of Ax
	 real mult(Vector &x, int i);
	 /// Multiplies x with A and puts the result in Ax
	 void mult(Vector &x, Vector &Ax);

	 /// Output
	 void show();
	 friend ostream& operator << (ostream& output, Matrix& sparseMatrix);

  protected:

	 real readElement  (int i, int j) const;
	 void writeElement (int i, int j, real value);
	 void addtoElement (int i, int j, real value);
	 
  private:

	 // Dimension
	 int m, n;

	 // Data
	 int  *rowsizes;
	 int  **columns;
	 real **values;

	 // Additional size to allocate when needed
	 int allocsize;
	 
  };

}

#endif
