// Copyright (C) 2006 Garth N. Wels.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-12-05
// Last changed: 2006-

#ifndef __DOF_MAP_H
#define __DOF_MAP_H

#include <set>  
#include <dolfin/constants.h>
#include <dolfin/Array.h>
#include <dolfin/FiniteElement.h>
#include <dolfin/Mesh.h>

namespace dolfin
{
  // Forward declarations
  class Cell;

  /// This class handles degree of freedom mappings. Its constructor takes
  /// the mesh and one or two finite elements. element_0 maps to a vector
  /// or a matrix row, and element_1 maps to the columns of a matrix
  /// Two finite elements are required to generate the sparsity pattern for 
  /// matrix assembly.

  class DofMap
  {
  public:
    
    /// Constructor
    DofMap(Mesh& mesh, FiniteElement* element_0 = 0, FiniteElement* element_1 = 0);

    /// Destructor
    ~DofMap();

    /// Attach finite elements
    void attach(const FiniteElement* element_0, const FiniteElement* element_1 = 0);

    /// Get dof map for a cell for element e (e=0 or e=1)
    void dofmap(int dof_map[], const Cell& cell, const uint e = 0) const;

//    /// Return global dof  map
//    const Array< Array<int> >& getMap() const;

    /// Return total number of degrees of freedom associated with element e (e=0 or e=1)
    const uint size(const uint e = 0);

    /// Compute maximum number of non-zeroes for a row of sparse matrix
    uint numNonZeroes();

    /// Compute number of non-zeroes for each row in a sparse matrix
    void numNonZeroesRow(int nz_row[]);

  private:

    /// Build complete dof mapping for a given element (e=0 or e=1) on mesh 
    void build(const uint e = 0);

    /// Compute sparsity pattern for a vector for element e (e=0 or e=1)
    void computeVectorSparsityPattern(const uint e = 0);    

    /// Compute sparsity pattern for a matrix where element_0 map to rows 
    /// (usually the test element) and element_1 maps to columns (usually
    //  the trial element)
    void computeMatrixSparsityPattern();    

    // Mesh associated with dof mapping
    Mesh* mesh;

    // Finite elements associated with dof mapping
    const FiniteElement* element[2];

    // Number of degrees of freedom associated with each element
    int _size[2];

    // Degree of freedom map
    Array< Array<int> > _map;    

    // Vector sparsity pattern represented as a set of nonzero positions
    std::set<int> vector_sparsity_pattern;    

    // Matrix sparsity pattern represented as an Array of set of nonzero 
    // positions. Each set corresponds to a matrix row
    Array< std::set<int> > matrix_sparsity_pattern;    

  };

}

#endif
