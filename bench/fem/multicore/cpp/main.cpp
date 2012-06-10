// Copyright (C) 2010 Anders Logg
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
// First added:  2010-11-11
// Last changed: 2010-11-29
//
// If run without command-line arguments, this benchmark iterates from
// zero to MAX_NUM_THREADS. If a command-line argument --num_threads n
// is given, the benchmark is run with the specified number of threads.

#include <cstdlib>

#include <dolfin.h>
#include <dolfin/fem/AssemblerTools.h>
#include <dolfin/graph/MatrixRenumbering.h>
#include "Poisson.h"
#include "NavierStokes.h"

#define MAX_NUM_THREADS 4
#define SIZE 12
#define NUM_REPS 20

using namespace dolfin;

class PoissonFactory
{
  public:

  static boost::shared_ptr<Form> a(const Mesh& mesh, bool renumber)
  {
    // Create function space
    boost::shared_ptr<FunctionSpace> _V(new Poisson::FunctionSpace(mesh));

    boost::shared_ptr<Form> _a;

    if (renumber)
    {
      // Access dofmap
      boost::shared_ptr<const GenericDofMap> dofmap = _V->dofmap();

      // Reorder dofs
      std::vector<const GenericDofMap*> dofmaps(2);
      dofmaps[0] = dofmap.get();
      dofmaps[1] = dofmap.get();

      TensorLayout tensor_layout(0, true);
      std::vector<uint> global_dimensions(2);
      std::vector<std::pair<uint, uint> > local_range(2);
      for (uint i = 0; i < 2; i++)
        {
          global_dimensions[i] = dofmap->global_dimension();
          local_range[i]       = dofmap->ownership_range();
        }
      tensor_layout.init(global_dimensions, local_range);
      SparsityPatternBuilder::build(*tensor_layout.sparsity_pattern(),
                                    mesh, dofmaps,
                                    true, false, false);

      MatrixRenumbering matrix_renumbering(tensor_layout);
      std::vector<uint> dof_remap = matrix_renumbering.compute_local_renumbering_map();

      // Renumber dofs
      const_cast<GenericDofMap*>(dofmap.get())->renumber(dof_remap);

      boost::shared_ptr<FunctionSpace> V(new FunctionSpace(_V->mesh(), _V->element(), dofmap));
      _a.reset(new Poisson::BilinearForm(V, V));
    }
    else
      _a.reset(new Poisson::BilinearForm(_V, _V));

    return _a;
  }

};

class NavierStokesFactory
{
  public:

  static boost::shared_ptr<Form> a(const Mesh& mesh, bool renumber)
  {
    boost::shared_ptr<FunctionSpace> _V(new NavierStokes::FunctionSpace(mesh));

    boost::shared_ptr<FunctionSpace> W0(new NavierStokes::Form_0_FunctionSpace_2(mesh));
    boost::shared_ptr<FunctionSpace> W1(new NavierStokes::Form_0_FunctionSpace_3(mesh));
    boost::shared_ptr<FunctionSpace> W2(new NavierStokes::Form_0_FunctionSpace_4(mesh));
    boost::shared_ptr<FunctionSpace> W3(new NavierStokes::Form_0_FunctionSpace_5(mesh));
    boost::shared_ptr<FunctionSpace> W4(new NavierStokes::Form_0_FunctionSpace_6(mesh));

    boost::shared_ptr<Function> w0(new Function(W0));
    boost::shared_ptr<Function> w1(new Function(W1));
    boost::shared_ptr<Function> w2(new Function(W2));
    boost::shared_ptr<Function> w3(new Function(W3));
    boost::shared_ptr<Function> w4(new Function(W4));

    boost::shared_ptr<Form> a;
    if (renumber)
    {
      // Access dofmap
      boost::shared_ptr<const GenericDofMap> dofmap = _V->dofmap();

      // Reorder dofs
      std::vector<const GenericDofMap*> dofmaps(2);
      dofmaps[0] = dofmap.get();
      dofmaps[1] = dofmap.get();

      TensorLayout tensor_layout(0, true);
      std::vector<uint> global_dimensions(2);
      std::vector<std::pair<uint, uint> > local_range(2);
      for (uint i = 0; i < 2; i++)
        {
          global_dimensions[i] = dofmap->global_dimension();
          local_range[i]       = dofmap->ownership_range();
        }
      tensor_layout.init(global_dimensions, local_range);
      SparsityPatternBuilder::build(*tensor_layout.sparsity_pattern(),
                                    mesh, dofmaps,
                                    true, false, false);

      MatrixRenumbering matrix_renumbering(tensor_layout);
      std::vector<uint> dof_remap = matrix_renumbering.compute_local_renumbering_map();

      // Renumber dofs
      const_cast<GenericDofMap*>(dofmap.get())->renumber(dof_remap);

      boost::shared_ptr<FunctionSpace> V(new FunctionSpace(_V->mesh(), _V->element(), dofmap));
      a.reset(new NavierStokes::BilinearForm(V, V));
    }
    else
      a.reset(new NavierStokes::BilinearForm(_V, _V));

    a->set_coefficient(0, w0);
    a->set_coefficient(1, w1);
    a->set_coefficient(2, w2);
    a->set_coefficient(3, w3);
    a->set_coefficient(4, w4);

    return a;
  }
};

