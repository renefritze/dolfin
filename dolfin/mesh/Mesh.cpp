// Copyright (C) 2006-2011 Anders Logg
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
// Modified by Johan Hoffman 2007
// Modified by Garth N. Wells 2007-2011
// Modified by Niclas Jansson 2008
// Modified by Kristoffer Selim 2008
// Modified by Andre Massing 2009-2010
// Modified by Johannes Ring 2012
// Modified by Mikael Mortensen 2012
//
// First added:  2006-05-09
// Last changed: 2012-10-24

#include <boost/serialization/map.hpp>
#include <dolfin/common/Array.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/ale/ALE.h>
#include <dolfin/common/Timer.h>
#include <dolfin/common/utils.h>
#include <dolfin/io/File.h>
#include <dolfin/log/log.h>
#include <dolfin/common/MPI.h>
#include "BoundaryMesh.h"
#include "Cell.h"
#include "LocalMeshData.h"
#include "MeshColoring.h"
#include "MeshData.h"
#include "MeshFunction.h"
#include "MeshOrdering.h"
#include "MeshPartitioning.h"
#include "MeshRenumbering.h"
#include "MeshSmoothing.h"
#include "MeshTransformation.h"
#include "TopologyComputation.h"
#include "Vertex.h"
#include "Mesh.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Mesh::Mesh() : Variable("mesh", "DOLFIN mesh"),
               Hierarchical<Mesh>(*this),
               _data(*this),
               _cell_type(0),
               _intersection_operator(*this),
               _ordered(false)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Mesh::Mesh(const Mesh& mesh) : Variable("mesh", "DOLFIN mesh"),
                               Hierarchical<Mesh>(*this),
                               _data(*this),
                               _cell_type(0),
                               _intersection_operator(*this),
                               _ordered(false)
{
  *this = mesh;
}
//-----------------------------------------------------------------------------
Mesh::Mesh(std::string filename) : Variable("mesh", "DOLFIN mesh"),
                                   Hierarchical<Mesh>(*this),
                                   _data(*this),
                                   _cell_type(0),
                                   _intersection_operator(*this),
                                   _ordered(false)
{
  File file(filename);
  file >> *this;
}
//-----------------------------------------------------------------------------
Mesh::Mesh(LocalMeshData& local_mesh_data)
                                 : Variable("mesh", "DOLFIN mesh"),
                                   Hierarchical<Mesh>(*this),
                                   _data(*this),
                                   _cell_type(0),
                                   _intersection_operator(*this),
                                   _ordered(false)
{
  MeshPartitioning::build_distributed_mesh(*this, local_mesh_data);
}
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
  clear();
}
//-----------------------------------------------------------------------------
const Mesh& Mesh::operator=(const Mesh& mesh)
{
  // Clear all data
  clear();

  // Assign data
  _topology = mesh._topology;
  _geometry = mesh._geometry;
  _domains = mesh._domains;
  _data = mesh._data;
  if (mesh._cell_type)
    _cell_type = CellType::create(mesh._cell_type->cell_type());

  // Rename
  rename(mesh.name(), mesh.label());

  // Call assignment operator for base class
  Hierarchical<Mesh>::operator=(mesh);

  return *this;
}
//-----------------------------------------------------------------------------
MeshData& Mesh::data()
{
  return _data;
}
//-----------------------------------------------------------------------------
const MeshData& Mesh::data() const
{
  return _data;
}
//-----------------------------------------------------------------------------
dolfin::uint Mesh::init(uint dim) const
{
  // This function is obviously not const since it may potentially compute
  // new connectivity. However, in a sense all connectivity of a mesh always
  // exists, it just hasn't been computed yet. The const_cast is also needed
  // to allow iterators over a const Mesh to create new connectivity.

  // Skip if mesh is empty
  if (num_cells() == 0)
  {
    warning("Mesh is empty, unable to create entities of dimension %d.", dim);
    return 0;
  }

  // Skip if already computed
  if (_topology.size(dim) > 0)
    return _topology.size(dim);

  // Skip vertices and cells (should always exist)
  if (dim == 0 || dim == _topology.dim())
    return _topology.size(dim);

  // Check that mesh is ordered
  if (!ordered())
  {
    dolfin_error("Mesh.cpp",
                 "initialize mesh entities",
                 "Mesh is not ordered according to the UFC numbering convention. Consider calling mesh.order()");
  }

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

  // Skip if mesh is empty
  if (num_cells() == 0)
  {
    warning("Mesh is empty, unable to create connectivity %d --> %d.", d0, d1);
    return;
  }

  // Skip if already computed
  if (!_topology(d0, d1).empty())
    return;

  // Check that mesh is ordered
  if (!ordered())
  {
    dolfin_error("Mesh.cpp",
                 "initialize mesh connectivity",
                 "Mesh is not ordered according to the UFC numbering convention. Consider calling mesh.order()");
  }

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
  _intersection_operator.clear();
  _ordered = false;
}
//-----------------------------------------------------------------------------
void Mesh::clean()
{
  const uint D = topology().dim();
  for (uint d0 = 0; d0 <= D; d0++)
  {
    for (uint d1 = 0; d1 <= D; d1++)
    {
      if (!(d0 == D && d1 == 0))
        _topology.clear(d0, d1);
    }
  }
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
dolfin::Mesh Mesh::renumber_by_color() const
{
  std::vector<uint> coloring_type;
  const uint D = topology().dim();
  coloring_type.push_back(D); coloring_type.push_back(0); coloring_type.push_back(D);
  return MeshRenumbering::renumber_by_color(*this, coloring_type);
}
//-----------------------------------------------------------------------------
void Mesh::rotate(double angle, uint axis)
{
  MeshTransformation::rotate(*this, angle, axis);
}
//-----------------------------------------------------------------------------
void Mesh::rotate(double angle, uint axis, const Point& p)
{
  MeshTransformation::rotate(*this, angle, axis, p);
}
//-----------------------------------------------------------------------------
void Mesh::move(BoundaryMesh& boundary)
{
  ALE::move(*this, boundary);
}
//-----------------------------------------------------------------------------
void Mesh::move(Mesh& mesh)
{
  ALE::move(*this, mesh);
}
//-----------------------------------------------------------------------------
void Mesh::move(const Function& displacement)
{
  ALE::move(*this, displacement);
}
//-----------------------------------------------------------------------------
void Mesh::smooth(uint num_iterations)
{
  MeshSmoothing::smooth(*this, num_iterations);
}
//-----------------------------------------------------------------------------
void Mesh::smooth_boundary(uint num_iterations, bool harmonic_smoothing)
{
  MeshSmoothing::smooth_boundary(*this, num_iterations, harmonic_smoothing);
}
//-----------------------------------------------------------------------------
void Mesh::snap_boundary(const SubDomain& sub_domain, bool harmonic_smoothing)
{
  MeshSmoothing::snap_boundary(*this, sub_domain, harmonic_smoothing);
}
//-----------------------------------------------------------------------------
const std::vector<dolfin::uint>& Mesh::color(std::string coloring_type) const
{
  // Define graph type
  const uint dim = MeshColoring::type_to_dim(coloring_type, *this);
  std::vector<uint> _coloring_type;
  _coloring_type.push_back(topology().dim());
  _coloring_type.push_back(dim);
  _coloring_type.push_back(topology().dim());

  return color(_coloring_type);
}
//-----------------------------------------------------------------------------
const std::vector<dolfin::uint>& Mesh::color(std::vector<uint> coloring_type) const
{
  // Find color data
  std::map<const std::vector<uint>, std::pair<std::vector<uint>,
           std::vector<std::vector<uint> > > >::const_iterator coloring_data;
  coloring_data = this->topology().coloring.find(coloring_type);

  if (coloring_data != this->topology().coloring.end())
  {
    dolfin_debug("Mesh has already been colored, not coloring again.");
    return coloring_data->second.first;
  }

  // We do the same const-cast trick here as in the init() functions
  // since we are not really changing the mesh, just attaching some
  // auxiliary data to it.
  Mesh* _mesh = const_cast<Mesh*>(this);
  return MeshColoring::color(*_mesh, coloring_type);
}
//-----------------------------------------------------------------------------
void Mesh::intersected_cells(const Point& point,
                             std::set<uint>& cells) const
{
  // CGAL needs mesh with more than 1 cell
  if (num_cells() > 1)
    _intersection_operator.all_intersected_entities(point, cells);
  else
  {
    // Num cells == 1
    const Cell cell(*this, 0);
    if (cell.intersects(point))
      cells.insert(0);
  }
}
//-----------------------------------------------------------------------------
void Mesh::intersected_cells(const std::vector<Point>& points,
                             std::set<uint>& cells) const
{
  // CGAL needs mesh with more than 1 cell
  if (num_cells() > 1)
    _intersection_operator.all_intersected_entities(points, cells);
  else
  {
    // Num cells == 1
    const Cell cell(*this, 0);
    for (std::vector<Point>::const_iterator p = points.begin(); p != points.end(); ++p)
    {
      if (cell.intersects(*p))
	cells.insert(0);
    }
  }
}
//-----------------------------------------------------------------------------
void Mesh::intersected_cells(const MeshEntity & entity,
                             std::vector<uint>& cells) const
{
  // CGAL needs mesh with more than 1 cell
  if (num_cells() > 1)
    _intersection_operator.all_intersected_entities(entity, cells);
  else
  {
    // Num cells == 1
    const Cell cell(*this, 0);
    if (cell.intersects(entity))
      cells.push_back(0);
  }
}
//-----------------------------------------------------------------------------
void Mesh::intersected_cells(const std::vector<MeshEntity>& entities,
                             std::set<uint>& cells) const
{
  // CGAL needs mesh with more than 1 cell
  if (num_cells() > 1)
    _intersection_operator.all_intersected_entities(entities, cells);
  else
  {
    // Num cells == 1
    const Cell cell(*this, 0);
    for (std::vector<MeshEntity>::const_iterator entity = entities.begin();
	 entity != entities.end(); ++entity)
    {
      if (cell.intersects(*entity))
	cells.insert(0);
    }
  }
}
//-----------------------------------------------------------------------------
void Mesh::intersected_cells(const Mesh& another_mesh,
                             std::set<uint>& cells) const
{
  _intersection_operator.all_intersected_entities(another_mesh, cells);
}
//-----------------------------------------------------------------------------
int Mesh::intersected_cell(const Point& point) const
{
  // CGAL needs mesh with more than 1 cell
  if (num_cells() > 1)
    return  _intersection_operator.any_intersected_entity(point);

  // Num cells == 1
  const Cell cell(*this, 0);
  return cell.intersects(point) ? 0 : -1;
}
//-----------------------------------------------------------------------------
Point Mesh::closest_point(const Point& point) const
{
  return _intersection_operator.closest_point(point);
}
//-----------------------------------------------------------------------------
dolfin::uint Mesh::closest_cell(const Point & point) const
{
  // CGAL exits with an assertion error whilst performing
  // the closest cell query if num_cells() == 1
  if (num_cells() > 1)
    return _intersection_operator.closest_cell(point);

  // Num cells == 1
  return 0;
}
//-----------------------------------------------------------------------------
std::pair<Point,dolfin::uint>
Mesh::closest_point_and_cell(const Point & point) const
{
  return _intersection_operator.closest_point_and_cell(point);
}
//-----------------------------------------------------------------------------
double Mesh::distance(const Point& point) const
{
  return _intersection_operator.distance(point);
}
//-----------------------------------------------------------------------------
IntersectionOperator& Mesh::intersection_operator()
{
  return _intersection_operator;
}
//-----------------------------------------------------------------------------
const IntersectionOperator& Mesh::intersection_operator() const
{
  return _intersection_operator;
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
std::size_t Mesh::hash() const
{
  // Compute hash based on the Cantor pairing function
  const std::size_t k1 = _topology.hash();
  const std::size_t k2 = _geometry.hash();
  return (k1 + k2)*(k1 + k2 + 1)/2 + k2;
}
//-----------------------------------------------------------------------------
std::string Mesh::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    s << indent(_geometry.str(true));
    s << indent(_topology.str(true));
    s << indent(_data.str(true));
  }
  else
  {
    std::string cell_type("undefined cell type");
    if (_cell_type)
      cell_type = _cell_type->description(true);
    s << "<Mesh of topological dimension "
      << topology().dim() << " ("
      << cell_type << ") with "
      << num_vertices() << " vertices and "
      << num_cells() << " cells, "
      << (_ordered ? "ordered" : "unordered") << ">";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
void Mesh::add_periodic_direction(const SubDomain& sub_domain)
{
  add_facet_pairs(reference_to_no_delete_pointer(sub_domain));
}

void Mesh::add_periodic_direction(boost::shared_ptr<const SubDomain> sub_domain)
{
  add_facet_pairs(sub_domain);
}

// Comparison operator for hashing coordinates. Note that two
// coordinates are considered equal if equal to within round-off.
struct lt_coordinate
{
  bool operator() (const std::vector<double>& x, const std::vector<double>& y) const
  {
    unsigned int n = std::max(x.size(), y.size());
    for (unsigned int i = 0; i < n; ++i)
    {
      double xx = 0.0;
      double yy = 0.0;

      if (i < x.size())
        xx = x[i];
      if (i < y.size())
        yy = y[i];

      if (xx < (yy - 1.0e-12))
        return true;
      else if (xx > (yy + 1.0e-12))
        return false;
    }

    return false;
  }
};
typedef std::pair<int, int> facet_data;
typedef std::pair<facet_data, facet_data> facet_pair;
typedef std::map<std::vector<double>, facet_pair, lt_coordinate> coordinate_map;
typedef coordinate_map::iterator coordinate_iterator;
struct merge_coordinate_map
{
  coordinate_map operator() (coordinate_map x, coordinate_map y)
  {
    coordinate_map z;
    for (coordinate_iterator it = x.begin(); it != x.end(); ++it)
      z[it->first] = it->second;

    for (coordinate_iterator it = y.begin(); it != y.end(); ++it)
    {
      coordinate_iterator match = z.find(it->first);
      if (match != z.end())
      {
        // Copy the degree of freedom indices and their parallel owners
        match->second.first.first
          = std::max(it->second.first.first, match->second.first.first);
        match->second.first.second
          = std::max(it->second.first.second, match->second.first.second);
        match->second.second.first
          = std::max(it->second.second.first, match->second.second.first);
        match->second.second.second
          = std::max(it->second.second.second, match->second.second.second);
      }
      else
        z[it->first] = it->second;
    }

    return z;
  }
};

void Mesh::add_facet_pairs(boost::shared_ptr<const SubDomain> sub_domain)
{
  Timer t0("Mesh compute facet pairs");
  
  const uint tdim = topology().dim();
  const uint gdim = geometry().dim();
  
  // Arrays used for mapping coordinates
  std::vector<double> x(gdim);
  std::vector<double> y(gdim);

  // Wrap x and y (Array view of x and y)
  Array<double> _x(gdim, &x[0]);
  Array<double> _y(gdim, &y[0]);
  Point facet_midpoint;
  
  // Initialize mesh facets
  init(tdim - 1, tdim);
  
  // MPI process number
  const int process_number = MPI::process_number();
  
  coordinate_map coordinate_facet_pairs;
  
  Progress p("Finding periodic face pairs", size(tdim - 1));
  for (FacetIterator facet(*this); !facet.end(); ++facet)
  {
    if (!facet->exterior())
    {
      p++;
      continue;
    }
    facet_midpoint = facet->midpoint();
    for (uint i=0; i<gdim; i++)
      x[i] = facet_midpoint[i];

    sub_domain->map(_x, _y);
    if (sub_domain->inside(_x, true))
    {
      coordinate_iterator it = coordinate_facet_pairs.find(x);
      if (it != coordinate_facet_pairs.end())
      {
        it->second.first = facet_data(facet->index(), process_number);
      }
      else
      {
        facet_data g_facet(facet->index(), process_number);        
        facet_data l_facet(-1, -1);
        facet_pair pair(g_facet, l_facet);
        coordinate_facet_pairs[x] = pair;
      }
    }
    else if(sub_domain->inside(_y, true))
    {
      coordinate_iterator it = coordinate_facet_pairs.find(y);
      if (it != coordinate_facet_pairs.end())
      {
        it->second.second = facet_data(facet->index(), process_number);
      }
      else
      {
        facet_data l_facet(facet->index(), process_number);        
        facet_data g_facet(-1, -1);
        facet_pair pair(g_facet, l_facet);
        coordinate_facet_pairs[y] = pair;
      }      
    }
  }
    
  #ifdef HAS_MPI
  coordinate_map final_coordinate_facet_pairs
      = MPI::all_reduce(coordinate_facet_pairs, merge_coordinate_map());
  #else
  coordinate_map final_coordinate_facet_pairs = coordinate_facet_pairs;
  #endif
  
  for (coordinate_iterator it = final_coordinate_facet_pairs.begin();
                           it != final_coordinate_facet_pairs.end(); ++it)
  {
    facet_pair pair = it->second;
    _facet_pairs.push_back(pair);
  }
    
//   cout << "Facet pairs " << _facet_pairs.size() << endl;
//   for (uint i=0; i<_facet_pairs.size(); i++)
//   {
//     facet_pair pair = _facet_pairs[i];
//     cout << " (" << pair.first.first << ", " << pair.first.second << ")"
//               ", (" << pair.second.first << ", " << pair.second.second << ")" << endl;
//   }  
}
//-----------------------------------------------------------------------------
std::vector<std::pair< std::pair<uint, uint>, std::pair<uint, uint> > > 
  Mesh::get_facet_pairs() const
{
  return _facet_pairs;
}
//-----------------------------------------------------------------------------
bool Mesh::is_periodic() const
{
  return !_facet_pairs.empty();
}
//-----------------------------------------------------------------------------
