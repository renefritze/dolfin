// Copyright (C) 2013-2015 Anders Logg
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
// First added:  2013-06-26
// Last changed: 2016-03-02
//
// This demo program solves Poisson's equation on a domain defined by
// three overlapping and non-matching meshes. The solution is computed
// on a sequence of rotating meshes to test the multimesh
// functionality.

#include <cmath>
#include <dolfin.h>
#include "MultiMeshAssemble.h"
#include "MeshAssemble.h"

using namespace dolfin;
using std::make_shared;

void assemble_scalar()
{
  // Create meshes
  double r = 0.5;
  auto mesh_0 = make_shared<RectangleMesh>(Point(0.,0.), Point(2., 2.), 16, 16);
  auto mesh_1 = make_shared<RectangleMesh>(Point(1., 1.), Point(3., 3.), 10, 10);
  auto mesh_2 = make_shared<RectangleMesh>(Point(-1., -1.),Point(0., 0.), 8, 8);
					   //Point(0.5, 0.5), 8, 8);

  // Build multimesh
  auto multimesh = make_shared<MultiMesh>();
  multimesh->add(mesh_0);
  multimesh->add(mesh_1);
  multimesh->add(mesh_2);
  multimesh->build();

  // The function v
  class MyFunction : public Expression
  {
  public:

    void eval(Array<double>& values, const Array<double>& x) const
    //{ values[0] = sin(x[0]) + cos(x[1]); }
    { values[0] = 1; }

  };

  // Create function space
  auto V = make_shared<MultiMeshAssemble::MultiMeshFunctionSpace>(multimesh);

  // Create forms
  auto v = std::make_shared<MyFunction>();
  auto u = make_shared<MultiMeshFunction>(V);

 
  //Assemble Functional
  MeshAssemble::Functional N(mesh_2);
  N.v = v;
  double b2 = assemble(N);
  cout << "Area of mesh_2"<<endl;
  cout << b2 << endl;
  
  // Assemble MultiMeshFunctional
  MultiMeshAssemble::MultiMeshFunctional M(multimesh);
  M.v = v;
  cout << "Area of the total mesh" <<endl;
  //cout << M.rank( ) <<endl;
  double b = assemble_multimesh(M);
  cout << b << endl;
}

int main(int argc, char* argv[])
{
  if (dolfin::MPI::size(MPI_COMM_WORLD) > 1)
  {
    info("Sorry, this demo does not (yet) run in parallel.");
    return 0;
  }

  // Compute solution
  assemble_scalar();

  return 0;
}
