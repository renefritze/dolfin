// Copyright (C) 2016 Benjamin Kehlet, August Johansson, and Anders Logg
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
// First added:  2016-05-03
// Last changed: 2016-05-07
//
// Developer note:
//
// This file contains reference implementations of collision detection
// algorithms using exact arithmetic with CGAL. It is not included in
// a normal build but is used as a reference for verification and
// debugging of the inexact DOLFIN collision detection algorithms.
// Enable by setting the flag DOLFIN_ENABLE_CGAL_EXACT_ARITHMETIC.

// FIXME: This line is a test - remove it

#ifndef __CGAL_EXACT_ARITHMETIC_H
#define __CGAL_EXACT_ARITHMETIC_H

// FIXME: Debugging
#define DOLFIN_ENABLE_CGAL_EXACT_ARITHMETIC 1

#ifndef DOLFIN_ENABLE_CGAL_EXACT_ARITHMETIC

// Comparison macro just bypasses CGAL and test when not enabled
#define CHECK_CGAL(RESULT_DOLFIN, RESULT_CGAL, __FUNCTION__) RESULT_DOLFIN

#else

// Includes
#include <algorithm>
#include <sstream>
#include <dolfin/log/log.h>
#include <dolfin/math/basic.h>
#include "Point.h"

// Check that results from DOLFIN and CGAL match
namespace dolfin
{
  //---------------------------------------------------------------------------
  // Functions to compare results between DOLFIN and CGAL
  //---------------------------------------------------------------------------

  inline bool
  check_cgal(bool result_dolfin,
             bool result_cgal,
             std::string function)
  {
    if (result_dolfin != result_cgal)
    {
      // Convert results to strings
      std::stringstream s_dolfin;
      std::stringstream s_cgal;
      s_dolfin << result_dolfin;
      s_cgal << result_cgal;

      // Issue error
      dolfin_error("CGALExactArithmetic.h",
                   "verify geometric predicate with exact types",
                   "Error in predicate %s\n DOLFIN: %s\n CGAL: %s",
                   function.c_str(), s_dolfin.str().c_str(), s_cgal.str().c_str());
    }

    return result_dolfin;
  }

  inline const std::vector<Point>&
    check_cgal(const std::vector<Point>& result_dolfin,
               const std::vector<Point>& result_cgal,
               std::string function)
  {
    // FIXME: Not implemented
    return result_dolfin;
  }

  inline const std::vector<std::vector<Point>>&
    check_cgal(const std::vector<std::vector<Point>>& result_dolfin,
               const std::vector<std::vector<Point>>& result_cgal,
               std::string function)
  {
    // FIXME: do we expect dolfin and cgal data to be in the same order?

    if (result_dolfin.size() != result_cgal.size())
    {
      std::stringstream s_dolfin;
      s_dolfin.precision(16);
      for (const std::vector<Point> s: result_dolfin)
      {
        s_dolfin << "[";
        for (const Point v : s)
          s_dolfin << v << ' ';
        s_dolfin.seekp(-1, s_dolfin.cur);
        s_dolfin << "]";
      }

      std::stringstream s_cgal;
      s_cgal.precision(16);
      for (const std::vector<Point> s : result_cgal)
      {
        s_cgal << "[";
        for (const Point v : s)
          s_cgal << v << ' ';
        s_cgal.seekp(-1, s_cgal.cur);
        s_cgal << "]";
      }

      dolfin_error("CGALExactArithmetic.h",
		   "verify intersections due to different sizes",
		   "Error in function %s\n DOLFIN: %s\n CGAL: %s",
		   function.c_str(),
		   s_dolfin.str().c_str(),
		   s_cgal.str().c_str());
    }
    else
    {
      // Flatten to vector<double> for easy sorting and comparison
      std::vector<double> sorted_result_dolfin;
      for (std::vector<Point> v : result_dolfin)
      {
        for (Point p : v)
        {
          sorted_result_dolfin.push_back(p.x());
          sorted_result_dolfin.push_back(p.y());
          sorted_result_dolfin.push_back(p.z());
        }
      }
      std::sort(sorted_result_dolfin.begin(), sorted_result_dolfin.end());

      std::vector<double> sorted_result_cgal;
      for (std::vector<Point> v : result_cgal)
      {
        for (Point p : v)
        {
          sorted_result_cgal.push_back(p.x());
          sorted_result_cgal.push_back(p.y());
          sorted_result_cgal.push_back(p.z());
        }
      }
      std::sort(sorted_result_cgal.begin(), sorted_result_cgal.end());

      for (std::size_t i = 0; i < sorted_result_dolfin.size(); ++i)
	if (!near(sorted_result_dolfin[i], sorted_result_cgal[i], DOLFIN_EPS_LARGE))
	{
	  std::stringstream s_dolfin;
	  s_dolfin.precision(16);
	  for (const std::vector<Point> p : result_dolfin)
          {
            for (const Point v : p)
            {
              s_dolfin << v.str() << ' ';
            }
          }

	  std::stringstream s_cgal;
	  s_cgal.precision(16);
          for (std::vector<Point> p : result_cgal)
          {
            for (const Point v : p)
            {
              s_cgal << v.str() << ' ';
            }
          }

	  dolfin_error("CGALExactArithmetic.h",
		       "verify intersections due to different data (NB: we sort data before comparing)",
		       "Error in function %s\n DOLFIN: %s\n CGAL: %s",
		       function.c_str(),
		       s_dolfin.str().c_str(),
		       s_cgal.str().c_str());
	}
    }

    return result_dolfin;
  }

