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
//
// First added:  2012-06-20
// Last changed: 2012-09-20

#ifndef __VTK_PLOTTABLE_GENERIC_FUNCTION_H
#define __VTK_PLOTTABLE_GENERIC_FUNCTION_H

#ifdef HAS_VTK

#include <memory>
#include <vtkWarpScalar.h>
#include <vtkWarpVector.h>
#include <vtkGlyph3D.h>

#include <dolfin/common/Variable.h>
#include "VTKPlottableMesh.h"

namespace dolfin
{

  // Forward declarations
  class Expression;
  class ExpressionWrapper;
  class Function;
  class GenericFunction;
  class GenericVTKPlottable;
  class Mesh;
  class Parameters;

  /// Data wrapper class for plotting generic functions, including
  /// instances of the Function and Expression classes.

  class VTKPlottableGenericFunction : public VTKPlottableMesh
  {
  public:

    explicit
      VTKPlottableGenericFunction(std::shared_ptr<const Function> function);

    VTKPlottableGenericFunction(std::shared_ptr<const Expression> expression,
                                std::shared_ptr<const Mesh> mesh);

    virtual ~VTKPlottableGenericFunction();

    //--- Implementation of the GenericVTKPlottable interface ---

    /// Additional parameters for VTKPlottableGenericFunction
    virtual void modify_default_parameters(Parameters& p) {}

    virtual void modify_user_parameters(Parameters& p)
    {
      std::string mode = p["mode"];
      Parameter& elevate = p["elevate"];
      if (dim() < 3 && value_rank() == 0 && mode != "color"
          && !elevate.is_set())
      {
        elevate = -65.0;
      }
    }

    /// Initialize the parts of the pipeline that this class controls
    void init_pipeline(const Parameters& p);

    /// Update the plottable data
    void update(std::shared_ptr<const Variable> var, const Parameters& p,
                int framecounter);

    /// Check if the plotter is compatible with a given variable
    /// (same-rank function on same mesh for example)
    bool is_compatible(const Variable& var) const;

    /// Update the scalar range of the plottable data
    void update_range(double range[2]);

    /// Inform the plottable about the range.
    virtual void rescale(double range[2], const Parameters& p);

    /// Return data to visualize
    vtkSmartPointer<vtkAlgorithmOutput> get_output() const;

  protected:

    // Update scalar values
    void update_scalar();

    // Update vector values
    void update_vector();

    // The function to visualize
    std::weak_ptr<const GenericFunction> _function;

    // The scalar warp filter
    vtkSmartPointer<vtkWarpScalar> _warpscalar;

    // The vector warp filter
    vtkSmartPointer<vtkWarpVector> _warpvector;

    // The glyph filter
    vtkSmartPointer<vtkGlyph3D> _glyphs;

    // Mode flag
    std::string _mode;

    std::size_t value_rank() const;

  };

  VTKPlottableGenericFunction*
    CreateVTKPlottable(std::shared_ptr<const Function>);
  VTKPlottableGenericFunction*
    CreateVTKPlottable(std::shared_ptr<const ExpressionWrapper>);
  VTKPlottableGenericFunction*
    CreateVTKPlottable(std::shared_ptr<const Expression>,
                       std::shared_ptr<const Mesh>);

}

#endif

#endif
