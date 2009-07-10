// Copyright (C) 2006-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-05-16
// Last changed: 2009-06-25

#include <dolfin/log/dolfin_log.h>
#include <dolfin/parameter/dolfin_parameter.h>
#include "Mesh.h"
#include "MeshFunction.h"
#include "Point.h"
#include "MeshEditor.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
MeshEditor::MeshEditor()
  : mesh(0), tdim(0), gdim(0),
    num_vertices(0), num_cells(0),
    next_vertex(0), next_cell(0),
    num_higher_order_vertices(0), num_higher_order_cells(0),
    next_higher_order_vertex(0), next_higher_order_cell(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MeshEditor::~MeshEditor()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void MeshEditor::open(Mesh& mesh, CellType::Type type, uint tdim, uint gdim)
{
  // Clear old mesh data
  mesh.clear();
  clear();

  // Save mesh and dimension
  this->mesh = &mesh;
  this->gdim = gdim;
  this->tdim = tdim;

  // Set cell type
  mesh._cell_type = CellType::create(type);

  // Initialize topological dimension
  mesh._topology.init(tdim);

  // Initialize temporary storage for local cell data
  vertices.reserve(mesh.type().num_vertices(tdim));
  for (uint i = 0; i < mesh.type().num_vertices(tdim); i++)
    vertices.push_back(0);

  // in the future, could set a string in MeshGeometry that indicates the type of
  //      higher order mesh... for now we assume P2 triangle!

  // Initialize temporary storage for local higher order cell data
  higher_order_cell_data.reserve(6); // assume P2 triangle!
  for (uint i = 0; i < 6; i++)
    higher_order_cell_data.push_back(0);
}
//-----------------------------------------------------------------------------
void MeshEditor::open(Mesh& mesh, std::string type, uint tdim, uint gdim)
{
  if (type == "point")
    open(mesh, CellType::point, tdim, gdim);
  else if (type == "interval")
    open(mesh, CellType::interval, tdim, gdim);
  else if (type == "triangle")
    open(mesh, CellType::triangle, tdim, gdim);
  else if (type == "tetrahedron")
    open(mesh, CellType::tetrahedron, tdim, gdim);
  else
    error("Unknown cell type \"%s\".", type.c_str());
}
//-----------------------------------------------------------------------------
void MeshEditor::init_vertices(uint num_vertices)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Initialize mesh data
  this->num_vertices = num_vertices;
  mesh->_topology.init(0,    num_vertices);
  mesh->_geometry.init(gdim, num_vertices);
}
//-----------------------------------------------------------------------------
void MeshEditor::init_higher_order_vertices(uint num_higher_order_vertices)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Initialize mesh data
  this->num_higher_order_vertices = num_higher_order_vertices;
  mesh->_geometry.init_higher_order_vertices(gdim, num_higher_order_vertices);
}
//-----------------------------------------------------------------------------
void MeshEditor::init_cells(uint num_cells)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Initialize mesh data
  this->num_cells = num_cells;
  mesh->_topology.init(tdim, num_cells);
  mesh->_topology(tdim, 0).init(num_cells, mesh->type().num_vertices(tdim));
}
//-----------------------------------------------------------------------------
void MeshEditor::init_higher_order_cells(uint num_higher_order_cells, 
                                         uint num_higher_order_cell_dof)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Initialize higher order mesh data
  this->num_higher_order_cells = num_higher_order_cells;
  mesh->_geometry.init_higher_order_cells(num_higher_order_cells, num_higher_order_cell_dof);

  // Initalize a boolean array that indicates whether cells are affinely mapped or not
  mesh->_geometry.init_affine_indicator(num_higher_order_cells);
}
//-----------------------------------------------------------------------------
void MeshEditor::set_affine_cell_indicator(uint c, const std::string affine_str)
{
  bool affine_value = true; // init

  if (affine_str=="false")
    affine_value = false;

  // Set affine indicator for specific cell
  mesh->_geometry.set_affine_indicator(c, affine_value);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_vertex(uint v, const Point& p)
{
  // Add vertex
  add_vertex_common(v, mesh->geometry().dim());

  // Set coordinate
  for (uint i = 0; i < mesh->geometry().dim(); i++)
    mesh->_geometry.set(v, i, p[i]);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_vertex(uint v, double x)
{
  // Add vertex
  add_vertex_common(v, 1);

  // Set coordinate, next_vertex doesn't seem to work right
//  mesh->_geometry.set(next_vertex, 0, x);

  // Set coordinate
  mesh->_geometry.set(v, 0, x);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_vertex(uint v, double x, double y)
{
  // Add vertex
  add_vertex_common(v, 2);

  // Set coordinate
  mesh->_geometry.set(v, 0, x);
  mesh->_geometry.set(v, 1, y);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_vertex(uint v, double x, double y, double z)
{
  // Add vertex
  add_vertex_common(v, 3);

  // Set coordinate
  mesh->_geometry.set(v, 0, x);
  mesh->_geometry.set(v, 1, y);
  mesh->_geometry.set(v, 2, z);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_vertex(uint v, const Point& p)
{
  // Add vertex
  add_higher_order_vertex_common(v, mesh->geometry().dim());

  // Set coordinate
  for (uint i = 0; i < mesh->geometry().dim(); i++)
    mesh->_geometry.set_higher_order_coordinates(v, i, p[i]);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_vertex(uint v, double x)
{
  // Add vertex
  add_higher_order_vertex_common(v, 1);

  // Set coordinate
  mesh->_geometry.set_higher_order_coordinates(v, 0, x);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_vertex(uint v, double x, double y)
{
  // Add vertex
  add_higher_order_vertex_common(v, 2);

  // Set coordinate
  mesh->_geometry.set_higher_order_coordinates(v, 0, x);
  mesh->_geometry.set_higher_order_coordinates(v, 1, y);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_vertex(uint v, double x, double y, double z)
{
  // Add vertex
  add_higher_order_vertex_common(v, 3);

  // Set coordinate
  mesh->_geometry.set_higher_order_coordinates(v, 0, x);
  mesh->_geometry.set_higher_order_coordinates(v, 1, y);
  mesh->_geometry.set_higher_order_coordinates(v, 2, z);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_cell(uint c, const std::vector<uint>& v)
{
  // Add cell
  add_cell_common(c, tdim);

  // Set data
  mesh->_topology(tdim, 0).set(c, v);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_cell(uint c, uint v0, uint v1)
{
  // Add cell
  add_cell_common(c, 1);

  // Set data
  vertices[0] = v0;
  vertices[1] = v1;
  mesh->_topology(tdim, 0).set(c, vertices);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_cell(uint c, uint v0, uint v1, uint v2)
{
  // Add cell
  add_cell_common(c, 2);

  // Set data
  vertices[0] = v0;
  vertices[1] = v1;
  vertices[2] = v2;
  mesh->_topology(tdim, 0).set(c, vertices);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_cell(uint c, uint v0, uint v1, uint v2, uint v3)
{
  // Add cell
  add_cell_common(c, 3);

  // Set data
  vertices[0] = v0;
  vertices[1] = v1;
  vertices[2] = v2;
  vertices[3] = v3;
  mesh->_topology(tdim, 0).set(c, vertices);
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_cell_data(uint c, uint v0, uint v1, uint v2,
                                            uint v3, uint v4, uint v5)
{
  // Add cell
  add_higher_order_cell_common(c, 6);

  // Set data
  higher_order_cell_data[0] = v0;
  higher_order_cell_data[1] = v1;
  higher_order_cell_data[2] = v2;
  higher_order_cell_data[3] = v3;
  higher_order_cell_data[4] = v4;
  higher_order_cell_data[5] = v5;
  mesh->_geometry.set_higher_order_cell_data(c, higher_order_cell_data);
}
//-----------------------------------------------------------------------------
void MeshEditor::close(bool order)
{
  // Order mesh if requested
  assert(mesh);
  if (order && !mesh->ordered())
    mesh->order();

  // Compute boundary indicators
  compute_boundary_indicators();

  // Clear data
  clear();
}
//-----------------------------------------------------------------------------
void MeshEditor::add_vertex_common(uint v, uint gdim)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Check that the dimension matches
  if (gdim != this->gdim)
    error("Illegal dimension for vertex coordinate: %d (should be %d).",
          gdim, this->gdim);

  // Check value of vertex index
  if (v >= num_vertices)
    error("Vertex index (%d) out of range [0, %d].",
          v, num_vertices - 1);

  // Check if there is room for more vertices
  if (next_vertex >= num_vertices)
    error("Vertex list is full, %d vertices already specified.",
          num_vertices);

  // Step to next vertex
  next_vertex++;
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_vertex_common(uint v, uint gdim)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Check that the dimension matches
  if (gdim != this->gdim)
    error("Illegal dimension for higher order vertex coordinate: %d (should be %d).",
          gdim, this->gdim);

  // Check value of vertex index
  if (v >= num_higher_order_vertices)
    error("Higher Order vertex index (%d) out of range [0, %d].",
          v, num_higher_order_vertices - 1);

  // Check if there is room for more vertices
  if (next_higher_order_vertex >= num_higher_order_vertices)
    error("Higher Order vertex list is full, %d vertices already specified.",
          num_higher_order_vertices);

  // Step to next vertex
  next_higher_order_vertex++;
}
//-----------------------------------------------------------------------------
void MeshEditor::add_cell_common(uint c, uint tdim)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Check that the dimension matches
  if (tdim != this->tdim)
    error("Illegal dimension for cell: %d (should be %d).",
          tdim, this->tdim);

  // Check value of cell index
  if (c >= num_cells)
    error("Cell index (%d) out of range [0, %d].",
          c, num_cells - 1);

  // Check if there is room for more cells
  if (next_cell >= num_cells)
    error("Cell list is full, %d cells already specified.", num_cells);

  // Step to next cell
  next_cell++;
}
//-----------------------------------------------------------------------------
void MeshEditor::add_higher_order_cell_common(uint c, uint tdim)
{
  // Check if we are currently editing a mesh
  if (!mesh)
    error("No mesh opened, unable to edit.");

  // Check that the dimension matches
  if (tdim != 6)
    error("Illegal dimension for higher order cell: %d (should be %d).",
          tdim, 6);

  // Check value of cell index
  if (c >= num_higher_order_cells)
    error("Higher order cell index (%d) out of range [0, %d].",
          c, num_higher_order_cells - 1);

  // Check if there is room for more cells
  if (next_higher_order_cell >= num_higher_order_cells)
    error("Higher order cell list is full, %d cells already specified.", num_cells);

  // Step to next cell
  next_higher_order_cell++;
}
//-----------------------------------------------------------------------------
void MeshEditor::compute_boundary_indicators()
{
  // Do nothing if mesh function "exterior facet domains" is present
  if (mesh->data().mesh_function("exterior facet domains"))
    return; 

  // Extract data for boundary indicators
  std::vector<uint>* facet_cells   = mesh->data().array("boundary facet cells");
  std::vector<uint>* facet_numbers = mesh->data().array("boundary facet numbers");
  std::vector<uint>* indicators    = mesh->data().array("boundary indicators");

  // Do nothing if there are no indicators
  if (!indicators)
    return;

  // Make sure mesh has facets
  const uint D = mesh->topology().dim();
  mesh->init(D - 1);

  // Need facet cells and numbers if indicators are present
  if (!facet_cells || !facet_numbers)
    error("Mesh has boundary indicators, but missing data for \"boundary facet cells\" and \"boundary facet numbers\".");
  const uint num_facets = facet_cells->size();
  assert(facet_numbers->size() == num_facets);
  assert(indicators->size() == num_facets);
  assert(num_facets > 0);

  // Create mesh function "exterior_facet_domains"
  MeshFunction<uint>* exterior_facet_domains =
    mesh->data().create_mesh_function("exterior facet domains", D - 1);

  // Initialize meshfunction to zero
  (*exterior_facet_domains) = 0;

  // Assign domain numbers for each facet
  for (uint i = 0; i < num_facets; i++)
  {
    // Get cell index and local facet index
    const uint cell_index = (*facet_cells)[i];
    const uint local_facet = (*facet_numbers)[i];
    
    // Get global facet index
    const uint global_facet = mesh->_topology(D, D - 1)(cell_index)[local_facet];
        
    // Set boundary indicator for facet
    exterior_facet_domains->set(global_facet, (*indicators)[i]);
  }
}
//-----------------------------------------------------------------------------
void MeshEditor::clear()
{
  tdim = 0;
  gdim = 0;
  num_vertices = 0;
  num_cells = 0;
  next_vertex = 0;
  next_cell = 0;
  num_higher_order_vertices = 0;
  num_higher_order_cells = 0;
  next_higher_order_vertex = 0;
  next_higher_order_cell = 0;
  mesh = 0;
  vertices.clear();
  higher_order_cell_data.clear();
}
//-----------------------------------------------------------------------------