  inline const Point&
  check_cgal(const Point& result_dolfin,
             const Point& result_cgal,
             std::string function)
  {
    for (std::size_t d = 0; d < 3; ++d)
    {
      if (!near(result_dolfin[d], result_cgal[d], DOLFIN_EPS_LARGE))
      {
	std::stringstream s_dolfin;
	s_dolfin.precision(16);
	std::stringstream s_cgal;
	s_cgal.precision(16);
	for (std::size_t i = 0; i < 3; ++i)
	{
	  s_dolfin << result_dolfin[i] << " ";
	  s_cgal << result_cgal[i] << " ";
	}

	dolfin_error("CGALExactArithmetic.h",
		     "verify intersections due to different Point data",
		     "Error in function %s\n DOLFIN: %s\n CGAL: %s",
		     function.c_str(),
		     s_dolfin.str().c_str(),
		     s_cgal.str().c_str());
      }
    }

    return result_dolfin;
  }

} // end namespace dolfin

// Comparison macro that calls comparison function
#define CHECK_CGAL(RESULT_DOLFIN, RESULT_CGAL) check_cgal(RESULT_DOLFIN, RESULT_CGAL, __FUNCTION__)

// CGAL includes
#define CGAL_HEADER_ONLY
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Triangle_2.h>
#include <CGAL/intersection_2.h>

namespace
{
  // CGAL typedefs
  typedef CGAL::Quotient<CGAL::MP_Float> ExactNumber;
  typedef CGAL::Cartesian<ExactNumber>   ExactKernel;
  typedef ExactKernel::Point_2           Point_2;
  typedef ExactKernel::Triangle_2        Triangle_2;
  typedef ExactKernel::Segment_2         Segment_2;
  typedef ExactKernel::Intersect_2       Intersect_2;

  //---------------------------------------------------------------------------
  // CGAL utility functions
  //---------------------------------------------------------------------------

  inline Point_2 convert_to_cgal(double a, double b)
  {
    return Point_2(a, b);
  }

  inline Point_2 convert_to_cgal(const dolfin::Point& p)
  {
    return Point_2(p[0], p[1]);
  }

  inline Segment_2 convert_to_cgal(const dolfin::Point& a,
                                   const dolfin::Point& b)
  {
    return Segment_2(convert_to_cgal(a), convert_to_cgal(b));
  }

  inline Triangle_2 convert_to_cgal(const dolfin::Point& a,
                                    const dolfin::Point& b,
                                    const dolfin::Point& c)
  {
    return Triangle_2(convert_to_cgal(a), convert_to_cgal(b), convert_to_cgal(c));
  }

  inline std::vector<dolfin::Point> convert_to_cgal(const Segment_2& t)
  {
    std::vector<dolfin::Point> p(2);
    for (std::size_t i = 0; i < 2; ++i)
      for (std::size_t j = 0; j < 2; ++j)
	p[i][j] = CGAL::to_double(t[i][j]);
    return p;
  }

