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
    IrrRef2(parent);
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

  parent->markEdge(2);

  if (parent->noMarkedEdges() != 1) dolfin_error("wrong size of refinement edges");

  Node *nnew;
  Node *ne0;
  Node *ne1;
  ShortList<Node*> nold(2);
  Cell* cnew1;
  Cell* cnew2;
  int cnt = 0;
  for (int i;i<parent->noEdges();i++){
    if (parent->edge(i)->marked()){
      nnew = grid.createNode(parent->level()+1,parent->edge(i)->midpoint());
      ne0  = grid.createNode(parent->level()+1,parent->edge(i)->node(0)->coord());
      ne1  = grid.createNode(parent->level()+1,parent->edge(i)->node(1)->coord());
      for (int j;j<parent->noNodes();j++){
	if ( (parent->edge(i)->node(0)->id() != j) && (parent->edge(i)->node(1)->id() != j) ){
	  nold(cnt++) = grid.createNode(parent->level()+1,parent->node(j)->coord());
	}
      }
      cnew1 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nnew,ne0,nold(0),nold(1));
      cnew2 = grid.createCell(parent->level()+1,Cell::TETRAHEDRON,nnew,ne1,nold(0),nold(1));
    }
  }
  if (_create_edges){
    grid.createEdges(cnew1);
    grid.createEdges(cnew2);
  }
}

void RefineGrid::IrrRef3(Cell* parent)
{
}

void RefineGrid::IrrRef4(Cell* parent)
{
}


/*
Refinement::IrrRef2(Cell *parent, int marked_edge)
{      
  // 1 edge is marked:
  // Insert 1 new node at the midpoint of the marked edge, then connect 
  // this new node to the 2 nodes not on the marked edge. This gives 2 new 
  // tetrahedrons. 
  
  int non_marked_nodes[2];
  int cnt = 0;

  Node *n = grid.createNode();
  
  n->Set(parent->GetEdge(marked_edge)->GetMidpoint());

  Cell *t1 = grid.createCell();
  Cell *t2 = grid.createCell();

  cnt = 0;
  for (int i=0; i<4; i++){
    if ( (parent->GetNode(i) != parent->GetEdge(marked_edge)->GetEndnode(0)) && 
	 (parent->GetNode(i) != parent->GetEdge(marked_edge)->GetEndnode(1)) ){
      non_marked_nodes[cnt] = i;
      cnt++;
    }
  }
  
  t1->Set(parent->GetEdge(marked_edge)->GetEndnode(0),n,parent->GetNode(non_marked_nodes[0]),parent->GetNode(non_marked_nodes[1]));
  t2->Set(parent->GetEdge(marked_edge)->GetEndnode(1),n,parent->GetNode(non_marked_nodes[0]),parent->GetNode(non_marked_nodes[1]));

}


Refinement::IrrRef3(Cell *parent, int marked_edge1, int marked_edge2)
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

  int non_marked_node;
  int double_marked_node;
  
  int once_marked_node[2];
  int node_marks[4];

  Node *n1 = grid.createNode();
  Node *n2 = grid.createNode();
  
  n1->Set(parent->GetEdge(marked_edge1)->GetMidpoint());
  n2->Set(parent->GetEdge(marked_edge2)->GetMidpoint());

  Cell *t1 = grid.createCell();
  Cell *t2 = grid.createCell();
  Cell *t3 = grid.createCell();

  for (int i=0; i<4; i++){
    node_marks[i] = 0;
    if ( (parent->GetNode(i) == parent->GetEdge(marked_edge1)->GetEndnode(0)) || 
	 (parent->GetNode(i) == parent->GetEdge(marked_edge1)->GetEndnode(1)) ||
	 (parent->GetNode(i) == parent->GetEdge(marked_edge2)->GetEndnode(0)) ||
	 (parent->GetNode(i) == parent->GetEdge(marked_edge2)->GetEndnode(1)) ){
      node_marks[i]++;
    }
  }  

  int cnt = 0;
  for (int i=0; i<4; i++){
    if (node_marks[i]==0) non_marked_node = i;
    else if (node_marks[i]==2) double_marked_node = i;
    else{ 
      once_marked_node[cnt] = i;
      cont++;
    }
  }
  
  t1->Set(parent->GetNode(double_marked_node),n1,n2,parent->GetNode(non_marked_node));

  t2->Set(parent->GetNode(once_marked_node[0]),n1,n2,parent->GetNode(non_marked_node));
  t3->Set(parent->GetNode(once_marked_node[1]),n1,n2,parent->GetNode(non_marked_node));

  t2->Set(parent->GetNode(double_marked_node),n1,n2,parent->GetNode(non_marked_node));
  t3->Set(parent->GetNode(double_marked_node),n1,n2,parent->GetNode(non_marked_node));
}


Refinement::IrrRef4(Cell *parent, int marked_edge1, int marked_edge2)
{      
  // 2 edges are marked, opposite to each other: 
  // insert 2 new nodes at the midpoints of the marked edges, 
  // insert 4 new edges by connecting the new nodes to the 
  // endpoints of the opposite edges of the respectively new nodes. 

  Node *n1 = grid.createNode();
  Node *n2 = grid.createNode();
  
  n1->Set(parent->GetEdge(marked_edge1)->GetMidpoint());
  n2->Set(parent->GetEdge(marked_edge2)->GetMidpoint());

  Cell *t1 = grid.createCell();
  Cell *t2 = grid.createCell();
  Cell *t3 = grid.createCell();
  Cell *t4 = grid.createCell();

  t1->Set(parent->GetEdge(marked_edge1)->GetEndnode(0),n1,parent->GetEdge(marked_edge2)->GetEndnode(0),n2);
  t2->Set(parent->GetEdge(marked_edge1)->GetEndnode(0),n1,parent->GetEdge(marked_edge2)->GetEndnode(1),n2);
  t3->Set(parent->GetEdge(marked_edge1)->GetEndnode(1),n1,parent->GetEdge(marked_edge2)->GetEndnode(0),n2);
  t4->Set(parent->GetEdge(marked_edge1)->GetEndnode(1),n1,parent->GetEdge(marked_edge2)->GetEndnode(1),n2);

}
*/
