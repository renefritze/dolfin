// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Grid.h>
#include <dolfin/Edge.h>
#include <dolfin/Cell.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/ShortList.h>
#include <dolfin/RefineGrid.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void RefineGrid::refine()
{
  cout << "Refining grid: " << grid << endl;

  _create_edges = dolfin_get("create edges"); 

  GlobalRegularRefinement();
}
//-----------------------------------------------------------------------------

/*
Refinement::GlobalRefinement()
{
}

Refinement::EvaluateMarks()
{
}

Refinement::CloseGrid()
{
}

Refinement::CloseElement()
{
}

Refinement::UnrefineGrid()
{
}

Refinement::RefineGrid()
{
}
*/


void RefineGrid::GlobalRegularRefinement()
{
  // Regular refinement: 
  // (1) Triangles: 1 -> 4 
  // (2) Tetrahedrons: 1 -> 8 

  //  cout << "no elms = " << grid.noCells() << endl;
  //  cout << "no nodes = " << grid.noNodes() << endl;
  
  List<Cell *> cells;
  for (CellIterator c(grid); !c.end(); ++c)
    cells.add(c);
  
  for (List<Cell *>::Iterator c(&cells); !c.end(); ++c){
    RegularRefinement((*c.pointer()));
  }
  
  //  cout << "new no elms = " << grid.noCells() << endl;
  //  cout << "new no nodes = " << grid.noNodes() << endl;
}


void RefineGrid::RegularRefinement(Cell* parent)
{
  // Regular refinement: 
  // (1) Triangles: 1 -> 4 
  // (2) Tetrahedrons: 1 -> 8 

  switch (parent->type()) {
  case Cell::TETRAHEDRON: 
    IrrRef3(parent);
    //RegularRefinementTetrahedron(parent);
    break;
  case Cell::TRIANGLE: 
    RegularRefinementTriangle(parent);
    break;
  default: 
    dolfin_error("Cell type not implemented.");
    exit(1);
  }
}


void RefineGrid::RegularRefinementTetrahedron(Cell* parent)
{
  // Refine 1 tetrahedron into 8 new ones, introducing new nodes 
  // at the midpoints of the edges. 
  Node *n0 = grid.createNode(parent->level()+1,parent->node(0)->coord());
  Node *n1 = grid.createNode(parent->level()+1,parent->node(1)->coord());
  Node *n2 = grid.createNode(parent->level()+1,parent->node(2)->coord());
  Node *n3 = grid.createNode(parent->level()+1,parent->node(3)->coord());

  Node *n01 = grid.createNode(parent->level()+1,parent->node(0)->coord().midpoint(parent->node(1)->coord()));
  Node *n02 = grid.createNode(parent->level()+1,parent->node(0)->coord().midpoint(parent->node(2)->coord()));
  Node *n03 = grid.createNode(parent->level()+1,parent->node(0)->coord().midpoint(parent->node(3)->coord()));
  Node *n12 = grid.createNode(parent->level()+1,parent->node(1)->coord().midpoint(parent->node(2)->coord()));
  Node *n13 = grid.createNode(parent->level()+1,parent->node(1)->coord().midpoint(parent->node(3)->coord()));
  Node *n23 = grid.createNode(parent->level()+1,parent->node(2)->coord().midpoint(parent->node(3)->coord()));

  Cell *t1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n0, n01,n02,n03);
  Cell *t2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n01,n1, n12,n13);
  Cell *t3 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n02,n12,n2, n23);
  Cell *t4 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n03,n13,n23,n3 );
  Cell *t5 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n01,n02,n03,n13);
  Cell *t6 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n01,n02,n12,n13);
  Cell *t7 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n02,n03,n13,n23);
  Cell *t8 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n02,n12,n13,n23);

  if (_create_edges){
    grid.createEdges(t1);
    grid.createEdges(t2);
    grid.createEdges(t3);
    grid.createEdges(t4);
    grid.createEdges(t5);
    grid.createEdges(t6);
    grid.createEdges(t7);
    grid.createEdges(t8);
  }
}

void RefineGrid::RegularRefinementTriangle(Cell* parent)
{
  // Refine 1 triangle into 4 new ones, introducing new nodes 
  // at the midpoints of the edges. 
  Node *n0 = grid.createNode(parent->level()+1,parent->node(0)->coord());
  Node *n1 = grid.createNode(parent->level()+1,parent->node(1)->coord());
  Node *n2 = grid.createNode(parent->level()+1,parent->node(2)->coord());

  Node *n01 = grid.createNode(parent->level()+1,parent->node(0)->coord().midpoint(parent->node(1)->coord()));
  Node *n02 = grid.createNode(parent->level()+1,parent->node(0)->coord().midpoint(parent->node(2)->coord()));
  Node *n12 = grid.createNode(parent->level()+1,parent->node(1)->coord().midpoint(parent->node(2)->coord()));

  Cell *t1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n0, n01,n02);
  Cell *t2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n01,n1, n12);
  Cell *t3 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n02,n12,n2 );
  Cell *t4 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n01,n12,n02);

  if (_create_edges){
    grid.createEdges(t1);
    grid.createEdges(t2);
    grid.createEdges(t3);
    grid.createEdges(t4);
  }
}

