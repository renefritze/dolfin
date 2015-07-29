// Copyright (C) 2014 August Johansson and Anders Logg
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
// First added:  2014-03-10
// Last changed: 2015-06-08
//

#include <dolfin/mesh/MultiMesh.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/math/basic.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/generation/UnitSquareMesh.h>

#include <CGAL/Triangle_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/intersection_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_set_2.h>


#define MULTIMESH_DEBUG_OUTPUT 0

using namespace dolfin;

enum CELL_STATUS
{
  UNKNOWN,
  COVERED,
  CUT,
  UNCUT
};

//------------------------------------------------------------------------------
double rotate(double x, double y, double cx, double cy, double w,
              double& xr, double& yr)
{
  // std::cout << "rotate:\n"
  // 	      << "\t"
  // 	      << "plot("<<x<<','<<y<<",'b.');plot("<<cx<<','<<cy<<",'o');";

  const double v = w*DOLFIN_PI/180.;
  const double dx = x-cx;
  const double dy = y-cy;
  xr = cx + dx*cos(v) - dy*sin(v);
  yr = cy + dx*sin(v) + dy*cos(v);
  //std::cout << "plot("<<xr<<','<<yr<<",'r.');"<<std::endl;
}
//------------------------------------------------------------------------------
bool rotation_inside(double x,double y, double cx, double cy, double w,
                     double& xr, double& yr)
{
  rotate(x,y,cx,cy,w, xr,yr);
  if (xr>0 and xr<1 and yr>0 and yr<1) return true;
  else return false;
}
//------------------------------------------------------------------------------
// Compute volume contributions from each cell
void compute_volume(const MultiMesh& multimesh,
                    std::vector<std::vector<std::pair<CELL_STATUS, double> > >& cells_status)
{
  cells_status.reserve(multimesh.num_parts());    

  // Sum contribution from all parts
  for (std::size_t part = 0; part < multimesh.num_parts(); part++)
  {
    std::cout << "Testing part " << part << std::endl;
    cells_status.push_back(std::vector<std::pair<CELL_STATUS, double> >());
    std::vector<std::pair<CELL_STATUS, double> >& current_cells_status = cells_status.back();

    std::shared_ptr<const Mesh> current_mesh = multimesh.part(part);
    current_cells_status.resize(current_mesh->num_cells());
    std::cout << "Number of cells: " << current_cells_status.size() << std::endl;

    // Uncut cell volume given by function volume
    {
      const std::vector<unsigned int>& uncut_cells = multimesh.uncut_cells(part);
      for (auto it = uncut_cells.begin(); it != uncut_cells.end(); ++it)
      {
          const Cell cell(*multimesh.part(part), *it);
          current_cells_status[*it] = std::make_pair(UNCUT, cell.volume());
        }
      }
      
      // Cut cell volume given by quadrature rule
      {
        const std::vector<unsigned int>& cut_cells = multimesh.cut_cells(part);
        for (auto it = cut_cells.begin(); it != cut_cells.end(); ++it)
        {
          std::cout << "Cut cell in part " << part << ": " << *it << std::endl;
          double volume = 0;
          const quadrature_rule& qr = multimesh.quadrature_rule_cut_cell(part, *it);
          std::cout << "QR: " << qr.first.size() << ", " << qr.second.size() << std::endl;
          for (std::size_t i = 0; i < qr.second.size(); ++i)
          {
            volume += qr.second[i];
          }
          current_cells_status[*it] = std::make_pair(CUT, volume);
        }
      }

      {
        const std::vector<unsigned int>& covered_cells = multimesh.covered_cells(part);
        for (auto it = covered_cells.begin(); it != covered_cells.end(); ++it)
        {
          current_cells_status[*it] = std::make_pair(COVERED, 0.);
        }
      }
    }
  }
  //------------------------------------------------------------------------------
  void get_cells_status_cgal(const MultiMesh& multimesh,
                             std::vector<std::vector<std::pair<CELL_STATUS, double> > >& cells_status)
  {
    typedef CGAL::Exact_predicates_exact_constructions_kernel ExactKernel;
    //typedef CGAL::Exact_predicates_inexact_constructions_kernel ExactKernel;
    typedef CGAL::Point_2<ExactKernel>                Point_2;
    typedef CGAL::Triangle_2<ExactKernel>             Triangle_2;
    typedef CGAL::Line_2<ExactKernel>                 Line_2;
    typedef CGAL::Polygon_2<ExactKernel>              Polygon_2;
    typedef Polygon_2::Vertex_const_iterator          Vertex_const_iterator;
    typedef CGAL::Polygon_with_holes_2<ExactKernel>   Polygon_with_holes_2;
    typedef Polygon_with_holes_2::Hole_const_iterator Hole_const_iterator;
    typedef CGAL::Polygon_set_2<ExactKernel>          Polygon_set_2;

    
    cells_status.reserve(multimesh.num_parts());

    ExactKernel::FT volume = 0;

    for (std::size_t i = 0; i < multimesh.num_parts(); i++)
    {
      std::cout << "Testing part " << i << std::endl;
      cells_status.push_back(std::vector<std::pair<CELL_STATUS, double> >());
      std::vector<std::pair<CELL_STATUS, double> >& current_cells_status = cells_status.back();

      std::shared_ptr<const Mesh> current_mesh = multimesh.part(i);
      const MeshGeometry& current_geometry = current_mesh->geometry();
      
      for (CellIterator cit(*current_mesh); !cit.end(); ++cit)
      {
        // Test every cell against every cell in overlaying meshes
        Triangle_2 current_cell(Point_2(current_geometry.x(cit->entities(0)[0], 0),
                                        current_geometry.x(cit->entities(0)[0], 1)),
                                Point_2(current_geometry.x(cit->entities(0)[1], 0),
                                        current_geometry.x(cit->entities(0)[1], 1)),
                                Point_2(current_geometry.x(cit->entities(0)[2], 0),
                                        current_geometry.x(cit->entities(0)[2], 1)));
        if (current_cell.orientation() == CGAL::CLOCKWISE)
        {
          //std::cout << "Orig: " << current_cell << std::endl;
          current_cell = current_cell.opposite();
          //std::cout << "Opposite: " << current_cell << std::endl;
        }
        Polygon_set_2 polygon_set;
        {
          std::vector<Point_2> vertices;
          vertices.push_back(current_cell[0]);
          vertices.push_back(current_cell[1]);
          vertices.push_back(current_cell[2]);

          Polygon_2 p(vertices.begin(), vertices.end());
          polygon_set.insert(p);
        }

        std::cout << "  Testing cell: " << current_cell << std::endl;
        bool is_uncut = true;
        for (std::size_t j = i+1; j < multimesh.num_parts(); j++)
        {
          std::cout << "    Testing against part " << j << std::endl;
          std::shared_ptr<const Mesh> other_mesh = multimesh.part(j);
          const MeshGeometry& other_geometry = other_mesh->geometry();
          for (CellIterator cit_other(*other_mesh); !cit_other.end(); ++cit_other)
          {
            std::vector<Point_2> vertices;
            Point_2 p0(other_geometry.x(cit_other->entities(0)[0], 0),
                       other_geometry.x(cit_other->entities(0)[0], 1));
            Point_2 p1(other_geometry.x(cit_other->entities(0)[1], 0),
                       other_geometry.x(cit_other->entities(0)[1], 1));
            Point_2 p2(other_geometry.x(cit_other->entities(0)[2], 0),
                       other_geometry.x(cit_other->entities(0)[2], 1));

            vertices.push_back(p0);
            if (Line_2(p0, p1).has_on_positive_side(p2))
            {
              vertices.push_back(p1);
              vertices.push_back(p2);
            }
            else
            {
              vertices.push_back(p2);
              vertices.push_back(p1);
            }
            Polygon_2 p(vertices.begin(), vertices.end());
            polygon_set.difference(p);
          }
        }

        std::vector<Polygon_with_holes_2> result;
        polygon_set.polygons_with_holes(std::back_inserter(result));

        if (result.size() == 0)
        {
          current_cells_status.push_back(std::make_pair(COVERED, 0.0));
          std::cout << "    Covered" << std::endl;
        }
        else
        {
          if (result.size() > 1)
            std::cout << "!!!!!!!! Several polygons !!!!!!!" << std::endl;

          Polygon_2::Vertex_const_iterator v = result[0].outer_boundary().vertices_begin();
          Polygon_2::Vertex_const_iterator v_end = result[0].outer_boundary().vertices_end();
          const std::size_t num_vertices = std::distance(v, v_end);
          const Point_2& v0 = *v; ++v;
          const Point_2& v1 = *v; ++v;
          const Point_2& v2 = *v;

          if (result.size() == 1 &&
              result[0].holes_begin() == result[0].holes_end() &&
              num_vertices == 3 &&
              Triangle_2(v0, v1, v2) == current_cell)
          {
            current_cells_status.push_back(std::make_pair(UNCUT,
                                                          CGAL::to_double(result[0].outer_boundary().area())));
            std::cout << "    Uncut" << std::endl;
          }
          else
          {
            ExactKernel::FT current_volume = 0;

            for(auto pit = result.begin(); pit != result.end(); pit++)
            {
              const Polygon_2& outerboundary = pit->outer_boundary();
              current_volume += outerboundary.area();
              std::cout << "    Polygon ";
              for (auto it = outerboundary.vertices_begin(); it != outerboundary.vertices_end(); it++) std::cout << *it << ", ";
              std::cout << std::endl;

              for (auto it = pit->holes_begin(); it != pit->holes_end(); it++)
                current_volume -= it->area();
            }
            current_cells_status.push_back(std::make_pair(CUT, CGAL::to_double(current_volume)));
            std::cout << "    Cut" << std::endl;
          }
        }
      }
    }
  }
  //------------------------------------------------------------------------------
  void test_multiple_meshes_with_rotation()
  {
    set_log_level(DBG);

    dolfin::seed(0);

    const double h = 0.5;
    UnitSquareMesh background_mesh((int)std::round(1./h),
				   (int)std::round(1./h));

    MultiMesh multimesh;
    multimesh.add(background_mesh);

    const std::size_t Nmeshes = 8;

    /* ---------------- Create multimesh ------------------------- */
    std::size_t i = 0;
    while (i < Nmeshes)
    {
      double x0 = dolfin::rand();
      double x1 = dolfin::rand();
      if (x0 > x1) std::swap(x0, x1);
      double y0 = dolfin::rand();
      double y1 = dolfin::rand();
      if (y0 > y1) std::swap(y0, y1);
      const double v = dolfin::rand()*90; // initial rotation
      const double speed = dolfin::rand()-0.5; // initial speed
       
      const double cx = (x0+x1) / 2;
      const double cy = (y0+y1) / 2;
      double xr, yr;
      rotate(x0, y0, cx, cy, v, xr, yr);
      if (xr > 0 and xr < 1 and yr > 0 and yr < 1)
      {
	rotate(x0, y1, cx, cy, v, xr, yr);
	if (xr > 0 and xr < 1 and yr > 0 and yr < 1)
	{
	  rotate(x1, y0, cx, cy, v, xr, yr);
	  if (xr > 0 and xr < 1 and yr > 0 and yr < 1)
	  {
	    rotate(x1, y1, cx, cy, v, xr, yr);
	    if (xr > 0 and xr < 1 and yr > 0 and yr < 1)
	    {
              std::shared_ptr<Mesh> mesh(new RectangleMesh(x0, y0, x1, y1,
                                                           std::max((int)std::round((x1-x0)/h), 1),
                                                           std::max((int)std::round((y1-y0)/h), 1)));
              mesh->rotate(v);              
              multimesh.add(mesh);
	      i++;
	    }
	  }
	}
      }
    }

    multimesh.build();
    
    std::cout << multimesh.plot_matplotlib() << std::endl;
    std::cout << "Done building multimesh" << std::endl;
    if (!multimesh.is_built())
      std::cout << "Problem! Multimesh not built" << std::endl;
    /* ---------------- Done creating multimesh ----------------------- */

    // Compute volume of each cell using cgal
    std::vector<std::vector<std::pair<CELL_STATUS, double>>> cell_status_cgal;
    // get_cells_status_cgal(multimesh, cell_status_cgal);
    // std::cout << "Done computing volumes with cgal" << std::endl;

    // Compute volume of each cell using dolfin::MultiMesh
    std::vector<std::vector<std::pair<CELL_STATUS, double> > > cell_status_multimesh;
    compute_volume(multimesh, cell_status_multimesh);
    std::cout << "Done computing volumes with multimesh" << std::endl;
    
    double cgal_volume = 0.;
    double multimesh_volume = 0.;

    dolfin_assert(cell_status_cgal.size() == cell_status_multimesh.size());
    for (std::size_t i = 0; i < cell_status_cgal.size(); i++)
    {
      const std::vector<std::pair<CELL_STATUS, double> >& current_cgal = cell_status_cgal[i];
      const std::vector<std::pair<CELL_STATUS, double> >& current_multimesh = cell_status_multimesh[i];

      dolfin_assert(current_cgal.size() == current_multimesh.size());
      
      std::cout << "Cells in part " << i << ": " << std::endl;
      for (std::size_t j = 0; j < current_cgal.size(); j++)
      {
        std::cout << "  Cell " << j << std::endl;
        std::cout << "    Multimesh: " << current_multimesh[j].first << " (" << current_multimesh[j].second << ")" << std::endl;
        std::cout << "    CGAL:      " << current_cgal[j].first << " (" << current_cgal[j].second << ")" << std::endl;
        // dolfin_assert(near(current_cgal[j].second, current_multimesh[j].second, DOLFIN_EPS_LARGE));
        // dolfin_assert(current_cgal[j].first == current_multimesh[j].first);
      }
      std::cout << std::endl;
    }

    if (MULTIMESH_DEBUG_OUTPUT)
    {
      std::cout << multimesh.plot_matplotlib() << std::endl;
    }

    // Exact volume is known
    //const double exact_volume = 1;

    //dolfin_assert(near(exact_volume, volume, DOLFIN_EPS_LARGE));
  }


// };

int main(int argc, char** argv)
{
  test_multiple_meshes_with_rotation();
}
