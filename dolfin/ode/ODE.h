// Copyright (C) 2003-2009 Johan Jansson and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-10-21
// Last changed: 2009-09-04

#ifndef __ODE_H
#define __ODE_H

#include <dolfin/common/types.h>
#include <dolfin/common/real.h>
#include <dolfin/common/constants.h>
#include <dolfin/log/Event.h>
#include <dolfin/parameter/Parameters.h>
#include "Dependencies.h"
#include "Sample.h"

namespace dolfin
{

  class ODESolution;
  class TimeStepper;

  /// An ODE represents an initial value problem of the form
  ///
  ///     u'(t) = f(u(t), t) on [0, T],
  ///
  ///     u(0)  = u0,
  ///
  /// where u(t) is a vector of length N.
  ///
  /// To define an ODE, a user must create a subclass of ODE and
  /// create the function u0() defining the initial condition, as well
  /// the function f() defining the right-hand side.
  ///
  /// DOLFIN provides two types of ODE solvers: a set of standard
  /// mono-adaptive solvers with equal adaptive time steps for all
  /// components as well as a set of multi-adaptive solvers with
  /// individual and adaptive time steps for the different
  /// components. The right-hand side f() is defined differently for
  /// the two sets of methods, with the multi-adaptive solvers
  /// requiring a component-wise evaluation of the right-hand
  /// side. Only one right-hand side function f() needs to be defined
  /// for use of any particular solver.
  ///
  /// It is also possible to solve implicit systems of the form
  ///
  ///     M(u(t), t) u'(t) = f(u(t),t) on (0,T],
  ///
  ///     u(0)  = u0,
  ///
  /// by setting the option "implicit" to true and defining the
  /// function M().
  ///
  /// Two different solve() functions are provided, one to solve the
  /// ODE on the time interval [0, T], including the solution of a
  /// dual problem for error control:
  ///
  ///     ode.solve();
  ///
  /// Alternatively, a time interval may be given in which case the
  /// solution will be computed in a single sweep over the given time
  /// interval without solution of dual problems:
  ///
  ///     ode.solve(t0, t1);
  ///
  /// This mode allows the state to be specified and retrieved in
  /// between intervals by calling set_state() and get_state().

  class ODE : public Variable
  {
  public:

    /// Create an ODE of size N with final time T
    ODE(uint N, real T);

    /// Destructor
    virtual ~ODE();

    /// Set initial values
    virtual void u0(real* u) = 0;

    /// Evaluate right-hand side y = f(u, t), mono-adaptive version (default, optional)
    virtual void f(const real* u, real t, real* y);

    /// Evaluate right-hand side f_i(u, t), multi-adaptive version (optional)
    virtual real f(const real* u, real t, uint i);

    /// Compute product y = Mx for implicit system (optional)
    virtual void M(const real* x, real* y, const real* u, real t);

    /// Compute product y = Jx for Jacobian J (optional)
    virtual void J(const real* x, real* y, const real* u, real t);

    /// Compute product y = tranpose(J)x for Jacobian J (optional, for dual problem)
    virtual void JT(const real* x, real* y, const real* u, real t);

    /// Compute entry of Jacobian (optional)
    virtual real dfdu(const real* u, real t, uint i, uint j);

    /// Time step to use for the whole system at a given time t (optional)
    virtual real timestep(real t, real k0) const;

    /// Time step to use for a given component at a given time t (optional)
    virtual real timestep(real t, uint i, real k0) const;

    /// Update ODE, return false to stop (optional)
    virtual bool update(const real* u, real t, bool end);

    /// Save sample (optional)
    virtual void save(Sample& sample);

    /// Return number of components N
    uint size() const;

    /// Return current time
    real time() const;

    /// Return real time (might be flipped backwards for dual)
    virtual real time(real t) const;

    /// Return end time (final time T)
    real endtime() const;

    /// Automatically detect sparsity (optional)
    void sparse();

    /// Solve ODE on [0, T]
    void solve();

    /// Solve ODE on [0, T]. Save solution in u
    void solve(ODESolution& u);

    /// Solve ODE on [t0, t1]
    void solve(ODESolution& u, real t0, real t1);

    /// Solve dual problem given an approximate solution u of the primal problem
    void solve_dual(ODESolution& u, ODESolution& z);

