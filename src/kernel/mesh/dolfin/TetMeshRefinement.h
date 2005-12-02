// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2005-12-01

#ifndef __TET_MESH_REFINEMENT_H
#define __TET_MESH_REFINEMENT_H

#include <dolfin/MeshRefinement.h>

namespace dolfin
{

  class Mesh;
  class Cell;
  class Face;

  /// Algorithm for the refinement of a tetrahedral mesh, based on
  /// the algorithm described in the paper "Tetrahedral Mesh Refinement"
  /// by J�rgen Bey, in Computing 55, pp. 355-378 (1995).
  
  class TetMeshRefinement : public MeshRefinement
  {
  public:

    /// Choose refinement rule
    static bool checkRule(Cell& cell, int no_marked_edges);

    /// Refine according to rule
    static void refine(Cell& cell, Mesh& mesh);

    // Friends
    friend class MeshRefinement;

  private:

    static bool checkRuleRegular   (Cell& cell, int no_marked_edges);
    static bool checkRuleIrregular1(Cell& cell, int no_marked_edges);
    static bool checkRuleIrregular2(Cell& cell, int no_marked_edges);
    static bool checkRuleIrregular3(Cell& cell, int no_marked_edges);
    static bool checkRuleIrregular4(Cell& cell, int no_marked_edges);

    static void refineNoRefine   (Cell& cell, Mesh& mesh);
    static void refineRegular    (Cell& cell, Mesh& mesh);
    static void refineIrregular1 (Cell& cell, Mesh& mesh);
    static void refineIrregular2 (Cell& cell, Mesh& mesh);
    static void refineIrregular3 (Cell& cell, Mesh& mesh);
    static void refineIrregular4 (Cell& cell, Mesh& mesh);

    static void refineIrregular31(Cell& cell, Mesh& mesh);
    static void refineIrregular32(Cell& cell, Mesh& mesh,
				  PArray<Vertex*>& sorted_vertices);
    static void refineIrregular33(Cell& cell, Mesh& mesh, 
				  PArray<Vertex*>& sorted_vertices, Cell& face_neighbor);

    static bool  markedEdgesOnSameFace (Cell& cell);
    static Cell* findNeighbor          (Cell& cell, Face& face);

    static Cell& createCell(Vertex& n0, Vertex& n1, Vertex& n2, Vertex& n3,
			    Mesh& mesh, Cell& cell);

    static Cell& createChildCopy(Cell& cell, Mesh& mesh);

  };

}

#endif
