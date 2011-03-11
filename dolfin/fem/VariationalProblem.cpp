// Copyright (C) 2008-2011 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Marie E. Rognes 2011
//
// First added:  2008-12-26
// Last changed: 2011-02-16

#include <dolfin/function/Function.h>
#include <dolfin/adaptivity/AdaptiveVariationalSolver.h>
#include "Form.h"
#include "LinearVariationalSolver.h"
#include "NonlinearVariationalSolver.h"
#include "VariationalProblem.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
VariationalProblem::VariationalProblem(const Form& form_0,
                                       const Form& form_1)
  : Hierarchical<VariationalProblem>(*this),
    _is_nonlinear(extract_is_nonlinear(form_0, form_1)),
    _linear_form(extract_linear_form(form_0, form_1)),
    _bilinear_form(extract_bilinear_form(form_0, form_1)),
    _cell_domains(0),
    _exterior_facet_domains(0),
    _interior_facet_domains(0)
{
  // Initialize parameters
  init_parameters();
}
//-----------------------------------------------------------------------------
VariationalProblem::VariationalProblem(const Form& form_0,
                                       const Form& form_1,
                                       const BoundaryCondition& bc)
  : Hierarchical<VariationalProblem>(*this),
    _is_nonlinear(extract_is_nonlinear(form_0, form_1)),
    _linear_form(extract_linear_form(form_0, form_1)),
    _bilinear_form(extract_bilinear_form(form_0, form_1)),
    _cell_domains(0),
    _exterior_facet_domains(0),
    _interior_facet_domains(0)
{
  // Initialize parameters
  init_parameters();

  // Store boundary conditions
  _bcs.push_back(reference_to_no_delete_pointer(bc));
}
//-----------------------------------------------------------------------------
VariationalProblem::VariationalProblem(const Form& form_0,
                                       const Form& form_1,
                                       const std::vector<const BoundaryCondition*>& bcs)
  : Hierarchical<VariationalProblem>(*this),
    _is_nonlinear(extract_is_nonlinear(form_0, form_1)),
    _linear_form(extract_linear_form(form_0, form_1)),
    _bilinear_form(extract_bilinear_form(form_0, form_1)),
    _cell_domains(0),
    _exterior_facet_domains(0),
    _interior_facet_domains(0)
{
  // Initialize parameters
  init_parameters();

  // Store boundary conditions
  for (uint i = 0; i < bcs.size(); i++)
    _bcs.push_back(reference_to_no_delete_pointer(*bcs[i]));
}
//-----------------------------------------------------------------------------
VariationalProblem::VariationalProblem(const Form& form_0,
                                       const Form& form_1,
                                       const std::vector<const BoundaryCondition*>& bcs,
                                       const MeshFunction<uint>* cell_domains,
                                       const MeshFunction<uint>* exterior_facet_domains,
                                       const MeshFunction<uint>* interior_facet_domains)
  : Hierarchical<VariationalProblem>(*this),
    _is_nonlinear(extract_is_nonlinear(form_0, form_1)),
    _linear_form(extract_linear_form(form_0, form_1)),
    _bilinear_form(extract_bilinear_form(form_0, form_1)),
    _cell_domains(cell_domains),
    _exterior_facet_domains(exterior_facet_domains),
    _interior_facet_domains(interior_facet_domains)
{
  // Initialize parameters
  init_parameters();

  // Store boundary conditions
  for (uint i = 0; i < bcs.size(); i++)
    _bcs.push_back(reference_to_no_delete_pointer(*bcs[i]));
}
//-----------------------------------------------------------------------------
VariationalProblem::VariationalProblem(boost::shared_ptr<const Form> form_0,
                                       boost::shared_ptr<const Form> form_1,
                                       std::vector<boost::shared_ptr<const BoundaryCondition> > bcs,
                                       const MeshFunction<uint>* cell_domains,
                                       const MeshFunction<uint>* exterior_facet_domains,
                                       const MeshFunction<uint>* interior_facet_domains)
  : Hierarchical<VariationalProblem>(*this),
    _is_nonlinear(extract_is_nonlinear(form_0, form_1)),
    _linear_form(extract_linear_form(form_0, form_1)),
    _bilinear_form(extract_bilinear_form(form_0, form_1)),
    _bcs(bcs),
    _cell_domains(cell_domains),
    _exterior_facet_domains(exterior_facet_domains),
    _interior_facet_domains(interior_facet_domains)
{
  // Initialize parameters
  init_parameters();
}
//-----------------------------------------------------------------------------
VariationalProblem::~VariationalProblem()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void VariationalProblem::solve(Function& u) const
{
  if (_is_nonlinear)
    NonlinearVariationalSolver::solve(u, *this, parameters("solver"));
  else
    LinearVariationalSolver::solve(u, *this, parameters("solver"));
}
//-----------------------------------------------------------------------------
void VariationalProblem::solve(Function& u0, Function& u1) const
{
  // Create function
  Function u(trial_space());

  // Solve variational problem
  solve(u);

  // Extract subfunctions
  u0 = u[0];
  u1 = u[1];
}
//-----------------------------------------------------------------------------
void VariationalProblem::solve(Function& u0, Function& u1, Function& u2) const
{
  // Create function
  Function u(trial_space());

  // Solve variational problem
  solve(u);

  // Extract subfunctions
  u0 = u[0];
  u1 = u[1];
  u2 = u[2];
}
//-----------------------------------------------------------------------------
void VariationalProblem::solve(Function& u,
                               const double tolerance,
                               GoalFunctional& M) const
{
  // Call adaptive solver
  AdaptiveVariationalSolver::solve(u, *this, tolerance, M, parameters("adaptivity"));
}
//-----------------------------------------------------------------------------
void VariationalProblem::solve(Function& u,
                               const double tolerance,
                               Form& M,
                               ErrorControl& ec) const
{
  // Call adaptive solver
  AdaptiveVariationalSolver::solve(u, *this, tolerance, M, ec, parameters("adaptivity"));
}
//-----------------------------------------------------------------------------
const bool VariationalProblem::is_nonlinear() const
{
  return _is_nonlinear;
}
//-----------------------------------------------------------------------------
const FunctionSpace& VariationalProblem::trial_space() const
{
  assert(_bilinear_form);
  return *_bilinear_form->function_space(1);
}
//-----------------------------------------------------------------------------
const FunctionSpace& VariationalProblem::test_space() const
{
  assert(_bilinear_form);
  return *_bilinear_form->function_space(0);
}
//-----------------------------------------------------------------------------
const Form& VariationalProblem::bilinear_form() const
{
  assert(_bilinear_form);
  return *_bilinear_form;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const dolfin::Form>
VariationalProblem::bilinear_form_shared_ptr() const
{
  return _bilinear_form;
}
//-----------------------------------------------------------------------------
const Form& VariationalProblem::linear_form() const
{
  assert(_linear_form);
  return *_linear_form;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const dolfin::Form>
VariationalProblem::linear_form_shared_ptr() const
{
  return _linear_form;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const dolfin::Form>
VariationalProblem::form_0_shared_ptr() const
{
  return (_is_nonlinear ? _linear_form : _bilinear_form);
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const dolfin::Form>
VariationalProblem::form_1_shared_ptr() const
{
  return (_is_nonlinear ? _bilinear_form : _linear_form);
}
//-----------------------------------------------------------------------------
const std::vector<const BoundaryCondition*> VariationalProblem::bcs() const
{
  std::vector<const BoundaryCondition*> bcs;
  for (uint i = 0; i < _bcs.size(); i++)
    bcs.push_back(_bcs[i].get());
  return bcs;
}
//-----------------------------------------------------------------------------
const std::vector<boost::shared_ptr<const BoundaryCondition> >
VariationalProblem::bcs_shared_ptr() const
{
  return _bcs;
}
//-----------------------------------------------------------------------------
const MeshFunction<dolfin::uint>* VariationalProblem::cell_domains() const
{
  return _cell_domains;
}
//-----------------------------------------------------------------------------
const MeshFunction<dolfin::uint>* VariationalProblem::exterior_facet_domains() const
{
  return _exterior_facet_domains;
}
//-----------------------------------------------------------------------------
const MeshFunction<dolfin::uint>* VariationalProblem::interior_facet_domains() const
{
  return _interior_facet_domains;
}
//-----------------------------------------------------------------------------
bool VariationalProblem::extract_is_nonlinear(const Form& form_0,
                                              const Form& form_1)
{
  return extract_is_nonlinear(reference_to_no_delete_pointer(form_0),
                              reference_to_no_delete_pointer(form_1));
}
//-----------------------------------------------------------------------------
bool VariationalProblem::extract_is_nonlinear(boost::shared_ptr<const Form> form_0,
                                              boost::shared_ptr<const Form> form_1)
{
  if (form_0->rank() == 1 && form_1->rank() == 2)
    return true;

  if (form_0->rank() == 2 && form_1->rank() == 1)
    return false;

  form_error();
  return true;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const Form>
VariationalProblem::extract_linear_form(const Form& form_0,
                                        const Form& form_1)
{
  return extract_linear_form(reference_to_no_delete_pointer(form_0),
                             reference_to_no_delete_pointer(form_1));
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const Form>
VariationalProblem::extract_linear_form(boost::shared_ptr<const Form> form_0,
                                        boost::shared_ptr<const Form> form_1)
{
  if (form_0->rank() == 1 && form_1->rank() == 2)
    return form_0;

  if (form_0->rank() == 2 && form_1->rank() == 1)
    return form_1;

  form_error();
  return form_0;
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const Form>
VariationalProblem::extract_bilinear_form(const Form& form_0,
                                          const Form& form_1)
{
  return extract_bilinear_form(reference_to_no_delete_pointer(form_0),
                               reference_to_no_delete_pointer(form_1));
}
//-----------------------------------------------------------------------------
boost::shared_ptr<const Form>
VariationalProblem::extract_bilinear_form(boost::shared_ptr<const Form> form_0,
                                          boost::shared_ptr<const Form> form_1)
{
  if (form_0->rank() == 2 && form_1->rank() == 1)
    return form_0;

  if (form_0->rank() == 1 && form_1->rank() == 2)
    return form_1;

  form_error();
  return form_0;
}
//-----------------------------------------------------------------------------
void VariationalProblem::init_parameters()
{
  // Set default parameters
  parameters = default_parameters();

  // Add solver parameters (linear or nonlinear)
  if (_is_nonlinear)
  {
    Parameters p = NonlinearVariationalSolver::default_parameters();
    p.rename("solver");
    parameters.add(p);

  }
  else
  {
    Parameters p = LinearVariationalSolver::default_parameters();
    p.rename("solver");
    parameters.add(p);
  }

  // Add adaptivity parameters (always)
  Parameters p = AdaptiveVariationalSolver::default_parameters();
  p.rename("adaptivity");
  parameters.add(p);
}
//-----------------------------------------------------------------------------
void VariationalProblem::form_error()
{
  info("A variational problem must be specified as a pair of forms:");
  info("  (1) F, J (nonlinear problem where F is linear and J = F' is bilinear), or");
  info("  (2) a, L (linear problem where a is bilinear and L is linear).");
  error("Variational problem is not correctly formulated.");
}
//-----------------------------------------------------------------------------
