// Copyright (C) 2014-2016 Anders Logg, August Johansson and Benjamin Kehlet
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
// First added:  2014-02-03
// Last changed: 2017-03-07

#include <iomanip>
#include <dolfin/mesh/MeshEntity.h>
#include "predicates.h"
#include "GeometryPredicates.h"
#include "GeometryTools.h"
#include "GeometryDebugging.h"
#include "CollisionPredicates.h"
#include "IntersectionConstruction.h"

using namespace dolfin;

namespace dolfin
{
  // Add points to vector
  template <typename T>
  inline void add(std::vector<T>& points,
                  const std::vector<T>& _points)
  {
    points.insert(points.end(), _points.begin(), _points.end());
  }

  // Filter unique points
  template <typename T>
  inline std::vector<T> unique(const std::vector<T>& points)
  {
    std::vector<T> _unique;
    _unique.reserve(points.size());

    for (std::size_t i = 0; i < points.size(); ++i)
    {
      bool found = false;
      for (std::size_t j = i+1; j < points.size(); ++j)
      {
        if (points[i] == points[j])
        {
          found = true;
          break;
        }
      }
      if (!found)
        _unique.push_back(points[i]);
    }

    return _unique;
  }

  // Convert vector of doubles to vector of points
  std::vector<Point> to_points(const std::vector<double>& points)
  {
    std::vector<Point> _points;
    for (auto x : points)
      _points.push_back(Point(x));
    return _points;
  }
}