double bench(std::string form, boost::shared_ptr<const Form> a)
{
  dolfin::uint num_threads = parameters["num_threads"];
  info_underline("Benchmarking %s, num_threads = %d", form.c_str(), num_threads);

  // Create STL matrix
  //STLMatrix A;
  Matrix A;

  // Intialise matrix
  AssemblerTools::init_global_tensor(A, *a, true, false);

  // Assemble
  Timer timer("Total time");
  for (dolfin::uint i = 0; i < NUM_REPS; ++i)
    assemble(A, *a, false);
  const double t = timer.stop();

  // Write summary
  summary(true);

  info("");

  return t;
}

int main(int argc, char* argv[])
{
  // Parse command-line arguments
  parameters.parse(argc, argv);

  //SubSystemsManager::init_petsc();
  //PetscInfoAllow(PETSC_TRUE, PETSC_NULL);
  //PetscOptionsSetValue("-mat_inode_limit", "5");

  // Set backend
  //parameters["linear_algebra_backend"] = "Epetra";

  // Create mesh
  UnitCube old_mesh(SIZE, SIZE, SIZE);
  old_mesh.color("vertex");
  Mesh mesh = old_mesh.renumber_by_color();

  const bool renumber = true;

  // Test cases
  std::vector<std::pair<std::string, boost::shared_ptr<const Form> > > forms;
  forms.push_back(std::make_pair("Poisson", PoissonFactory::a(mesh, renumber)));
  forms.push_back(std::make_pair("NavierStokes", NavierStokesFactory::a(mesh, renumber)));

  // If parameter num_threads has been set, just run once
  if (parameters["num_threads"].change_count() > 0)
  {
    for (unsigned int i = 0; i < forms.size(); i++)
      bench(forms[i].first, forms[i].second);
  }

  // Otherwise, iterate from 1 to MAX_NUM_THREADS
  else
  {
    Table timings("Timings");
    Table speedups("Speedups");

    // Iterate over number of threads
    for (int num_threads = 0; num_threads <= MAX_NUM_THREADS; num_threads++)
    {
      // Set the number of threads
      parameters["num_threads"] = num_threads;

      // Iterate over forms
      for (unsigned int i = 0; i < forms.size(); i++)
      {

        // Create form
        /*
        boost::shared_ptr<Form> a;
        if (forms[i] == "Poisson")
          a = PoissonFactory::a(mesh);
        else if (forms[i] == "NavierStokes")
          a = NavierStokesFactory::a(mesh);
        else
          error("Form name unknown");
        */

        // Run test case
        const double t = bench(forms[i].first, forms[i].second);

        // Store results and scale to get speedups
        std::stringstream s;
        s << num_threads << " threads";
        timings(s.str(), forms[i].first) = t;
        speedups(s.str(), forms[i].first) = timings.get_value("0 threads", forms[i].first) / t;
        if (num_threads == 0)
          speedups(s.str(), "(rel 1 thread " + forms[i].first + ")") = "-";
        else
          speedups(s.str(),  "(rel 1 thread " + forms[i].first + ")") = timings.get_value("1 threads", forms[i].first) / t;
      }
    }

    // Display results
    info("");
    info(timings, true);
    info("");
    info(speedups, true);
  }

  return 0;
}
