// Copyright (C) 2014 Anders Logg and August Johansson
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
// Last changed: 2014-02-24

#include <vector>
#include <dolfin/log/log.h>

#ifndef __COLLISION_DETECTION_H
#define __COLLISION_DETECTION_H

namespace dolfin
{

  // Forward declarations
  class MeshEntity;

  /// This class implements algorithms for detecting pairwise
  /// collisions between mesh entities of varying dimensions.

  class CollisionDetection
  {
  public:

    /// Check whether entity collides with point.
    ///
    /// *Arguments*
    ///     entity (_MeshEntity_)
    ///         The entity.
    ///     point (_Point_)
    ///         The point.
    ///
    /// *Returns*
    ///     bool
    ///         True iff entity collides with cell.
    static bool collides(const MeshEntity& entity,
			 const Point& point);

    /// Check whether two entities collide.
    ///
    /// *Arguments*
    ///     entity_0 (_MeshEntity_)
    ///         The first entity.
    ///     entity_1 (_MeshEntity_)
    ///         The second entity.
    ///
    /// *Returns*
    ///     bool
    ///         True iff entity collides with cell.
    static bool collides(const MeshEntity& entity_0,
			 const MeshEntity& entity_1);

    /// Check whether interval collides with point.
    ///
    /// *Arguments*
    ///     interval (_MeshEntity_)
    ///         The interval.
    ///     point (_Point_)
    ///         The point.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_interval_point(const MeshEntity& interval,
					const Point& point);

