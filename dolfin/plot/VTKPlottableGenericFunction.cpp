// Copyright (C) 2012 Fredrik Valdmanis
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
// Modified by Joachim B Haga 2012
// Modified by Benjamin Kehlet 2012
//
// First added:  2012-06-20
// Last changed: 2013-11-15

#ifdef HAS_VTK

#include <vtkArrowSource.h>
#include <vtkUnstructuredGrid.h>

#include <dolfin/common/Timer.h>
#include <dolfin/function/Expression.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/mesh/Vertex.h>

#include "VTKPlottableMesh.h"
#include "VTKPlottableGenericFunction.h"
#include "VTKPlottableGenericFunction1D.h"
#include "ExpressionWrapper.h"

using namespace dolfin;

//----------------------------------------------------------------------------
VTKPlottableGenericFunction::VTKPlottableGenericFunction(std::shared_ptr<const Function> function)
  : VTKPlottableMesh(function->function_space()->mesh()),
    _function(function)
{
  // Do nothing
}
//----------------------------------------------------------------------------
VTKPlottableGenericFunction::VTKPlottableGenericFunction(std::shared_ptr<const Expression> expression,
                                                         std::shared_ptr<const Mesh> mesh)
  : VTKPlottableMesh(mesh), _function(expression)
{
  // Do nothing
}
//----------------------------------------------------------------------------
std::size_t VTKPlottableGenericFunction::value_rank() const
{
  return _function->value_rank();
}
//----------------------------------------------------------------------------
void VTKPlottableGenericFunction::init_pipeline(const Parameters& p)
{
  _warpscalar = vtkSmartPointer<vtkWarpScalar>::New();
  // Required in VTK6.0/6.1, default and XYPlaneOn() doesn't work
  _warpscalar->UseNormalOn();
  _warpscalar->SetNormal(0, 0, 1);
  _warpvector = vtkSmartPointer<vtkWarpVector>::New();
  _glyphs = vtkSmartPointer<vtkGlyph3D>::New();
  _mode = (std::string)p["mode"];

  VTKPlottableMesh::init_pipeline(p);

  switch (_function->value_rank())
  {
    // Setup pipeline for scalar functions
    case 0:
    {
      if (mesh()->topology().dim() < 3)
      {
        if (_mode == "auto")
        {
          // Default mode for 2D scalars
          _mode = "warp";
        }
        if (_mode != "color" && _mode != "warp")
        {
          warning("Mode not valid for 2D scalar plot: "+_mode);
          _mode = "warp";
        }

        // In 2D, we warp the mesh according to the scalar values
        // (1D is normally plotted by separate Plottable class)
        if (_mode == "warp")
        {
          insert_filter(_warpscalar);
        }
      }
      else
      {
        if (_mode == "auto")
        {
          // In 3D, we just show the scalar values as colors on the mesh
          _mode = "color";
        }
        if (_mode != "color")
        {
          warning("Mode not valid for 3D scalar plot: "+_mode);
          _mode = "color";
        }

        // Nothing to do
      }
    }
    break;

    // Setup pipeline for vector functions. Everything is set up except the
    // mapper
    case 1:
    {
      if (_mode == "auto")
      {
        // Default mode is glyphs for vectors (2D and 3D)
        _mode = "glyphs";
      }
      if (_mode != "color" && _mode != "glyphs" && _mode != "displacement")
      {
        warning("Mode not valid for vector plot: "+_mode);
        _mode = "glyphs";
      }

      // Setup pipeline for warp visualization
      if (_mode == "displacement")
      {
        insert_filter(_warpvector);
      }

      // Setup pipeline for glyph visualization
      vtkSmartPointer<vtkArrowSource> arrow =
        vtkSmartPointer<vtkArrowSource>::New();
      arrow->SetTipRadius(0.08);
      arrow->SetTipResolution(16);
      arrow->SetTipLength(0.25);
      arrow->SetShaftRadius(0.05);
      arrow->SetShaftResolution(16);

      // Create the glyph object, set source (the arrow) and input
      // (the grid) and adjust various parameters
      _glyphs->SetSourceConnection(arrow->GetOutputPort());
      #if VTK_MAJOR_VERSION <= 5
      _glyphs->SetInput(grid());
      #else
      _glyphs->SetInputData(grid());
      #endif
      _glyphs->SetVectorModeToUseVector();
      _glyphs->SetScaleModeToScaleByVector();
      _glyphs->SetColorModeToColorByVector();
    }
    break;
    default:
    {
      dolfin_error("VTKPlotter.cpp",
                   "plot function of rank > 2.",
                   "Plotting of higher order functions is not supported.");
    }
  }
}
//----------------------------------------------------------------------------
bool VTKPlottableGenericFunction::is_compatible(const Variable &var) const
{
  const GenericFunction *function(dynamic_cast<const Function*>(&var));
  const ExpressionWrapper
    *wrapper(dynamic_cast<const ExpressionWrapper*>(&var));
  const Mesh *mesh(NULL);

  if (function)
  {
    mesh
      = static_cast<const Function*>(function)->function_space()->mesh().get();
  }
  else if (wrapper)
  {
    function = wrapper->expression().get();
    mesh = wrapper->mesh().get();
  }
  else
    return false;

  if (function->value_rank() > 1 || (function->value_rank() == 0)
      != !_glyphs->GetInput())
  {
    return false;
  }
  return VTKPlottableMesh::is_compatible(*mesh);
}
//----------------------------------------------------------------------------
void VTKPlottableGenericFunction::update(std::shared_ptr<const Variable> var,
                                         const Parameters& p, int frame_counter)
{
  std::shared_ptr<const Mesh> mesh = VTKPlottableMesh::mesh();
  if (var)
  {
    std::shared_ptr<const Function>
      function(std::dynamic_pointer_cast<const Function>(var));
    std::shared_ptr<const ExpressionWrapper>
      wrapper(std::dynamic_pointer_cast<const ExpressionWrapper>(var));
    dolfin_assert(function || wrapper);
    if (function)
    {
      mesh = function->function_space()->mesh();
      _function = function;
    }
    else
    {
      mesh = wrapper->mesh();
      _function = wrapper->expression();
    }
  }

  // Update the mesh
  VTKPlottableMesh::update(mesh, p, frame_counter);

  // Update the values on the mesh
  const Function *func = dynamic_cast<const Function *>(_function.get());
  if (func && func->vector()->local_size()
      == (std::size_t)grid()->GetNumberOfCells() && dim() > 1)
  {
    // Hack to display DG0 functions. Should really be implemented using
    // duplicate points (one point per vertex per cell), so that warping.
    // DG1 and 1D work as expected.
    // Also: How do we find out if a FunctionSpace is discontinuous?
    insert_filter(NULL); // expel the warpscalar filter
    std::vector<double> cell_values;
    func->vector()->get_local(cell_values);
    setCellValues(cell_values.size(), &cell_values[0], p);
  }
  else
  {
    std::vector<double> vertex_values;
    _function->compute_vertex_values(vertex_values, *mesh);
    if (dim() == 1)
    {
      // Sort 1D data on x-coordinate because vtkXYPlotActor does not
      // recognise cell connectivity information
      std::vector<std::pair<double, double> > point_value;
      VertexIterator v(*mesh);
      for(std::size_t i = 0; i < vertex_values.size(); ++i)
      {
        const double xpos = v[i].point().x();
        point_value.push_back(std::make_pair(xpos, vertex_values[i]));
      }

      std::sort(point_value.begin(), point_value.end());
      for(std::size_t i = 0; i < vertex_values.size(); ++i)
        vertex_values[i] = point_value[i].second;
    }

    setPointValues(vertex_values.size(), &vertex_values[0], p);
  }
}
//----------------------------------------------------------------------------
void VTKPlottableGenericFunction::rescale(double range[2], const Parameters& p)
{
  const double scale = p["scale"];
  const double* bounds = grid()->GetBounds();
  const double length[3]
    = {bounds[1]-bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4]};

  const double bbox_diag = std::sqrt(length[0]*length[0] + length[1]*length[1]
                                     + length[2]*length[2]);

  // The scale for displacements is absolute
  _warpvector->SetScaleFactor(scale);

  // Compute the scale for vector glyphs, so that the longest arrows
  // cover about two cells
  double vector_scale = scale * mesh()->hmax() * 2.0;
  vector_scale /= (range[1] > 0 ? range[1] : 1.0);

  _glyphs->SetScaleFactor(vector_scale);

  // Set the default warp such that the max warp is one sixth of the
  // diagonal of the mesh
  double scalar_scale = scale*bbox_diag/6.0;
  scalar_scale /= (range[1] > range[0] ? range[1] - range[0] : 1.0);

  _warpscalar->SetScaleFactor(scalar_scale);
}
//----------------------------------------------------------------------------
void VTKPlottableGenericFunction::update_range(double range[2])
{
  // Superclass gets the range from the grid
  VTKPlottableMesh::update_range(range);
}
//----------------------------------------------------------------------------
vtkSmartPointer<vtkAlgorithmOutput>
VTKPlottableGenericFunction::get_output() const
{
  // In the 3D glyph case, return the glyphs' output
  if (_function->value_rank() == 1 && _mode == "glyphs")
    return _glyphs->GetOutputPort();
  else
    return VTKPlottableMesh::get_output();
}
//----------------------------------------------------------------------------
VTKPlottableGenericFunction*
dolfin::CreateVTKPlottable(std::shared_ptr<const Function> function)
{
  if (function->function_space()->mesh()->topology().dim() == 1)
    return new VTKPlottableGenericFunction1D(function);
  else
    return new VTKPlottableGenericFunction(function);
}
//----------------------------------------------------------------------------
VTKPlottableGenericFunction*
dolfin::CreateVTKPlottable(std::shared_ptr<const ExpressionWrapper> wrapper)
{
  return CreateVTKPlottable(wrapper->expression(), wrapper->mesh());
}
//----------------------------------------------------------------------------
VTKPlottableGenericFunction*
dolfin::CreateVTKPlottable(std::shared_ptr<const Expression> expr,
                           std::shared_ptr<const Mesh> mesh)
{
  if (mesh->topology().dim() == 1)
    return new VTKPlottableGenericFunction1D(expr, mesh);
  else
    return new VTKPlottableGenericFunction(expr, mesh);
}
//----------------------------------------------------------------------------
#endif
