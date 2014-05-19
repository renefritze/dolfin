// Copyright (C) 2013 Benjamin Kehlet
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
// First added:  2013-06-22
// Last changed: 2013-08-06

#ifdef HAS_CGAL

#include <memory>

#include "CSGGeometry.h"
#include <dolfin/geometry/Point.h>


struct CSGCGALDomain2DImpl;

namespace dolfin
{

class CSGCGALDomain2D
{
 public:
  // Create empty polygon
  CSGCGALDomain2D();

  // Construct polygon from Dolfin CSG geometry
  CSGCGALDomain2D(const CSGGeometry *csg);

  // Destructor
  ~CSGCGALDomain2D();

  // Copy constructor
  CSGCGALDomain2D(const CSGCGALDomain2D &other);
  CSGCGALDomain2D &operator=(const CSGCGALDomain2D &other);

  // Boolean operators
  void join_inplace(const CSGCGALDomain2D& other);
  void intersect_inplace(const CSGCGALDomain2D& other);
  void difference_inplace(const CSGCGALDomain2D& other);

  bool point_in_domain(Point p) const;
  double compute_boundingcircle_radius() const ;

  // TODO: Replace this with a more C++-ish
  // implementation, ie, take an outputiterator as arugment
  // or define iterator
  void get_vertices(std::list<std::vector<Point> >& v,
                    double truncate_threshold) const;

  void get_holes(std::list<std::vector<Point> >& h,
                 double truncate_threshold) const;

  std::unique_ptr<CSGCGALDomain2DImpl> impl;

};
}

#endif