  inline std::vector<dolfin::Point> convert_to_cgal(const Triangle_2& t)
  {
    std::vector<dolfin::Point> p(3);
    for (std::size_t i = 0; i < 3; ++i)
      for (std::size_t j = 0; j < 2; ++j)
	p[i][j] = CGAL::to_double(t[i][j]);
    return p;
  }

  inline bool is_degenerate(const dolfin::Point& a,
			    const dolfin::Point& b)
  {
    Segment_2 s(convert_to_cgal(a), convert_to_cgal(b));
    return s.is_degenerate();
  }

  inline bool is_degenerate(const dolfin::Point& a,
			    const dolfin::Point& b,
			    const dolfin::Point& c)
  {
    Triangle_2 t(convert_to_cgal(a), convert_to_cgal(b), convert_to_cgal(c));
    return t.is_degenerate();
  }

  inline dolfin::Point convert_from_cgal(const Point_2& p)
  {
    return dolfin::Point(CGAL::to_double(p.x()),CGAL::to_double(p.y()));
  }

  inline std::vector<dolfin::Point> convert_from_cgal(const Segment_2& s)
  {
    const std::vector<dolfin::Point> triangulation = {{ dolfin::Point(CGAL::to_double(s.vertex(0)[0]),
                                                                      CGAL::to_double(s.vertex(0)[1])),
                                                        dolfin::Point(CGAL::to_double(s.vertex(1)[0]),
                                                                      CGAL::to_double(s.vertex(1)[1]))
                                                     }};
    return triangulation;
  }

  inline std::vector<dolfin::Point> convert_from_cgal(const Triangle_2& t)
  {
    const std::vector<dolfin::Point> triangulation = {{ dolfin::Point(CGAL::to_double(t.vertex(0)[0]),
                                                              CGAL::to_double(t.vertex(0)[1])),
                                                        dolfin::Point(CGAL::to_double(t.vertex(2)[0]),
                                                              CGAL::to_double(t.vertex(2)[1])),
                                                        dolfin::Point(CGAL::to_double(t.vertex(1)[0]),
                                                              CGAL::to_double(t.vertex(1)[1]))
                                                     }};
    return triangulation;
  }

  inline std::vector<std::vector<dolfin::Point>> triangulate_polygon(const std::vector<Point_2>& cgal_points)
  {
    dolfin_assert(cgal_points.size() == 4);
    const std::vector<std::vector<dolfin::Point>> triangulation = { { dolfin::Point(CGAL::to_double(cgal_points[0].x()),
                                                                    CGAL::to_double(cgal_points[0].y())),
                                                              dolfin::Point(CGAL::to_double(cgal_points[3].x()),
                                                                    CGAL::to_double(cgal_points[3].y())),
                                                              dolfin::Point(CGAL::to_double(cgal_points[1].x()),
                                                                    CGAL::to_double(cgal_points[1].y())) },
                                                            { dolfin::Point(CGAL::to_double(cgal_points[1].x()),
                                                                    CGAL::to_double(cgal_points[1].y())),
                                                              dolfin::Point(CGAL::to_double(cgal_points[3].x()),
                                                                    CGAL::to_double(cgal_points[3].y())),
                                                              dolfin::Point(CGAL::to_double(cgal_points[2].x()),
                                                                    CGAL::to_double(cgal_points[2].y()))
                                                            } };
    return triangulation;
  }

  //------------------------------------------------------------------------------
  // Explicit handling of CGAL intersections
  //------------------------------------------------------------------------------

  inline std::vector<dolfin::Point>
  parse_segment_segment_intersection
  (const CGAL::cpp11::result_of<Intersect_2(Segment_2, Segment_2)>::type ii)
  {
    const Point_2* p = boost::get<Point_2>(&*ii);
    if (p)
      return std::vector<dolfin::Point>{convert_from_cgal(*p)};

    const Segment_2* s = boost::get<Segment_2>(&*ii);
    if (s)
      return convert_from_cgal(*s);

    dolfin::error("Unexpected behavior in CGALExactArithmetic parse_segment_segment");
    return std::vector<dolfin::Point>();
  }

