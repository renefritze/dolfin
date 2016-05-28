// Copyright (C) 2013-2016 Anders Logg
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
// Modified by August Johansson 2015
//
// First added:  2013-08-05
// Last changed: 2016-05-28

#include <cmath>
#include <dolfin/log/log.h>
#include <dolfin/plot/plot.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/geometry/BoundingBoxTree.h>
#include <dolfin/geometry/SimplexQuadrature.h>
#include <dolfin/geometry/IntersectionTriangulation.h>
#include "Cell.h"
#include "Facet.h"
#include "BoundaryMesh.h"
#include "MeshFunction.h"
#include "MultiMesh.h"

// FIXME August
//#define Augustcheckqrpositive
#define Augustdebug
//#define Augustnormaldebug

#ifdef Augustdebug
#include <dolfin/geometry/dolfin_simplex_tools.h>
#endif

using namespace dolfin;

//-----------------------------------------------------------------------------
MultiMesh::MultiMesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MultiMesh::MultiMesh(std::vector<std::shared_ptr<const Mesh>> meshes,
                     std::size_t quadrature_order)
{
  // Add and build
  for (auto mesh : meshes)
    add(mesh);
  build(quadrature_order);
}
//-----------------------------------------------------------------------------
MultiMesh::MultiMesh(std::shared_ptr<const Mesh> mesh_0,
                     std::size_t quadrature_order)
{
  // Add and build
  add(mesh_0);
  build(quadrature_order);
}
//-----------------------------------------------------------------------------
MultiMesh::MultiMesh(std::shared_ptr<const Mesh> mesh_0,
                     std::shared_ptr<const Mesh> mesh_1,
                     std::size_t quadrature_order)
{
  // Add and build
  add(mesh_0);
  add(mesh_1);
  build(quadrature_order);
}
//-----------------------------------------------------------------------------
MultiMesh::MultiMesh(std::shared_ptr<const Mesh> mesh_0,
                     std::shared_ptr<const Mesh> mesh_1,
                     std::shared_ptr<const Mesh> mesh_2,
                     std::size_t quadrature_order)
{
  // Add and build
  add(mesh_0);
  add(mesh_1);
  add(mesh_2);
  build(quadrature_order);
}
//-----------------------------------------------------------------------------
MultiMesh::~MultiMesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::size_t MultiMesh::num_parts() const
{
  return _meshes.size();
}
//-----------------------------------------------------------------------------
std::shared_ptr<const Mesh> MultiMesh::part(std::size_t i) const
{
  dolfin_assert(i < _meshes.size());
  return _meshes[i];
}
//-----------------------------------------------------------------------------
const std::vector<unsigned int>&
MultiMesh::uncut_cells(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _uncut_cells[part];
}
//-----------------------------------------------------------------------------
const std::vector<unsigned int>&
MultiMesh::cut_cells(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _cut_cells[part];
}
//-----------------------------------------------------------------------------
const std::vector<unsigned int>&
MultiMesh::covered_cells(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _covered_cells[part];
}
//-----------------------------------------------------------------------------
const std::map<unsigned int,
               std::vector<std::pair<std::size_t, unsigned int>>>&
  MultiMesh::collision_map_cut_cells(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _collision_maps_cut_cells[part];
}
//-----------------------------------------------------------------------------
const std::map<unsigned int, quadrature_rule> &
MultiMesh::quadrature_rule_cut_cells(std::size_t part) const

