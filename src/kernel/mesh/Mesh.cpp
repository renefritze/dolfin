// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Johan Hoffman 2007.
//
// First added:  2006-05-09
// Last changed: 2007-02-06

#include <dolfin/File.h>
#include <dolfin/UniformMeshRefinement.h>
#include <dolfin/LocalMeshRefinement.h>
#include <dolfin/LocalMeshCoarsening.h>
#include <dolfin/TopologyComputation.h>
#include <dolfin/MeshFunction.h>
#include <dolfin/Mesh.h>
#include <dolfin/BoundaryMesh.h>
#include <dolfin/Cell.h>
#include <dolfin/Vertex.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Mesh::Mesh() : Variable("mesh", "DOLFIN mesh")
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Mesh::Mesh(const Mesh& mesh) : Variable("mesh", "DOLFIN mesh")
{
  *this = mesh;
}
//-----------------------------------------------------------------------------
Mesh::Mesh(std::string filename) : Variable("mesh", "DOLFIN mesh")
{
  File file(filename);
  file >> *this;
}
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
const Mesh& Mesh::operator=(const Mesh& mesh)
{
  data = mesh.data;
  rename(mesh.name(), mesh.label());
  return *this;
}
//-----------------------------------------------------------------------------
dolfin::uint Mesh::init(uint dim)
{
  return TopologyComputation::computeEntities(*this, dim);
}
//-----------------------------------------------------------------------------
void Mesh::init(uint d0, uint d1)
{
  TopologyComputation::computeConnectivity(*this, d0, d1);
}
//-----------------------------------------------------------------------------
void Mesh::init()
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
void Mesh::disp() const
{
  data.disp();
}
//-----------------------------------------------------------------------------
void Mesh::refine()
{
  dolfin_info("No cells marked for refinement, assuming uniform mesh refinement.");
  UniformMeshRefinement::refine(*this);
}
//-----------------------------------------------------------------------------
void Mesh::refine(MeshFunction<bool>& cell_marker, bool refine_boundary)
{
  LocalMeshRefinement::refineMeshByEdgeBisection(*this,cell_marker,
                                                 refine_boundary);
}
//-----------------------------------------------------------------------------
void Mesh::coarsen()
{
  dolfin_info("No cells marked for coarsening, assuming uniform mesh coarsening.");
  MeshFunction<bool> cell_marker(*this);
  cell_marker.init(this->topology().dim());
  for (CellIterator c(*this); !c.end(); ++c)
    cell_marker.set(c->index(),true);

  LocalMeshCoarsening::coarsenMeshByEdgeCollapse(*this,cell_marker);
}
//-----------------------------------------------------------------------------
void Mesh::coarsen(MeshFunction<bool>& cell_marker, bool coarsen_boundary)
{
  LocalMeshCoarsening::coarsenMeshByEdgeCollapse(*this,cell_marker,
                                                 coarsen_boundary);
}
//-----------------------------------------------------------------------------
void Mesh::smooth() 
{
  MeshFunction<bool> bnd_vertex(*this); 
  bnd_vertex.init(0); 
  for (VertexIterator v(*this); !v.end(); ++v)
   bnd_vertex.set(v->index(),false);

  MeshFunction<uint> bnd_vertex_map; 
  MeshFunction<uint> bnd_cell_map; 
  BoundaryMesh boundary(*this,bnd_vertex_map,bnd_cell_map);

  for (VertexIterator v(boundary); !v.end(); ++v)
    bnd_vertex.set(bnd_vertex_map.get(v->index()),true);

  Point midpoint = 0.0; 
  uint num_neighbors = 0;
  for (VertexIterator v(*this); !v.end(); ++v)
  {
    if ( !bnd_vertex.get(v->index()) )
    {
      midpoint = 0.0;
      num_neighbors = 0;
      for (VertexIterator vn(*v); !vn.end(); ++vn)
      {
        if ( v->index() != vn->index() )
        {
          midpoint += vn->point();
          num_neighbors++;
        }
      }
      midpoint /= real(num_neighbors);

      for (uint sd=0; sd<this->geometry().dim();sd++)
        this->geometry().set(v->index(),sd,midpoint[sd]);

    }
  }
  
}
//-----------------------------------------------------------------------------
dolfin::LogStream& dolfin::operator<< (LogStream& stream, const Mesh& mesh)
{
  stream << "[ Mesh of topological dimension "
	 << mesh.topology().dim()
	 << " with "
	 << mesh.numVertices()
	 << " vertices and "
	 << mesh.numCells()
	 << " cells ]";
  
  return stream;
}
//-----------------------------------------------------------------------------
