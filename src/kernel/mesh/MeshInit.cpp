// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Mesh.h>
#include <dolfin/Node.h>
#include <dolfin/Edge.h>
#include <dolfin/GenericCell.h>
#include <dolfin/MeshInit.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void MeshInit::init(Mesh& mesh)
{
  // Write a message
  dolfin_start("Computing mesh connectivity:");
  cout << "Found " << mesh.noNodes() << " nodes" << endl;
  cout << "Found " << mesh.noCells() << " cells" << endl;
  
  // Reset all previous connections
  clear(mesh);

  // Compute connectivity
  initConnectivity(mesh);

  // Renumber all objects
  renumber(mesh);

  dolfin_end();
}
//-----------------------------------------------------------------------------
void MeshInit::init(Mesh& mesh, uint nx, uint ny)
{

  dolfin_error("in preparation...");

  // Assuming mesh has been cleared

  /*

  // Create nodes
  for (uint iy = 0; iy <= ny; iy++)
  {
    const real y = static_cast<real>(iy) / ny;
    for (uint ix = 0; ix <= nx; ix++)
    {
      const real x = static_cast<real>(ix) / nx;
      const Point p(x, y);
      mesh->createNode(p);
    }
  }

  // Create triangles
  for (uint iy = 0; iy < ny; iy++)
  {
    for (uint ix = 0; ix < nx; ix++)
    {
      const uint n0 = iy*(n+1) + ix;
      const uint n1 = n0 + 1;
      const uint n2 = n0 + n + 1;
      const uint n3 = n1 + n + 1;

      createCell(n0, n1, n3);
      createCell(n0, n3, n2);
    }
  }

  // Compute connectivity
  init(mesh);

  */
}
//-----------------------------------------------------------------------------
void MeshInit::init(Mesh& mesh, uint nx, uint ny, uint nz)
{
  
  dolfin_error("in preparation...");

  // Assuming mesh has been cleared
  
  /*

  // Create nodes
  for (uint iz = 0; iz <= nz; iz++)
  {
    const real z = static_cast<real>(iz) / nz;
    for (uint iy = 0; iy <= ny; iy++)
    {
      const real y = static_cast<real>(iy) / ny;
      for (uint ix = 0; ix <= nx; ix++)
      {
	const real x = static_cast<real>(ix) / nx;
	const Point p(x, y, z);
	mesh->createNode(p);
      }
    }
  }

  // Create tetrahedrons
  for (uint iz = 0; iz < nz; iz++)
  {
    for (uint iy = 0; iy < ny; iy++)
    {
      for (uint ix = 0; ix < nx; ix++)
      {
	const uint n0 = iz*(n+1)*(n+1) + iy*(n+1) + ix;
	const uint n1 = n0 + 1;
	const uint n2 = n0 + n + 1;
	const uint n3 = n1 + n + 1;
	const uint n4 = n0 + (n+1)*(n+1);
	const uint n5 = n1 + (n+1)*(n+1);
	const uint n6 = n2 + (n+1)*(n+1);
	const uint n7 = n3 + (n+1)*(n+1);

	createCell(n0, n1, n3, n5);
	
      }
    }
  }

  // Compute connectivity
  init(mesh);

  */
}
//-----------------------------------------------------------------------------
void MeshInit::renumber(Mesh& mesh)
{
  // Renumber nodes
  int i = 0;
  for (NodeIterator n(mesh); !n.end(); ++n)
    n->setID(i++, mesh);

  // Renumber cells
  i = 0;
  for (CellIterator c(mesh); !c.end(); ++c)
    c->setID(i++, mesh);
  
  // Renumber edges
  i = 0;
  for (EdgeIterator e(mesh); !e.end(); ++e)
    e->setID(i++, mesh);

  // Renumber faces
  i = 0;
  for (FaceIterator f(mesh); !f.end(); ++f)
    f->setID(i++, mesh);
}
//-----------------------------------------------------------------------------
void MeshInit::clear(Mesh& mesh)
{  
  // Clear edges
  mesh.md->edges.clear();

  // Clear faces
  mesh.md->faces.clear();

  // Clear boundary data for mesh
  mesh.bd->clear();

  // Clear connectivity for nodes
  for (NodeIterator n(mesh); !n.end(); ++n) {
    n->nn.clear();
    n->nc.clear();
    n->ne.clear();
  }
  
  // Clear connectivity for cells (c-n is not touched)
  for (CellIterator c(mesh); !c.end(); ++c) {
    c->c->cc.clear();
    c->c->ce.clear();
    c->c->cf.clear();
  }
  
  // Clear connectivity and boundaryids for edges 
  for (EdgeIterator e(mesh); !e.end(); ++e) {
    e->ec.clear();
    e->ebids.clear();
  }
  // Clear connectivity and boundaryids for faces 
  for (FaceIterator f(mesh); !f.end(); ++f) {
    f->fc.clear();
    f->fbids.clear(); 
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initConnectivity(Mesh& mesh)
{
  // The data needs to be computed in the correct order, see MeshData.h.

  // Compute n-c connections [1]
  initNodeCell(mesh);

  // Compute c-c connections [2]
  initCellCell(mesh);

  // Compute edges [3]
  initEdges(mesh);

  // Compute n-e connections [4]
  initNodeEdge(mesh);

  // Compute n-n connections [5]
  initNodeNode(mesh);

  // Compute faces [6]
  initFaces(mesh);

  // Compute e-c connections [7]
  initEdgeCell(mesh);

  // Compute f-c connections [8]
  initFaceCell(mesh);

  // Compute the boundaryids for edge, ebids [9]
  initEdgeBoundaryids(mesh);

  // Compute the boundaryids for face, fbids [11]
  initFaceBoundaryids(mesh);  

}  
//-----------------------------------------------------------------------------
void MeshInit::initEdges(Mesh& mesh)
{
  // Go through all cells and create edges. Each edge checks with its
  // cell neighbors to see if an edge has already been added.

  for (CellIterator c(mesh); !c.end(); ++c)
    c->createEdges();
  
  // Write a message
  cout << "Created " << mesh.noEdges() << " edges" << endl;
}
//-----------------------------------------------------------------------------
void MeshInit::initFaces(Mesh& mesh)
{
  // Go through all cells and add new faces

  // Go through all cells for a mesh of tetrahedrons
  for (CellIterator c(mesh); !c.end(); ++c)
    c->createFaces();

  // Write a message
  cout << "Created " << mesh.noFaces() << " faces" << endl;
}
//-----------------------------------------------------------------------------
void MeshInit::initNodeCell(Mesh& mesh)
{
  // Go through all cells and add each cell as a neighbour to all its nodes.
  // This is done in three steps:
  //
  //   1. Count the number of cell neighbors for each node
  //   2. Allocate memory for each node
  //   3. Add the cell neighbors

  // Count the number of cells the node appears in
  for (CellIterator c(mesh); !c.end(); ++c)
    for (NodeIterator n(c); !n.end(); ++n)
      n->nc.setsize(n->nc.size()+1);

  // Allocate memory for the cell lists
  for (NodeIterator n(mesh); !n.end(); ++n)
    n->nc.init();

  // Add the cells to the cell lists
  for (CellIterator c(mesh); !c.end(); ++c){
    for (NodeIterator n(c); !n.end(); ++n)
      n->nc.add(c);
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initCellCell(Mesh& mesh)
{
  // Go through all cells and count the cell neighbors.
  // This is done in four steps:
  //
  //   1. Count the number of cell neighbors for each cell
  //   2. Allocate memory for each cell (overestimate)
  //   3. Add the cell neighbors
  //   4. Reallocate

  for (CellIterator c1(mesh); !c1.end(); ++c1) {

    // Count the number of cell neighbors (overestimate)
    for (NodeIterator n(c1); !n.end(); ++n)
      for (CellIterator c2(n); !c2.end(); ++c2)
	if ( c1->neighbor(*c2) )
	  c1->c->cc.setsize(c1->c->cc.size()+1);

    // Allocate memory
    c1->c->cc.init();
    
    // Add all *unique* cell neighbors
    for (NodeIterator n(c1); !n.end(); ++n)
      for (CellIterator c2(n); !c2.end(); ++c2)
	if ( c1->neighbor(*c2) )
	  if ( !c1->c->cc.contains(c2) )
	    c1->c->cc.add(c2);
    
    // Reallocate
    c1->c->cc.resize();
    
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initNodeEdge(Mesh& mesh)
{
  // Go through all edges and add each edge as a neighbour to all its nodes.
  // This is done in three steps:
  //
  //   1. Count the number of edge neighbors for each node
  //   2. Allocate memory for each node
  //   3. Add the edge neighbors

  // Count the number of edges the node appears in
  for (EdgeIterator e(mesh); !e.end(); ++e) {
    Node& n0 = e->node(0);
    Node& n1 = e->node(1);
    n0.ne.setsize(n0.ne.size() + 1);
    n1.ne.setsize(n1.ne.size() + 1);
  }
  
  // Allocate memory for the edge lists
  for (NodeIterator n(mesh); !n.end(); ++n)
    n->ne.init();
  
  // Add the edges to the edge lists
  for (EdgeIterator e(mesh); !e.end(); ++e) {
    Node& n0 = e->node(0);
    Node& n1 = e->node(1);
    n0.ne.add(e);
    n1.ne.add(e);
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initNodeNode(Mesh& mesh)
{
  // Go through all nodes and compute the node neighbors of each node
  // from the edge neighbors.

  for (NodeIterator n(mesh); !n.end(); ++n)
  {
    // Allocate the list of nodes
    n->nn.init(1 + n->ne.size());

    // First add the node itself
    n->nn(0) = n;
    
    // Then add the other nodes
    for (int i = 0; i < n->ne.size(); i++)
    {
      Edge* e = n->ne(i);

      if ( &e->node(0) != n )
	n->nn(i+1) = &e->node(0);
      else
	n->nn(i+1) = &e->node(1);
    }
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initEdgeCell(Mesh& mesh)
{
  // Go through all cells and add each cell as a neighbour to all its edges.
  // This is done in three steps:
  //
  //   1. Count the number of cell neighbors for each edge  
  //   2. Allocate memory for each edge
  //   3. Add the cell neighbors
  
  // Count the number of cells the edge appears in
  for (CellIterator c(mesh); !c.end(); ++c)
    for (EdgeIterator e(c); !e.end(); ++e)
      e->ec.setsize(e->ec.size()+1);
  
  // Allocate memory for the cell lists
  for (EdgeIterator e(mesh); !e.end(); ++e)
    e->ec.init();

  // Add the cells to the cell lists
  for (CellIterator c(mesh); !c.end(); ++c)
    for (EdgeIterator e(c); !e.end(); ++e)
      e->ec.add(c);
}
//-----------------------------------------------------------------------------
void MeshInit::initFaceCell(Mesh& mesh)
{
  // Go through all cells and add each cell as a neighbour to all its faces.
  // This is done in three steps:
  //
  //   1. Count the number of cell neighbors for each face 
  //      (should be 2 for interior faces and 1 for boundary faces) 
  //   2. Allocate memory for each face
  //   3. Add the cell neighbors
  
  // Count the number of cells the face appears in
  for (CellIterator c(mesh); !c.end(); ++c)
    for (FaceIterator f(c); !f.end(); ++f)
      f->fc.setsize(f->fc.size()+1);
  
  // Allocate memory for the cell lists
  for (FaceIterator f(mesh); !f.end(); ++f)
    f->fc.init();

  // Add the cells to the cell lists
  for (CellIterator c(mesh); !c.end(); ++c)
    for (FaceIterator f(c); !f.end(); ++f)
      f->fc.add(c);
}
//-----------------------------------------------------------------------------
void MeshInit::initEdgeBoundaryids(Mesh& mesh)
{
  for (EdgeIterator e(mesh); !e.end(); ++e){
    // Intersect boundaryids in node 0 and 1, put the result in e->ebids
    set_intersection( e->node(0).nbids.begin(), e->node(0).nbids.end(),
		      e->node(1).nbids.begin(), e->node(1).nbids.end(),
		      inserter(e->ebids,e->ebids.begin()) );
  }
}
//-----------------------------------------------------------------------------
void MeshInit::initFaceBoundaryids(Mesh& mesh)
{
  std::set <int> tmp;
  
  // Faces are only relevant in 3D
  if ( mesh.type() == Mesh::tetrahedrons )
    for (FaceIterator f(mesh); !f.end(); ++f){
      tmp.clear();
      // Intersect boundaryids in edge 0 and 1, put the result in tmp
      set_intersection( f->edge(0).ebids.begin(), f->edge(0).ebids.end(),
			f->edge(1).ebids.begin(), f->edge(1).ebids.end(),
			inserter(tmp,tmp.begin()) );
      // Intersect boundaryids in edge 2 and tmp, put the result in f->fbids
      set_intersection( f->edge(2).ebids.begin(), f->edge(2).ebids.end(),
			tmp.begin(), tmp.end(),
			inserter(f->fbids,f->fbids.begin()) );
    }

}
//-----------------------------------------------------------------------------