{
  dolfin_assert(part < num_parts());
  return _quadrature_rules_cut_cells[part];
}
//-----------------------------------------------------------------------------
quadrature_rule
MultiMesh::quadrature_rule_cut_cell(std::size_t part,
                                    unsigned int cell_index) const
{
  auto q = quadrature_rule_cut_cells(part);
  return q[cell_index];
}
//-----------------------------------------------------------------------------
const std::map<unsigned int, std::vector<quadrature_rule>>&
  MultiMesh::quadrature_rule_overlap(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _quadrature_rules_overlap[part];
}
//-----------------------------------------------------------------------------
const std::map<unsigned int, std::vector<quadrature_rule>>&
  MultiMesh::quadrature_rule_interface(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _quadrature_rules_interface[part];
}
//-----------------------------------------------------------------------------
const std::map<unsigned int, std::vector<std::vector<double>>>&
  MultiMesh::facet_normals(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _facet_normals[part];
}
//-----------------------------------------------------------------------------
std::shared_ptr<const BoundingBoxTree>
MultiMesh::bounding_box_tree(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _trees[part];
}
//-----------------------------------------------------------------------------
std::shared_ptr<const BoundingBoxTree>
MultiMesh::bounding_box_tree_boundary(std::size_t part) const
{
  dolfin_assert(part < num_parts());
  return _boundary_trees[part];
}
//-----------------------------------------------------------------------------
void MultiMesh::add(std::shared_ptr<const Mesh> mesh)
{
  _meshes.push_back(mesh);
  log(PROGRESS, "Added mesh to multimesh; multimesh has %d part(s).",
      _meshes.size());
}
//-----------------------------------------------------------------------------
void MultiMesh::build(std::size_t quadrature_order)
{
  begin(PROGRESS, "Building multimesh.");

  // Build boundary meshes
  _build_boundary_meshes();

  // Build bounding box trees
  _build_bounding_box_trees();

  // Build collision maps
  _build_collision_maps();

  // FIXME: For collisions with meshes of same type we get three types
  // of quadrature rules: the cut cell qr, qr of the overlap part and
  // qr of the interface.

  // Build quadrature rules of the cut cells' overlap. Do this before
  // we build the quadrature rules of the cut cells
  _build_quadrature_rules_overlap(quadrature_order);

  // Build quadrature rules of the cut cells
  _build_quadrature_rules_cut_cells(quadrature_order);

  // Build quadrature rules and normals of the interface
  _build_quadrature_rules_interface(quadrature_order);

  end();
}
//-----------------------------------------------------------------------------
void MultiMesh::clear()
{
  _boundary_meshes.clear();
  _trees.clear();
  _boundary_trees.clear();
  _uncut_cells.clear();
  _cut_cells.clear();
  _covered_cells.clear();
  _collision_maps_cut_cells.clear();
  _collision_maps_cut_cells_boundary.clear();
  _quadrature_rules_cut_cells.clear();
  _quadrature_rules_overlap.clear();
  _quadrature_rules_interface.clear();
}
//-----------------------------------------------------------------------------
std::string MultiMesh::plot_matplotlib(double delta_z) const
{
  dolfin_assert(num_parts() > 0);
  dolfin_assert(part(0)->geometry().dim() == 2);

  std::stringstream ss;

  ss << "def plot_multimesh() :\n";
  ss << "    from mpl_toolkits.mplot3d import Axes3D\n";
  ss << "    from matplotlib import cm\n";
  ss << "    import matplotlib.pyplot as plt\n";
  ss << "    import numpy as np\n";
  ss << "    fig = plt.figure()\n";
  ss << "    ax = fig.gca(projection='3d')\n";

  for (std::size_t p = 0; p < num_parts(); p++)
  {
    std::shared_ptr<const Mesh> current = part(p);
    std::stringstream x, y;
    x << "    x = np.array((";
    y << "    y = np.array((";
    for (std::size_t i = 0; i < current->num_vertices(); i++)
    {
      x << current->coordinates()[i*2] << ", ";
      y << current->coordinates()[i*2 + 1] << ",";
    }
    x << "))\n";
    y << "))\n";
    ss << x.str() << y.str();

    ss << "    facets = np.array((";
    for (CellIterator cit(*current); !cit.end(); ++cit)
    {
      const unsigned int* vertices = cit->entities(0);
      ss << "(" << vertices[0] << ", " << vertices[1] << ", " << vertices[2] << "), ";
    }

    ss << "), dtype=int)\n";
    ss << "    z = np.zeros(x.shape) + " << (p*delta_z) << "\n";
    ss << "    ax.plot_trisurf(x, y, z, triangles=facets, alpha=.4)\n";
  }
  ss << "    plt.show()\n";
  return ss.str();
}
//-----------------------------------------------------------------------------
void MultiMesh::_build_boundary_meshes()
{
  begin(PROGRESS, "Building boundary meshes.");

  // Clear boundary meshes
  _boundary_meshes.clear();

  // Build boundary mesh for each part
  for (std::size_t i = 0; i < num_parts(); i++)
  {
    std::shared_ptr<BoundaryMesh>
      boundary_mesh(new BoundaryMesh(*_meshes[i], "exterior"));
    _boundary_meshes.push_back(boundary_mesh);
  }

  end();
}
//-----------------------------------------------------------------------------
void MultiMesh::_build_bounding_box_trees()
{
  begin(PROGRESS, "Building bounding box trees for all meshes.");

  // Clear bounding box trees
  _trees.clear();
  _boundary_trees.clear();

  // Build trees for each part
  for (std::size_t i = 0; i < num_parts(); i++)
  {
    // Build tree for mesh
    std::shared_ptr<BoundingBoxTree> tree(new BoundingBoxTree());
    tree->build(*_meshes[i]);
    _trees.push_back(tree);

    // Build tree for boundary mesh
    std::shared_ptr<BoundingBoxTree> boundary_tree(new BoundingBoxTree());

    // FIXME: what if the boundary mesh is empty?
    dolfin_assert(_boundary_meshes[i]->num_vertices()>0);
    if (_boundary_meshes[i]->num_vertices()>0)
      boundary_tree->build(*_boundary_meshes[i]);
    _boundary_trees.push_back(boundary_tree);
  }

  end();
}
//-----------------------------------------------------------------------------
void MultiMesh::_build_collision_maps()
{
  begin(PROGRESS, "Building collision maps.");

  // Clear collision maps
  _uncut_cells.clear();
  _cut_cells.clear();
  _covered_cells.clear();
  _collision_maps_cut_cells.clear();
  _collision_maps_cut_cells_boundary.clear();

  // Iterate over all parts
  for (std::size_t i = 0; i < num_parts(); i++)
  {
    // Extract uncut, cut and covered cells:
    //
    // 0: uncut   = cell not colliding with any higher domain
    // 1: cut     = cell colliding with some higher boundary and is not covered
    // 2: covered = cell colliding with some higher domain but not its boundary

    // Create vector of markers for cells in part `i` (0, 1, or 2)
    std::vector<char> markers(_meshes[i]->num_cells(), 0);

    // Create local arrays for marking domain and boundary collisions
    // for cells in part `i`. Note that in contrast to the markers
    // above which are global to part `i`, these markers are local to
    // the collision between part `i` and part `j`.
    std::vector<bool> collides_with_boundary(_meshes[i]->num_cells());
    std::vector<bool> collides_with_domain(_meshes[i]->num_cells());

    // Create empty collision map for cut cells in part `i`
    std::map<unsigned int, std::vector<std::pair<std::size_t, unsigned int>>>
      collision_map_cut_cells;

    // Iterate over covering parts (with higher part number)
    for (std::size_t j = i + 1; j < num_parts(); j++)
    {
      log(PROGRESS, "Computing collisions for mesh %d overlapped by mesh %d.", i, j);

      // Compute domain-boundary collisions
      const auto& boundary_collisions = _trees[i]->compute_collisions(*_boundary_trees[j]);

      // Reset boundary collision markers
      std::fill(collides_with_boundary.begin(), collides_with_boundary.end(), false);

      // Iterate over boundary collisions.
      for (std::size_t k = 0; k < boundary_collisions.first.size(); ++k)
      {
	// Get the colliding cell
	const std::size_t cell_i = boundary_collisions.first[k];

	// Do a careful check if not already marked as colliding
	if (!collides_with_boundary[cell_i])
	{
	  const Cell cell(*_meshes[i], cell_i);
	  const Cell boundary_cell(*_boundary_meshes[j], boundary_collisions.second[k]);
	  collides_with_boundary[cell_i] = cell.collides(boundary_cell);
	}

	// Mark as cut cell if not previously covered
	if (collides_with_boundary[cell_i] and markers[cell_i] != 2)
	{
	  // Mark as cut cell
	  markers[cell_i] = 1;

	  // Add empty list of collisions into map if it does not exist
	  if (collision_map_cut_cells.find(cell_i) == collision_map_cut_cells.end())
	  {
            std::vector<std::pair<std::size_t, unsigned int>> collisions;
            collision_map_cut_cells[cell_i] = collisions;
          }
        }
      }

      // Compute domain-domain collisions
      const auto& domain_collisions = _trees[i]->compute_collisions(*_trees[j]);

      // Reset domain collision markers
      std::fill(collides_with_domain.begin(), collides_with_domain.end(), false);

      // Iterate over domain collisions
      dolfin_assert(domain_collisions.first.size() == domain_collisions.second.size());
      for (std::size_t k = 0; k < domain_collisions.first.size(); k++)
      {
        // Get the two colliding cells
        const std::size_t cell_i = domain_collisions.first[k];
	const std::size_t cell_j = domain_collisions.second[k];

        // Store collision in collision map if we have a cut cell
        if (markers[cell_i] == 1)
        {
	  const Cell cell(*_meshes[i], cell_i);
	  const Cell other_cell(*_meshes[j], cell_j);
	  if (cell.collides(other_cell))
	  {
	    collides_with_domain[cell_i] = true;
	    auto it = collision_map_cut_cells.find(cell_i);
	    dolfin_assert(it != collision_map_cut_cells.end());
	    it->second.push_back(std::make_pair(j, cell_j));
	  }
        }

        // Possibility to cell as covered if it does not collide with boundary
        if (!collides_with_boundary[cell_i])
        {
	  // Detailed check if it is not marked as colliding with domain
	  if (!collides_with_domain[cell_i])
	  {
	    const Cell cell(*_meshes[i], cell_i);
	    const Cell other_cell(*_meshes[j], cell_j);
	    collides_with_domain[cell_i] = cell.collides(other_cell);
	  }

	  if (collides_with_domain[cell_i])
	  {
	    // Remove from collision map if previously marked as as cut cell
	    if (markers[cell_i] == 1)
	    {
	      dolfin_assert(collision_map_cut_cells.find(cell_i) != collision_map_cut_cells.end());
	      collision_map_cut_cells.erase(cell_i);
	    }

	    // Mark as covered cell (may already be marked)
	    markers[cell_i] = 2;
	  }

        }
      }
    }

    // Extract uncut, cut and covered cells from markers
    std::vector<unsigned int> uncut_cells;
    std::vector<unsigned int> cut_cells;
    std::vector<unsigned int> covered_cells;
    for (unsigned int c = 0; c < _meshes[i]->num_cells(); c++)
    {
      switch (markers[c])
      {
      case 0:
        uncut_cells.push_back(c);
        break;
      case 1:
        cut_cells.push_back(c);
        break;
      default:
        covered_cells.push_back(c);
      }
    }

    // Store data for this mesh
    _uncut_cells.push_back(uncut_cells);
    _cut_cells.push_back(cut_cells);
    _covered_cells.push_back(covered_cells);
    _collision_maps_cut_cells.push_back(collision_map_cut_cells);

    // Report results
    log(PROGRESS, "Part %d has %d uncut cells, %d cut cells, and %d covered cells.",
        i, uncut_cells.size(), cut_cells.size(), covered_cells.size());
  }

#ifdef Augustdebug
  //   for (std::size_t part = 0; part < num_parts(); ++part)
  //   {
  //     std::vector<std::size_t> marker(_meshes[part]->num_cells(),-1);
  //     for (const auto c: _uncut_cells[part]) marker[c] = 0;
  //     for (const auto c: _cut_cells[part]) marker[c] = 1;
  //     for (const auto c: _covered_cells[part]) marker[c] = 2;
  //     tools::dolfin_write_medit_triangles("markers",*_meshes[part],3*(_counter++)+part,&marker);
  //   }
  //   tools::dolfin_write_medit_triangles("multimesh",*this);
#endif

  end();
}
//-----------------------------------------------------------------------------
void MultiMesh::_build_quadrature_rules_overlap(std::size_t quadrature_order)
{
  begin(PROGRESS, "Building quadrature rules of cut cells' overlap.");
#ifdef Augustdebug
  std::cout << __FUNCTION__ << std::endl;
#endif

  // Clear quadrature rules
  _quadrature_rules_overlap.clear();

  // Resize quadrature rules
  _quadrature_rules_overlap.resize(num_parts());

#ifdef Augustdebug
  std::cout.precision(15);
  for (std::size_t cut_part = 0; cut_part < num_parts(); cut_part++)
  {
    std::cout << "cut part " << cut_part << std::endl;
    // Iterate over cut cells for current part
    const auto& cmap = collision_map_cut_cells(cut_part);
    for (auto it = cmap.begin(); it != cmap.end(); ++it)
    {
      const std::vector<std::string> color = {{ "'b'", "'g'", "'r'", "'m'", "'y'" }};

      // Get cut cell
      const unsigned int cut_cell_index = it->first;
      const Cell cut_cell(*(_meshes[cut_part]), cut_cell_index);
      std::cout << tools::drawtriangle(cut_cell, color[cut_part]);

      // Loop over all cutting cells to construct the polyhedra to be
      // used in the inclusion-exclusion principle
      for (auto jt = it->second.begin(); jt != it->second.end(); jt++)
      {
  	// Get cutting part and cutting cell
	const std::size_t cutting_part = jt->first;
	const std::size_t cutting_cell_index = jt->second;
	const Cell cutting_cell(*(_meshes[cutting_part]), cutting_cell_index);
  	std::cout << tools::drawtriangle(cutting_cell, color[cutting_part]);
      }
      std::cout << tools::zoom()<<std::endl;
    }
  }
  PPause;
#endif

  // Iterate over all parts
  for (std::size_t cut_part = 0; cut_part < num_parts(); cut_part++)
  {
    std::cout << "----- cut part: " << cut_part << std::endl;

    // Iterate over cut cells for current part
    const auto& cmap = collision_map_cut_cells(cut_part);
    for (auto it = cmap.begin(); it != cmap.end(); ++it)
    {
#ifdef Augustdebug
      std::cout << "-------- new cut cell\n";
#endif

      // Get cut cell
      const unsigned int cut_cell_index = it->first;
      const Cell cut_cell(*(_meshes[cut_part]), cut_cell_index);

      // Get dimensions
      const std::size_t tdim = cut_cell.mesh().topology().dim();
      const std::size_t gdim = cut_cell.mesh().geometry().dim();
#ifdef Augustdebug
      std::cout << "tdim = " << tdim << std::endl
		<< "gdim = " << gdim << std::endl;
#endif

      // Data structure for the first intersections (this is the first
      // stage in the inclusion exclusion principle). These are the
      // polyhedra to be used in the exlusion inclusion.
      std::vector<std::pair<std::size_t, Polyhedron> > initial_polyhedra;

      // Loop over all cutting cells to construct the polyhedra to be
      // used in the inclusion-exclusion principle
      for (auto jt = it->second.begin(); jt != it->second.end(); jt++)
      {
	// Get cutting part and cutting cell
        const std::size_t cutting_part = jt->first;
        const std::size_t cutting_cell_index = jt->second;
        const Cell cutting_cell(*(_meshes[cutting_part]), cutting_cell_index);
#ifdef Augustdebug
	{
	  std::cout << "\ncut cutting (cut_cell="<<cut_cell_index<<" and cutting part="<<cutting_part<<" and cutting cell="<<cutting_cell_index<<")" << std::endl;

	  //(cutting part=" << cutting_part << ")" << std::endl;
	  std::cout << tools::drawtriangle(cut_cell,"'y'")<<tools::drawtriangle(cutting_cell,"'m'")<<tools::zoom()<<std::endl;
	}
#endif

  	// Only allow same type of cell for now
      	dolfin_assert(cutting_cell.mesh().topology().dim() == tdim);
      	dolfin_assert(cutting_cell.mesh().geometry().dim() == gdim);

  	// Compute the intersection (a polyhedron)
  	const Polyhedron polyhedron = IntersectionTriangulation::triangulate(cut_cell, cutting_cell);

	// FIXME: Flip triangles in polyhedron to maximize minimum angle here?
	// FIXME: only include large polyhedra

	// Store key and polyhedron but only use polyhedron of same tdim
	Polyhedron polyhedron_same_tdim;
	for (const Simplex& s: polyhedron)
	  if (s.size() == tdim + 1)
	    polyhedron_same_tdim.push_back(s);
#ifdef Augustdebug
	{
	  std::cout << "% intersection same tdim (size="<<polyhedron_same_tdim.size()<<")\n";
	  if (polyhedron_same_tdim.size())
	  {
	    for (const auto simplex: polyhedron_same_tdim)
	    {
	      dolfin_assert(!IntersectionTriangulation::is_degenerate(simplex));
	      std::cout << "sub simplex size "<<simplex.size() << " at line "<<__LINE__<<std::endl;
	      std::cout << tools::drawtriangle(simplex,"'k'");
	    }
	    std::cout <<tools::zoom()<< std::endl;
	  }
	}
#endif

	if (polyhedron_same_tdim.size())
	  initial_polyhedra.push_back(std::make_pair(initial_polyhedra.size(),
						     polyhedron_same_tdim));
      }

      // Data structure for the overlap quadrature rule
      std::vector<quadrature_rule> overlap_qr;

      _inclusion_exclusion(overlap_qr, initial_polyhedra,
			   tdim, gdim, quadrature_order, 1.);

#ifdef Augustdebug
      // std::cout << "\n summarize all intersections for part=" << cut_part<< " (there are "<<all_intersections.size()<< " stages)" << std::endl;

      // for (std::size_t stage = 0; stage < all_intersections.size(); ++stage)
      // {
      // 	std::cout << "\nstage " << stage << " has " << all_intersections[stage].size() << " polyhedra: " << std::endl
      // 		  << "figure("<<stage+1<<"),title('stage="<<stage<<"'),axis equal,hold on;\n";
      // 	for (const auto polyhedron: all_intersections[stage])
      // 	{
      // 	  for (const auto simplex: polyhedron.second)
      // 	    std::cout << tools::drawtriangle(simplex);

      // 	  // for (const auto key: polyhedron.first)
      // 	  //   std::cout << key <<' ';
      // 	  // std::cout << "    ";
      // 	}
      // }
      // std::cout << std::endl;

      // // qr is pair of point and weight (each is a vector<double>)
      // for (const auto qr: overlap_qr)
      // 	for (std::size_t i = 0; i < qr.second.size(); ++i)
      // 	{
      // 	  const Point pt(qr.first[gdim*i],qr.first[gdim*i+1]);
      // 	  if (qr.second[i] < 0)
      // 	    std::cout << tools::matlabplot(pt,"'o'");
      // 	  else
      // 	    std::cout << tools::matlabplot(pt,"'.'");
      // 	  std::cout << std::endl;
      // 	}

      // PPause;
#endif

#ifdef Augustcheckqrpositive
      // Check qr overlap
      double net_weight = 0;
      for (const auto qro: overlap_qr)
	net_weight += std::accumulate(qro.second.begin(),
				      qro.second.end(), 0.);
      if (net_weight - cut_cell.volume() > DOLFIN_EPS)
      {
	std::cout<< __FUNCTION__  << ": cut part " << cut_part<<" cell " << cut_cell_index << " net weight = " << net_weight << " area = " << cut_cell.volume() << ") "<<std::endl;
	for (const auto qro: overlap_qr)
	{
	  std::cout << "weights: ";
	  for (const auto w: qro.second)
	    std::cout << w << ' ';
	  std::cout << std::endl;
	}
	std::cout <<"weight and volume " << std::setprecision(15)<< net_weight <<' '<<cut_cell.volume() <<' '<<net_weight-cut_cell.volume()<< std::endl;


	// how to fix?
	//qr = quadrature_rule();
	PPause;
	//std::cout << "exiting";
	//exit(1);
      }

#endif

      // Store quadrature rules for cut cell
      _quadrature_rules_overlap[cut_part][cut_cell_index] = overlap_qr;
    }

#ifdef Augustdebug
    // {
    //   // sum
    //   double part_volume = 0;
    //   double volume = 0;

    //   // Uncut cell volume given by function volume
    //   const auto uncut_cells = this->uncut_cells(cut_part);
    //   for (auto it = uncut_cells.begin(); it != uncut_cells.end(); ++it)
    //   {
    // 	const Cell cell(*part(cut_part), *it);
    // 	volume += cell.volume();
    // 	part_volume += cell.volume();
    //   }

    //   // Cut cell volume given by quadrature rule
    //   const auto& cut_cells = this->cut_cells(cut_part);
    //   for (auto it = cut_cells.begin(); it != cut_cells.end(); ++it)
    //   {
    // 	//const auto& qr = quadrature_rule_cut_cell(cut_part, *it);
    // 	const auto& overlap_qr = _quadrature_rules_overlap[cut_part][*it];
    // 	for (const auto qr: overlap_qr)
    // 	  for (std::size_t i = 0; i < qr.second.size(); ++i)
    // 	  {
    // 	    volume += qr.second[i];
    // 	    part_volume += qr.second[i];
    // 	  }
    //   }

    //   //std::cout<<" volumes "<<std::setprecision(16) << volume <<' ' << part_volume << ' ' <<(areapos-areaminus)<<std::endl;

    //   // std::cout << "area" << cut_part << " = " << std::setprecision(15)<<areapos << " - " << areaminus << ",   1-("<<areapos<<"-"<<areaminus<<")"<<std::endl;

    //   double err=0;
    //   if (cut_part == num_parts()-1)
    // 	err = 0.36-part_volume;
    //   else
    // 	err = (areapos-areaminus)-part_volume;
    //   std::cout << "error " << cut_part << " " << err << std::endl;
    // }
#endif
  }

  end();
}
//-----------------------------------------------------------------------------
void MultiMesh::_build_quadrature_rules_cut_cells(std::size_t quadrature_order)
{
  begin(PROGRESS, "Building quadrature rules of cut cells.");

  // Clear quadrature rules
  _quadrature_rules_cut_cells.clear();
  _quadrature_rules_cut_cells.resize(num_parts());

  // Iterate over all parts
  for (std::size_t cut_part = 0; cut_part < num_parts(); cut_part++)
  {
    // Iterate over cut cells for current part
    const auto& cmap = collision_map_cut_cells(cut_part);
    for (auto it = cmap.begin(); it != cmap.end(); ++it)
    {
      // Get cut cell
      const unsigned int cut_cell_index = it->first;
      const Cell cut_cell(*(_meshes[cut_part]), cut_cell_index);

      // Get dimension
      const std::size_t gdim = cut_cell.mesh().geometry().dim();

      // Compute quadrature rule for the cell itself.
      auto qr = SimplexQuadrature::compute_quadrature_rule(cut_cell, quadrature_order);

      // Get the quadrature rule for the overlapping part
      const auto& qr_overlap = _quadrature_rules_overlap[cut_part][cut_cell_index];

      // Add the quadrature rule for the overlapping part to the
      // quadrature rule of the cut cell with flipped sign
      for (std::size_t k = 0; k < qr_overlap.size(); k++)
        _add_quadrature_rule(qr, qr_overlap[k], gdim, -1);

#ifdef Augustcheckqrpositive
      // Check qr overlap
      double overlap_weight = 0;
      for (const auto qro: qr_overlap)
	overlap_weight += std::accumulate(qro.second.begin(),
					  qro.second.end(), 0.);

      // Check positivity
      double net_weight = std::accumulate(qr.second.begin(),
					  qr.second.end(), 0.);
      if (net_weight < 0)
      {
	std::cout<< __FUNCTION__  << ": cut part " << cut_part<<" cell " << cut_cell_index << " net weight = " << net_weight << " (overlap weight = " << overlap_weight << " area = " << cut_cell.volume() << ") "<<std::endl;

	// how to fix?
	//qr = quadrature_rule();
      }

#endif

      // Store quadrature rule for cut cell
      _quadrature_rules_cut_cells[cut_part][cut_cell_index] = qr;
    }
  }

  end();
}
//------------------------------------------------------------------------------
void MultiMesh::_build_quadrature_rules_interface(std::size_t quadrature_order)
{
  begin(PROGRESS, "Building quadrature rules of interface.");

  // This is similar to _build_quadrature_rules_overlap, except
  // - For the edge E_ij, i<j, we only intersect with triangles T_k
  //   where k>i and k!=j
  // - We note that the sign change is opposite of the other inc exc:
  //   |E_ij \ U_k T_k| = |E_ij| - |E_ij \cap U_k T_k|
  //                    = |E_ij| - |U_k E_ij \cap T_k|

  // Clear quadrature rules
  _quadrature_rules_interface.clear();

  // Resize quadrature rules
  _quadrature_rules_interface.resize(num_parts());

  // First we prebuild a map from the boundary facets to full mesh
  // cells for all meshes: Loop over all boundary mesh facets to find
  // the full mesh cell which contains the facet. This is done in two
  // steps: Since the facet is on the boundary mesh, we first map this
  // facet to a facet in the full mesh using the
  // boundary_cell_map. Then we use the full_facet_cell_map to find
  // the corresponding cell in the full mesh. This cell is to match
  // the cutting_cell_no.

  // Build map from boundary facets to full mesh
  std::vector<std::vector<std::vector<std::pair<std::size_t, std::size_t>>>> full_to_bdry(num_parts());
  for (std::size_t part = 0; part < num_parts(); ++part)
  {
    full_to_bdry[part].resize(_meshes[part]->num_cells());

    // Get map from boundary mesh to facets of full mesh
    const std::size_t tdim_boundary = _boundary_meshes[part]->topology().dim();
    const auto& boundary_cell_map = _boundary_meshes[part]->entity_map(tdim_boundary);

    // Generate facet to cell connectivity for full mesh
    const std::size_t tdim = _meshes[part]->topology().dim();
    _meshes[part]->init(tdim_boundary, tdim);
    const MeshConnectivity& full_facet_cell_map = _meshes[part]->topology()(tdim_boundary, tdim);

    for (std::size_t boundary_facet = 0; boundary_facet < boundary_cell_map.size(); ++boundary_facet)
    {
      // Find the facet in the full mesh
      const std::size_t full_mesh_facet = boundary_cell_map[boundary_facet];
      // Find the cells in the full mesh (for interior facets we
      // can have 2 facets, but here we should only have 1)
      dolfin_assert(full_facet_cell_map.size(full_mesh_facet) == 1);
      const auto& full_cells = full_facet_cell_map(full_mesh_facet);
      // full_to_bdry[part][full_cells[0]].push_back(std::make_pair(boundary_facet,
      // 								 full_mesh_facet));
      full_to_bdry[part][full_cells[0]].emplace_back(boundary_facet, full_mesh_facet);
    }
  }

#ifdef August_cut_cutting_bdry_debug
  // test
  for (std::size_t cut_part = 0; cut_part < num_parts(); cut_part++)
  {
    const std::map<unsigned int,
                   std::vector<std::pair<std::size_t,
                                         unsigned int>>>&
      cmap = collision_map_cut_cells(cut_part);
    for (const auto cut_i: cmap)
    {
      const std::size_t cut_cell_index_i = cut_i.first;
      const Cell cut_cell_i(*(_meshes[cut_part]), cut_cell_index_i);
      std::cout << tools::drawtriangle(cut_cell_i)<<" % cut cell"<<std::endl;
      for (const std::pair<size_t, unsigned int>& cutting_j : cut_i.second)
      {
	// Get cutting part and cutting cell
        const std::size_t cutting_part_j = cutting_j.first;
        const std::size_t cutting_cell_index_j = cutting_j.second;
        const Cell cutting_cell_j(*(_meshes[cutting_part_j]), cutting_cell_index_j);
	std::cout << tools::drawtriangle(cutting_cell_j) << " % cutting_cell_j " << std::endl;
        for (const auto boundary_cell_index_j: full_to_bdry[cutting_part_j][cutting_cell_index_j])
        {
          const Cell boundary_cell_j(*_boundary_meshes[cutting_part_j], boundary_cell_index_j.first);
	  std::cout << tools::drawtriangle(boundary_cell_j) << " % boundary cell" << std::endl;
	}
      }
    }
  }
  PPause;
#endif


  // Iterate over all parts
  for (std::size_t cut_part = 0; cut_part < num_parts(); cut_part++)
  {
    std::cout << "----- cut part: " << cut_part << std::endl;

    // Iterate over cut cells for current part
    const std::map<unsigned int,
                   std::vector<std::pair<std::size_t,
                                         unsigned int>>>&
      cmap = collision_map_cut_cells(cut_part);
    for (const auto cut_i: cmap)
    {
      // Get cut cell
      const std::size_t cut_cell_index_i = cut_i.first;
      const Cell cut_cell_i(*(_meshes[cut_part]), cut_cell_index_i);

      // Get dimensions
      const std::size_t tdim = cut_cell_i.mesh().topology().dim();
      const std::size_t gdim = cut_cell_i.mesh().geometry().dim();
#ifdef Augustdebug
      std::cout << "=====================\ncut cell i=" << cut_cell_index_i << " from part="<<cut_part
		<<" tdim="<<tdim<<" gdim="<<gdim<<std::endl
		<< ' '<<tools::drawtriangle(cut_cell_i)<<std::endl;
#endif

      // Data structure for the interface quadrature rule
      std::vector<quadrature_rule> interface_qr;

      // Loop over all cutting cells to construct the polyhedra to be
      // used in the inclusion-exclusion principle
      for (const std::pair<size_t, unsigned int>& cutting_j : cut_i.second)
      {
	// Get cutting part and cutting cell
        const std::size_t cutting_part_j = cutting_j.first;
        const std::size_t cutting_cell_index_j = cutting_j.second;
        const Cell cutting_cell_j(*(_meshes[cutting_part_j]), cutting_cell_index_j);
	dolfin_assert(cutting_part_j > cut_part);
#ifdef Augustdebug
	std::cout << "\ncutting cell j " << cutting_cell_index_j<<" from part j="<<cutting_part_j<<std::endl
		  <<' '<<tools::drawtriangle(cutting_cell_j) << std::endl;
#endif

	// Iterate over boundary cells of this cutting cell (for
	// triangles we have one or two sides that cut). Here we can
	// change to create a full E_ij used in the E_ij \cap T_k, or
	// we can only take a part of the Eij

	// Net qr for the Eij
	quadrature_rule Eij_qr;

        for (const auto boundary_cell_index_j: full_to_bdry[cutting_part_j][cutting_cell_index_j])
        {
          // Get the boundary facet as a cell in the boundary mesh
          // (remember that this is of one less topological dimension)
          const Cell boundary_cell_j(*_boundary_meshes[cutting_part_j],
				     boundary_cell_index_j.first);
	  dolfin_assert(boundary_cell_j.mesh().topology().dim() == tdim - 1);
#ifdef Augustdebug
	  std::cout << "boundary cell j " << boundary_cell_index_j.first << std::endl
		    << ' '<<tools::drawtriangle(boundary_cell_j) << std::endl;
#endif

	  // Data structure for the first intersections (this is the first
	  // stage in the inclusion exclusion principle). These are the
	  // polygons to be used in the exlusion inclusion, i.e. E_ij \cap
	  // T_k.
	  std::vector<std::pair<std::size_t, Polyhedron> > initial_polygons;

	  // Triangulate intersection of cut cell and boundary cell
	  const Polyhedron Eij_part = IntersectionTriangulation::triangulate(cut_cell_i,
									     boundary_cell_j);
	  dolfin_assert(Eij_part.size() <= 1); // either empty or one simplex?

	  if (Eij_part.size())
	  {
	    // test: don't include if point, or if line is degenerate (here and below)
	    if (Eij_part[0].size() == tdim and
		!IntersectionTriangulation::is_degenerate(Eij_part[0]))
	    {

#ifdef Augustdebug
	      std::cout << "The intersection of the following cut_cell_i and the boundary of the cutting cell j give the Eij_part\n"
			<< tools::drawtriangle(cut_cell_i)<<tools::drawtriangle(boundary_cell_j)<<std::endl;
	      std::cout << "the intersection is Eij_part:\n";
	      for (const auto s: Eij_part) {
		std::cout << tools::drawtriangle(s) << " % simplex.size="<<s.size()<<' '<<__LINE__<<std::endl;
		//dolfin_assert(!IntersectionTriangulation::is_degenerate(s));
	      }
#endif
	      // Need to store qr here as std::vector, but is flattened later
	      std::vector<quadrature_rule> Eij_part_qr;

	      // Store qr for |Eij| (or rather Eij_part) since inc exc
	      // only for Eij \cap T_k (this is pre stage 0)
	      for (const Simplex& s: Eij_part)
		_add_quadrature_rule(Eij_qr, s, gdim, quadrature_order, 1.);

	      for (const std::pair<size_t, unsigned int>& cutting_k: cut_i.second)
	      {
		const std::size_t cutting_part_k = cutting_k.first;
		if (//cutting_part_k > cut_part and // this is always true
		    cutting_part_k != cutting_part_j) // ignore cells in same part
		{
		  const std::size_t cutting_cell_index_k = cutting_k.second;
		  const Cell cutting_cell_k(*(_meshes[cutting_part_k]), cutting_cell_index_k);
#ifdef Augustdebug
		  std::cout << "\ncutting cell k " << cutting_cell_index_k << " from part k " << cutting_part_k << std::endl;
#endif
		  // Find E_ij_part \cap T_k. This is the initial_polygon
		  const Polyhedron Eij_part_cap_Tk = IntersectionTriangulation::triangulate(cutting_cell_k,
											    Eij_part);
		  if (Eij_part_cap_Tk.size())
		  {
		    // test: don't include if point, or if line is degenerate (here and below)
		    if (Eij_part_cap_Tk[0].size() == tdim and
			!IntersectionTriangulation::is_degenerate(Eij_part_cap_Tk[0]))
		    {
#ifdef Augustdebug
		      std::cout << "The cutting cell k and E_ij part is:\n";
		      std::cout << ' '<<tools::drawtriangle(cutting_cell_k)<<"   ";
		      for (const auto s: Eij_part) {
			dolfin_assert(!IntersectionTriangulation::is_degenerate(s));
			std::cout << tools::drawtriangle(s);
		      }
		      std::cout << std::endl;
		      std::cout << "the intersection of these is the Eij_part_cap_Tk (size="<<Eij_part_cap_Tk.size()<<"):"<<std::endl;
		      for (const auto s: Eij_part_cap_Tk) {
			dolfin_assert(!IntersectionTriangulation::is_degenerate(s));
			std::cout << tools::drawtriangle(s) << " % simplex.size="<<s.size()<<' '<<__LINE__<<std::endl;
		      }
#endif
		      // Store key and polygon
		      initial_polygons.push_back(std::make_pair(initial_polygons.size(),
								Eij_part_cap_Tk));
		    }
		    else {
#ifdef Augustdebug
		      std::cout << " not of size tdim or degenerate"<<std::endl;
#endif
		    }
		  }
		  else {
#ifdef Augustdebug
		    std::cout << " empty intersection Eij_part_cap_Tk"<<std::endl;
#endif
		  }
		}
	      } // end cutting_k

#ifdef Augustdebug
	      std::cout << "initial_polygons before calling inc exc (size="<<initial_polygons.size()<<std::endl;
	      for (const auto p: initial_polygons) {
		std::cout << "key: " << p.first<<std::endl;
		std::cout << "polyhedron: ";
		for (const auto s: p.second)
		  std::cout << tools::drawtriangle(s);
		std::cout << std::endl;
	      }
#endif

	      if (initial_polygons.size())
	      {
		// Call inclusion exclusion
		_inclusion_exclusion(Eij_part_qr, initial_polygons,
				     tdim - 1, gdim, quadrature_order, -1.);
	      }
	      else {
#ifdef Augustdebug
		std::cout << "   no inc exc"<<std::endl;
#endif
	      }

	      // Flatten the qr
	      for (auto qr: Eij_part_qr)
	      {
		Eij_qr.first.insert(Eij_qr.first.end(), qr.first.begin(), qr.first.end());
		Eij_qr.second.insert(Eij_qr.second.end(), qr.second.begin(), qr.second.end());
	      }
#ifdef Augustdebug
	      std::cout << "net qr (Eij_qr)"<<std::endl;
	      tools::cout_qr(Eij_qr);
	      std::cout << "net qr area = " << tools::area(Eij_qr)<<std::endl;
#endif
	    }
	  }
	  else {
#ifdef Augustdebug
	    std::cout << " no intersection Eij_part"<<std::endl;
#endif
	  }
	} // end boundary_cell_j loop

	// Save the qr for this Eij (push_back makes order correct)
	interface_qr.push_back(Eij_qr);
#ifdef Augustdebug
	std::cout << "push back this net qr (Eij_qr)"<<std::endl;
	std::cout << "interface_qr is now\n";
	double area = 0;
	for (const auto qr: interface_qr)
	{
	  tools::cout_qr(qr);
	  area += tools::area(qr);
	}
	std::cout << " area=" << area << std::endl;
#endif


      } // end loop over cutting_j

#ifdef Augustdebug
      std::cout << "  summarize cut part " << cut_part << " cut_cell_index_i " << cut_cell_index_i << std::endl;
      double area = 0;
      for (const auto qr: interface_qr)
      {
	tools::cout_qr(qr);
	area += tools::area(qr);
      }
      std::cout << " area=" << area << std::endl;
#endif

      _quadrature_rules_interface[cut_part][cut_cell_index_i] = interface_qr;
    } // end loop over cut_i
  } // end loop over parts

}

