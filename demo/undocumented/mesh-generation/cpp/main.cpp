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
// First added:  2012-02-02
// Last changed: 2012-07-05
//
// This program generates a mesh for a polygonal domain that is
// represented by a list of its vertices.

#include <vector>
#include <dolfin.h>

using namespace dolfin;

#ifdef HAS_CGAL

// Class that implicitly defines a warped sphere
class WarpedSphere : public ImplicitSurface
{
public:

  WarpedSphere() : ImplicitSurface(Sphere(Point(0.0, 0.0, 0.0), 2.1), "manifold") {}
  //WarpedSphere() : ImplicitSurface(Sphere(Point(0.5, 0.5, 0.5), 2.1), "manifold_with_boundary") {}
  //WarpedSphere() : ImplicitSurface(Sphere(Point(0.5, 0.5, 0.5), 3.1), "manifold") {}

  double operator()(const Point& p) const
  {
    const double R = sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    const double theta = asin(p[0]/R);
    //if ( R + 0.05*sin(10.0*theta) < 2.0)
    //  return -1.0;
    //else
    //  return 1.0;
    return R + 0.05*sin(10.0*theta) - 2.0;

    /*
    //if ((p[0] > 0.0 && p[0] < 1.0) && (p[1] > 0.0 && p[1] < 0.01) && (p[2] > 0.0 && p[2] < 1.0))
    if ((p[0] > 0.0 && p[0] < 1.0) && (p[1] > 0.0 && p[1] < 0.01) && (p[2] > 0.0 && p[2] < 1.0))
      return -1.0;
    else
      return 1.0;
    */
  }

  bool on_surface(const Point& point) const
  {
    if (point[1] > 0.0)
      return true;
    else
      return false;
  }

};

/*
// Class that implicitly defines a cube
class BubbleCube : public ImplicitSurface
{
public:

  BubbleCube() : ImplicitSurface(Sphere(Point(0.0, 0.0, 0.0), 2.1), "manifold") {}

  double operator()(const Point& p) const
  {
    const double R = sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    const double theta = asin(p[0]/R);
    return R + 0.05*sin(10.0*theta) - 2.0;
  }

};
*/

int main()
{
  // Create empty mesh
  Mesh mesh;

  // Polygonal domain vertices
  std::vector<Point> domain_vertices;
  domain_vertices.push_back(Point(0.0,  0.0));
  domain_vertices.push_back(Point(10.0, 0.0));
  domain_vertices.push_back(Point(10.0, 2.0));
  domain_vertices.push_back(Point(8.0,  2.0));
  domain_vertices.push_back(Point(7.5,  1.0));
  domain_vertices.push_back(Point(2.5,  1.0));
  domain_vertices.push_back(Point(2.0,  4.0));
  domain_vertices.push_back(Point(0.0,  4.0));
  domain_vertices.push_back(Point(0.0,  0.0));

  // Generate 2D mesh and plot
  //PolygonalMeshGenerator::generate(mesh, domain_vertices, 0.25);
  //plot(mesh);

  // Polyhedron face vertices
  std::vector<Point> face_vertices;
  face_vertices.push_back(Point(0.0, 0.0, 0.0));
  face_vertices.push_back(Point(0.0, 0.0, 1.0));
  face_vertices.push_back(Point(0.0, 1.0, 0.0));
  face_vertices.push_back(Point(1.0, 0.0, 0.0));

  // Polyhedron faces (of a tetrahedron)
  std::vector<std::vector<std::size_t> > faces(4, std::vector<std::size_t>(3));
  faces[0][0] = 3;
  faces[0][1] = 2;
  faces[0][2] = 1;

  faces[1][0] = 0;
  faces[1][1] = 3;
  faces[1][2] = 1;

  faces[2][0] = 0;
  faces[2][1] = 2;
  faces[2][2] = 3;

  faces[3][0] = 0;
  faces[3][1] = 1;
  faces[3][2] = 2;

  // Generate volume mesh (tetrahedral cells)
  //PolyhedralMeshGenerator::generate(mesh, face_vertices, faces, 0.04);
  //cout << "Dim: " << mesh.topology().dim() << endl;
  //plot(mesh);
  //interactive();

  // Generate surface mesh (triangular cells)
  //PolyhedralMeshGenerator::generate_surface_mesh(mesh, face_vertices, faces, 0.04);
  //cout << "Dim: " << mesh.topology().dim() << endl;
  //plot(mesh);
  //interactive();

  // Generate volume mesh from OFF file input (a cube) and plot
  //PolyhedralMeshGenerator::generate(mesh, "../cube.off", 0.05);
  //plot(mesh);
  //interactive();

  // Generate surface mesh from OFF file input (a cube) and plot
  //PolyhedralMeshGenerator::generate_surface_mesh(mesh, "../cube.off", 0.05);
  //plot(mesh);
  //interactive();

  // Generate surface in 3D mesh from OFF file input (a cube) and plot
  //PolyhedralMeshGenerator::generate_surface_mesh(mesh, "../cube.off", 0.05);em
  //File file("mesh.pvd");
  //file << mesh;
  //plot(mesh);
  //interactive();

  WarpedSphere surface;
  //SurfaceMeshGenerator::generate(mesh, surface, 30.0, 0.01, 0.01, 10);
  //plot(mesh);
  //interactive();

  //ImplicitDomainMeshGenerator::generate(mesh, surface, 0.05);
  ImplicitDomainMeshGenerator::generate_surface(mesh, surface, 0.1);
  cout << "T-dim: " << mesh.topology().dim() << endl;
  cout << "Num cells: " << mesh.num_cells() << endl;
  File file("mesh.pvd");
  file << mesh;
  plot(mesh);
  interactive();

}

#else

int main()
{
  info("DOLFIN must be compiled with CGAL to run this demo.");
}

#endif
