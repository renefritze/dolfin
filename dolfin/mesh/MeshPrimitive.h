// Copyright (C) 2009 Andre Massing 
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-09-11
// Last changed: 2009-11-11

#ifndef  meshprimitive_INC
#define  meshprimitive_INC

#ifdef HAS_CGAL

#include <CGAL/Bbox_3.h>

#include "Cell.h"
#include "Mesh.h"
#include "MeshEntityIterator.h"
#include "Vertex.h"

#include "Primitive_Traits.h"

namespace dolfin {

template <typename PrimitiveTrait>
class MeshPrimitive
{
public:
    typedef uint Id;
    typedef typename PrimitiveTrait::Datum Datum;
    typedef typename PrimitiveTrait::K K;
    typedef typename PrimitiveTrait::Primitive Primitive;
    typedef typename K::Point_3 Point_3;

    //topological dimension of the Cell;
    static const uint dim = PrimitiveTrait::dim;

    //static function, so that only reference to a mesh and cell index have to
    //be saved.
    static Cell getEntity(const MeshPrimitive & p) {return Cell(p.mesh(),p.index);}

    const Mesh& mesh() const { return *_mesh; }

private:
    Id index; // this is what the AABB tree stores internally
    const Mesh * _mesh;

public:
    MeshPrimitive(): index(0), _mesh(0)   {} // default constructor needed

    // the following constructor is the one that receives the iterators from the 
    // iterator range given as input to the AABB_tree
    MeshPrimitive(MeshEntityIterator cell)
        : index(cell->index()), _mesh(&(cell->mesh())) {}
            
    Id id() const { return index;}

    ///*Not* required by the CGAL primitive concept, but added for efficieny
    //reasons. Explanation: We use a modified AABB_tree, in which the local BBox
    //functor class has been redefined to use the bbox function of dolfin mesh entities.
    ///Otherwise the bbox function of the Datum object (see below) would have
    //been used, which means that we would have had to convert dolfin cells into
    //CGAL primitives only to initialize the tree, which is probably very costly
    //for 1 million of triangles.
//    CGAL::Bbox_3 bbox () const {return MeshPrimitive<PrimitiveTrait>::getEntity(*this).bbox<K>();}

    ///Provides a reference point required by the Primitive concept of CGAL
    //AABB_tree. Uses conversion operator in dolfin::Point to create a certain
    //CGAL Point_3 type.
    Point_3 reference_point() const
    { 
      return VertexIterator(MeshPrimitive<PrimitiveTrait>::getEntity(*this))->point();
    }

    //First line compiles but not the second..?
    Datum datum() const { return Primitive_Traits<Primitive,K>::datum(MeshPrimitive<PrimitiveTrait>::getEntity(*this));}
//    Datum datum() const { return		PrimitiveTrait::datum(MeshPrimitive<PrimitiveTrait>::getEntity(*this));}
};

}

#endif   
#endif   /* ----- #ifndef meshprimitive_INC  ----- */
