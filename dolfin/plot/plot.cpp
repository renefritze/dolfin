// Copyright (C) 2007-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Joachim Berdal Haga, 2008.
// Modified by Garth N. Wells, 2008.
//
// First added:  2007-05-02
// Last changed: 2009-10-07

#include <stdlib.h>
#include <sstream>

#include <dolfin/main/MPI.h>
#include <dolfin/parameter/GlobalParameters.h>
#include <dolfin/io/File.h>
#include <dolfin/log/log.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/Expression.h>
#include "FunctionPlotData.h"
#include "plot.h"

using namespace dolfin;

// Template function for plotting objects
template <class T>
void plot_object(const T& t, std::string title, std::string mode)
{
  if (dolfin::MPI::num_processes() > 1)
  {
    if (dolfin::MPI::process_number() == 0)
      info("On screen plotting from C++ not yet working in parallel.");
    return;
  }

  info("Plotting %s (%s), press 'q' to continue...",
          t.name().c_str(), t.label().c_str());

  // Get filename prefix
  std::string prefix = parameters["plot_filename_prefix"];

  // Special treatment when running in parallel
  if (dolfin::MPI::num_processes() > 1)
  {
    std::stringstream p;
    p << dolfin::MPI::process_number();
    prefix += std::string("_p") + p.str();
    title += " (process " + p.str();
  }

  // Save to file
  std::string filename = prefix + std::string(".xml");
  File file(filename);
  file << t;

  // Plot data from file
  std::stringstream command;
  command << "viper --mode=" << mode << " " << "--title=\"" << title << "\" " << filename;
  if (system(command.str().c_str()) != 0)
    warning("Unable to plot.");
}
//-----------------------------------------------------------------------------
void dolfin::plot(const Function& v,
                  std::string title, std::string mode)
{
  FunctionPlotData w(v, v.function_space().mesh());
  plot_object(w, title, mode);
}
//-----------------------------------------------------------------------------
void dolfin::plot(const Expression& v, const Mesh& mesh,
                  std::string title, std::string mode)
{
  FunctionPlotData w(v, mesh);
  plot_object(w, title, mode);
}
//-----------------------------------------------------------------------------
void dolfin::plot(const Mesh& mesh,
                  std::string title)
{
  plot_object(mesh, title, "auto");
}
//-----------------------------------------------------------------------------
void dolfin::plot(const MeshFunction<uint>& f,
                  std::string title)
{
  plot_object(f, title, "auto");
}
//-----------------------------------------------------------------------------
void dolfin::plot(const MeshFunction<double>& f,
                  std::string title)
{
  plot_object(f, title, "auto");
}
//-----------------------------------------------------------------------------
void dolfin::plot(const MeshFunction<bool>& f,
                  std::string title)
{
  plot_object(f, title, "auto");
}
//-----------------------------------------------------------------------------
