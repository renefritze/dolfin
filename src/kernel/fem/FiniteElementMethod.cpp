// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Map.h>
#include <dolfin/Quadrature.h>
#include <dolfin/P1TriElement.h>
#include <dolfin/P1TetElement.h>
#include <dolfin/P1TriMap.h>
#include <dolfin/P1TetMap.h>
#include <dolfin/GaussQuadrature.h>
#include <dolfin/TriangleMidpointQuadrature.h>
#include <dolfin/TetrahedronMidpointQuadrature.h>
#include <dolfin/FiniteElementMethod.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
FiniteElementMethod::FiniteElementMethod(Mesh::Type type, unsigned int noeq)
{
  // Create default finite element
  switch ( type ) {
  case Mesh::triangles:
    
    cout << "Using standard piecewise linears on triangles." << endl;
    
    _element = new FiniteElement::Vector(noeq);
    for (unsigned int i = 0; i < noeq; ++i)
      (*_element)(i) = new P1TriElement();
    
    _map = new P1TriMap();
    _interior_quadrature = new TriangleMidpointQuadrature();
    _boundary_quadrature = new GaussQuadrature(2);
    
    break;
    
  case Mesh::tetrahedrons:
    
    cout << "Using standard piecewise linears on tetrahedrons." << endl;
    
    _element = new FiniteElement::Vector(noeq);
    for (unsigned int i = 0; i < noeq; ++i)
      (*_element)(i) = new P1TetElement();
    
    _map = new P1TetMap();
    _interior_quadrature = new TetrahedronMidpointQuadrature();
    _boundary_quadrature = new TriangleMidpointQuadrature();

    break;
    
  default:
    dolfin_error("No default method for this type of mesh.");
  }

  // User does not specify method
  user = true;
}
//-----------------------------------------------------------------------------
FiniteElementMethod::FiniteElementMethod(FiniteElement::Vector& element,
					 Map& map, 
					 Quadrature& interior_quadrature,
					 Quadrature& boundary_quadrature)
{
  // Save user data
  _element = &element;
  _map = &map;
  _interior_quadrature = &interior_quadrature;
  _boundary_quadrature = &boundary_quadrature;
  
  // User specifies method
  user = true;
}
//-----------------------------------------------------------------------------
FiniteElementMethod::~FiniteElementMethod()
{
  // Delete method data if not given by user
  if ( !user )
  {
    if ( _element )
    {
      for (unsigned int i = 0; i < _element->size(); ++i)
	delete (*_element)(i);
      delete _element;
    }
    
    if ( _map )
      delete _map;

    
    if ( _interior_quadrature )
      delete _interior_quadrature;

    if ( _boundary_quadrature )
      delete _boundary_quadrature;
  }

  _element = 0;
  _map = 0;
  _interior_quadrature = 0;
  _boundary_quadrature = 0;
}
//-----------------------------------------------------------------------------
FiniteElement::Vector& FiniteElementMethod::element()
{
  if (!_element)
    dolfin_error("Finite element method not initialized.");

  return *_element;
}
//-----------------------------------------------------------------------------
Map& FiniteElementMethod::map()
{
  if (!_map)
    dolfin_error("Finite element method not initialized.");

  return *_map;
}
//-----------------------------------------------------------------------------
Quadrature& FiniteElementMethod::interiorQuadrature()
{
  if (!_interior_quadrature)
    dolfin_error("Finite element method not initialized.");

  return *_interior_quadrature;
}
//-----------------------------------------------------------------------------
Quadrature& FiniteElementMethod::boundaryQuadrature()
{
  if (!_boundary_quadrature)
    dolfin_error("Finite element method not initialized.");

  return *_boundary_quadrature;
}
//-----------------------------------------------------------------------------
