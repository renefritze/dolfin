// Copyright (C) 2012 Garth N. Wells
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
// Modified by Joachim B Haga 2012
//
// First added:  2012-02-02
// Last changed: 2012-09-05

#ifndef __DOLFIN_CGALMESHBUILDER_H
#define __DOLFIN_CGALMESHBUILDER_H

#ifdef HAS_CGAL

#include <vector>

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Triangulation_3.h>

#include <dolfin/geometry/ImplicitSurface.h>
#include <dolfin/geometry/Point.h>
#include <dolfin/log/log.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshEditor.h>

// This file should not be indlued in any header files to avoid
// polluting DOLFIN with CGAL code.

namespace dolfin
{

  /// This class provides a function to build a DOLFIN Mesh from a
  /// CGAL mesh or triangulation

  class CGALMeshBuilder
  {
  public:

    /// Build DOLFIN Mesh from a CGAL triangulation
    template<typename T>
    static void build(Mesh& mesh, T& triangulation);

    /// Build DOLFIN Mesh from a CGAL mesh (C3t3)
    template<typename T>
    static void build_from_mesh(Mesh& mesh, T& cgal_mesh);

    /// Build DOLFIN Mesh from a CGAL surface mesh (C2t3)
    template<typename T>
      static void build_surface_mesh_c3t3(Mesh& mesh, T& cgal_mesh,
                                          const ImplicitSurface* surface=NULL);

    /// Build DOLFIN Mesh from a CGAL surface mesh (C2t3)
    template<typename T>
      static void build_surface_mesh_c2t3(Mesh& mesh, T& cgal_mesh,
                                          const ImplicitSurface* surface=NULL);

    /// Build DOLFIN surface mesh from a CGAL polyhedron_3
    template<typename T>
    static void build_surface_mesh_poly(Mesh& mesh, T& polyhedron);

  private:

    // Get number of cells in triangulation (2D)
    template<typename X, typename Y>
    static std::size_t _num_cells(const CGAL::Triangulation_2<X, Y>& t)
    { return t.number_of_faces(); }

    // Get number of cells in Delaunay triangulation (2D)
    template<typename X, typename Y>
    static std::size_t
      _num_cells(const CGAL::Constrained_Delaunay_triangulation_2<X, Y>& t)
    {
      std::size_t num_cells = 0;
      typename CGAL::Constrained_Delaunay_triangulation_2<X, Y>::Finite_faces_iterator cgal_cell;
      for (cgal_cell = t.finite_faces_begin();
           cgal_cell != t.finite_faces_end(); ++cgal_cell)
      {
        if(cgal_cell->is_in_domain())
          ++num_cells;
      }
      return num_cells;
    }

    // Get number of cells in triangulation (3D)
    template<typename X, typename Y>
    static std::size_t _num_cells(const CGAL::Triangulation_3<X, Y>& t)
    { return t.number_of_cells(); }

    // Add cells to mesh (default)
    template<typename T, typename Y>
    static void add_cells(MeshEditor& mesh_editor, const T& t)
    {
      dolfin_error("CGALMeshBuilder.h",
                   "add CGAL cells to DOLFIN Mesh",
                   "Cannot find suitable specialized template funtion");
    }

    // Add cells (from 2D CGAL triangulation)
    template<typename X, typename Y>
    static void add_cells(MeshEditor& mesh_editor,
                          const CGAL::Triangulation_2<X, Y>& t)
    {
      std::size_t cell_index = 0;
      typename CGAL::Triangulation_2<X, Y>::Finite_faces_iterator cgal_cell;
      for (cgal_cell = t.finite_faces_begin();
           cgal_cell != t.finite_faces_end(); ++cgal_cell)
      {
        std::vector<std::size_t> cell_data(3);
        for (std::size_t i = 0; i < 3; ++i)
          cell_data[i] = cgal_cell->vertex(i)->info();
        mesh_editor.add_cell(cell_index++, cell_data);
      }
    }

    // Add cells (from 2D CGAL constrained Delaunay triangulation)
    template<typename X, typename Y>
    static void add_cells(MeshEditor& mesh_editor,
                          const CGAL::Constrained_Delaunay_triangulation_2<X, Y>& t)
    {
      std::size_t cell_index = 0;
      typename CGAL::Constrained_Delaunay_triangulation_2<X, Y>::Finite_faces_iterator cgal_cell;
      for (cgal_cell = t.finite_faces_begin();
           cgal_cell != t.finite_faces_end(); ++cgal_cell)
      {
        // Add cell if it is in the domain
        if(cgal_cell->is_in_domain())
        {
          std::vector<std::size_t> cell_data(3);
          for (std::size_t i = 0; i < 3; ++i)
            cell_data[i] = cgal_cell->vertex(i)->info();
          mesh_editor.add_cell(cell_index++, cell_data);
        }
      }
    }