//------------------------------------------------------------------------------
std::size_t
MultiMesh::_add_quadrature_rule(quadrature_rule& qr,
                                const Simplex& simplex,
                                std::size_t gdim,
                                std::size_t quadrature_order,
                                double factor) const
{
#ifdef Augustdebug
  std::cout << __FUNCTION__<<" simplex size " << simplex.size() << std::endl;
#endif

  // Compute quadrature rule for simplex
  const auto dqr = SimplexQuadrature::compute_quadrature_rule(simplex,
							      gdim,
							      quadrature_order);
  // Add quadrature rule
  const std::size_t num_points = _add_quadrature_rule(qr, dqr, gdim, factor);

  return num_points;
}

//-----------------------------------------------------------------------------
std::size_t MultiMesh::_add_quadrature_rule(quadrature_rule& qr,
                                            const quadrature_rule& dqr,
                                            std::size_t gdim,
                                            double factor) const
{
  // Get the number of points
  dolfin_assert(dqr.first.size() == gdim*dqr.second.size());
  const std::size_t num_points = dqr.second.size();

  // Skip if sum of weights is too small
  double wsum = 0.0;
  for (std::size_t i = 0; i < num_points; i++)
    wsum += std::abs(dqr.second[i]);
  if (wsum < DOLFIN_EPS)
    return 0;

  // Append points and weights
  for (std::size_t i = 0; i < num_points; i++)
  {
    // Add point
    for (std::size_t j = 0; j < gdim; j++)
      qr.first.push_back(dqr.first[i*gdim + j]);

    // Add weight
    qr.second.push_back(factor*dqr.second[i]);
  }

#ifdef Augustdebug
  std::cout << "% display quadrature rule w factor = "<<factor<<" (last " << num_points << " added):"<< std::endl;
  for (std::size_t i = 0; i < qr.second.size(); ++i)
  {
    std::cout << "plot(" << qr.first[2*i]<<","<<qr.first[2*i+1]<<",'ro') % "<<qr.second[i]<<' ';
    if (i >= (qr.second.size() - num_points))
      std::cout << "(new)";
    std::cout << std::endl;
    //std::cout  << dqr.first[2*i]<<' '<<dqr.first[2*i+1]<< std::endl;
  }
  std::cout << "_add_quadrature_rule net area " << tools::area(qr) << std::endl;
#endif

  return num_points;
}
//-----------------------------------------------------------------------------
void MultiMesh::_add_normal(std::vector<double>& normals,
			    const Point& normal,
			    std::size_t npts,
			    std::size_t gdim) const
{
  for (std::size_t i = 0; i < npts; ++i)
    for (std::size_t j = 0; j < gdim; ++j)
      normals.push_back(normal[j]);
#ifdef Augustdebug
  tools::cout_normals(normals);
#endif
}

