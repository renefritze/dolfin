// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-12-01
// Last changed: 
//
// This file is used for testing parallel assembly

#include <dolfin.h>
#include <dolfin/Poisson2D.h>
#include <parmetis.h>
extern "C"
{
  #include <metis.h>
}

using namespace dolfin;

//-----------------------------------------------------------------------------
void testMeshPartition(Mesh& mesh, MeshFunction<dolfin::uint>& cell_partition_function,
          MeshFunction<dolfin::uint>& vertex_partition_function, int num_partitions)
{
  int num_cells     = mesh.numCells() ;
  int num_vertices  = mesh.numVertices();
  
  int index_base = 0;  // zero-based indexing
  int edges_cut  = 0;

  int cell_type = 0;
  idxtype* cell_partition   = new int[num_cells];
  idxtype* vertex_partition = new int[num_vertices];
  idxtype* mesh_data = 0;

  // Set cell type and allocate memory for METIS mesh structure
  if(mesh.type().cellType() == CellType::triangle)
  {
    cell_type = 1;
    mesh_data = new int[3*num_cells];
  }
  else if(mesh.type().cellType() == CellType::tetrahedron) 
  {
    cell_type = 2;
    mesh_data = new int[4*num_cells];
  }
  else
    dolfin_error("Do not know how to partition mesh of this type");
  
  // Create mesh structure for METIS
  dolfin::uint i = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    for (VertexIterator vertex(cell); !vertex.end(); ++vertex)
      mesh_data[i++] = vertex->index();

  // Use METIS to partition mesh
  METIS_PartMeshNodal(&num_cells, &num_vertices, mesh_data, &cell_type, &index_base, 
                      &num_partitions, &edges_cut, cell_partition, vertex_partition);

  cell_partition_function.init(mesh, mesh.topology().dim());
  vertex_partition_function.init(mesh, 0);

  // Set partition numbers on cells
  i = 0;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
    cell_partition_function.set(cell->index(), cell_partition[i++]);

  // Set partition numbers on vertexes
  i = 0;
  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
    vertex_partition_function.set(vertex->index(), vertex_partition[i++]);

  // Clean up
  delete [] cell_partition;
  delete [] vertex_partition;
  delete [] mesh_data;
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Initialise PETSc  
  PETScManager::init();

  // Get number of processes
  int num_processes_int;
  MPI_Comm_size(PETSC_COMM_WORLD, &num_processes_int);
  unsigned int num_processes = num_processes_int;

  // Get this process number
  int process_int;
  MPI_Comm_rank(PETSC_COMM_WORLD, &process_int);
  unsigned int process = process_int;

  // Create mesh
  UnitSquare mesh(2,2);

  // Initialize connectivity
  for(dolfin::uint i = 0; i < mesh.topology().dim(); i++)
    mesh.init(i);

  // Create linear and bilinear form
  Function f = 1.0;
  Poisson2D::BilinearForm a; 
  Poisson2D::LinearForm L(f); 
  

  if ( num_processes < 2 )
    dolfin_error("Cannot create single partition. You need to run woth mpirun -np X . . . ");

  // Partition mesh (number of partitions = number of processes)
  // Create mesh functions for partition numbers
  MeshFunction<dolfin::uint> cell_partition_function;
  MeshFunction<dolfin::uint> vertex_partition_function;
  testMeshPartition(mesh, cell_partition_function, vertex_partition_function,
                    num_processes);

  // FIXME: Need to regenerate degree of freedom mapping here which
  //        is appropriate.
  
  // Global matrix size
  const int N = FEM::size(mesh, a.test());

  // Compute number of vertices belonging to this processor 
  int local_num_vertices = 0;
  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
    if ( vertex_partition_function.get(*vertex) == process )
      ++local_num_vertices;

  cout << "Proc " << process << ", local num nodes  " << local_num_vertices 
          << "  " << mesh.numVertices() << endl;

  //int n = local_num_vertices;

  // Create PETSc vector
  Vec b;
  VecCreateMPI(PETSC_COMM_WORLD, PETSC_DECIDE, N, &b);
  Vec x;
  VecCreateMPI(PETSC_COMM_WORLD, PETSC_DECIDE, N, &x);

  // Create PETSc matrix
  Mat A;
  MatCreate(PETSC_COMM_WORLD, &A);
//  MatSetSizes(A, n, PETSC_DECIDE, N, N);   
  MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, N, N);   
  MatSetType(A, MATMPIAIJ);
 