    // Add cells (from 3D CGAL triangulation)
    template<typename X, typename Y>
    static void add_cells(MeshEditor& mesh_editor,
                          const CGAL::Triangulation_3<X, Y>& t)
    {
      std::size_t cell_index = 0;
      typename CGAL::Triangulation_3<X, Y>::Finite_cells_iterator cgal_cell;
      for (cgal_cell = t.finite_cells_begin();
           cgal_cell != t.finite_cells_end(); ++cgal_cell)
      {
        std::vector<std::size_t> cell_data(4);
        for (std::size_t i = 0; i < 4; ++i)
          cell_data[i] = cgal_cell->vertex(i)->info();
        mesh_editor.add_cell(cell_index++, cell_data);
      }
    }

  };

  //---------------------------------------------------------------------------
  template<typename T>
  void CGALMeshBuilder::build(Mesh& mesh, T& triangulation)
  {
    // Clear mesh
    mesh.clear();

    // Get various dimensions
    const std::size_t gdim
      = triangulation.finite_vertices_begin()->point().dimension();
    const std::size_t tdim = triangulation.dimension();
    const std::size_t num_vertices = triangulation.number_of_vertices();
    const std::size_t num_cells = _num_cells(triangulation);

    // Create a MeshEditor and open
    dolfin::MeshEditor mesh_editor;
    mesh_editor.open(mesh, tdim, gdim);
    mesh_editor.init_vertices(num_vertices, num_vertices);
    mesh_editor.init_cells(num_cells, num_cells);

    // Add vertices to mesh
    std::size_t vertex_index = 0;
    typename T::Finite_vertices_iterator cgal_vertex;
    for (cgal_vertex = triangulation.finite_vertices_begin();
         cgal_vertex != triangulation.finite_vertices_end(); ++cgal_vertex)
    {
      // Get vertex coordinates and add vertex to the mesh
      Point p;
      p[0] = cgal_vertex->point()[0];
      p[1] = cgal_vertex->point()[1];
      if (gdim == 3)
        p[2] = cgal_vertex->point()[2];

      // Add mesh vertex
      mesh_editor.add_vertex(vertex_index, p);

      // Attach index to vertex and increment
      cgal_vertex->info() = vertex_index++;
    }

    // Add cells to mesh (calls specialized function because CGAL
    // function names differ in 2D and 3D)
    add_cells(mesh_editor, triangulation);

    // Close mesh editor
    mesh_editor.close();
  }
  //---------------------------------------------------------------------------
  template<typename T>
  void CGALMeshBuilder::build_from_mesh(Mesh& mesh, T& cgal_mesh)
  {
    // Clear mesh
    mesh.clear();

    // CGAL triangulation
    typename T::Triangulation t = cgal_mesh.triangulation();

    // Get various dimensions
    const std::size_t gdim = t.finite_vertices_begin()->point().dimension();
    const std::size_t tdim = t.dimension();
    const std::size_t num_vertices = t.number_of_vertices();
    const std::size_t num_cells = cgal_mesh.number_of_cells();

    // Create a MeshEditor and open
    dolfin::MeshEditor mesh_editor;
    mesh_editor.open(mesh, tdim, gdim);
    mesh_editor.init_vertices(num_vertices, num_vertices);
    mesh_editor.init_cells(num_cells, num_cells);

    // Add vertices to mesh
    std::size_t vertex_index = 0;
    typename T::Triangulation::Finite_vertices_iterator v;
    for (v = t.finite_vertices_begin(); v != t.finite_vertices_end(); ++v)
    {
      // Get vertex coordinates and add vertex to the mesh
      Point p;
      p[0] = v->point()[0];
      p[1] = v->point()[1];
      p[2] = v->point()[2];

      // Add mesh vertex
      mesh_editor.add_vertex(vertex_index, p);

      // Attach index to vertex and increment
      v->info() = vertex_index++;
    }

    // Sanity check on number of vertices
    dolfin_assert(vertex_index == num_vertices);

    // Iterate over all cell in triangulation
    std::size_t cell_index = 0;
    typename T::Triangulation::Finite_cells_iterator c;
    for (c = t.finite_cells_begin(); c != t.finite_cells_end(); ++c)
    {
      // Add cell if in CGAL mesh, and increment index
      if (cgal_mesh.is_in_complex(c))
      {
        std::vector<std::size_t> cell_data(4);
        for (std::size_t i = 0; i < 4; ++i)
          cell_data[i] = c->vertex(i)->info();
        mesh_editor.add_cell(cell_index++, cell_data);
      }
    }

    // Sanity check on number of cells
    dolfin_assert(cell_index == num_cells);

    // Close mesh editor
    mesh_editor.close();
  }
  //---------------------------------------------------------------------------
  template<typename T>
    void CGALMeshBuilder::build_surface_mesh_c3t3(Mesh& mesh, T& cgal_mesh,
                                                  const ImplicitSurface* surface )
  {
    // Clear mesh
    mesh.clear();

    // CGAL triangulation
    typename T::Triangulation t = cgal_mesh.triangulation();

    // Get various dimensions
    const std::size_t gdim = 3;
    const std::size_t tdim = 2;
    const std::size_t num_vertices = t.number_of_vertices();
    const std::size_t num_cells = cgal_mesh.number_of_facets();

    // Create a MeshEditor and open
    dolfin::MeshEditor mesh_editor;
    mesh_editor.open(mesh, tdim, gdim);
    mesh_editor.init_vertices(num_vertices, num_vertices);
    mesh_editor.init_cells(num_cells, num_cells);

    // Set all vertex indices to -1
    typename T::Facets_in_complex_iterator c;
    for (c = cgal_mesh.facets_in_complex_begin();
         c != cgal_mesh.facets_in_complex_end(); ++c)
    {
      c->first->vertex( (c->second + 1)%4 )->info() = -1;
      c->first->vertex( (c->second + 2)%4 )->info() = -1;
      c->first->vertex( (c->second + 3)%4 )->info() = -1;
    }

    // Iterate over facets
    std::size_t cell_index = 0;
    std::size_t vertex_index = 0;
    for (c = cgal_mesh.facets_in_complex_begin();
         c != cgal_mesh.facets_in_complex_end(); ++c)
    {
      // Notes:
      // - c->first is the volume cell
      // - c->second is the local facet index. Facet local index is same
      //   loocal index of opposite vertex

      typedef typename T::Triangulation::Point Point_3;
      if (surface)
      {
        // Compute centroid of facet
        Point p;
        for (std::size_t i = 1; i < 4; ++i)
        {
          // Get the vertex point
          const Point_3& _p = c->first->vertex((c->second + i)%4)->point();
          p[0] += _p.x(); p[1] += _p.y(); p[2] += _p.z();
        }
        p[0] /= 3.0; p[1] /= 3.0; p[2] /= 3.0;

        // Check if facet should be added. If not, continue
        if (surface->f1(p) > 0.0)
          continue;
      }

      // Add vertex if not already added and increment index
      for (std::size_t i = 1; i < 4; ++i)
      {
        // Get the ((c->second + i)%4) cell vertex info
        const int v_index = c->first->vertex( (c->second + i)%4 )->info();

        // In vertex index has not been set, set now and add to mesh
        if (v_index < 0)
        {
          c->first->vertex((c->second + i)%4)->info() = vertex_index;

          // Get vertex coordinates and add vertex to the mesh
          Point p;
          for (std::size_t j = 0; j < 3; ++j)
            p[j] = c->first->vertex((c->second + i) % 4)->point()[j];
          mesh_editor.add_vertex(vertex_index, p);
          ++vertex_index;
        }
      }

      // Get cell vertices and add to Mesh
      std::vector<std::size_t> vertex_indices(3);
      for (std::size_t i = 0; i < 3; ++i)
        vertex_indices[i] = c->first->vertex( (c->second + i + 1)%4 )->info();
      mesh_editor.add_cell(cell_index++, vertex_indices);
    }

    // Close mesh editor
    mesh_editor.close();
  }
  //---------------------------------------------------------------------------
  template<typename T>
    void CGALMeshBuilder::build_surface_mesh_c2t3(Mesh& mesh, T& cgal_mesh,
                                                  const ImplicitSurface* surface)
  {
    if (surface)
      error("CGALMeshBuilder::build_surface_mesh_c2t3 does not yet support implicit surfaces");

    // Clear mesh
    mesh.clear();

    // CGAL triangulation
    typename T::Triangulation t = cgal_mesh.triangulation();

    // Get various dimensions
    const std::size_t gdim = 3;
    const std::size_t tdim = 2;
    const std::size_t num_vertices = t.number_of_vertices();
    const std::size_t num_cells = cgal_mesh.number_of_facets();

    // Create a MeshEditor and open
    dolfin::MeshEditor mesh_editor;
    mesh_editor.open(mesh, tdim, gdim);
    mesh_editor.init_vertices(num_vertices, num_vertices);
    mesh_editor.init_cells(num_cells, num_cells);

    std::size_t vertex_index = 0;
    typename T::Vertex_iterator v;
    for (v = cgal_mesh.vertices_begin(); v != cgal_mesh.vertices_end(); ++v)
      v->info() = -1;

    std::size_t cell_index = 0;
    typename T::Facet_iterator c;
    for (c = cgal_mesh.facets_begin(); c != cgal_mesh.facets_end(); ++c)
    {
      // Add cell if in CGAL mesh, and increment index
      if (cgal_mesh.is_in_complex(*c))
      {
        // Check that cell is on surface
        typedef typename T::Triangulation::Point Point_3;
        if (surface)
        {
          // Compute centroid of facet
          Point p;
          for (std::size_t i = 1; i < 4; ++i)
          {
            // Get the vertex point
            const Point_3& _p = c->first->vertex((c->second + i)%4)->point();
            p[0] += _p.x(); p[1] += _p.y(); p[2] += _p.z();
          }
          p[0] /= 3.0; p[1] /= 3.0; p[2] /= 3.0;

          // Check if facet should be added. If not, continue
          if (surface->f1(p) > 0.0)
            continue;
        }

        // Add vertex if not already added and increment index
        for (std::size_t i = 1; i < 4; ++i)
        {
          // Get the ((c->second + i)%4) cell vertex info
          const int v_index = c->first->vertex( (c->second + i)%4 )->info();

          // In vertex index has not been set, set now and add to mesh
          if (v_index < 0)
          {
            c->first->vertex((c->second + i)%4)->info() = vertex_index;

            // Get vertex coordinates and add vertex to the mesh
            Point p;
            for (std::size_t j = 0; j < 3; ++j)
              p[j] = c->first->vertex((c->second + i) % 4)->point()[j];
            mesh_editor.add_vertex(vertex_index, p);
            ++vertex_index;
          }
        }

        // Get cell vertices and add to Mesh
        std::vector<std::size_t> vertex_indices(3);
        for (std::size_t i = 0; i < 3; ++i)
          vertex_indices[i] = c->first->vertex( (c->second + i + 1)%4 )->info();
        mesh_editor.add_cell(cell_index++, vertex_indices);
      }
    }

    // Close mesh editor
    mesh_editor.close();
  }
  //---------------------------------------------------------------------------
  template<typename T>
  void CGALMeshBuilder::build_surface_mesh_poly(Mesh& mesh, T& poly)
  {
    // Clear mesh
    mesh.clear();

    // Get various dimensions
    const std::size_t gdim = 3;
    const std::size_t tdim = 2;
    const std::size_t num_vertices = poly.size_of_vertices();
    const std::size_t num_cells = poly.size_of_facets();

    // The vertices have no info(), so make a separate point map (vertices
    // aren't ordered)
    std::map<typename T::Point_3, std::size_t> point_map;

    // Create a MeshEditor and open
    dolfin::MeshEditor mesh_editor;
    mesh_editor.open(mesh, tdim, gdim);
    mesh_editor.init_vertices(num_vertices, num_vertices);
    mesh_editor.init_cells(num_cells, num_cells);

    // Add vertices to mesh
    std::size_t vertex_index = 0;
    for (typename T::Vertex_iterator v = poly.vertices_begin();
         v != poly.vertices_end(); ++v)
    {
      // Get vertex coordinates add vertex to the mesh
      Point p;
      p[0] = v->point()[0];
      p[1] = v->point()[1];
      p[2] = v->point()[2];

      // Add mesh vertex
      mesh_editor.add_vertex(vertex_index, p);

      // Attach index to vertex and increment
      point_map[v->point()] = vertex_index++;
    }

    std::size_t cell_index = 0;
    for (typename T::Facet_iterator c = poly.facets_begin();
         c != poly.facets_end(); c++)
    {
      std::vector<std::size_t> vertex_indices;
      typename T::Facet::Halfedge_around_facet_circulator
        halfedge(c->facet_begin());
      do
      {
        vertex_indices.push_back(point_map[halfedge->vertex()->point()]);
        halfedge++;
      }
      while (halfedge != c->facet_begin());

      mesh_editor.add_cell(cell_index++, vertex_indices);
    }

    dolfin_assert(vertex_index == num_vertices);
    dolfin_assert(cell_index == num_cells);

    // Close mesh editor
    mesh_editor.close();
  }
  //---------------------------------------------------------------------------

}

#endif
#endif
