// Copyright (C) 2016-2017 Anders Logg, August Johansson and Benjamin Kehlet
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
// First added:  2016-06-01
// Last changed: 2017-06-29

#include <algorithm>
#include <tuple>
#include <set>
#include <iomanip>
#include "predicates.h"
#include "GeometryPredicates.h"
#include "GeometryTools.h"
#include "ConvexTriangulation.h"

//-----------------------------------------------------------------------------
namespace
{
  struct point_strictly_less
  {
    bool operator()(const dolfin::Point& p0, const dolfin::Point& p1)
    {
      if (p0.x() != p1.x())
	return p0.x() < p1.x();


      if (p0.y() != p1.y())
        return p0.y() < p1.y();

      return p0.z() < p1.z();
    }
  };

  inline bool operator==(const dolfin::Point& p0, const dolfin::Point& p1)
  {
    return p0.x() == p1.x() && p0.y() == p1.y() && p0.z() == p1.z();
  }

  inline bool operator!=(const dolfin::Point& p0, const dolfin::Point& p1)
  {
    return p0.x() != p1.x() || p0.y() != p1.y() || p0.z() != p1.z();
  }
}

using namespace dolfin;

//------------------------------------------------------------------------------
bool ConvexTriangulation::Edge::operator==(const Edge& e) const
{
  return (p0 == e.p0 and p1 == e.p1) or
    (p1 == e.p0 and p0 == e.p1);
}
//------------------------------------------------------------------------------
bool ConvexTriangulation::Triangle::operator==(const Triangle& t) const
{
  return (p0 == t.p0 or p0 == t.p1 or p0 == t.p2) and
    (p1 == t.p0 or p1 == t.p1 or p1 == t.p2) and
    (p2 == t.p0 or p2 == t.p1 or p2 == t.p2);
}
//------------------------------------------------------------------------------
bool ConvexTriangulation::Triangle::contains_vertex(const Point& v) const
{
  return p0 == v or p1 == v or p2 == v;
}
//------------------------------------------------------------------------------
bool ConvexTriangulation::Triangle::circumcircle_contains(const Point& v) const
{
  const double ab = p0.squared_norm();
  const double cd = p1.squared_norm();
  const double ef = p2.squared_norm();

  const double circum_x = (ab * (p2.y() - p1.y()) + cd * (p0.y() - p2.y()) + ef * (p1.y() - p0.y())) / (p0.x() * (p2.y() - p1.y()) + p1.x() * (p0.y() - p2.y()) + p2.x() * (p1.y() - p0.y())) / 2.;
  const double circum_y = (ab * (p2.x() - p1.x()) + cd * (p0.x() - p2.x()) + ef * (p1.x() - p0.x())) / (p0.y() * (p2.x() - p1.x()) + p1.y() * (p0.x() - p2.x()) + p2.y() * (p1.x() - p0.x())) / 2.;

  const Point c(circum_x, circum_y);
  const double circum_radius = (p1 - c).norm();

  const double dist = (v - c).norm();
  return dist <= circum_radius;
}
//------------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::triangulate(const std::vector<Point>& p,
                                 std::size_t gdim,
                                 std::size_t tdim)
{
  if (p.empty())
    return std::vector<std::vector<Point>>();

  if (tdim == 1)
  {
    return triangulate_1d(p, gdim);
  }
  else if (tdim == 2 && gdim == 2)
  {
    return triangulate_graham_scan_2d(p);
    //return triangulate_delaunay_2d(p);
  }
  else if (tdim == 3 && gdim == 3)
  {
    return triangulate_graham_scan_3d(p);
  }

  dolfin_error("ConvexTriangulation.cpp",
               "triangulate convex polyhedron",
               "Triangulation of polyhedron of topological dimension %u and geometric dimension %u not implemented", tdim, gdim);
}
//-----------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::_triangulate_1d(const std::vector<Point>& p,
				     std::size_t gdim)
{
  // A convex polyhedron of topological dimension 1 can not have more
  // than two points. If more, they must be collinear (more or
  // less). This can happen due to tolerances in
  // IntersectionConstruction::intersection_segment_segment_2d.

  const std::vector<Point> unique_p = unique_points(p, gdim, DOLFIN_EPS);

  if (unique_p.size() > 2)
  {
    // Make sure the points are approximately collinear
    bool collinear = true;
    for (std::size_t i = 2; i < unique_p.size(); ++i)
    {
      const double o = orient2d(unique_p[0], unique_p[1], unique_p[i]);
      if (std::abs(o) > DOLFIN_EPS_LARGE)
      {
	collinear = false;
	break;
      }
    }

    dolfin_assert(collinear);

    // Average
    Point average(0.0, 0.0, 0.0);
    for (const Point& q: unique_p)
      average += q;
    average /= unique_p.size();
    std::vector<std::vector<Point>> t = {{ average }};
    return t;


    // if (unique_p.size() == 3)
    // {
    //   const double o = orient2d(unique_p[0],unique_p[1],unique_p[2]);
    //   if (std::abs(o) < DOLFIN_EPS_LARGE)
    //   {
    // 	Point average = (unique_p[0] + unique_p[1] + unique_p[2]) / 3.0;
    // 	std::vector<std::vector<Point>> t = {{ p }};
    // 	return t;
    //   }
    // }

    // std::cout << __FUNCTION__<< " input:\n";
    // for (const Point pp: p)
    //   std::cout << pp[0]<<' '<<pp[1]<<'\n';
    // std::cout <<"unique:\n";
    // for (const Point pp: unique_p)
    //   std::cout <<std::setprecision(15)<< pp[0]<<' '<<pp[1]<<'\n';
    // std::cout << "orient2d " << orient2d(unique_p[0],unique_p[1],unique_p[2])<<std::endl;

    dolfin_error("ConvexTriangulation.cpp",
  		 "triangulate convex polyhedron",
  		 "A convex polyhedron of topological dimension 1 can not have more than 2 points");
  }

  std::vector<std::vector<Point>> t = { unique_p };
  return t;
}

