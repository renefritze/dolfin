// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Mesh.h>
#include <dolfin/BoundaryInit.h>
#include <dolfin/Boundary.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Boundary::Boundary(Mesh& mesh)
{
  this->mesh = &mesh;
  init();
}
//-----------------------------------------------------------------------------
Boundary::~Boundary()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
int Boundary::noNodes() const
{
  dolfin_assert(mesh);
  return mesh->bd->noNodes();
}
//-----------------------------------------------------------------------------
int Boundary::noEdges() const
{
  dolfin_assert(mesh);
  return mesh->bd->noEdges();
}
//-----------------------------------------------------------------------------
int Boundary::noFaces() const
{
  dolfin_assert(mesh);
  return mesh->bd->noFaces();
}
//-----------------------------------------------------------------------------
void Boundary::init()
{
  if ( mesh->bd->empty() )
    BoundaryInit::init(*mesh);
}
//-----------------------------------------------------------------------------
void Boundary::clear()
{
  mesh->bd->clear();
}
//-----------------------------------------------------------------------------
