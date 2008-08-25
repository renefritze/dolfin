// Copyright (C) 2008 Kent-Andre Mardal.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-08-25


#ifndef __BLOCKVECTOR_H
#define __BLOCKVECTOR_H

#include <map>
#include "GenericVector.h"
#include "VectorNormType.h"

namespace dolfin
{

  class BlockVector 
  {
    private: 
      uint n; 
      //    std::map<int, const GenericVector*> vectors; 
      GenericVector* vectors;  

    public:

      /// Constructor  
      BlockVector(uint n_=0);  

      /// Destructor
      ~BlockVector(); 

      /// Return GenericVector reference number i 
      /* FIXME these functions should probably be inline
       * and all the LA function should rely on these */
      const GenericVector& vec(uint i) const; 
      GenericVector& vec(uint i); 

      /// Add multiple of given vector (AXPY operation)
      void axpy(real a, const BlockVector& x);

      /// Return inner product with given vector
      real inner(const BlockVector& x) const;

      /// Return norm of vector
      real norm(VectorNormType type=l2) const;

      /// Return minimum value of vector
      real min() const;

      /// Return maximum value of vector
      real max() const;

      /// Multiply vector by given number
      const BlockVector& operator*= (real a);

      /// Divide vector by given number
      const BlockVector& operator/= (real a);

      /// Add given vector
      const BlockVector& operator+= (const BlockVector& x);

      /// Subtract given vector
      const BlockVector& operator-= (const BlockVector& x);

      /// Assignment operator
      const BlockVector& operator= (const BlockVector& x);

      /// Assignment operator
      const BlockVector& operator= (real a);

      /// Number of vectors
      uint size() const; 

      /// Display vectors 
      virtual void disp(uint precision=2) const = 0;
  }; 
}

#endif 