void RefineGrid::LocalIrregularRefinement(Cell *parent)
{
  switch(parent->noMarkedEdges()){ 
  case 6: 
    RegularRefinementTetrahedron(parent);
    break;
  case 5: 
    RegularRefinementTetrahedron(parent);
    break;
  case 4: 
    RegularRefinementTetrahedron(parent);
    break;
  case 3: 
    if (parent->markedEdgesOnSameFace()) IrrRef1(parent);
    else RegularRefinementTetrahedron(parent);    
    break;
  case 2: 
    if (parent->markedEdgesOnSameFace()) IrrRef3(parent);
    else IrrRef4(parent);  
    break;
  case 1: 
    IrrRef2(parent);  
    break;
  default:
    dolfin_error("wrong number of marked edges");
  }
}


void RefineGrid::IrrRef1(Cell* parent)
{
  // 3 edges are marked on the same face: 
  // insert 3 new nodes at the midpoints on the marked edges, connect the 
  // new nodes to each other, as well as to the node that is not on the 
  // marked face. This gives 4 new tetrahedrons. 

  if (parent->noMarkedEdges() != 3) dolfin_error("wrong size of refinement edges");
  if (!parent->markedEdgesOnSameFace()) dolfin_error("marked edges not on the same face");
  
  //cout << "parent = " << parent->id() << endl;
  
  int marked_nodes[3];
  int marked_edges[3];
  marked_nodes[0] = marked_nodes[1] = marked_nodes[2] = -1;
  marked_edges[0] = marked_edges[1] = marked_edges[2] = -1;
  int cnt_1 = 0;
  int cnt_2 = 0;

  /*
  parent->markEdge(0);
  parent->markEdge(1);
  parent->markEdge(3);
  */
  bool taken;
  for (int i=0;i<parent->noEdges();i++){
    if (parent->edge(i)->marked()){
      marked_edges[cnt_1++] = i;
      for (int j=0;j<parent->noNodes();j++){
	if ( parent->edge(i)->node(0)->id() == parent->node(j)->id() ){
	  taken = false;
	  for (int k=0;k<3;k++){
	    //	    cout << "check 0: marked_nodes[k] = " << marked_nodes[k] << ", j = " << j << endl;
	    if ( marked_nodes[k] == j ) taken = true;
	  }
	  if (!taken) marked_nodes[cnt_2++] = j; 	
	}
	if ( parent->edge(i)->node(1)->id() == parent->node(j)->id() ){
	  taken = false;
	  for (int k=0;k<3;k++){
	    if ( marked_nodes[k] == j ) taken = true;
	  }
	  if (!taken) marked_nodes[cnt_2++] = j; 	
	}
      }
    }
  }

  //  cout << "what nodes = " << marked_nodes[0] << ", " << marked_nodes[1] << ", " << marked_nodes[2] << endl;
  //  cout << "1. cnt_1 = " << cnt_1 << ", cnt_2 = " << cnt_2 << endl;

  int face_node;
  for (int i=0;i<4;i++){
    taken = false;
    for (int j=0;j<3;j++){
      if (marked_nodes[j] == i) taken = true;
    }
    if (!taken){
      face_node = i;
      break;
    } 
  }
  
  /*
    cout << "1. marked edges = " << marked_edges[0] << endl;
    cout << "2. marked edges = " << marked_edges[1] << endl;
    cout << "3. marked edges = " << marked_edges[2] << endl;
    
    cout << "1. marked edges = " << parent->edge(marked_edges[0])->midpoint() << endl;
    cout << "2. marked edges = " << parent->edge(marked_edges[1])->midpoint() << endl;
    cout << "3. marked edges = " << parent->edge(marked_edges[2])->midpoint() << endl;
    
    cout << "1. level = " << parent->level()+1 << endl;
  */
  

  Node *nf = grid.createNode(parent->level()+1,parent->node(face_node)->coord());
  Node *n0 = grid.createNode(parent->level()+1,parent->node(marked_nodes[0])->coord());
  Node *n1 = grid.createNode(parent->level()+1,parent->node(marked_nodes[1])->coord());
  Node *n2 = grid.createNode(parent->level()+1,parent->node(marked_nodes[2])->coord());

  ShortList<Node*> edge_nodes(3);
  edge_nodes(0) = grid.createNode(parent->level()+1,parent->edge(marked_edges[0])->midpoint());
  edge_nodes(1) = grid.createNode(parent->level()+1,parent->edge(marked_edges[1])->midpoint());
  edge_nodes(2) = grid.createNode(parent->level()+1,parent->edge(marked_edges[2])->midpoint());

  ShortList<Cell*> new_cell(4);
  for (int i=0;i<3;i++){
    for (int j=0;j<3;j++){
      if ( (parent->node(marked_nodes[i])->id() != parent->edge(marked_edges[j])->node(0)->id()) &&
	   (parent->node(marked_nodes[i])->id() != parent->edge(marked_edges[j])->node(1)->id()) ){
	if (j == 0){
	  new_cell(i) = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n0,edge_nodes(1),edge_nodes(2),nf);
	}
	if (j == 1){
	  new_cell(i) = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n1,edge_nodes(0),edge_nodes(2),nf);
	}
	if (j == 2){
	  new_cell(i) = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n2,edge_nodes(0),edge_nodes(1),nf);
	}
      }
    }
  }

  new_cell(3) = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,edge_nodes(0),edge_nodes(1),edge_nodes(2),nf);
  
  if (_create_edges){
    grid.createEdges(new_cell(0));
    grid.createEdges(new_cell(1));
    grid.createEdges(new_cell(2));
    grid.createEdges(new_cell(3));
  }
}