  inline std::vector<dolfin::Point>
  parse_triangle_segment_intersection
  (const CGAL::cpp11::result_of<Intersect_2(Triangle_2, Segment_2)>::type ii)
  {
    const Point_2* p = boost::get<Point_2>(&*ii);
    if (p)
      return std::vector<dolfin::Point>{convert_from_cgal(*p)};

    const Segment_2* s = boost::get<Segment_2>(&*ii);
    if (s)
      return convert_from_cgal(*s);

    dolfin::error("Unexpected behavior in CGALExactArithmetic parse_triangle_segment");
    return std::vector<dolfin::Point>();
  }

  inline std::vector<std::vector<dolfin::Point>>
  parse_triangle_triangle_intersection
  (const CGAL::cpp11::result_of<Intersect_2(Triangle_2, Triangle_2)>::type ii)
  {
    const Point_2* p = boost::get<Point_2>(&*ii);
    if (p)
      return std::vector<std::vector<dolfin::Point>>{{convert_from_cgal(*p)}};

    const Segment_2* s = boost::get<Segment_2>(&*ii);
    if (s)
      return std::vector<std::vector<dolfin::Point>>{{convert_from_cgal(*s)}};

    const Triangle_2* t = boost::get<Triangle_2>(&*ii);
    if (t)
      return std::vector<std::vector<dolfin::Point>>{{convert_from_cgal(*t)}};

    const std::vector<Point_2>* cgal_points = boost::get<std::vector<Point_2>>(&*ii);
    if (cgal_points)
    {
      dolfin_assert(cgal_points->size() == 4);
      return triangulate_polygon(*cgal_points);
    }

    dolfin::error("Unexpected behavior in CGALExactArithmetic parse_triangle_triangle");
    return std::vector<std::vector<dolfin::Point>>();
  }
}

namespace dolfin
{
  //---------------------------------------------------------------------------
  // Reference implementations of DOLFIN collision detection predicates
  // using CGAL exact arithmetic
  // ---------------------------------------------------------------------------

  inline bool cgal_collides_segment_point(const Point& p0,
                                          const Point& p1,
                                          const Point& point)
  {
    return CGAL::do_intersect(convert_to_cgal(p0, p1),
			      convert_to_cgal(point));
  }

  inline bool cgal_collides_segment_segment(const Point& p0,
                                            const Point& p1,
                                            const Point& q0,
                                            const Point& q1)
  {
    return CGAL::do_intersect(convert_to_cgal(p0, p1),
			      convert_to_cgal(q0, q1));
  }


  inline bool cgal_collides_triangle_point(const Point& p0,
					   const Point& p1,
					   const Point& p2,
					   const Point &point)
  {
    return CGAL::do_intersect(convert_to_cgal(p0, p1, p2),
			      convert_to_cgal(point));
  }

  inline bool cgal_collides_triangle_segment(const Point& p0,
                                             const Point& p1,
                                             const Point& p2,
                                             const Point& q0,
                                             const Point& q1)
  {
    return CGAL::do_intersect(convert_to_cgal(p0, p1, p2),
			      convert_to_cgal(q0, q1));
  }

  inline bool cgal_collides_triangle_triangle(const Point& p0,
					      const Point& p1,
					      const Point& p2,
					      const Point& q0,
					      const Point& q1,
					      const Point& q2)
  {
    return CGAL::do_intersect(convert_to_cgal(p0, p1, p2),
			      convert_to_cgal(q0, q1, q2));
  }

  //----------------------------------------------------------------------------
  // Reference implementations of DOLFIN intersection triangulation
  // functions using CGAL exact arithmetic
  // ---------------------------------------------------------------------------

  inline
  std::vector<Point> cgal_triangulate_segment_segment_2d(const Point& p0,
						       const Point& p1,
						       const Point& q0,
						       const Point& q1)
  {
    dolfin_assert(!is_degenerate(p0, p1));
    dolfin_assert(!is_degenerate(q0, q1));

    const auto I0 = convert_to_cgal(p0, p1);
    const auto I1 = convert_to_cgal(q0, q1);
    const auto ii = CGAL::intersection(I0, I1);
    dolfin_assert(ii);
    const std::vector<Point> triangulation = parse_segment_segment_intersection(ii);

    if (triangulation.size() == 0)
      dolfin_error("CGALExactArithmetic.h",
		   "in cgal_intersection_segment_segment function",
		   "unknown intersection");

    return triangulation;
  }