    /// Check whether interval collides with interval.
    ///
    /// *Arguments*
    ///     interval_0 (_MeshEntity_)
    ///         The first interval.
    ///     interval_1 (_MeshEntity_)
    ///         The second interval.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_interval_interval(const MeshEntity& interval_0,
					   const MeshEntity& interval_1);

    /// Check whether triangle collides with point.
    ///
    /// *Arguments*
    ///     triangle (_MeshEntity_)
    ///         The triangle.
    ///     point (_Point_)
    ///         The point.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_triangle_point(const MeshEntity& triangle,
					const Point& point);

    /// Check whether triangle collides with triangle.
    ///
    /// *Arguments*
    ///     triangle_0 (_MeshEntity_)
    ///         The first triangle.
    ///     triangle_1 (_MeshEntity_)
    ///         The second triangle.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_triangle_triangle(const MeshEntity& triangle_0,
					   const MeshEntity& triangle_1);

    /// Check whether tetrahedron collides with point.
    ///
    /// *Arguments*
    ///     tetrahedron (_MeshEntity_)
    ///         The tetrahedron.
    ///     point (_Point_)
    ///         The point.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_tetrahedron_point(const MeshEntity& tetrahedron,
                                           const Point& point);

    /// Check whether tetrahedron collides with triangle.
    ///
    /// *Arguments*
    ///     tetrahedron (_MeshEntity_)
    ///         The tetrahedron.
    ///     triangle (_MeshEntity_)
    ///         The triangle.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_tetrahedron_triangle(const MeshEntity& tetrahedron,
                                              const MeshEntity& triangle);

    /// Check whether tetrahedron collides with tetrahedron.
    ///
    /// *Arguments*
    ///     tetrahedron_0 (_MeshEntity_)
    ///         The first tetrahedron.
    ///     tetrahedron_1 (_MeshEntity_)
    ///         The second tetrahedron.
    ///
    /// *Returns*
    ///     bool
    ///         True iff objects collide.
    static bool collides_tetrahedron_tetrahedron(const MeshEntity& tetrahedron_0,
                                                 const MeshEntity& tetrahedron_1);

    /// Check whether edge a-b collides with edge c-d.
    static bool collides_edge_edge(const Point& a, const Point& b,
				   const Point& c, const Point& d);

  private:

    // The implementation of collides_triangle_point
    static bool collides_triangle_point(const Point& p0,
					const Point& p1,
					const Point& p2,
					const Point& point);

    // The implementation of collides_triangle_triangle
    static bool collides_triangle_triangle(const Point& p0,
					   const Point& p1,
					   const Point& p2,
					   const Point& q0,
					   const Point& q1,
					   const Point& q2);

    // The implementation of collides_tetrahedron_point
    static bool collides_tetrahedron_point(const Point& p0,
					   const Point& p1,
					   const Point& p2,
					   const Point& p3,
					   const Point& point);

    // The implementation of collides_tetrahedron_triangle
    static bool collides_tetrahedron_triangle(const Point& p0,
					      const Point& p1,
					      const Point& p2,
					      const Point& p3,
					      const Point& q0,
					      const Point& q1,
					      const Point& q2);

    // Helper function for triangle-triangle collision
    static bool edge_edge_test(int i0,
                               int i1,
                               double Ax,
                               double Ay,
			       const Point& V0,
			       const Point& U0,
			       const Point& U1);

    // Helper function for triangle-triangle collision
    static bool edge_against_tri_edges(int i0,
                                       int i1,
				       const Point& V0,
				       const Point& V1,
				       const Point& U0,
				       const Point& U1,
				       const Point& U2);

    // Helper function for triangle-triangle collision
    static bool point_in_tri(int i0,
                             int i1,
			     const Point& V0,
			     const Point& U0,
			     const Point& U1,
			     const Point& U2);

    // Helper function for triangle-triangle collision
    static bool coplanar_tri_tri(const Point& N,
				 const Point& V0,
				 const Point& V1,
				 const Point& V2,
				 const Point& U0,
				 const Point& U1,
				 const Point& U2);

    // Helper function for triangle-triangle collision
    static bool compute_intervals(double VV0,
                                  double VV1,
                                  double VV2,
				  double D0,
                                  double D1,
                                  double D2,
				  double D0D1,
                                  double D0D2,
				  double& A,
                                  double& B,
                                  double& C,
				  double& X0,
                                  double& X1);

    // Helper function for collides_tetrahedron_tetrahedron: checks if
    // plane pv1 is a separating plane. Stores local coordinates bc
    // and the mask bit mask_edges.
    static bool separating_plane_face_A_1(const std::vector<Point>& pv1,
					  const Point& n,
					  std::vector<double>& bc,
					  int& mask_edges);

    // Helper function for collides_tetrahedron_tetrahedron: checks if
    // plane v1, v2 is a separating plane. Stores local coordinates bc
    // and the mask bit mask_edges.
    static bool separating_plane_face_A_2(const std::vector<Point>& v1,
					  const std::vector<Point>& v2,
					  const Point& n,
					  std::vector<double>& bc,
					  int& mask_edges);

    // Helper function for collides_tetrahedron_tetrahedron: checks if
    // plane pv2 is a separating plane.
    static bool separating_plane_face_B_1(const std::vector<Point>& P_V2,
					  const Point& n)
    {
      return ((P_V2[0].dot(n) > 0) &&
	      (P_V2[1].dot(n) > 0) &&
	      (P_V2[2].dot(n) > 0) &&
	      (P_V2[3].dot(n) > 0));
    }

    // Helper function for collides_tetrahedron_tetrahedron: checks if
    // plane v1, v2 is a separating plane.
    static bool separating_plane_face_B_2(const std::vector<Point>& V1,
					  const std::vector<Point>& V2,
					  const Point& n)
    {
      return (((V1[0] - V2[1]).dot(n) > 0) &&
	      ((V1[1] - V2[1]).dot(n) > 0) &&
	      ((V1[2] - V2[1]).dot(n) > 0) &&
	      ((V1[3] - V2[1]).dot(n) > 0));
    }

    // Helper function for collides_tetrahedron_tetrahedron: checks if
    // edge is in the plane separating faces f0 and f1.
    static bool separating_plane_edge_A(const std::vector<std::vector<double> >& coord_1,
					const std::vector<int>& masks,
					int f0,
					int f1);

  };

}

#endif