    /// Compute stability factors as function of T (including solving the dual problem).
    /// The stability factor is the integral of the norm of the q'th derivative of the dual.
    void analyze_stability(uint q, ODESolution& u);

    /// Compute stability factors as function of T (including solving the dual problem).
    /// The stability factor accounts for stability wrt the discretization scheme.
    void analyze_stability_discretization(ODESolution& u);

    /// Compute stability factors as function of T (including solving the dual problem).
    /// The stability factor accounts for stability wrt the round-off errors.
    void analyze_stability_computation(ODESolution& u);

    /// Compute stability factors as function of T (including solving the dual problem).
    /// The stability factor accounts for stability wrt errors in initial data.
    void analyze_stability_initial(ODESolution& u);

    /// Set state for ODE (only available during interval stepping)
    void set_state(const real* u);

    /// Get state for ODE (only available during interval stepping)
    void get_state(real* u);

    /// Default parameter values
    static Parameters default_parameters()
    {
      Parameters p("ode");

      // FIXME: These parameters need to be cleaned up

      p.add("fixed_time_step", false);
      p.add("solve_dual_problem", false);
      p.add("save_solution", true);
      p.add("save_final_solution", false);
      p.add("adaptive_samples", false);
      p.add("automatic_modeling", false);
      p.add("implicit", false);
      p.add("matrix_piecewise_constant", true);
      p.add("M_matrix_constant", false);
      p.add("monitor_convergence", false);
      p.add("updated_jacobian", false);           // only multi-adaptive Newton
      p.add("diagonal_newton_damping", false);    // only multi-adaptive fixed-point
      p.add("matrix-free_jacobian", true);

      p.add("order", 1);
      p.add("number_of_samples", 100);
      p.add("sample_density", 1);
      p.add("maximum_iterations", 100);
      p.add("maximum_local_iterations", 2);
      p.add("average_samples", 1000);
      p.add("size_threshold", 50);

      p.add("tolerance", 0.1);
      p.add("start_time", 0.0);
      p.add("end_time", 10.0);
      p.add("discrete_tolerance", 0.001);
      p.add("discrete_tolerance_factor", 0.001);
      p.add("discrete_krylov_tolerance_factor", 0.01);
      p.add("initial_time_step", 0.01);
      p.add("maximum_time_step", 0.1);
      p.add("partitioning_threshold", 0.1);
      p.add("interval_threshold", 0.9);
      p.add("safety_factor", 0.9);
      p.add("time_step_conservation", 5.0);
      p.add("sparsity_check_increment", 0.01);
      p.add("average_length", 0.1);
      p.add("average_tolerance", 0.1);
      p.add("fixed-point_damping", 1.0);
      p.add("fixed-point_stabilize", false);
      p.add("fixed-point_stabilization_m", 3);
      p.add("fixed-point_stabilization_l", 4);
      p.add("fixed-point_stabilization_ramp", 2.0);

      p.add("method", "cg");
      p.add("nonlinear_solver", "default");
      p.add("linear_solver", "auto");
      p.add("solution_file_name", "solution.py");
      p.add("number_of_threads", 1);

      return p;
    }

    /// Friends
    friend class Dual;
    friend class RHS;
    friend class TimeSlab;
    friend class TimeSlabJacobian;
    friend class MonoAdaptiveTimeSlab;
    friend class MonoAdaptiveJacobian;
    friend class MultiAdaptiveTimeSlab;
    friend class MultiAdaptiveJacobian;
    friend class MultiAdaptivity;
    friend class NewMultiAdaptiveJacobian;
    friend class MultiAdaptivePreconditioner;
    friend class ReducedModel;
    friend class JacobianMatrix;
    friend class TimeStepper;

  protected:

    // Number of components
    uint N;

    // Current time
    real t;

    // Final time
    real T;

    // Dependencies
    Dependencies dependencies;

    // Transpose of dependencies
    Dependencies transpose;

    // Default time step
    real default_timestep;

    // Time stepper
    TimeStepper* time_stepper;

  private:

    // Temporary vectors used for computing Jacobian
    real* tmp0;
    real* tmp1;

    // Events
    Event not_impl_f;
    Event not_impl_M;
    Event not_impl_J;
    Event not_impl_JT;

  };

}

#endif
