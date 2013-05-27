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
// Last changed: 2013-05-27

#ifndef __GENERIC_BOUNDING_BOX_TREE_H
#define __GENERIC_BOUNDING_BOX_TREE_H

#include <vector>

namespace dolfin
{

  class Mesh;
  class MeshEntity;
  class Point;

  /// Base class for bounding box implementations (envelope-letter
  /// design)

  class GenericBoundingBoxTree
  {
  public:

    /// Constructor
    GenericBoundingBoxTree();

    /// Destructor
    virtual ~GenericBoundingBoxTree() {}

    /// Build bounding box tree for mesh entites of given dimension
    void build(const Mesh& mesh, unsigned int tdim);

    /// Build bounding box tree for point cloud
    void build(const std::vector<Point>& points);

    /// Compute all collisions between bounding boxes and given _Point_
    std::vector<unsigned int> compute_collisions(const Point& point) const;

    /// Compute all collisions between entities and given _Point_
    std::vector<unsigned int> compute_entity_collisions(const Point& point,
                                                        const Mesh& mesh) const;

    /// Compute first collision between bounding boxes and given _Point_
    unsigned int compute_first_collision(const Point& point) const;

    /// Compute first collision between entities and given _Point_
    unsigned int compute_first_entity_collision(const Point& point,
                                                const Mesh& mesh) const;

    /// Compute closest entity and distance to given _Point_
    std::pair<unsigned int, double> compute_closest_entity(const Point& point,
                                                           const Mesh& mesh) const;

  protected:

    // Bounding box data. Leaf nodes are indicated by setting child_0
    // equal to the node itself. For leaf nodes, child_1 is set to the
    // index of the entity contained in the leaf bounding box.
    struct BBox
    {
      unsigned int child_0;
      unsigned int child_1;
    };

    // Topological dimension of leaf entities
    unsigned int _tdim;

    // List of bounding boxes (parent-child-entity relations)
    std::vector<BBox> _bboxes;

    // List of bounding box coordinates
    std::vector<double> _bbox_coordinates;

    // Build bounding box tree for entities (recursive)
    unsigned int build(const std::vector<double>& leaf_bboxes,
                       const std::vector<unsigned int>::iterator& begin,
                       const std::vector<unsigned int>::iterator& end,
                       unsigned int gdim);

    // Build bounding box tree for points (recursive)
    unsigned int build(const std::vector<Point>& points,
                       const std::vector<unsigned int>::iterator& begin,
                       const std::vector<unsigned int>::iterator& end,
                       unsigned int gdim);

    /// Compute collisions (recursive)
    void compute_collisions(const Point& point,
                            unsigned int node,
                            std::vector<unsigned int>& entities) const;

    /// Compute entity collisions (recursive)
    void compute_entity_collisions(const Point& point,
                                   unsigned int node,
                                   std::vector<unsigned int>& entities,
                                   const Mesh& mesh) const;

    /// Compute first collision (recursive)
    unsigned int compute_first_collision(const Point& point,
                                         unsigned int node) const;

    /// Compute first entity collision (recursive)
    unsigned int compute_first_entity_collision(const Point& point,
                                                unsigned int node,
                                                const Mesh& mesh) const;

    /// Compute closest entity (recursive)
    void compute_closest_entity(const Point& point,
                                unsigned int node,
                                const Mesh& mesh,
                                unsigned int& closest_entity,
                                double& R2) const;

    // Compute bounding box of mesh entity
    void compute_bbox_of_entity(double* b,
                                const MeshEntity& entity,
                                unsigned int gdim) const;

    // Sort points along given axis
    void sort_points(unsigned short int axis,
                     const std::vector<Point>& points,
                     const std::vector<unsigned int>::iterator& begin,
                     const std::vector<unsigned int>::iterator& middle,
                     const std::vector<unsigned int>::iterator& end);

    // Add bounding box and coordinates
    inline unsigned int add_bbox(const BBox& bbox,
                                 const double* b,
                                 unsigned int gdim)
    {
      // Add bounding box
      _bboxes.push_back(bbox);

      // Add bounding box coordinates
      for (unsigned int i = 0; i < 2*gdim; ++i)
        _bbox_coordinates.push_back(b[i]);

      return _bboxes.size() - 1;
    }

    // Add bounding box and point coordinates
    inline unsigned int add_point(const BBox& bbox,
                                  const Point& point,
                                  unsigned int gdim)
    {
      // Add bounding box
      _bboxes.push_back(bbox);

      // Add point coordinates (twice)
      const double* x = point.coordinates();
      for (unsigned int i = 0; i < gdim; ++i)
        _bbox_coordinates.push_back(x[i]);
      for (unsigned int i = 0; i < gdim; ++i)
        _bbox_coordinates.push_back(x[i]);

      return _bboxes.size() - 1;
    }

    // Check whether bounding box is a leaf node
    bool is_leaf(const BBox& bbox, unsigned int node) const
    {
      // Leaf nodes are marked by setting child_0 equal to the node itself
      return bbox.child_0 == node;
    }

    // Comparison operators for sorting of points. The corresponding
    // comparison operators for bounding boxes are dimension-dependent
    // and are therefore implemented in the subclasses.

    struct less_x_point
    {
      const std::vector<Point>& points;
      less_x_point(const std::vector<Point>& points): points(points) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* pi = points[i].coordinates();
        const double* pj = points[j].coordinates();
        return pi[0] < pj[0];
      }
    };

    struct less_y_point
    {
      const std::vector<Point>& points;
      less_y_point(const std::vector<Point>& points): points(points) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* pi = points[i].coordinates();
        const double* pj = points[j].coordinates();
        return pi[1] < pj[1];
      }
    };

    struct less_z_point
    {
      const std::vector<Point>& points;
      less_z_point(const std::vector<Point>& points): points(points) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* pi = points[i].coordinates();
        const double* pj = points[j].coordinates();
        return pi[2] < pj[2];
      }
    };

    //--- Dimension-dependent functions to be implemented by subclass ---

    // Return geometric dimension
    virtual unsigned int gdim() const = 0;

    // Check whether point is in bounding box
    virtual bool
    point_in_bbox(const double* x, unsigned int node) const = 0;

    // Compute squared distance between point and bounding box
    virtual double
    compute_squared_distance(const double* x, unsigned int node) const = 0;

    // Compute bounding box of bounding boxes
    virtual void
    compute_bbox_of_bboxes(double* bbox,
                           unsigned short int& axis,
                           const std::vector<double>& leaf_bboxes,
                           const std::vector<unsigned int>::iterator& begin,
                           const std::vector<unsigned int>::iterator& end) = 0;

    // Compute bounding box of points
    virtual void
    compute_bbox_of_points(double* bbox,
                           unsigned short int& axis,
                           const std::vector<Point>& points,
                           const std::vector<unsigned int>::iterator& begin,
                           const std::vector<unsigned int>::iterator& end) = 0;

    // Sort leaf bounding boxes along given axis
    virtual void
    sort_bboxes(unsigned short int axis,
                const std::vector<double>& leaf_bboxes,
                const std::vector<unsigned int>::iterator& begin,
                const std::vector<unsigned int>::iterator& middle,
                const std::vector<unsigned int>::iterator& end) = 0;

  };

}

#endif
