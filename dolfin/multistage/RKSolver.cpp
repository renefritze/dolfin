// Copyright (C) 2013 Johan Hake
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
// First added:  2013-02-15
// Last changed: 2013-04-02

#include <cmath>

#include <dolfin/log/log.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/Constant.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/fem/assemble.h>
#include <dolfin/fem/solve.h>
#include <dolfin/fem/DirichletBC.h>

#include "MultiStageScheme.h"
#include "RKSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
RKSolver::RKSolver(std::shared_ptr<MultiStageScheme> scheme) : 
  _scheme(scheme)
{}
//-----------------------------------------------------------------------------
void RKSolver::step(double dt)
{
  dolfin_assert(dt > 0.0);

  // Update time constant of scheme
  *_scheme->dt() = dt;

  // Time at start of timestep
  const double t0 = *_scheme->t();

  // Get scheme data
  std::vector<std::vector<std::shared_ptr<const Form> > >& stage_forms = \
    _scheme->stage_forms();
  std::vector<std::shared_ptr<Function> >& stage_solutions = _scheme->stage_solutions();
  std::vector<const DirichletBC* > bcs = _scheme->bcs();
  
  // Iterate over stage forms
  for (unsigned int stage=0; stage < stage_forms.size(); stage++)
  {

    // Update time
    *_scheme->t() = t0 + dt*_scheme->dt_stage_offset()[stage];

    // Check if we have an explicit stage (only 1 form)
    if (stage_forms[stage].size()==1)
    {

      // Just do an assemble
      _assembler.assemble(*stage_solutions[stage]->vector(), *stage_forms[stage][0]);
      
      // Apply boundary conditions
      // FIXME: stage solutions are time derivatives and we cannot apply the 
      // bcs directly on them
      for (unsigned int j = 0; j < bcs.size(); j++)
      {
	dolfin_assert(bcs[j]);
	bcs[j]->apply(*stage_solutions[stage]->vector());
      }
      
    }
    
    // or an implicit stage (2 forms)
    else
    {
      // FIXME: applying the bcs on stage solutions are probably wrong...
      // FIXME: Include solver parameters
      // Do a nonlinear solve
      solve(*stage_forms[stage][0] == 0, *stage_solutions[stage], bcs, *stage_forms[stage][1]);
    }
  }

  // Do the last stage
  FunctionAXPY last_stage = _scheme->last_stage()*dt;
  
  // Update solution with last stage
  GenericVector& solution_vector = *_scheme->solution()->vector();
  
  // Update with stage solutions
  for (std::vector<std::pair<double, const Function*> >::const_iterator \
	 it=last_stage.pairs().begin();
       it!=last_stage.pairs().end(); it++)
  {
    solution_vector.axpy(it->first, *(it->second->vector()));
  }

  // Update time
  *_scheme->t() = t0 + dt;
  
}
//-----------------------------------------------------------------------------
void RKSolver::step_interval(double t0, double t1, double dt)
{
  if (dt<=0.0)
  {
    dolfin_error("RKSolver.cpp",
		 "stepping RKSolver",
		 "Expecting a positive dt");
  }

  if (t0>=t1)
  {
    dolfin_error("RKSolver.cpp",
		 "stepping RKSolver",
		 "Expecting t0 to be smaller than t1");
  }
  
  // Set start time
  *_scheme->t() = t0;
  double t = t0;
  double next_dt = std::min(t1-t, dt);

  // Step interval
  while (t + next_dt <= t1)
  {
    if (next_dt < DOLFIN_EPS)
      break;
    step(next_dt);
    t = *_scheme->t();
    next_dt = std::min(t1-t, dt);
  }
}
//-----------------------------------------------------------------------------
