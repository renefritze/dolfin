// Copyright (C) 2012 Anders Logg
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
// Modified by Benjamin Kehlet, 2012
// Modified by Johannes Ring, 2012
//
// First added:  2012-04-11
// Last changed: 2012-11-12

#ifndef __CSG_PRIMITIVES_3D_H
#define __CSG_PRIMITIVES_3D_H

#include <cstddef>
#include <dolfin/geometry/Point.h>
#include "CSGPrimitive.h"

namespace dolfin
{

  /// Base class for 3D primitives
  class CSGPrimitive3D : public CSGPrimitive
  {
  public:

    /// Return dimension of geometry
    std::size_t dim() const { return 3; }

  };

  /// This class describes a 3D sphere which can be used to build
  /// geometries using Constructive Solid Geometry (CSG).
  class Sphere : public CSGPrimitive3D
  {
  public:

    /// Create sphere with center (x0, x1, x2) and radius r.
    ///
    /// *Arguments*
    ///     center (Point)
    ///         Center of sphere.
    ///     r (double)
    ///         radius.
    Sphere(Point center, double radius, std::size_t slices=16);

    /// Informal string representation
    std::string str(bool verbose) const;

    Type getType() const
    { return CSGGeometry::Sphere; }

    const Point c;
    const double r;
    const std::size_t _slices;

  };

  /// This class describes a 3D box which can be used to build
  /// geometries using Constructive Solid Geometry (CSG).
  class Box : public CSGPrimitive3D
  {
  public:

    /// Create box defined by two opposite corners
    /// x = (x0, x1, x2) and y = (y0, y1, y2).
    ///
    /// *Arguments*
    ///     x0 (double)
    ///         x0-coordinate of first corner.
    ///     x1 (double)
    ///         x1-coordinate of first corner.
    ///     x2 (double)
    ///         x2-coordinate of first corner.
    ///     y0 (double)
    ///         y0-coordinate of second corner.
    ///     y1 (double)
    ///         y1-coordinate of second corner.
    ///     y2 (double)
    ///         y2-coordinate of second corner.
    Box(double x0, double x1, double x2,
        double y0, double y1, double y2);

    /// Informal string representation
    std::string str(bool verbose) const;

    Type getType() const
    { return CSGGeometry::Box; }

    double _x0, _x1, _x2, _y0, _y1, _y2;
  };

  /// This class describes a 3D cone which can be used to build
  /// geometries using Constructive Solid Geometry (CSG).
  class Cone : public CSGPrimitive3D
  {
  public:

    /// Create cone defined by upper and lower center
    /// and radius respectively.
    ///
    /// *Arguments*
    ///     top (Point)
    ///         Center at top of cone.
    ///     top_radius(double)
    ///         Radius bottom of cone.
    ///     bottom(Point)
    ///         Center at top of cone.
    ///     bottom_radius (double)
    ///         radius at top of cone.
    ///     slices (std::size_t)
    ///         number of faces on the side when generating a
    ///         polyhedral approximation.
    Cone(Point top, Point bottom, double top_radius, double bottom_radius,
         std::size_t slices=32);

    /// Informal string representation
    std::string str(bool verbose) const;

    Type getType() const
    { return CSGGeometry::Cone; }

    const Point _top, _bottom;
    const double _top_radius, _bottom_radius;
    const std::size_t _slices;
  };

  /// This class describes a 3D cylinder which can be used to build
  /// geometries using Constructive Solid Geometry (CSG). A cylinder
  /// is here just a special case of a cone.
  class Cylinder : public Cone
  {
  public:

    /// Create cylinder defined by upper and lower center
    /// and radius respectively.
    ///
    /// *Arguments*
    ///     top (Point)
    ///         Center at top of cylinder.
    ///     bottom(Point)
    ///         Center at bottom of cylinder.
    ///     r (double)
    ///         radius of cylinder.
    ///     slices (std::size_t)
    ///         number of faces on the side when generating a
    ///         polyhedral approximation.
    Cylinder(Point top, Point bottom, double r, std::size_t slices=32)
      : Cone(top, bottom, r, r, slices) {}
  };

  /// This class describes a Tetrahedron which can be used to build
  /// geometries using Constructive Solid Geometry (CSG).
  class Tetrahedron : public CSGPrimitive3D
  {
  public:
    /// Create tetrahedron defined by four corner points.
    ///
    /// *Arguments*
    ///     x0 (Point)
    ///         Point.
    ///     x1 (Point)
    ///         Point.
    ///     x2 (Point)
    ///         Point.
    ///     x3 (Point)
    ///         Point.
    Tetrahedron(Point x0, Point x1, Point x2, Point x3);

    /// Informal string representation
    std::string str(bool verbose) const;

    Type getType() const
    { return CSGGeometry::Tetrahedron; }

    Point _x0, _x1, _x2, _x3;
  };

  /// This class describes a 3D surface loaded from file.
  /// The supported file types
  class Surface3D : public CSGPrimitive3D
  {
  public:
    Surface3D(std::string filename);

    /// Informal string representation
    std::string str(bool verbose) const;

    Type getType() const
    { return CSGGeometry::Surface3D; }

    std::string _filename;
  };
}
#endif