//------------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::_triangulate_graham_scan_2d(const std::vector<Point>& input_points)
{
  dolfin_assert(GeometryPredicates::is_finite(input_points));

  // Make sure the input points are unique
  const std::size_t gdim = 2;
  std::vector<Point> points = unique_points(input_points, gdim, DOLFIN_EPS);

  if (points.size() < 3)
    return std::vector<std::vector<Point>>();

  if (points.size() == 3)
  {
    std::vector<std::vector<Point>> triangulation(1, points);
    return triangulation;
  }

  // Sometimes we can get an extra point on an edge: a-----c--b. This
  // point c may cause problems for the graham scan. To avoid this,
  // use an extra center point.  Use this center point and point no 0
  // as reference for the angle calculation
  Point pointscenter = points[0];
  for (std::size_t m = 1; m < points.size(); ++m)
    pointscenter += points[m];
  pointscenter /= points.size();

  // Reference
  const Point ref = points[0] - pointscenter;

  // Calculate and store angles
  std::vector<std::pair<double, std::size_t>> order;
  for (std::size_t m = 1; m < points.size(); ++m)
  {
    const double A = orient2d(pointscenter, points[0], points[m]);
    const Point s = points[m] - pointscenter;
    double alpha = std::atan2(A, s.dot(ref));
    if (alpha < 0)
      alpha += 2.0*DOLFIN_PI;
    order.emplace_back(alpha, m);
  }

  // Sort angles
  std::sort(order.begin(), order.end());

  // Tessellate
  std::vector<std::vector<Point>> triangulation(order.size() - 1);

  for (std::size_t m = 0; m < order.size()-1; ++m)
  {
    // FIXME: We could consider only triangles with area > tolerance here.
    triangulation[m] = {{ points[0],
    			  points[order[m].second],
    			  points[order[m + 1].second] }};
  }

  return triangulation;
}
//-----------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::_triangulate_delaunay_2d(const std::vector<Point>& input_points)
{
  // Delaunay triangulation using the Bowyer-Watson algorithm
  // https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm
  // Implementation from
  // https://github.com/Bl4ckb0ne/delaunay-triangulation/blob/master/delaunay.h
  dolfin_assert(GeometryPredicates::is_finite(input_points));

  // Only do 2D for now
  const std::size_t gdim = 2;

  // Make sure the input points are unique
  std::vector<Point> points = unique_points(input_points, gdim, DOLFIN_EPS);

  if (points.size() < 3)
    return std::vector<std::vector<Point>>();

  std::vector<std::vector<Point>> triangulation;

  if (points.size() == 3)
  {
    triangulation.push_back(points);
    return triangulation;
  }

  // Find point bounds
  Point pmin = points[0];
  Point pmax = points[0];

  for (std::size_t i = 1; i < points.size(); ++i)
  {
    for (std::size_t d = 0; d < gdim; ++d)
    {
      pmin[d] = (pmin[d] > points[i][d]) ? pmin[d] : points[i][d];
      pmax[d] = (pmax[d] < points[i][d]) ? pmax[d] : points[i][d];
    }
  }

  // Find mid point
  const Point pmid = (pmin + pmax) / 2.0;

  // Find max bound
  double dx;
  for (std::size_t d = 0; d < gdim; ++d)
    dx = std::max(dx, std::abs(pmin[d] - pmax[d]));

  // Create super triangle containing all points
  Point a;
  a[0] = pmid[0] - 20*dx;
  a[1] = pmid[1] - dx;
  Point b;
  b[0] = pmid[0];
  b[1] = pmid[1] + 20*dx;
  Point c;
  c[0] = pmid[0] + 20*dx;
  c[1] = pmid[1] - dx;

  // Add the super triangle to the triangulation
  std::vector<Triangle> triangles;
  Triangle abc(a, b, c);
  triangles.push_back(abc);

  for (const Point p: points)
  {
    std::vector<Triangle> bad_triangles;
    std::vector<Edge> polygon;

    for (const Triangle t: triangles)
    {
      if (t.circumcircle_contains(p))
      {
        bad_triangles.push_back(t);
        polygon.push_back(t.e0);
        polygon.push_back(t.e1);
        polygon.push_back(t.e2);
      }
    }

    triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
				   [bad_triangles](const Triangle &t)
				   {
				     for (const Triangle& bt: bad_triangles)
				       if (bt == t)
					 return true;
				     return false;
				   }),
		    triangles.end());

    std::vector<Edge> bad_edges;

    for (std::vector<Edge>::const_iterator e0 = polygon.begin(); e0 != polygon.end(); ++e0)
      for (std::vector<Edge>::const_iterator e1 = polygon.begin(); e1 != polygon.end(); ++e1)
      {
	if (e0 == e1)
	  continue;
	if (*e0 == *e1)
	{
	  bad_edges.push_back(*e0);
	  bad_edges.push_back(*e1);
	}
      }

    polygon.erase(std::remove_if(polygon.begin(), polygon.end(),
				 [bad_edges](const Edge& e)
				 {
				   for (const Edge& be: bad_edges)
				     if (be == e)
				       return true;
				   return false;
				 }),
		  polygon.end());

    for (const Edge e: polygon)
    {
      Triangle te(e.p0, e.p1, p);
      triangles.push_back(te);
    }
  } // end loop over points

  triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
				 [a, b, c](const Triangle& t)
				 {
				   return t.contains_vertex(a) or
				     t.contains_vertex(b) or
				     t.contains_vertex(c);
				 }),
		  end(triangles));

  dolfin_assert(triangles.size());

  triangulation.resize(triangles.size());
  for (std::size_t i = 0; i < triangles.size(); ++i)
    triangulation[i] = {{ triangles[i].p0, triangles[i].p1, triangles[i].p2 }};
  return triangulation;
}
//-----------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::_triangulate_graham_scan_3d(const std::vector<Point>& input_points)
{
  dolfin_assert(GeometryPredicates::is_finite(input_points));

  //std::cout << "Input to 3D Graham scan:" << std::endl;
  //for (auto p : input_points)
  //  std::cout << p << std::endl;

  const double coplanar_tol = 1000*DOLFIN_EPS_LARGE;

  // Make sure the input points are unique. We assume this has
  // negligble effect on volume
  const std::size_t gdim = 3;
  std::vector<Point> points = unique_points(input_points, gdim, DOLFIN_EPS);

  std::vector<std::vector<Point>> triangulation;

  if (points.size() < 4)
  {
    // Empty
    return triangulation;
  }
  else if (points.size() == 4)
  {
    // Single tetrahedron
    triangulation.push_back(points);
    return triangulation;
  }
  else
  {
    // Construct tetrahedra using facet points and a center point
    Point polyhedroncenter(0,0,0);
    for (const Point& p: points)
      polyhedroncenter += p;
    polyhedroncenter /= points.size();

    // FIXME: Better data structure than set?
    std::set<std::tuple<std::size_t, std::size_t, std::size_t> > checked;

    // Loop over all triplets
    for (std::size_t i = 0; i < points.size(); ++i)
    {
      for (std::size_t j = i+1; j < points.size(); ++j)
      {
        for (std::size_t k = j+1; k < points.size(); ++k)
        {
	  if (checked.emplace(std::make_tuple(i, j, k)).second)
	  {
            // Test for the special case where i, j, k are collinear
            {
              const Point ij = points[j] - points[i];
              const Point ik = points[k] - points[i];
              if ( -(std::abs( (ij/ij.norm() ).dot(ik/ik.norm()))-1)  < DOLFIN_EPS)
                continue;
            }

            // Check whether all other points are on one side of this
            // (i,j,k) facet, i.e. we're on the convex
            // hull. Initialize as true for the case of only three
            // coplanar points.
	    bool on_convex_hull = true;

	    // Use orient3d to determine if the plane (i,j,k) is on the
	    // convex hull.
	    std::vector<std::size_t> coplanar = { i, j, k };
	    double previous_orientation;
	    bool first = true;

	    for (std::size_t m = 0; m < points.size(); ++m)
	    {
	      if (m != i and m != j and m != k)
	      {
		const double orientation = orient3d(points[i],
						    points[j],
						    points[k],
						    points[m]);
		// Save point index if we find coplanar points
		if (orientation == 0)
		  coplanar.push_back(m);
                else
                {
                  if (first)
                  {
                    previous_orientation = orientation;
                    first = false;
                  }
                  else
                  {
                    // Sign change: triangle is not on convex hull
                    if (previous_orientation * orientation < 0)
                    {
                      on_convex_hull = false;
                      // break;
                    }
                  }
		}
	      }
	    }

	    if (on_convex_hull)
	    {
	      if (coplanar.size() == 3)
	      {
		// Form one tetrahedron
		std::vector<Point> cand = { points[i],
					    points[j],
					    points[k],
					    polyhedroncenter };

#ifdef DOLFIN_ENABLE_GEOMETRY_DEBUGGING
                if (cgal_tet_is_degenerate(cand))
                  dolfin::dolfin_error("ConvexTriangulation.cpp",
                                       "triangulation 3d points",
                                       "tet is degenerate");

#endif

		// FIXME: Here we could include if determinant is sufficiently large
                //for (auto p : cand)
                //  std::cout << " " << p;
                //std::cout << std::endl;
		triangulation.push_back(cand);
	      }
	      else // At least four coplanar points
	      {
		// Tessellate as in the triangle-triangle intersection
		// case: First sort points using a Graham scan, then
		// connect to form triangles. Finally form tetrahedra
		// using the center of the polyhedron.

		// Use the center of the coplanar points and point no 0
		// as reference for the angle calculation
		Point pointscenter = points[coplanar[0]];
		for (std::size_t m = 1; m < coplanar.size(); ++m)
		  pointscenter += points[coplanar[m]];
		pointscenter /= coplanar.size();

		// Reference
		Point ref = points[coplanar[0]] - pointscenter;
		ref /= ref.norm();

		// Normal
		Point normal = GeometryTools::cross_product(points[i], points[j], points[k]);
		normal /= normal.norm();

		// Calculate and store angles
		std::vector<std::pair<double, std::size_t>> order;
		for (std::size_t m = 0; m < coplanar.size(); ++m)
		{
		  const Point v = points[coplanar[m]] - pointscenter;
		  const double frac = ref.dot(v) / v.norm();
		  double alpha;

		  if (frac <= -1)
		    alpha = DOLFIN_PI;
		  else if (frac >= 1)
		    alpha = 0;
		  else
		  {
		    alpha = std::acos(frac);
		    if (v.dot(normal.cross(ref)) < 0)
		      alpha = 2*DOLFIN_PI - alpha;
		  }
		  order.push_back(std::make_pair(alpha, m));
		}

		// Sort angles
		std::sort(order.begin(), order.end());

                // Scan for a point which is not colinear with two other
                // consecutive points (as this will introduce a degenerate triangle)

                int ref_index = -1;
                bool is_colinear = false;
                do {
                  is_colinear = false;
                  ref_index++;
                  const Point ref = points[ coplanar[ order[ ref_index].second ]];
                  for (std::size_t i = 1; i < coplanar.size()-1; i++)
                  {
                    const Point p1 = points[ coplanar[ order[ (ref_index+i)%coplanar.size()].second]];
                    const Point p2 = points[ coplanar[ order[ (ref_index+i+1)%coplanar.size()].second]];
                    const Point a = p1-ref;
                    const Point b = p2-ref;
                    const double cos_angle = (a/a.norm()).dot(b/b.norm());

                    if (-(std::abs(cos_angle) - 1.0) < DOLFIN_EPS )
                    {
                      is_colinear = true;
                      break;
                    }
                  }
                } while (is_colinear);

                // Do ear cut
                while (order.size() > 2)
                {
                  double cos_max_angle = 1.;
                  std::size_t index_max_angle;

                  for (int i = 0; i < order.size(); i++)
                  {
                    const Point p0 = points[coplanar[order[i].second]];
                    const Point p1 = points[coplanar[order[(i+1)%order.size()].second]];
                    const Point p2 = points[coplanar[order[(i+2)%order.size()].second]];
                    const Point v0 = p1-p0;
                    const Point v1 = p2-p1;

                    const double cos_angle = (v0/v0.norm()).dot(v1/v1.norm());
                    if (cos_angle < cos_max_angle)
                    {
                      cos_max_angle = cos_angle;
                      index_max_angle = i;
                    }
                  }

                  std::vector<Point> cand = {  points[coplanar[order[i].second]],
                                               points[coplanar[order[(i+1)%order.size()].second]],
                                               points[coplanar[order[(i+2)%order.size()].second]] };

                  // Suboptimal to erase from vector, but sizes are small som ok for now.
                  std::vector<std::pair<double, std::size_t>>::iterator
                    it = order.begin() + (index_max_angle+1)%order.size();

                  order.erase(it);




		  // Tessellate
		  // for (std::size_t m = 1; m < coplanar.size() - 1; ++m)
		  // {
		  //   const std::size_t p1_index = (ref_index + m)%coplanar.size();
		  //   const std::size_t p2_index = (p1_index+1)%coplanar.size();

		  //   std::vector<Point> cand = { points[coplanar[order[ref_index].second]],
		  // 			      points[coplanar[order[p1_index].second]],
		  // 			      points[coplanar[order[p2_index].second]],
		  // 			      polyhedroncenter };
		  // FIXME: Possibly only include if tet is large enough
                  //for (auto p : cand)
                  //  std::cout << " " << p;
                  //std::cout << std::endl;
                  triangulation.push_back(cand);

#ifdef DOLFIN_ENABLE_GEOMETRY_DEBUGGING
                  if (cgal_tet_is_degenerate(cand))
                  {
                    dolfin::dolfin_error("ConvexTriangulation.cpp:544",
                                         "triangulation 3d points",
                                         "tet is degenerate");
                  }

                  if (cgal_triangulation_overlap(triangulation))
                  {
                    dolfin::dolfin_error("ConvexTriangulation.cpp:544",
                                         "triangulation 3d points",
                                         "now triangulation overlaps");
                  }
#endif
		}

                std::sort(coplanar.begin(), coplanar.end());

                for (int i = 0; i < coplanar.size()-2; i++)
                {
                  for (int j = i+1; j < coplanar.size()-1; j++)
                  {
                    for (int k = j+1; k < coplanar.size(); k++)
                    {
                      checked.emplace( std::make_tuple(coplanar[i], coplanar[j], coplanar[k]) );
                    }
                  }
                }
	      } // end coplanar.size() > 3
	    } // end on_convexhull
	  }
	}
      }
    }

    return triangulation;
  }


  //   // Data structure for storing checked triangle indices (do this
  //   // better with some fancy stl structure?)
  //   const std::size_t N = points.size(), N2 = points.size()*points.size();

  //   // FIXME: this is expensive
  //   std::vector<bool> checked(N*N2 + N2 + N, false);

  //   // Find coplanar points
  //   for (std::size_t i = 0; i < N; ++i)
  //   {
  //     for (std::size_t j = i+1; j < N; ++j)
  //     {
  //       for (std::size_t k = 0; k < N; ++k)
  //       {
  //         if (!checked[i*N2 + j*N + k] and k != i and k != j)
  //         {
  //           Point n = (points[j] - points[i]).cross(points[k] - points[i]);
  //           const double tridet = n.norm();

  //           // FIXME: Here we could check that the triangle is sufficiently large
  //           // if (tridet < tri_det_tol)
  //           //   break;

  //           // Normalize normal
  //           n /= tridet;

  //           // Compute triangle center
  //           const Point tricenter = (points[i] + points[j] + points[k]) / 3.;

  //           // Check whether all other points are on one side of thus
  //           // facet. Initialize as true for the case of only three
  //           // coplanar points.
  //           bool on_convex_hull = true;

  //           // Compute dot products to check which side of the plane
  //           // (i,j,k) we're on. Note: it seems to be better to compute
  //           // n.dot(points[m]-n.dot(tricenter) rather than
  //           // n.dot(points[m]-tricenter).
  //           std::vector<double> ip(N, -(n.dot(tricenter)));
  //           for (std::size_t m = 0; m < N; ++m)
  //             ip[m] += n.dot(points[m]);

  //           // Check inner products range by finding max & min (this
  //           // seemed possibly more numerically stable than checking all
  //           // vs all and then break).
  //           double minip = std::numeric_limits<double>::max();
  //      double maxip = -std::numeric_limits<double>::max();
  //           for (size_t m = 0; m < N; ++m)
  //             if (m != i and m != j and m != k)
  //             {
  //               minip = (minip > ip[m]) ? ip[m] : minip;
  //               maxip = (maxip < ip[m]) ? ip[m] : maxip;
  //             }

  //           // Different sign => triangle is not on the convex hull
  //           if (minip*maxip < -DOLFIN_EPS)
  //             on_convex_hull = false;

  //           if (on_convex_hull)
  //           {
  //             // Find all coplanar points on this facet given the
  //             // tolerance coplanar_tol
  //             std::vector<std::size_t> coplanar;
  //             for (std::size_t m = 0; m < N; ++m)
  //               if (std::abs(ip[m]) < coplanar_tol)
  //                 coplanar.push_back(m);

  //             // Mark this plane (how to do this better?)
  //             for (std::size_t m = 0; m < coplanar.size(); ++m)
  //               for (std::size_t n = m+1; n < coplanar.size(); ++n)
  //                 for (std::size_t o = n+1; o < coplanar.size(); ++o)
  //                   checked[coplanar[m]*N2 + coplanar[n]*N + coplanar[o]]
  //                     = checked[coplanar[m]*N2 + coplanar[o]*N + coplanar[n]]
  //                     = checked[coplanar[n]*N2 + coplanar[m]*N + coplanar[o]]
  //                     = checked[coplanar[n]*N2 + coplanar[o]*N + coplanar[m]]
  //                     = checked[coplanar[o]*N2 + coplanar[n]*N + coplanar[m]]
  //                     = checked[coplanar[o]*N2 + coplanar[m]*N + coplanar[n]]
  //                     = true;

  //             // Do the actual tessellation using the coplanar points and
  //             // a center point
  //             if (coplanar.size() == 3)
  //             {
  //               // Form one tetrahedron
  //               std::vector<Point> cand(4);
  //               cand[0] = points[coplanar[0]];
  //               cand[1] = points[coplanar[1]];
  //               cand[2] = points[coplanar[2]];
  //               cand[3] = polyhedroncenter;

  //               // FIXME: Here we could include if determinant is sufficiently large
  //               triangulation.push_back(cand);
  //             }
  //             else if (coplanar.size() > 3)
  //             {
  //               // Tessellate as in the triangle-triangle intersection
  //               // case: First sort points using a Graham scan, then
  //               // connect to form triangles. Finally form tetrahedra
  //               // using the center of the polyhedron.

  //               // Use the center of the coplanar points and point no 0
  //               // as reference for the angle calculation
  //               Point pointscenter = points[coplanar[0]];
  //               for (std::size_t m = 1; m < coplanar.size(); ++m)
  //                 pointscenter += points[coplanar[m]];
  //               pointscenter /= coplanar.size();

  //               std::vector<std::pair<double, std::size_t>> order;
  //               Point ref = points[coplanar[0]] - pointscenter;
  //               ref /= ref.norm();

  //               // Calculate and store angles
  //               for (std::size_t m = 1; m < coplanar.size(); ++m)
  //               {
  //                 const Point v = points[coplanar[m]] - pointscenter;
  //                 const double frac = ref.dot(v) / v.norm();
  //                 double alpha;
  //                 if (frac <= -1)
  //                   alpha=DOLFIN_PI;
  //                 else if (frac>=1)
  //                   alpha=0;
  //                 else
  //                 {
  //                   alpha = acos(frac);
  //                   if (v.dot(n.cross(ref)) < 0)
  //                     alpha = 2*DOLFIN_PI-alpha;
  //                 }
  //                 order.push_back(std::make_pair(alpha, m));
  //               }

  //               // Sort angles
  //               std::sort(order.begin(), order.end());

  //               // Tessellate
  //               for (std::size_t m = 0; m < coplanar.size() - 2; ++m)
  //               {
  //                 // Candidate tetrahedron:
  //                 std::vector<Point> cand(4);
  //                 cand[0] = points[coplanar[0]];
  //                 cand[1] = points[coplanar[order[m].second]];
  //                 cand[2] = points[coplanar[order[m + 1].second]];
  //                 cand[3] = polyhedroncenter;

  //                 // FIXME: Possibly only include if tet is large enough
  //                 triangulation.push_back(cand);
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
  //   return triangulation;
  // }
}
//-----------------------------------------------------------------------------
std::vector<std::vector<Point>>
ConvexTriangulation::triangulate_graham_scan_3d(const std::vector<Point>& pm)
{
  std::vector<std::vector<Point>> triangulation =
    _triangulate_graham_scan_3d(pm);

#ifdef DOLFIN_ENABLE_GEOMETRY_DEBUGGING

  if (cgal_triangulation_has_degenerate(triangulation))
    dolfin::dolfin_error("ConvexTriangulation.cpp",
                         "verify convex triangulation",
                         "triangulation contains degenerate tetrahedron");

  if (cgal_triangulation_overlap(triangulation))
  {
    dolfin::dolfin_error("ConvexTriangulation.cpp",
                         "verify convex triangulation",
                         "tetrahedrons overlap");
  }


  double volume = .0;
  for (const std::vector<Point>& tet : triangulation)
  {
    dolfin_assert(tet.size() == 4);
    // for (const Point& p : tet)
    //   std::cout << "(" << p.x() << ", " << p.y() << ", " << p.z() << ")" << std::endl;
    const double tet_volume = std::abs(orient3d(tet[0], tet[1], tet[2], tet[3]))/6.0;
    // std::cout << "  vol: " << volume << std::endl;
    // std::cout << "  ref: " << cgal_tet_volume(tet) << std::endl;
    volume += tet_volume;
  }

  const double reference_volume = cgal_polyhedron_volume(pm);

  if (std::abs(volume - reference_volume) > DOLFIN_EPS)
    dolfin::dolfin_error("ConvexTriangulation.cpp",
                         "verifying convex triangulation",
                         "computed volume %f, but reference volume is %f",
                         volume, reference_volume);


#endif
  return triangulation;
}
//-----------------------------------------------------------------------------
std::vector<Point>
ConvexTriangulation::unique_points(const std::vector<Point>& input_points,
				   std::size_t gdim,
				   double tol)
{
  // Create a unique list of points in the sense that |p-q| > tol in each dimension

  std::vector<Point> points;

  for (std::size_t i = 0; i < input_points.size(); ++i)
  {
    bool unique = true;
    for (std::size_t j = i+1; unique and j < input_points.size(); ++j)
    {
      std::size_t cnt = 0;
      for (std::size_t d = 0; d < gdim; ++d)
      {
      	if (std::abs(input_points[i][d] - input_points[j][d]) > tol)
      	{
      	  cnt++;
      	}
      }
      if (cnt == 0)
      {
      	unique = false;
      	break;
      }
    }

    if (unique)
      points.push_back(input_points[i]);
  }

  return points;
}
//-----------------------------------------------------------------------------