  inline
  std::vector<Point> cgal_triangulate_triangle_segment_2d(const Point& p0,
                                                       const Point& p1,
                                                       const Point& p2,
                                                       const Point& q0,
							  const Point& q1)
  {
    dolfin_assert(!is_degenerate(p0, p1, p2));
    dolfin_assert(!is_degenerate(q0, q1));

    const auto T = convert_to_cgal(p0, p1, p2);
    const auto I = convert_to_cgal(q0, q1);
    const auto ii = CGAL::intersection(T, I);
    dolfin_assert(ii);
    const std::vector<Point> triangulation = parse_triangle_segment_intersection(ii);

    if (triangulation.size() == 0)
      dolfin_error("CGALExactArithmetic.h",
		   "in cgal_intersection_triangle_segment function",
		   "unknown intersection");

    return triangulation;
  }

  inline
  std::vector<std::vector<Point>> cgal_triangulate_triangle_triangle_2d(const Point& p0,
                                                                     const Point& p1,
                                                                     const Point& p2,
                                                                     const Point& q0,
                                                                     const Point& q1,
                                                                     const Point& q2)

  {
    dolfin_assert(!is_degenerate(p0, p1, p2));
    dolfin_assert(!is_degenerate(q0, q1, q2));

    const auto T0 = convert_to_cgal(p0, p1, p2);
    const auto T1 = convert_to_cgal(q0, q1, q2);
    const auto ii = CGAL::intersection(T0, T1);

    // We can have empty ii if we use
    // CGAL::Exact_predicates_inexact_constructions_kernel
    dolfin_assert(ii);

    const std::vector<std::vector<dolfin::Point>> triangulation =
      parse_triangle_triangle_intersection(ii);

    // NB: the parsing can return triangulation of size 0, for example
    // if it detected a triangle but it was found to be flat.
    if (triangulation.size() == 0)
      dolfin_error("CGALExactArithmetic.h",
		 "find intersection of two triangles in cgal_intersection_triangle_triangle function",
		 "no intersection found");

    return triangulation;
  }

  // inline dolfin::Point cgal_intersection_edge_edge_2d(const Point& a,
  //                                                     const Point& b,
  //                                                     const Point& c,
  //                                                     const Point& d)
  // {
  //   dolfin_assert(!is_degenerate(a, b));
  //   dolfin_assert(!is_degenerate(c, d));

  //   const auto E0 = convert_to_cgal(a, b);
  //   const auto E1 = convert_to_cgal(c, d);

  //   const auto ii = CGAL::intersection(E0, E1);
  //   dolfin_assert(ii);

  //   const std::vector<Point> triangulation = parse_segment_segment_intersection(ii);

  //   dolfin_assert(triangulation.size() == 2 or
  // 		  triangulation.size() == 4);

  //   if (triangulation.size() == 1)
  //     return triangulation[0];
  //   else if (triangulation.size() == 2)
  //     return triangulation[0]*.5 + triangulation[1];

  //   dolfin_error("CGALExactArithmetic.h",
  // 		 "find intersection of two triangles in cgal_intersection_edge_edge_2d function",
  // 		 "no or strange intersection found");
  //   return Point();
  // }

  // inline Point cgal_intersection_face_edge_2d(const Point& r,
  // 					      const Point& s,
  // 					      const Point& t,
  // 					      const Point& a,
  // 					      const Point& b)
  // {
  //   // NB: this is only for 2D

  //   dolfin_assert(!is_degenerate(r, s, t));
  //   dolfin_assert(!is_degenerate(a, b));

  //   const auto T = convert_to_cgal(r, s, t);
  //   const auto I = convert_to_cgal(a, b);

  //   const auto ii = CGAL::intersection(T, I);
  //   dolfin_assert(ii);

  //   const std::vector<Point> triangulation = parse_triangle_segment_intersection(ii);

  //   dolfin_assert(triangulation.size() == 2 or
  // 		  triangulation.size() == 4);

  //   if (triangulation.size() == 1)
  //     return triangulation[0];
  //   else if (triangulation.size() == 2)
  //     return triangulation[0]*.5 + triangulation[1]*.5;

  //   dolfin_error("CGALExactArithmetic.h",
  // 		 "find intersection of two triangles in cgal_intersection_face_edge_2d function",
  // 		 "no or strange intersection found");
  //   return Point();
  // }

}
#endif

#endif