void RefineGrid::IrrRef2(Cell* parent)
{
  // 1 edge is marked:
  // Insert 1 new node at the midpoint of the marked edge, then connect 
  // this new node to the 2 nodes not on the marked edge. This gives 2 new 
  // tetrahedrons. 
  
  cout << "parent = " << parent->id() << endl;

  //  parent->markEdge(2);

  if (parent->noMarkedEdges() != 1) dolfin_error("wrong size of refinement edges");

  Node *nnew;
  Node *ne0;
  Node *ne1;
  ShortList<Node*> nold(2);
  Cell* cnew1;
  Cell* cnew2;
  int cnt = 0;
  for (int i=0;i<parent->noEdges();i++){
    if (parent->edge(i)->marked()){
      nnew = grid.createNode(parent->level()+1,parent->edge(i)->midpoint());
      ne0  = grid.createNode(parent->level()+1,parent->edge(i)->node(0)->coord());
      ne1  = grid.createNode(parent->level()+1,parent->edge(i)->node(1)->coord());
      for (int j=0;j<parent->noNodes();j++){
	if ( (parent->edge(i)->node(0)->id() != j) && (parent->edge(i)->node(1)->id() != j) ){
	  nold(cnt++) = grid.createNode(parent->level()+1,parent->node(j)->coord());
	}
      }
      cnew1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nnew,ne0,nold(0),nold(1));
      cnew2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nnew,ne1,nold(0),nold(1));
      break;
    }
  }

  if (_create_edges){
    grid.createEdges(cnew1);
    grid.createEdges(cnew2);
  }
}

