// Copyright (C) 2010 Andre Massing
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN.  If not, see <http://www.gnu.org/licenses/>.
//
// First added:  2010-02-09
// Last changed: 2010-03-05

#include "Mesh.h"
#include "MeshEntity.h"
#include "Point.h"
#include "PrimitiveIntersector.h"

using namespace dolfin;

#ifdef HAS_CGAL
#include "cgal_includes.h"

//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect(const MeshEntity& entity_1,
                                        const MeshEntity& entity_2)
{
  return PrimitiveIntersector::do_intersect_with_kernel<SCK>(entity_1, entity_2);
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect(const MeshEntity& entity,
                                        const Point& point)
{
  return PrimitiveIntersector::do_intersect_with_kernel<SCK>(PrimitiveTraits<PointPrimitive, SCK>::datum(point), entity);
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect_exact(const MeshEntity& entity_1,
                                              const MeshEntity& entity_2)
{
  return PrimitiveIntersector::do_intersect_with_kernel<EPICK>(entity_1, entity_2);
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect_exact(const MeshEntity& entity,
                                              const Point& point)
{
  return PrimitiveIntersector::do_intersect_with_kernel<EPICK>(PrimitiveTraits<PointPrimitive, EPICK>::datum(point), entity);
}
//-----------------------------------------------------------------------------
template <typename K, typename T, typename U >
bool PrimitiveIntersector::do_intersect_with_kernel(const T& entity_1,
                                                    const U& entity_2)
{
  return CGAL::do_intersect(entity_1, entity_2);
}
//-----------------------------------------------------------------------------
template<typename K, typename T >
bool PrimitiveIntersector::do_intersect_with_kernel(const T& entity_1,
                                                    const MeshEntity& entity_2)
{
  switch(entity_2.dim())
  {
  case 0 :
    return do_intersect_with_kernel<K>(PrimitiveTraits<PointCell,K>::datum(entity_2), entity_1);
  case 1 :
    return do_intersect_with_kernel<K>(PrimitiveTraits<IntervalCell,K>::datum(entity_2), entity_1);
  case 2 :
    return do_intersect_with_kernel<K>(PrimitiveTraits<TriangleCell,K>::datum(entity_2),  entity_1);
  case 3 :
    return do_intersect_with_kernel<K>(PrimitiveTraits<TetrahedronCell,K>::datum(entity_2), entity_1);
  default:  error("DOLFIN PrimitiveIntersector::do_intersect: \n Dimension of  is not known."); return false;
  }
}
//-----------------------------------------------------------------------------

#else

#include <dolfin/log/log.h>

//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect(const MeshEntity& entity_1,
                                        const MeshEntity& entity_2)
{
  warning("DOLFIN has been compiled without CGAL support");
  dolfin_not_implemented();
  return false;
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect_exact(const MeshEntity& entity_1,
                                              const MeshEntity& entity_2)
{
  warning("DOLFIN has been compiled without CGAL support");
  dolfin_not_implemented();
  return false;
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect(const MeshEntity& entity_1,
                                        const Point& point)
{
  warning("DOLFIN has been compiled without CGAL support");
  dolfin_not_implemented();
  return false;
}
//-----------------------------------------------------------------------------
bool PrimitiveIntersector::do_intersect_exact(const MeshEntity& entity_1,
                                              const Point& point)
{
  warning("DOLFIN has been compiled without CGAL support");
  dolfin_not_implemented();
  return false;
}
//-----------------------------------------------------------------------------

#endif
