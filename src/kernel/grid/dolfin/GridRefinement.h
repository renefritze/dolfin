// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

// A couple of comments:
//
//   - Return List<Cell *> from closeCell() won't work?
//   - Rename refineGrid(int level) and unrefineGrid(int level) to refine(int level) and unrefine(int level)?
//   - Should there be an unrefine()?

#ifndef __GRID_REFINEMENT_H
#define __GRID_REFINEMENT_H

namespace dolfin {

  class Grid;
  class Edge;
  class Cell;

  /// Algorithm for the refinement of a triangular or tetrahedral grid.
  ///
  /// Based on the algorithm described in the paper "Tetrahedral Grid Refinement"
  /// by J�rgen Bey, in Computing 55, pp. 355-378 (1995).
  
  class GridRefinement {
  public:

    static void refine(Grid& grid);
    
  private:

    /*
    void globalRegularRefinement();
    void globalRefinement();
    
    void evaluateMarks(int grid_level);
    staticvoid closeGrid(int grid_level);

    List<Cell *> closeCell(Cell *parent);
    
    void refineGrid(int grid_level);
    void unrefineGrid(int grid_level);
    
    void regularRefinement(Cell* parent);
    
    void regularRefinementTetrahedron(Cell* parent);
    void regularRefinementTriangle(Cell* parent);
    
    void localIrregularRefinement(Cell *parent);
    
    void irregularRefinementBy1(Cell *parent);
    void irregularRefinementBy2(Cell *parent);
    void irregularRefinementBy3(Cell *parent);
    void irregularRefinementBy4(Cell *parent);
    
    void refine();
    
    bool _create_edges;
    */

  };

}

#endif
