// Copyright (C) 2006-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Hoffman, 2007.
// Modified by Garth N. Wells 2007.
// Modified by Niclas Jansson 2008.
// Modified by Kristoffer Selim 2008.
//
// First added:  2006-05-09
// Last changed: 2009-09-30

#include <sstream>

#include <dolfin/log/log.h>
#include <dolfin/io/File.h>
#include <dolfin/main/MPI.h>
#include <dolfin/ale/ALE.h>
#include <dolfin/io/File.h>
#include <dolfin/common/Timer.h>
#include <dolfin/common/utils.h>
#include "UniformMeshRefinement.h"
#include "LocalMeshRefinement.h"
#include "LocalMeshCoarsening.h"
#include "IntersectionDetector.h"
#include "TopologyComputation.h"
#include "MeshSmoothing.h"
#include "MeshOrdering.h"
#include "MeshFunction.h"
#include "LocalMeshData.h"
#include "MeshPartitioning.h"
#include "BoundaryMesh.h"
#include "Cell.h"
#include "Vertex.h"
#include "Mesh.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Mesh::Mesh()
  : Variable("mesh", "DOLFIN mesh"),
    _data(*this), _cell_type(0), detector(0), _ordered(false)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Mesh::Mesh(const Mesh& mesh)
  : Variable("mesh", "DOLFIN mesh"),
    _data(*this), _cell_type(0), detector(0), _ordered(false)
{
  *this = mesh;
}
//-----------------------------------------------------------------------------
Mesh::Mesh(std::string filename)
  : Variable("mesh", "DOLFIN mesh"),
    _data(*this), _cell_type(0), detector(0), _ordered(false)
{
  if (MPI::num_processes() > 1)
  {
    // Read local mesh data
    Timer timer("PARALLEL 0: Parse local mesh data");
    File file(filename);
    LocalMeshData data;
    file >> data;
    timer.stop();

    // Partition data
    MeshPartitioning::partition(*this, data);
  }
  else
  {
    File file(filename);
    file >> *this;
  }
}
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
  clear();
}
//-----------------------------------------------------------------------------
const Mesh& Mesh::operator=(const Mesh& mesh)
{
  clear();

  _topology = mesh._topology;
  _geometry = mesh._geometry;
  _data = mesh._data;

  if (mesh._cell_type)
    _cell_type = CellType::create(mesh._cell_type->cell_type());

  rename(mesh.name(), mesh.label());

  _ordered = mesh._ordered;

  return *this;
}
//-----------------------------------------------------------------------------
dolfin::uint Mesh::init(uint dim) const
{
  // This function is obviously not const since it may potentially compute
  // new connectivity. However, in a sense all connectivity of a mesh always
  // exists, it just hasn't been computed yet. The const_cast is also needed
  // to allow iterators over a const Mesh to create new connectivity.

  // Skip if already computed
  if (_topology.size(dim) > 0)
    return _topology.size(dim);

  // Check that mesh is ordered
  if (!ordered())
    error("Mesh is not ordered according to the UFC numbering convention, consider calling mesh.order().");

  // Compute connectivity
  Mesh* mesh = const_cast<Mesh*>(this);
  TopologyComputation::compute_entities(*mesh, dim);

  // Order mesh if necessary
  if (!ordered())
    mesh->order();

  return _topology.size(dim);
}
//-----------------------------------------------------------------------------
void Mesh::init(uint d0, uint d1) const
{
  // This function is obviously not const since it may potentially compute
  // new connectivity. However, in a sense all connectivity of a mesh always
  // exists, it just hasn't been computed yet. The const_cast is also needed
  // to allow iterators over a const Mesh to create new connectivity.

  // Skip if already computed
  if (_topology(d0, d1).size() > 0)
    return;

  // Check that mesh is ordered
  if (!ordered())
    error("Mesh is not ordered according to the UFC numbering convention, consider calling mesh.order().");

  // Compute connectivity
  Mesh* mesh = const_cast<Mesh*>(this);
  TopologyComputation::compute_connectivity(*mesh, d0, d1);

  // Order mesh if necessary
  if (!ordered())
    mesh->order();
}
//-----------------------------------------------------------------------------
void Mesh::init() const
{
  // Compute all entities
  for (uint d = 0; d <= topology().dim(); d++)
    init(d);

  // Compute all connectivity
  for (uint d0 = 0; d0 <= topology().dim(); d0++)
    for (uint d1 = 0; d1 <= topology().dim(); d1++)
      init(d0, d1);
}
//-----------------------------------------------------------------------------
void Mesh::clear()
{
  _topology.clear();
  _geometry.clear();
  _data.clear();
  delete _cell_type;
  _cell_type = 0;
  delete detector;
  detector = 0;
  _ordered = false;
}
//-----------------------------------------------------------------------------
void Mesh::order()
{
  // Order mesh
  MeshOrdering::order(*this);

  // Remember that the mesh has been ordered
  _ordered = true;
}
//-----------------------------------------------------------------------------
bool Mesh::ordered() const
{
  // Don't check if we know (or think we know) that the mesh is ordered
  if (_ordered)
    return true;

  _ordered = MeshOrdering::ordered(*this);
  return _ordered;
}
//-----------------------------------------------------------------------------
void Mesh::refine()
{
  not_working_in_parallel("Mesh refinement");

  info("No cells marked for refinement, assuming uniform mesh refinement.");
  UniformMeshRefinement::refine(*this);

  // Mesh may not be ordered
  _ordered = false;
}
//-----------------------------------------------------------------------------
//void Mesh::refine(MeshFunction<bool>& cell_markers, bool refine_boundary)
//{
//  LocalMeshRefinement::refineMeshByEdgeBisection(*this, cell_markers,
//                                                refine_boundary);
//
//  // Mesh may not be ordered
//  _ordered = false;
//}
//-----------------------------------------------------------------------------
void Mesh::refine(MeshFunction<bool>& cell_markers)
{
  cout << "check in Mesh" << endl;
  not_working_in_parallel("Mesh refinement");
  cout << "check after" << endl;

  //LocalMeshRefinement::refineIterativelyByEdgeBisection(*this, cell_markers);
  LocalMeshRefinement::refineRecursivelyByEdgeBisection(*this, cell_markers);

  // Mesh may not be ordered
  _ordered = false;
}
//-----------------------------------------------------------------------------
void Mesh::coarsen()
{
  // FIXME: Move implementation to separate class and just call function here

  info("No cells marked for coarsening, assuming uniform mesh coarsening.");
  MeshFunction<bool> cell_marker(*this);
  cell_marker.init(this->topology().dim());
  for (CellIterator c(*this); !c.end(); ++c)
    cell_marker.set(c->index(),true);

  LocalMeshCoarsening::coarsen_mesh_by_edge_collapse(*this, cell_marker);

  // Mesh may not be ordered
  _ordered = false;
}
//-----------------------------------------------------------------------------
void Mesh::coarsen(MeshFunction<bool>& cell_markers, bool coarsen_boundary)
{
  LocalMeshCoarsening::coarsen_mesh_by_edge_collapse(*this,
                                                     cell_markers,
                                                     coarsen_boundary);

  // Mesh may not be ordered
  _ordered = false;
}
//-----------------------------------------------------------------------------
void Mesh::move(BoundaryMesh& boundary, ALEType method)
{
  ALE::move(*this, boundary, method);
}
//-----------------------------------------------------------------------------
void Mesh::move(Mesh& mesh, ALEType method)
{
  ALE::move(*this, mesh, method);
}
//-----------------------------------------------------------------------------
void Mesh::move(const Function& displacement)
{
  ALE::move(*this, displacement);
}
//-----------------------------------------------------------------------------
void Mesh::smooth(uint num_smoothings)
{
  for (uint i = 0; i < num_smoothings; i++)
    MeshSmoothing::smooth(*this);
}
//-----------------------------------------------------------------------------
void Mesh::intersection(const Point& p, std::vector<uint>& cells, bool fixed_mesh)
{
  // Don't reuse detector if mesh has moved
  if (!fixed_mesh)
  {
    delete detector;
    detector = 0;
  }

  // Create detector if necessary
  if (!detector)
    detector = new IntersectionDetector(*this);

  detector->intersection(p, cells);
}
//-----------------------------------------------------------------------------
void Mesh::intersection(const Point& p1, const Point& p2, std::vector<uint>& cells, bool fixed_mesh)
{
  // Don't reuse detector if the mesh has moved
  if (!fixed_mesh)
  {
    delete detector;
    detector = 0;
  }

  // Create detector if necessary
  if (!detector)
    detector = new IntersectionDetector(*this);

  detector->intersection(p1, p2, cells);
}
//-----------------------------------------------------------------------------
void Mesh::intersection(Cell& cell, std::vector<uint>& cells, bool fixed_mesh)
{
  // Don't reuse detector if the has has moved
  if (!fixed_mesh)
  {
    delete detector;
    detector = 0;
  }

  // Create detector if necessary
  if (!detector)
    detector = new IntersectionDetector(*this);

  detector->intersection(cell, cells);
}
//-----------------------------------------------------------------------------
void Mesh::intersection(std::vector<Point>& points, std::vector<uint>& intersection, bool fixed_mesh)
{
  // Don't reuse detector if the mesh has moved
  if (!fixed_mesh)
  {
    delete detector;
    detector = 0;
  }

  // Create detector if necessary
  if (!detector)
    detector = new IntersectionDetector(*this);

  detector->intersection(points, intersection);
}
//-----------------------------------------------------------------------------
void Mesh::intersection(Mesh& mesh, std::vector<uint>& intersection, bool fixed_mesh)
{
  // Don't reuse detector if the mesh has moved
  if (!fixed_mesh)
  {
    delete detector;
    detector = 0;
  }

  // Create detector if necessary
  if (!detector)
    detector = new IntersectionDetector(*this);

  detector->intersection(mesh, intersection);
}
//-----------------------------------------------------------------------------
double Mesh::hmin() const
{
  CellIterator cell(*this);
  double h = cell->diameter();
  for (; !cell.end(); ++cell)
    h = std::min(h, cell->diameter());

  return h;
}
//-----------------------------------------------------------------------------
double Mesh::hmax() const
{
  CellIterator cell(*this);
  double h = cell->diameter();
  for (; !cell.end(); ++cell)
    h = std::max(h, cell->diameter());

  return h;
}
//-----------------------------------------------------------------------------
std::string Mesh::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    s << indent(_topology.str(true));
    s << indent(_geometry.str(true));
    s << indent(_data.str(true));
  }
  else
  {
    s << "<Mesh of topological dimension "
      << topology().dim() << " ("
      << _cell_type->description(true) << ") with "
      << num_vertices() << " vertices and "
      << num_cells() << " cells, "
      << (_ordered ? "ordered" : "unordered") << ">";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
