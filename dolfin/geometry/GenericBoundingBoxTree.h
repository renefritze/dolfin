// Copyright (C) 2013 Anders Logg
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
// First added:  2013-04-23
// Last changed: 2013-05-02

#ifndef __GENERIC_BOUNDING_BOX_TREE_H
#define __GENERIC_BOUNDING_BOX_TREE_H

#include <vector>
#include <dolfin/common/constants.h>

namespace dolfin
{

  class Mesh;
  class MeshEntity;
  class Point;

  /// Base class for bounding box implementations

  class GenericBoundingBoxTree
  {
  public:

    /// Constructor
    GenericBoundingBoxTree();

    /// Build bounding box tree for cells of mesh
    void build(const Mesh& mesh);

    /// Build bounding box tree for mesh entites of given dimension
    void build(const Mesh& mesh, unsigned int dimension);

    /// Destructor
    virtual ~GenericBoundingBoxTree() {}

    /// Find entities intersecting the given _Point_
    std::vector<unsigned int> find(const Point& point) const;

  protected:

    // Bounding box data. The 'entity' field is only set for leaves
    // and is otherwise undefined. A leaf is signified by both children
    // being set to 0.
    struct BBox
    {
      // Bounding box data
      unsigned int child_0;
      unsigned int child_1;
    };

    // List of bounding boxes (parent-child-entity relations)
    std::vector<BBox> bboxes;

    // List of bounding box coordinates
    std::vector<double> bbox_coordinates;

    // Add bounding box and coordinates
    inline void add_bbox(const BBox& bbox, const double* b, unsigned int gdim)
    {
      // Add bounding box
      bboxes.push_back(bbox);

      // Add bounding box coordinates
      for (unsigned int i = 0; i < 2*gdim; ++i)
        bbox_coordinates.push_back(b[i]);
    }

    // Check whether bounding box is a leaf node
    bool is_leaf(const BBox& bbox, unsigned int node) const
    {
      // FIXME: Explain
      return bbox.child_0 == node;
    }

    // Check whether point is in bounding box
    virtual bool point_in_bbox(const double* x, unsigned int node) const = 0;

    // Compute bounding box of bounding boxes (3d)
    virtual void
    compute_bbox_of_bboxes(double* bbox,
                           unsigned short int& axis,
                           const std::vector<double>& leaf_bboxes,
                           const std::vector<unsigned int>::iterator& begin,
                           const std::vector<unsigned int>::iterator& end) = 0;


    virtual void sort_bboxes(unsigned short int axis,
                             const std::vector<double>& leaf_bboxes,
                             const std::vector<unsigned int>::iterator& begin,
                             const std::vector<unsigned int>::iterator& middle,
                             const std::vector<unsigned int>::iterator& end) = 0;


  private:

    // Build bounding box tree (recursive, 3d)
    unsigned int build(std::vector<double>& leaf_bboxes,
                       const std::vector<unsigned int>::iterator& begin,
                       const std::vector<unsigned int>::iterator& end,
                       unsigned int gdim);

    /// Find entities intersecting the given coordinate (recursive)
    void find(const double* x,
              unsigned int node,
              std::vector<unsigned int>& entities) const;

    // Compute bounding box of mesh entity
    void compute_bbox_of_entity(double* b,
                                const MeshEntity& entity,
                                unsigned int gdim) const;

  };

}

#endif
