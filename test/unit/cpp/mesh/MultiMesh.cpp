// Copyright (C) 2017 August Johansson, Benjamin Kehlet
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
// First added:  2017-09-04
// Last changed: 2017-09-04
//
// Unit tests for the multimesh functionality

//These tests currently only instantiates and initializes some
// multimesh objects, ie. the only thing that is tested is that it
// doesn't crash

#include <dolfin.h>
#include <gtest/gtest.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
TEST(MultiMesh, DISABLED_trivialCase3D)
{
  // FIXME: Enable this test when it is working
  
  std::shared_ptr<Mesh> background(new Mesh(UnitTetrahedronMesh::create()));
  std::shared_ptr<Mesh> overlapping(new Mesh(UnitTetrahedronMesh::create()));
  overlapping->translate(Point(.1, .1, .1));

  MultiMesh multimesh(background, overlapping, 1);
}
//-----------------------------------------------------------------------------
TEST(MultiMesh, DISABLED_trivialCase3D2)
{
  // FIXME: Enable this test when it is working
  
  std::shared_ptr<Mesh> background = std::make_shared<UnitCubeMesh>(1,1,1);

  std::shared_ptr<Mesh> refmesh = std::make_shared<BoxMesh>(Point(0.394383, 0.783099, 0.197551),
							    Point(0.840188, 0.79844, 0.911647),
							    1,1,1);
  const std::vector<double>& refmeshcoords = refmesh->coordinates();

  std::shared_ptr<Mesh> tetmesh(new Mesh(UnitTetrahedronMesh::create()));
  std::vector<double>& tetmesh_coords = tetmesh->coordinates();

  for (CellIterator cell(*refmesh); !cell.end(); ++cell)
  {
    const unsigned int* vertex_indices = cell->entities(0);

    for (int i = 0; i < 4; i++)
    {
      tetmesh_coords[i*3] = refmeshcoords[vertex_indices[i]];
      tetmesh_coords[i*3 + 1] = refmeshcoords[vertex_indices[i] + 1];
      tetmesh_coords[i*3 + 2] = refmeshcoords[vertex_indices[i] + 2];
    }

    MultiMesh multimesh(background, tetmesh, 1);
  }
}
//-----------------------------------------------------------------------------
TEST(MultiMesh, DISABLED_takesForever)
{
  // FIXME: Enable this test when it is working
  
  std::shared_ptr<Mesh> background(new Mesh(UnitTetrahedronMesh::create()));
  {
    std::vector<double>& coords = background->coordinates();
    coords = { 0, 0, 0,
	       0, 0, 1,
	       1, 0, 1,
	       1, 1, 1 };
  }

  std::shared_ptr<Mesh> overlapping = std::make_shared<BoxMesh>(Point(0.394383, 0.783099, 0.197551),
								Point(0.840188, 0.79844,  0.911647),
								1,1,1);
  MultiMesh multimesh(background, overlapping, 1);
}
//-----------------------------------------------------------------------------
int MultiMesh_main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
//-----------------------------------------------------------------------------
