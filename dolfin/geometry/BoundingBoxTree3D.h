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
// First added:  2013-04-09
// Last changed: 2013-05-21

#ifndef __BOUNDING_BOX_TREE_3D_H
#define __BOUNDING_BOX_TREE_3D_H

#include <algorithm>
#include <vector>
#include <dolfin/common/constants.h>
#include "GenericBoundingBoxTree.h"

namespace dolfin
{

  // Specialization of bounding box implementation to 3D

  class BoundingBoxTree3D : public GenericBoundingBoxTree
  {
  protected:

    // Comparison operators for sorting of bounding boxes. Boxes are
    // sorted by their midpoints along the longest axis.

    struct less_x
    {
      const std::vector<double>& bboxes;
      less_x(const std::vector<double>& bboxes): bboxes(bboxes) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* bi = bboxes.data() + 6*i;
        const double* bj = bboxes.data() + 6*j;
        return bi[0] + bi[3] < bj[0] + bj[3];
      }
    };

    struct less_y
    {
      const std::vector<double>& bboxes;
      less_y(const std::vector<double>& bboxes): bboxes(bboxes) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* bi = bboxes.data() + 6*i;
        const double* bj = bboxes.data() + 6*j;
        return bi[1] + bi[4] < bj[1] + bj[4];
      }
    };

    struct less_z
    {
      const std::vector<double>& bboxes;
      less_z(const std::vector<double>& bboxes): bboxes(bboxes) {}

      inline bool operator()(unsigned int i, unsigned int j)
      {
        const double* bi = bboxes.data() + 6*i;
        const double* bj = bboxes.data() + 6*j;
        return bi[2] + bi[5] < bj[2] + bj[5];
      }
    };

    // Check whether point is in bounding box
    bool point_in_bbox(const double* x, unsigned int node) const
    {
      const double* b = _bbox_coordinates.data() + 6*node;
      return (b[0] - DOLFIN_EPS < x[0] && x[0] < b[3] + DOLFIN_EPS &&
              b[1] - DOLFIN_EPS < x[1] && x[1] < b[4] + DOLFIN_EPS &&
              b[2] - DOLFIN_EPS < x[2] && x[2] < b[5] + DOLFIN_EPS);
    }

    // Compute squared distance between point and bounding box
    double compute_squared_distance(const double* x, unsigned int node) const
    {
      // Note: Some else-if might be in order here but I assume the
      // compiler can do a better job at optimizing/parallelizing this
      // version. This is also the way the algorithm is presented in
      // Ericsson.

      const double* b = _bbox_coordinates.data() + 6*node;
      double r2 = 0.0;

      if (x[0] < b[0]) r2 += (x[0] - b[0])*(x[0] - b[0]);
      if (x[0] > b[3]) r2 += (x[0] - b[3])*(x[0] - b[3]);
      if (x[1] < b[1]) r2 += (x[1] - b[1])*(x[1] - b[1]);
      if (x[1] > b[4]) r2 += (x[1] - b[4])*(x[1] - b[4]);
      if (x[2] < b[2]) r2 += (x[2] - b[2])*(x[2] - b[2]);
      if (x[2] > b[5]) r2 += (x[2] - b[5])*(x[2] - b[5]);

      return r2;
    }

    // Compute bounding box of bounding boxes
    void compute_bbox_of_bboxes(double* bbox,
                                unsigned short int& axis,
                                const std::vector<double>& leaf_bboxes,
                                const std::vector<unsigned int>::iterator& begin,
                                const std::vector<unsigned int>::iterator& end)
    {
      typedef std::vector<unsigned int>::const_iterator iterator;

      // Get coordinates for first box
      iterator it = begin;
      const double* b = leaf_bboxes.data() + 6*(*it);
      bbox[0] = b[0];
      bbox[1] = b[1];
      bbox[2] = b[2];
      bbox[3] = b[3];
      bbox[4] = b[4];
      bbox[5] = b[5];

      // Compute min and max over remaining boxes
      for (; it != end; ++it)
      {
        const double* b = leaf_bboxes.data() + 6*(*it);
        if (b[0] < bbox[0]) bbox[0] = b[0];
        if (b[1] < bbox[1]) bbox[1] = b[1];
        if (b[2] < bbox[2]) bbox[2] = b[2];
        if (b[3] > bbox[3]) bbox[3] = b[3];
        if (b[4] > bbox[4]) bbox[4] = b[4];
        if (b[5] > bbox[5]) bbox[5] = b[5];
      }

      // Compute longest axis
      const double x = b[3] - b[0];
      const double y = b[4] - b[1];
      const double z = b[5] - b[2];

      if (x > y && x > z)
        axis = 0;
      else if (y > z)
        axis = 1;
      else
        axis = 2;
    }

    // Sort leaf bounding boxes along given axis
    void sort_bboxes(unsigned short int axis,
                     const std::vector<double>& leaf_bboxes,
                     const std::vector<unsigned int>::iterator& begin,
                     const std::vector<unsigned int>::iterator& middle,
                     const std::vector<unsigned int>::iterator& end)
    {
      switch (axis)
      {
      case 0:
        std::nth_element(begin, middle, end, less_x(leaf_bboxes));
        break;
      case 1:
        std::nth_element(begin, middle, end, less_y(leaf_bboxes));
        break;
      default:
        std::nth_element(begin, middle, end, less_z(leaf_bboxes));
      }
    }

  };

}

#endif