void RefineGrid::IrrRef3(Cell* parent)
{
  // 2 edges are marked, on the same face 
  // (here there are 2 possibilities, and the chosen 
  // alternative must match the corresponding face 
  // of the neighbor tetrahedron): 
  // insert 2 new nodes at the midpoints of the marked edges, 
  // insert 3 new edges by connecting the two new nodes to each 
  // other and the node opposite the face of the 2 marked edges, 
  // and insert 1 new edge by 
  // alt.1: connecting new node 1 with the endnode of marked edge 2, 
  // that is not common with marked edge 1, or 
  // alt.2: connecting new node 2 with the endnode of marked edge 1, 
  // that is not common with marked edge 2.

  cout << "parent = " << parent->id() << endl;

  /*
  parent->markEdge(0);  
  parent->markEdge(4);
  */

  if (parent->noMarkedEdges() != 2) dolfin_error("wrong size of refinement edges");
  if (!parent->markedEdgesOnSameFace()) dolfin_error("marked edges not on the same face");

  if (parent->refinedByFaceRule()){
    parent->refineByFaceRule(false);
    return;
  }

  int cnt = 0;
  int marked_edge[2];
  for (int i=0;i<parent->noEdges();i++){
    if (parent->edge(i)->marked()){
      marked_edge[cnt++] = i;
    }
  }

  int face_node;
  int enoded;
  int enode1;
  int enode2;
  int cnt1,cnt2;
  for (int i=0;i<4;i++){
    cnt1 = cnt2 = 0;
    for (int j=0;j<2;j++){
      if (parent->edge(marked_edge[0])->node(j)->id() == parent->node(i)->id()) cnt1++;
      if (parent->edge(marked_edge[1])->node(j)->id() == parent->node(i)->id()) cnt2++;
    }
    cout << "cnt1 = " << cnt1 << ", cnt2 = " << cnt2 << endl;
    if ( (cnt1 == 0) && (cnt2 == 0) ) face_node = i;
    else if ( (cnt1 == 1) && (cnt2 == 1) ) enoded = i;	 
    else if ( (cnt1 == 1) && (cnt2 == 0) ) enode1 = i;	 
    else if ( (cnt1 == 0) && (cnt2 == 1) ) enode2 = i;	 
    else dolfin_error("impossible node");
  }

  Node *nf = grid.createNode(parent->level()+1,parent->node(face_node)->coord());
  Node *nd = grid.createNode(parent->level()+1,parent->node(enoded)->coord());
  Node *n1 = grid.createNode(parent->level()+1,parent->node(enode1)->coord());
  Node *n2 = grid.createNode(parent->level()+1,parent->node(enode2)->coord());

  Node *midnode1 = grid.createNode(parent->level()+1,parent->edge(marked_edge[0])->midpoint());
  Node *midnode2 = grid.createNode(parent->level()+1,parent->edge(marked_edge[1])->midpoint());
  
  // Find element with common face (enoded,enode1,enode2) 
  // (search neighbors of parent)
  int face_neighbor;
  for (int i=0;i<parent->noCellNeighbors();i++){
    for (int j=0;j<parent->neighbor(i)->noNodes();j++){
      if (parent->neighbor(i)->node(j)->id() == parent->node(enoded)->id()){
	for (int k=0;k<parent->neighbor(i)->noNodes();k++){
	  if (k != j){
	    if (parent->neighbor(i)->node(k)->id() == parent->node(enode1)->id()){
	      for (int l=0;l<parent->neighbor(i)->noNodes();l++){
		if ( (l != j) && (l != k) && (parent->neighbor(i)->node(l)->id() == parent->node(enode2)->id()) ){
		  face_neighbor = i;
		}
	      }
	    }		  
	  }
	} 
      }
    }
  }   


  Cell *c1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nd,midnode1,midnode2,nf);
  Cell *c2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n1,midnode1,midnode2,nf);
  Cell *c3 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n1,n2,midnode2,nf);
  
  int neighbor_face_node;
  for (int i=0;i<4;i++){
    if ( (nd->id() != parent->neighbor(face_neighbor)->node(i)->id()) && 
	 (n1->id() != parent->neighbor(face_neighbor)->node(i)->id()) && 
	 (n2->id() != parent->neighbor(face_neighbor)->node(i)->id()) ) neighbor_face_node = i;
  }

  Node *nnf = grid.createNode(parent->level()+1,parent->neighbor(face_neighbor)->node(neighbor_face_node)->coord());
  
  Cell *nc1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nd,midnode1,midnode2,nnf);
  Cell *nc2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n1,midnode1,midnode2,nnf);
  Cell *nc3 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,n1,n2,midnode2,nnf);

  parent->neighbor(face_neighbor)->refineByFaceRule(true);
}

void RefineGrid::IrrRef4(Cell* parent)
{
  // 2 edges are marked, opposite to each other: 
  // insert 2 new nodes at the midpoints of the marked edges, 
  // insert 4 new edges by connecting the new nodes to the 
  // endpoints of the opposite edges of the respectively new nodes. 

  if (parent->noMarkedEdges() != 2) dolfin_error("wrong size of refinement edges");
  if (parent->markedEdgesOnSameFace()) dolfin_error("marked edges on the same face");

  cout << "parent = " << parent->id() << endl;

  /*
  parent->markEdge(0);
  parent->markEdge(2);
  */

  int cnt = 0;
  int marked_edge[2];
  for (int i=0;i<parent->noEdges();i++){
    if (parent->edge(i)->marked()){
      marked_edge[cnt++] = i;
    }
  }

  Node *e1n1 = grid.createNode(parent->level()+1,parent->edge(marked_edge[0])->node(0)->coord());
  Node *e1n2 = grid.createNode(parent->level()+1,parent->edge(marked_edge[0])->node(1)->coord());
  Node *e2n1 = grid.createNode(parent->level()+1,parent->edge(marked_edge[1])->node(0)->coord());
  Node *e2n2 = grid.createNode(parent->level()+1,parent->edge(marked_edge[1])->node(1)->coord());

  Node *midnode1 = grid.createNode(parent->level()+1,parent->edge(marked_edge[0])->midpoint());
  Node *midnode2 = grid.createNode(parent->level()+1,parent->edge(marked_edge[1])->midpoint());

  Cell *c1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,e1n1,midnode1,midnode2,e2n1);
  Cell *c2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,e1n1,midnode1,midnode2,e2n2);
  Cell *c3 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,e1n2,midnode1,midnode2,e2n1);
  Cell *c4 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,e1n2,midnode1,midnode2,e2n2);

  if (_create_edges){
    grid.createEdges(c1);
    grid.createEdges(c2);
    grid.createEdges(c3);
    grid.createEdges(c4);
  }
}






