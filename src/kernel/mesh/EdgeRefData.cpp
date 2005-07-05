// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003
// Last changed: 2005

#include <dolfin/Cell.h>
#include <dolfin/EdgeRefData.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void EdgeRefData::mark(Cell& cell)
{
  if ( !cells.contains(&cell) )
    cells.add(&cell);
}
//-----------------------------------------------------------------------------
bool EdgeRefData::marked() const
{
  return cells.size() > 0;
}
//-----------------------------------------------------------------------------
bool EdgeRefData::marked(Cell& cell)
{
  return cells.contains(&cell);
}
//-----------------------------------------------------------------------------
void EdgeRefData::clear()
{
  cells.clear();
}
//-----------------------------------------------------------------------------