//-----------------------------------------------------------------------------
void MultiMesh::_plot() const
{
  // Developer note: This function is implemented here rather than
  // in the plot library since it is too specialized to be implemented
  // there.

  std::cout << "Plotting multimesh with " << num_parts() << " parts" << std::endl;

  // Iterate over parts
  for (std::size_t p = 0; p < num_parts(); ++p)
  {
    // Create a cell function and mark cells
    std::shared_ptr<MeshFunction<std::size_t>>
      f(new MeshFunction<std::size_t>(part(p),
                                      part(p)->topology().dim()));

    // Set all entries to 0 (uncut cells)
    f->set_all(0);

    // Mark cut cells as 1
    for (auto it : cut_cells(p))
      f->set_value(it, 1);

    // Mart covered cells as 2
    for (auto it : covered_cells(p))
      f->set_value(it, 2);

    // Write some debug data
    const std::size_t num_cut = cut_cells(p).size();
    const std::size_t num_covered = covered_cells(p).size();
    const std::size_t num_uncut = part(p)->num_cells() - num_cut - num_covered;
    std::cout << "Part " << p << " has "
	      << num_uncut   << " uncut cells (0), "
	      << num_cut     << " cut cells (1), and "
	      << num_covered << " covered cells (2)." << std::endl;

    // Plot
    std::stringstream s;
    s << "Map of cell types for multimesh part " << p;
    dolfin::plot(f, s.str());
  }
}
//------------------------------------------------------------------------------
void MultiMesh::_inclusion_exclusion
(std::vector<quadrature_rule>& qr,
 const std::vector<std::pair<std::size_t, Polyhedron> >& initial_polyhedra,
 std::size_t tdim,
 std::size_t gdim,
 std::size_t quadrature_order,
 int signflip) const
{
  begin(PROGRESS, "The inclusion exclusion principle.");
#ifdef Augustdebug
  std::cout << std::endl<<__FUNCTION__<<" tdim="<<tdim<<" gdim="<<gdim
	    <<" quadrature_order="<<quadrature_order<<std::endl;
#endif

  // Exclusion-inclusion principle. There are N stages in the
  // principle, where N = polyhedra.size(). The first stage is
  // simply the polyhedra themselves A, B, C, ... etc. The second
  // stage is for the pairwise intersections A \cap B, A \cap C, B
  // \cap C, etc, with different sign. There are
  // n_choose_k(N,stage) intersections for each stage.

  // Data structure for storing the previous intersections: the key
  // and the intersections.
  const std::size_t N = initial_polyhedra.size();
  std::vector<std::pair<IncExcKey, Polyhedron> > previous_intersections(N);
  for (std::size_t i = 0; i < N; ++i)
    previous_intersections[i] = std::make_pair(IncExcKey(1, initial_polyhedra[i].first),
					       initial_polyhedra[i].second);

#ifdef Augustdebug
  std::cout << "---------------- stage 0: the inclusion inclusion is over the following:\n";
  for (std::size_t i = 0; i < N; ++i)
  {
    std::cout << "key "<<previous_intersections[i].first[0]<<":"<<std::endl;
    for (const auto s: previous_intersections[i].second)
      std::cout << tools::drawtriangle(s);
    std::cout << std::endl;
  }
#endif

  // Do stage = 1 up to stage = polyhedra.size in the
  // principle. Recall that stage 1 is the pairwise
  // intersections. There are up to n_choose_k(N,stage)
  // intersections in each stage (there may be less). The
  // intersections are found using the polyhedra data and the
  // previous_intersections data. We only have to intersect if the
  // key doesn't contain the polyhedron.

  // Add quadrature rule for stage 0
  quadrature_rule part_qr;
  for (const std::pair<IncExcKey, Polyhedron>& pol_pair: previous_intersections)
    for (const Simplex& simplex: pol_pair.second)
      if (simplex.size() == tdim + 1)
	_add_quadrature_rule(part_qr, simplex, gdim,
			     quadrature_order, 1.*signflip);

  // Add quadrature rule for overlap part
  qr.push_back(part_qr);

  for (std::size_t stage = 1; stage < N; ++stage)
  {
#ifdef Augustdebug
    std::cout << "----------------- stage " << stage << std::endl;
#endif

    // Structure for storing new intersections
    std::vector<std::pair<IncExcKey, Polyhedron> > new_intersections;

    // Loop over all intersections from the previous stage
    for (const std::pair<IncExcKey, Polyhedron>& previous_polyhedron: previous_intersections)
    {
      // Loop over all initial polyhedra.
      for (const std::pair<std::size_t, Polyhedron>& initial_polyhedron: initial_polyhedra)
      {

	// test: only check if initial_polyhedron key <
	// previous_polyhedron key[0]
	if (initial_polyhedron.first < previous_polyhedron.first[0])
	{
#ifdef Augustdebug
	  std::cout << "----------\nkeys previous_polyhedron: ";
	  for (const auto key: previous_polyhedron.first)
	    std::cout << key <<' ';
	  std::cout << "\nsimplices previous polyhedron:\n";
	  for (const auto s: previous_polyhedron.second)
	    std::cout << ' '<<tools::drawtriangle(s) << " % simplex.size()="<<s.size()<<' '<<__LINE__<<std::endl;
	  std::cout << "keys initial_polyhedron: " << initial_polyhedron.first <<std::endl;
	  std::cout << "simplices initial polyhedron\n";
	  for (const auto s: initial_polyhedron.second)
	    std::cout << ' '<<tools::drawtriangle(s,"'r'") <<" % simplex.size()="<<s.size()<<' '<<__LINE__<<std::endl;
	  std::cout << tools::zoom()<<std::endl;
#endif

	  // We want to save the intersection of the previous
	  // polyhedron and the initial polyhedron in one single
	  // polyhedron.
	  Polyhedron new_polyhedron;
	  IncExcKey new_keys;

	  // Loop over all simplices in the initial_polyhedron and
	  // the previous_polyhedron and append the intersection of
	  // these to the new_polyhedron
	  bool any_intersections = false;

	  for (const Simplex& previous_simplex: previous_polyhedron.second)
	  {
	    for (const Simplex& initial_simplex: initial_polyhedron.second)
	    {
	      // Compute the intersection (a polyhedron)
#ifdef Augustdebug
	      std::cout << "try intersect:\n"
			<< tools::drawtriangle(initial_simplex,"'r'")<<tools::drawtriangle(previous_simplex)<<tools::zoom()<<std::endl;
#endif
	      // To save all intersections as a single polyhedron,
	      // we don't call this a polyhedron yet, but rather a
	      // std::vector<Simplex> since we are still filling
	      // the polyhedron with simplices

	      // Only allow same types for now
	      if (previous_simplex.size() == tdim + 1 &&
		  initial_simplex.size() == tdim + 1)
	      {
		const std::vector<Simplex> ii = IntersectionTriangulation::triangulate(initial_simplex, previous_simplex, gdim);

		if (ii.size())
		{
#ifdef Augustdebug
		  dolfin_assert(tools::tdimcheck(ii));
#endif

		  // To save all intersections as a single
		  // polyhedron, we don't call this a polyhedron
		  // yet, but rather a std::vector<Simplex> since we
		  // are still filling the polyhedron with simplices

		  // FIXME: We could add only if area is suff large
		  for (const Simplex& simplex: ii)
		    if (simplex.size() == tdim + 1 and
			!IntersectionTriangulation::is_degenerate(simplex))
		    {
		      //dolfin_assert(!IntersectionTriangulation::is_degenerate(simplex));
		      new_polyhedron.push_back(simplex);
		      any_intersections = true;
		    }
#ifdef Augustdebug
		  {
		    std::cout << "we intersected these two:\n";
		    std::cout << tools::drawtriangle(previous_simplex,"'b'")
			      << tools::drawtriangle(initial_simplex,"'r'")<<tools::zoom()<<std::endl;
		    std::cout << "areas: " << tools::area(previous_simplex)<<' '<<tools::area(initial_simplex)<<std::endl;
		    const double min_area = std::min(tools::area(previous_simplex), tools::area(initial_simplex));

		    std::cout << "resulting intersection size= "<<ii.size()<<": \n";
		    for (const auto simplex: ii)
		    {
		      std::cout << "sub simplex size "<<simplex.size() << " at line " << __LINE__<<std::endl;
		      std::cout << tools::drawtriangle(simplex,"'g'");
		    }
		    std::cout<<tools::zoom()<<std::endl;
		    double intersection_area = 0;
		    std::cout << "areas=[ ";
		    for (const auto simplex: ii) {
		      intersection_area += tools::area(simplex);
		      std::cout << tools::area(simplex) <<' ';
		    }
		    std::cout<<"];\n";
		    if (intersection_area >= min_area) { std::cout << "Warning, intersection area ~ minimum area\n"; /*PPause;*/ }
		  }
#endif
		}
	      }
	    }
	  }

	  if (any_intersections)
	  {
	    new_keys.push_back(initial_polyhedron.first);
	    new_keys.insert(new_keys.end(),
			    previous_polyhedron.first.begin(),
			    previous_polyhedron.first.end());

	    // FIXME: Test improve quality
	    //maximize_minimum_angle(new_polyhedron);

	    // Save data
	    new_intersections.push_back(std::make_pair(new_keys, new_polyhedron));
	  }

	}
      }
    }

    // Update before next stage
    previous_intersections = new_intersections;

    // Add quadrature rule with correct sign
    const double sign = signflip*std::pow(-1, stage);
    quadrature_rule overlap_part_qr;

    for (const std::pair<IncExcKey, Polyhedron>& polyhedron: new_intersections)
      for (const Simplex& simplex: polyhedron.second)
	if (simplex.size() == tdim + 1)
	  _add_quadrature_rule(overlap_part_qr, simplex, gdim, quadrature_order, sign);

    // Add quadrature rule for overlap part
    qr.push_back(overlap_part_qr);

#ifdef Augustdebug
    std::cout << "\n summarize at stage="<<stage<<std::endl;
    std::cout << "sign = (-1)^stage = "<<sign<<std::endl;
    std::cout << "the previous intersections were:\n";
    for (const auto previous_polyhedron: previous_intersections)
    {
      for (const auto key: previous_polyhedron.first)
	std::cout << key<<' ';
      std::cout << "   ";
    }
    std::cout << std::endl;
    for (const auto previous_polyhedron: previous_intersections)
    {
      for (const auto simplex: previous_polyhedron.second)
	std::cout << tools::drawtriangle(simplex);
      std::cout << "    ";
    }
    std::cout << std::endl;


    std::cout << "the new intersections are:\n";
    for (const auto new_polyhedron: new_intersections)
    {
      for (const auto key: new_polyhedron.first)
	std::cout << key<<' ';
      std::cout << "   ";
    }
    std::cout << std::endl;
    for (const auto new_polyhedron: new_intersections)
    {
      for (const auto simplex: new_polyhedron.second)
	std::cout << tools::drawtriangle(simplex);
      std::cout <<"    ";
    }
    std::cout << std::endl;

    //if (cut_part == 1) { PPause; }
    //if (cut_part == 0) { PPause; }

    std::cout << "end summary"<<std::endl;
#endif
  } // end loop over stages

#ifdef Augustdebug
  std::cout << "inc exc summary\n";
  double area =0;
  for (const auto qr0: qr) {
    tools::cout_qr(qr0);
    area += tools::area(qr0);
  }
  std::cout << "net area inc exc " << area << std::endl;
  std::cout << "end inc exc"<<std::endl<<std::endl;
#endif



}
//------------------------------------------------------------------------------