//-----------------------------------------------------------------------------
// High-level intersection construction functions
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection(const MeshEntity& entity_0,
                                       const MeshEntity& entity_1)
{
  // Get data
  const MeshGeometry& g0 = entity_0.mesh().geometry();
  const MeshGeometry& g1 = entity_1.mesh().geometry();
  const unsigned int* v0 = entity_0.entities(0);
  const unsigned int* v1 = entity_1.entities(0);

  // Pack data as vectors of points
  std::vector<Point> points_0(entity_0.dim() + 1);
  std::vector<Point> points_1(entity_1.dim() + 1);
  for (std::size_t i = 0; i <= entity_0.dim(); i++)
    points_0[i] = g0.point(v0[i]);
  for (std::size_t i = 0; i <= entity_1.dim(); i++)
    points_1[i] = g1.point(v1[i]);

  // Only look at first entity to get geometric dimension
  std::size_t gdim = g0.dim();

  // Call common implementation
  return intersection(points_0, points_1, gdim);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection(const std::vector<Point>& p,
                                       const std::vector<Point>& q,
                                       std::size_t gdim)
{
  // Get topological dimensions
  const std::size_t d0 = p.size() - 1;
  const std::size_t d1 = q.size() - 1;

  // Swap if d0 < d1 (reduce from 16 to 10 cases)
  if (d0 < d1)
    return intersection(q, p, gdim);

  // Pick correct specialized implementation
  if (d0 == 0 and d1 == 0)
  {
    switch (gdim) {
    case 1: return to_points(intersection_point_point_1d(p[0][0], q[0][0]));
    case 2: return intersection_point_point_2d(p[0], q[0]);
    case 3: return intersection_point_point_3d(p[0], q[0]);
    }
  }
  else if (d0 == 1 and d1 == 0)
  {
    switch (gdim) {
    case 1: return to_points(intersection_segment_point_1d(p[0][0], p[1][0], q[0][0]));
    case 2: return intersection_segment_point_2d(p[0], p[1], q[0]);
    case 3: return intersection_segment_point_3d(p[0], p[1], q[0]);
    }
  }
  else if (d0 == 1 and d1 == 1)
  {
    switch (gdim) {
    case 1: return to_points(intersection_segment_segment_1d(p[0][0], p[1][0], q[0][0], q[1][0]));
    case 2: return intersection_segment_segment_2d(p[0], p[1], q[0], q[1]);
    case 3: return intersection_segment_segment_3d(p[0], p[1], q[0], q[1]);
    }
  }
  else if (d0 == 2 and d1 == 0)
  {
    switch (gdim) {
    case 2: return intersection_triangle_point_2d(p[0], p[1], p[2], q[0]);
    case 3: return intersection_triangle_point_3d(p[0], p[1], p[2], q[0]);
    }
  }
  else if (d0 == 2 and d1 == 1)
  {
    switch (gdim) {
    case 2: return intersection_triangle_segment_2d(p[0], p[1], p[2], q[0], q[1]);
    case 3: return intersection_triangle_segment_3d(p[0], p[1], p[2], q[0], q[1]);
    }
  }
  else if (d0 == 2 and d1 == 2)
  {
    switch (gdim) {
    case 2: return intersection_triangle_triangle_2d(p[0], p[1], p[2], q[0], q[1], q[2]);
    case 3: return intersection_triangle_triangle_3d(p[0], p[1], p[2], q[0], q[1], q[2]);
    }
  }
  else if (d0 == 3 and d1 == 0)
  {
    switch (gdim) {
    case 3: return intersection_tetrahedron_point_3d(p[0], p[1], p[2], p[3], q[0]);
    }
  }
  else if (d0 == 3 and d1 == 1)
  {
    switch (gdim) {
    case 3: return intersection_tetrahedron_segment_3d(p[0], p[1], p[2], p[3], q[0], q[1]);
    }
  }
  else if (d0 == 3 and d1 == 2)
  {
    switch (gdim) {
    case 3: return intersection_tetrahedron_triangle_3d(p[0], p[1], p[2], p[3], q[0], q[1], q[2]);
    }
  }
  else if (d0 == 3 and d1 == 3)
  {
    switch (gdim) {
    case 3: return intersection_tetrahedron_tetrahedron_3d(p[0], p[1], p[2], p[3], q[0], q[1], q[2], q[3]);
    }
  }

  // We should not reach this point
  dolfin_error("IntersectionConstruction.cpp",
               "compute intersection",
               "Unexpected intersection: %d-%d in %d dimensions", d0, d1, gdim);

  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
// Low-level intersection construction functions
//-----------------------------------------------------------------------------
std::vector<double>
IntersectionConstruction::intersection_point_point_1d(double p0,
                                                      double q0)
{
  if (p0 == q0)
    return std::vector<double>(1, p0);
  return std::vector<double>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_point_point_2d(const Point& p0,
                                                      const Point& q0)
{
  if (p0.x() == q0.x() && p0.y() == q0.y())
    return std::vector<Point>(1, p0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_point_point_3d(const Point& p0,
                                                      const Point& q0)
{
  if (p0.x() == q0.x() && p0.y() == q0.y() && p0.z() == q0.z())
    return std::vector<Point>(1, p0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<double>
IntersectionConstruction::intersection_segment_point_1d(double p0,
                                                        double p1,
                                                        double q0)
{
  if (CollisionPredicates::collides_segment_point_1d(p0, p1, q0))
    return std::vector<double>(1, q0);
  return std::vector<double>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_point_2d(const Point& p0,
                                                        const Point& p1,
                                                        const Point& q0)
{
  if (CollisionPredicates::collides_segment_point_2d(p0, p1, q0))
    return std::vector<Point>(1, q0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_point_3d(const Point& p0,
                                                        const Point& p1,
                                                        const Point& q0)
{
  if (CollisionPredicates::collides_segment_point_3d(p0, p1, q0))
    return std::vector<Point>(1, q0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_point_2d(const Point& p0,
                                                         const Point& p1,
                                                         const Point& p2,
                                                         const Point& q0)
{
  if (CollisionPredicates::collides_triangle_point_2d(p0, p1, p2, q0))
    return std::vector<Point>(1, q0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_point_3d(const Point& p0,
                                                         const Point& p1,
                                                         const Point& p2,
                                                         const Point& q0)
{
  if (CollisionPredicates::collides_triangle_point_3d(p0, p1, p2, q0))
    return std::vector<Point>(1, q0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_tetrahedron_point_3d(const Point& p0,
                                                            const Point& p1,
                                                            const Point& p2,
                                                            const Point& p3,
                                                            const Point& q0)
{
  if (CollisionPredicates::collides_tetrahedron_point_3d(p0, p1, p2, p3, q0))
    return std::vector<Point>(1, q0);
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<double>
IntersectionConstruction::intersection_segment_segment_1d(double p0,
                                                          double p1,
                                                          double q0,
                                                          double q1)
{
  // The list of points (convex hull)
  std::vector<double> points;

  // Add point intersections (2)
  add(points, intersection_segment_point_1d(p0, p1, q0));
  add(points, intersection_segment_point_1d(p0, p1, q1));
  add(points, intersection_segment_point_1d(q0, q1, p0));
  add(points, intersection_segment_point_1d(q0, q1, p1));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_segment_2d(const Point& p0,
                                                          const Point& p1,
                                                          const Point& q0,
                                                          const Point& q1)
{
  std::vector<Point> pold = intersection_segment_segment_2d_old(p0, p1, q0, q1);
  std::vector<Point> pnew = intersection_segment_segment_2d_new(p0, p1, q0, q1);


  // if (pold.size() != pnew.size())
  // {
  //   cout << "size_old = " << pold.size() << endl;
  //   cout << "size_new = " << pnew.size() << endl;
  //   GeometryDebugging::plot({p0, p1}, {q0, q1});
  //   GeometryDebugging::plot(pold);
  //   GeometryDebugging::plot(pnew);

  //   cout << "cgal: ";
  //   const std::vector<Point> cgal = cgal_intersection_segment_segment_2d(p0,p1,q0,q1);
  //   GeometryDebugging::plot(cgal);

  //   if (pnew.size() == 0)
  //   {
  //     std::cout << __FUNCTION__<<' '<<__LINE__<<std::endl;
  //     char apa; std::cin >> apa;
  //   }
  // }

  return pnew;
  // return pold;
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_segment_2d_old(const Point& p0,
                                                              const Point& p1,
                                                              const Point& q0,
                                                              const Point& q1)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (2 + 2 = 4)
  add(points, intersection_segment_point_2d(p0, p1, q0));
  add(points, intersection_segment_point_2d(p0, p1, q1));
  add(points, intersection_segment_point_2d(q0, q1, p0));
  add(points, intersection_segment_point_2d(q0, q1, p1));

  // If we found at least one point intersection we are done
  if (points.size() > 0)
    return unique(points);

  // Compute orientation of segment end points wrt other segment
  const double p0o = orient2d(q0, q1, p0);
  const double p1o = orient2d(q0, q1, p1);
  const double q0o = orient2d(p0, p1, q0);
  const double q1o = orient2d(p0, p1, q1);

  // Compute total orientation of segments wrt other segment
  const double po = p0o*p1o;
  const double qo = q0o*q1o;

  // If both points are on the same side we are done. Note that if
  // po = 0 or qo = 0, we should have found a point collision above,
  // unless the two segments are only collinear but not colliding.
  if (po >= 0. or qo >= 0.)
    return std::vector<Point>();

  // At this point, we know that both po < 0 and qo < 0 which means
  // that we have an intersection and it is internal to both segments.
  // This is the main case. The point is given by the formula
  //
  //   x = p0 + num / den * (p1 - p0)
  //
  // However, the computation may be unstable when the two segments
  // are nearly collinear (when den is small) so special hanndling is
  // needed when this happens. To improve the chance of the point
  // ending up inside both segments, we swap the points so that the
  // computation is based on the shortest segment.

  // Compute intersection point based on shortest segment
  double num = 0., den = 0.; Point x;
  if (p0.squared_distance(p1) < q0.squared_distance(q1))
  {
    num = p0o;
    den = (p1.x() - p0.x())*(q1.y() - q0.y())
        - (p1.y() - p0.y())*(q1.x() - q0.x());
    x = p0 + num / den * (p1 - p0);
  }
  else
  {
    num = q0o;
    den = (q1.x() - q0.x())*(p1.y() - p0.y())
        - (q1.y() - q0.y())*(p1.x() - p0.x());
    x = q0 + num / den * (q1 - q0);
  }

  // Special case: almost collinear segments. Intersection is very
  // hard to compute so just make sure we pick a sensible point which
  // we know (almost) belongs to both segments.
  if (std::abs(den*den) < DOLFIN_EPS_LARGE*std::abs(num))
  {
    // Compute major axis
    const std::size_t major_axis = GeometryTools::major_axis_2d(p1 - p0);

    // Sort the points along major axis
    std::array<Point, 4> _points = {p0, p1, q0, q1};
    std::sort(_points.begin(), _points.end(),
              [major_axis](const Point& a, const Point& b)
              { return a[major_axis] < b[major_axis]; });

    // Compute midpoint
    const Point x = 0.5*(_points[1] + _points[2]);

    return std::vector<Point>(1, x);
  }

  return std::vector<Point>(1, x);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_segment_2d_new(const Point& p0,
                                                              const Point& p1,
                                                              const Point& q0,
                                                              const Point& q1)
{
  // FIXME: This version is not yet working for some reason... Make
  // this version work and then remove the other implementation.  Note
  // that this function is very similar to triangle_segment_3d so if a
  // bug is found here, it is likely present also in that function.

  // We consider the following 4 cases for the segment q0-q1
  // relative to the line defined by the segment p0-p1:
  //
  // Case 0: qo = q0o*q1o > 0.
  //
  //   --> points on the same side.
  //   --> no intersection
  //
  // Case 1: (q0o == 0. and q1o != 0.) or (q0o != 0. and q1o == 0.)
  //
  //   --> exactly one point in plane
  //   --> possible point intersection
  //
  // Case 2: q0o = 0. and q10 = 0. [or unstable case]]
  //
  //   --> points in plane
  //   --> project to 1D
  //
  // Case 3: qo = q0o*q1o < 0.
  //
  //   --> points on different sides
  //   --> compute intersection point with line
  //   --> project to 1D and check if point is inside segment
  //
  // Note that the computation in Case 3 may be sensitive to rounding
  // errors if both points are almost on the line. If this happens
  // we instead consider the points to be on the plane [Case 2] to
  // obtain one or more sensible if points (if any).

   // Compute orientation of segment end points wrt line
  const double q0o = orient2d(p0, p1, q0);
  const double q1o = orient2d(p0, p1, q1);

  // Compute total orientation of segment wrt line
  const double qo = q0o*q1o;

  // std::cout << "q0o q1o and product "<< q0o<<' '<<q1o<<' '<<qo<<std::endl;

  // Case 0: points on the same side --> no intersection
  if (qo > 0.)
    return std::vector<Point>();

  // Case 1: exactly one point on line --> possible point intersection
  if (q0o == 0. and q1o != 0.)
    return intersection_segment_point_2d(p0, p1, q0);
  else if (q0o != 0 and q1o == 0.)
    return intersection_segment_point_2d(p0, p1, q1);

  // Compute line vector and major axis
  const Point v = p1 - p0;
  const std::size_t major_axis = GeometryTools::major_axis_2d(v);

  // Project points to major axis
  const double P0 = GeometryTools::project_to_axis_2d(p0, major_axis);
  const double P1 = GeometryTools::project_to_axis_2d(p1, major_axis);
  const double Q0 = GeometryTools::project_to_axis_2d(q0, major_axis);
  const double Q1 = GeometryTools::project_to_axis_2d(q1, major_axis);

  // Case 2: both points on line (or almost)
  if (std::abs(q0o) < DOLFIN_EPS_LARGE and std::abs(q1o) < DOLFIN_EPS_LARGE)
  {
    // std::cout << __FUNCTION__<<" parallel"<<std::endl;


    // // Compute major axis
    // const std::size_t major_axis = GeometryTools::major_axis_2d(p1 - p0);

    // // Sort the points along major axis
    // std::array<Point, 4> _points = {p0, p1, q0, q1};
    // std::sort(_points.begin(), _points.end(),
    //           [major_axis](const Point& a, const Point& b)
    //           { return a[major_axis] < b[major_axis]; });

    // // Compute midpoint
    // const Point x = 0.5*(_points[1] + _points[2]);

    // return std::vector<Point>(1, x);



    // Compute 1D intersection points
    const std::vector<double>
      points_1d = intersection_segment_segment_1d(P0, P1, Q0, Q1);

    // Unproject points: add back second coordinate
    std::vector<Point> points;
    switch (major_axis)
    {
    case 0:
      for (auto p : points_1d)
      {
	// std::cout << p0.y()<<" "<<p<<' '<<p0.x()<<' '<<v.y()<<' '<<v.x()<<std::endl;

        const double y = p0.y() + (p - p0.x()) * v.y() / v.x();
        points.push_back(Point(p, y));
      }
      break;
    default:
      for (auto p : points_1d)
      {
        const double x = p0.x() + (p - p0.y()) * v.x() / v.y();
        points.push_back(Point(x, p));
      }
    }

    return unique(points);
  }

  // Case 3: points on different sides (main case)

  // Compute intersection point x

  // const double num = q0o;
  // const double den = (q1.x() - q0.x())*v.y() - (q1.y() - q0.y())*v.x();
  // //const Point x = q0 + num / den * (q1 - q0);

  // Point x;
  // const double f = num / den;

  // // If f is close to 1, swap for improved accuracy
  // std::cout << "f = " << f << std::endl;
  // if (std::abs(f - 1) < 0.1)
  // {
  //   // Swapping q0 and q1 means changing numerator and flipping sign
  //   // of denominator
  //   std::cout << "swapping"<<std::endl;
  //   const double num_swapped = q1o;
  //   x = q1 - num_swapped / den * (q0 - q1);
  // }
  // else
  // {
  //   x = q0 + num / den * (q1 - q0);
  // }

  // {
  //   std::cout << "lengths " << (q0-q1).squared_norm()<<' '<<(p0-p1).squared_norm()<<std::endl;
  //   std::cout << "num den " << num << ' ' << den << " gives " << x << " with major axis " << major_axis << std::endl;
  //   const double X = GeometryTools::project_to_axis_2d(x, major_axis);
  //   std::cout<<std::setprecision(15) << "check major axis " << major_axis << " " <<P0<<' '<<P1<<' '<<X<<std::endl;
  //   std::cout << "Collision? " << CollisionPredicates::collides_segment_point_1d(P0, P1, X) << std::endl;

  //   // Test use p
  //   const double num = orient2d(q0, q1, p0);
  //   const Point v = q1 - q0;
  //   const double den = (p1.x() - p0.x())*v.y() - (p1.y() - p0.y())*v.x();
  //   std::cout << "new num den " << num << ' ' << den << std::endl;
  //   const Point x = p0 + num / den * (p1 - p0);
  //   const std::size_t major_axis = GeometryTools::major_axis_2d(v);
  //   const double Y = GeometryTools::project_to_axis_2d(x, major_axis);
  //   const double P0 = GeometryTools::project_to_axis_2d(p0, major_axis);
  //   const double P1 = GeometryTools::project_to_axis_2d(p1, major_axis);
  //   const double Q0 = GeometryTools::project_to_axis_2d(q0, major_axis);
  //   const double Q1 = GeometryTools::project_to_axis_2d(q1, major_axis);
  //   std::cout << "Collision? " << CollisionPredicates::collides_segment_point_1d(Q0, Q1, Y) << std::endl;

  // }

  // Test use the smallest numerator
  const double p0o = orient2d(q0, q1, p0);
  const double p1o = orient2d(q0, q1, p1);
  const double den = (q1.x() - q0.x())*v.y() - (q1.y() - q0.y())*v.x();
  std::array<std::pair<double, std::size_t>, 4> oo = {{ { std::abs(p0o), 0},
							{ std::abs(p1o), 1},
							{ std::abs(q0o), 2},
							{ std::abs(q1o), 3 } }};
  const auto it = std::min_element(oo.begin(), oo.end());

  Point a, b;
  int sign;
  double num;
  switch (it->second)
  {
  case 0: // p0o
    num = p0o;
    a = p0;
    b = p1;
    sign = -1;
    break;
  case 1: // p1o
    num = p1o;
    a = p1;
    b = p0;
    sign = 1;
    break;
  case 2: // q0o
    num = q0o;
    a = q0;
    b = q1;
    sign = 1;
    break;
  case 3: // q1o
    num = q1o;
    a = q1;
    b = q0;
    sign = -1;
    break;
  default:
    dolfin_assert(false);
  }

  const Point x = a + sign*num / den * (b - a);

  // std::cout << a<<' '<<b<<' '<<num<<' '<<sign*den<<"    "<<x<<std::endl;

  // Project point to major axis and check if inside segment
  const double X = GeometryTools::project_to_axis_2d(x, major_axis);
  if (CollisionPredicates::collides_segment_point_1d(P0, P1, X))
    return std::vector<Point>(1, x);

  //std::cout << "empty return"<<std::endl;
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_segment_segment_3d(const Point& p0,
                                                          const Point& p1,
                                                          const Point& q0,
                                                          const Point& q1)
{
  // This function is not used so no need to spend time on the implementation.
  dolfin_not_implemented();
  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_segment_2d(const Point& p0,
                                                           const Point& p1,
                                                           const Point& p2,
                                                           const Point& q0,
                                                           const Point& q1)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (2)
  add(points, intersection_triangle_point_2d(p0, p1, p2, q0));
  add(points, intersection_triangle_point_2d(p0, p1, p2, q1));

  // Add segment-segment intersections (3)
  add(points, intersection_segment_segment_2d(p0, p1, q0, q1));
  add(points, intersection_segment_segment_2d(p0, p2, q0, q1));
  add(points, intersection_segment_segment_2d(p1, p2, q0, q1));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_segment_3d(const Point& p0,
                                                           const Point& p1,
                                                           const Point& p2,
                                                           const Point& q0,
                                                           const Point& q1)
{
  // We consider the following 4 cases for the segment q0-q1
  // relative to the plane defined by the triangle p0-p1-p2:
  //
  // Case 0: qo = q0o*q1o > 0.
  //
  //   --> points on the same side
  //   --> no intersection
  //
  // Case 1: (q0o == 0. and q1o != 0.) or (q0o != 0. and q1o == 0.)
  //
  //   --> exactly one point in plane
  //   --> possible point intersection
  //
  // Case 2: q0o = 0. and q10 = 0. [or unstable case]
  //
  //   --> points in plane
  //   --> project to 2D
  //
  // Case 3: qo = q0o*q1o < 0.
  //
  //   --> points on different sides
  //   --> compute intersection point with plane
  //   --> project to 2D and check if point is inside triangle
  //
  // Note that the computation in Case 3 may be sensitive to rounding
  // errors if both points are almost in the plane. If this happens
  // we instead consider the points to be in the plane [Case 2] to
  // obtain one or more sensible if points (if any).

  // Compute orientation of segment end points wrt plane
  const double q0o = orient3d(p0, p1, p2, q0);
  const double q1o = orient3d(p0, p1, p2, q1);

  // Compute total orientation of segment wrt plane
  const double qo = q0o*q1o;

  // Case 0: points on the same side --> no intersection
  if (qo > 0.)
    return std::vector<Point>();

  // Case 1: exactly one point in plane --> possible point intersection
  if (q0o == 0. and q1o != 0.)
    return intersection_triangle_point_3d(p0, p1, p2, q0);
  else if (q0o != 0. and q1o == 0.)
    return intersection_triangle_point_3d(p0, p1, p2, q1);

  // Compute plane normal and major axis
  const Point n = GeometryTools::cross_product(p0, p1, p2);
  const std::size_t major_axis = GeometryTools::major_axis_3d(n);

  // Project points to major axis plane
  const Point P0 = GeometryTools::project_to_plane_3d(p0, major_axis);
  const Point P1 = GeometryTools::project_to_plane_3d(p1, major_axis);
  const Point P2 = GeometryTools::project_to_plane_3d(p2, major_axis);
  const Point Q0 = GeometryTools::project_to_plane_3d(q0, major_axis);
  const Point Q1 = GeometryTools::project_to_plane_3d(q1, major_axis);

  // Case 2: both points in plane (or almost)
  if (std::abs(q0o) < DOLFIN_EPS_LARGE and std::abs(q1o) < DOLFIN_EPS_LARGE)
  {
    // Compute 2D intersection points
    const std::vector<Point>
      points_2d = intersection_triangle_segment_2d(P0, P1, P2, Q0, Q1);

    // Unproject points: add back third coordinate
    std::vector<Point> points;
    switch (major_axis)
    {
    case 0:
      for (auto P : points_2d)
      {
        const double x = p0.x() + ((p0.y() - P.x())*n.y() + (p0.z() - P.y())*n.z()) / n.x();
        points.push_back(Point(x, P.x(), P.y()));
      }
      break;
    case 1:
      for (auto P : points_2d)
      {
        const double y = p0.y() + ((p0.x() - P.x())*n.x() + (p0.z() - P.y())*n.z()) / n.y();
        points.push_back(Point(P.x(), y, P.y()));
      }
      break;
    default:
      for (auto P : points_2d)
      {
        const double z = p0.z() + ((p0.x() - P.x())*n.x() + (p0.y() - P.y())*n.y()) / n.z();
        points.push_back(Point(P.x(), P.y(), z));
      }
    }

    return unique(points);
  }

  // Case 3: points on different sides (main case)

  // Compute intersection point
  const double num = n.dot(p0 - q0);
  const double den = n.dot(q1 - q0);
  const Point x = p0 + num / den * (p1 - p0);

  // Project point to major axis plane and check if inside triangle
  const Point X = GeometryTools::project_to_plane_3d(x, major_axis);
  if (CollisionPredicates::collides_triangle_point_2d(P0, P1, P2, X))
    return std::vector<Point>(1, x);

  return std::vector<Point>();
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_tetrahedron_segment_3d(const Point& p0,
                                                              const Point& p1,
                                                              const Point& p2,
                                                              const Point& p3,
                                                              const Point& q0,
                                                              const Point& q1)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (4 + 4 = 8)
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q0));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q1));

  // Add triangle-segment intersections (4)
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q1));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
// Intersections with triangles and tetrahedra: computed by delegation
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_triangle_2d(const Point& p0,
                                                            const Point& p1,
                                                            const Point& p2,
                                                            const Point& q0,
                                                            const Point& q1,
                                                            const Point& q2)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (3 + 3 = 6)
  add(points, intersection_triangle_point_2d(p0, p1, p2, q0));
  add(points, intersection_triangle_point_2d(p0, p1, p2, q1));
  add(points, intersection_triangle_point_2d(p0, p1, p2, q2));
  add(points, intersection_triangle_point_2d(q0, q1, q2, p0));
  add(points, intersection_triangle_point_2d(q0, q1, q2, p1));
  add(points, intersection_triangle_point_2d(q0, q1, q2, p2));

  // Add segment-segment intersections (3 x 3 = 9)
  add(points, intersection_segment_segment_2d(p0, p1, q0, q1));
  add(points, intersection_segment_segment_2d(p0, p1, q0, q2));
  add(points, intersection_segment_segment_2d(p0, p1, q1, q2));
  add(points, intersection_segment_segment_2d(p0, p2, q0, q1));
  add(points, intersection_segment_segment_2d(p0, p2, q0, q2));
  add(points, intersection_segment_segment_2d(p0, p2, q1, q2));
  add(points, intersection_segment_segment_2d(p1, p2, q0, q1));
  add(points, intersection_segment_segment_2d(p1, p2, q0, q2));
  add(points, intersection_segment_segment_2d(p1, p2, q1, q2));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_triangle_triangle_3d(const Point& p0,
                                                            const Point& p1,
                                                            const Point& p2,
                                                            const Point& q0,
                                                            const Point& q1,
                                                            const Point& q2)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (3 + 3 = 6)
  add(points, intersection_triangle_point_3d(p0, p1, p2, q0));
  add(points, intersection_triangle_point_3d(p0, p1, p2, q1));
  add(points, intersection_triangle_point_3d(p0, p1, p2, q2));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p0));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p1));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p2));

  // Add triangle-segment intersections (3 + 3 = 6)
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q1, q2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p1));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p1, p2));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_tetrahedron_triangle_3d(const Point& p0,
                                                               const Point& p1,
                                                               const Point& p2,
                                                               const Point& p3,
                                                               const Point& q0,
                                                               const Point& q1,
                                                               const Point& q2)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (3 + 4 = 7)
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q0));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q1));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q2));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p0));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p1));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p2));
  add(points, intersection_triangle_point_3d(q0, q1, q2, p3));

  // Add triangle-segment intersections (4 x 3 + 1 x 6 = 18)
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q1, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p1));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p1, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p1, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p2, p3));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
std::vector<Point>
IntersectionConstruction::intersection_tetrahedron_tetrahedron_3d(const Point& p0,
                                                                  const Point& p1,
                                                                  const Point& p2,
                                                                  const Point& p3,
                                                                  const Point& q0,
                                                                  const Point& q1,
                                                                  const Point& q2,
                                                                  const Point& q3)
{
  // The list of points (convex hull)
  std::vector<Point> points;

  // Add point intersections (4 + 4 = 8)
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q0));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q1));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q2));
  add(points, intersection_tetrahedron_point_3d(p0, p1, p2, p3, q3));
  add(points, intersection_tetrahedron_point_3d(q0, q1, q2, q3, p0));
  add(points, intersection_tetrahedron_point_3d(q0, q1, q2, q3, p1));
  add(points, intersection_tetrahedron_point_3d(q0, q1, q2, q3, p2));
  add(points, intersection_tetrahedron_point_3d(q0, q1, q2, q3, p3));

  // Let's hope we got this right... :-)

  // Add triangle-segment intersections (4 x 6 + 4 x 6 = 48)
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q0, q3));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q1, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q1, q3));
  add(points, intersection_triangle_segment_3d(p0, p1, p2, q2, q3));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q0, q3));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q1, q3));
  add(points, intersection_triangle_segment_3d(p0, p1, p3, q2, q3));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q0, q3));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q1, q3));
  add(points, intersection_triangle_segment_3d(p0, p2, p3, q2, q3));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q1));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q2));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q0, q3));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q1, q2));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q1, q3));
  add(points, intersection_triangle_segment_3d(p1, p2, p3, q2, q3));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p1));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p0, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p1, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p1, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q2, p2, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p0, p1));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p0, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p0, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p1, p2));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p1, p3));
  add(points, intersection_triangle_segment_3d(q0, q1, q3, p2, p3));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p0, p1));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p0, p2));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p0, p3));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p1, p2));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p1, p3));
  add(points, intersection_triangle_segment_3d(q0, q2, q3, p2, p3));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p0, p1));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p0, p2));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p0, p3));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p1, p2));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p1, p3));
  add(points, intersection_triangle_segment_3d(q1, q2, q3, p2, p3));

  dolfin_assert(GeometryPredicates::is_finite(points));
  return unique(points);
}
//-----------------------------------------------------------------------------