//  int m_range;
//  int n_range;
//  MatGetOwnershipRange(A, &m_range, &n_range);
//  cout << "Proc " << process << "  " << m_range << "  " << n_range << endl;

  /// Start assembly
 
  // Create affine map
  AffineMap map;

  int vertices_per_cell = 0;
  if(mesh.type().cellType() == CellType::triangle)
    vertices_per_cell = 3;
  else if(mesh.type().cellType() == CellType::tetrahedron) 
    vertices_per_cell = 4;
  else
    dolfin_error("Do not know how to work with meshes of this type");

  real* A_block = new real[vertices_per_cell*vertices_per_cell];
  real* b_block = new real[vertices_per_cell];
  int*  pos     = new int[vertices_per_cell];

  // Zero matrix
  MatZeroEntries(A);
  VecZeroEntries(b);

  // Assemble if cell belongs to this process's partition
  for(CellIterator cell(mesh); !cell.end(); ++cell)
  {
    if(cell_partition_function.get(*cell) == static_cast<unsigned int>(process) )
    {
      map.update(*cell);

      // Update form
      a.update(map);
      L.update(map);

      // Create mapping for cell
      int i = 0;
      for(VertexIterator vertex(cell); !vertex.end(); ++vertex)
        pos[i++] = vertex->index();

      // Evaluate element matrix and vector
      a.eval(A_block, map);
      L.eval(b_block, map);

      MatSetValues(A, vertices_per_cell, pos, vertices_per_cell, pos, A_block, ADD_VALUES);
      VecSetValues(b, vertices_per_cell, pos, b_block, ADD_VALUES);
    }
  }  
  
  // Finalise assembly
  VecAssemblyBegin(b);
  VecAssemblyEnd(b);
  MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

  // Apply some boundary conditions so that the system can be solved
  // Just apply homogeneous Dirichlet bc to first three vertices
  IS is = 0;
  int nrows = 3;
  int rows[3] = {0, 1, 2};
  ISCreateGeneral(PETSC_COMM_WORLD, nrows, rows, &is);
  PetscScalar one = 1.0;
  MatZeroRowsIS(A, is, one);
  ISDestroy(is);

  real bc_values[3] = {0, 0, 0};
  VecSetValues(b, nrows, rows, bc_values, INSERT_VALUES);
  VecAssemblyBegin(b);
  VecAssemblyEnd(b);

  // Solve system
  KSP ksp;
  KSPCreate(PETSC_COMM_WORLD, &ksp);
  KSPSetFromOptions(ksp);
  KSPSetOperators(ksp, A, A, SAME_NONZERO_PATTERN);
  KSPSolve(ksp, b, x);


//  cout << "Parallel RHS vector " << endl;
//  VecView(b, PETSC_VIEWER_STDOUT_WORLD);
//  cout << "Parallel matrix " << endl;
//  MatView(A, PETSC_VIEWER_STDOUT_WORLD);
  cout << "Parallel solution vector " << endl;
  VecView(x, PETSC_VIEWER_STDOUT_WORLD);

  delete [] A_block;
  delete [] b_block;
  delete [] pos;

  if(process == 0)
  {
    dolfin_log(false);
    PETScMatrix Aref;
    PETScVector bref;
    PETScVector xref;
    FEM::assemble(a, L, Aref, bref, mesh); 
    Aref.ident(rows, nrows);
    bref(0) = 0.0;
    bref(1) = 0.0;
    bref(2) = 0.0;
    dolfin_log(true);
    KrylovSolver solver;
    solver.solve(Aref, xref, bref);

//    cout << "Single process reference vector " << endl;
//    VecView(bref.vec(), PETSC_VIEWER_STDOUT_SELF);
//    cout << "Single process reference matrix " << endl;
//    MatView(Aref.mat(), PETSC_VIEWER_STDOUT_SELF);
    cout << "Single process solution vector " << endl;
    VecView(xref.vec(), PETSC_VIEWER_STDOUT_SELF);
  }


  return 0;
}
