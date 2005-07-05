// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005
// Last changed: 2005

#ifndef __HOMOTOPY_H
#define __HOMOTOPY_H

#include <dolfin/constants.h>
#include <dolfin/Array.h>
#include <dolfin/LU.h>
#include <dolfin/Vector.h>

namespace dolfin
{

  class LinearSolver;
  class HomotopyODE;
  class ComplexODE;

  class Homotopy
  {
  public:
    
    /// Create homotopy for system of given size
    Homotopy(uint n);

    /// Destructor
    virtual ~Homotopy();

    /// Solve homotopy
    void solve();

    /// Return array of solutions found
    const Array<complex*>& solutions() const;

    /// Return initial value (solution of G(z) = 0), optional
    virtual complex z0(uint i);

    /// Compute y = F(z)
    virtual void F(const complex z[], complex y[]) = 0;

    /// Compute y = F'(z) x
    virtual void JF(const complex z[], const complex x[], complex y[]) = 0;

    /// Compute y = G(z), optional
    virtual void G(const complex z[], complex y[]);

    /// Compute y = G'(z) x, optional
    virtual void JG(const complex z[], const complex x[], complex y[]);

    /// Modify or substitute found solution (optional)
    virtual void modify(complex z[]);

    /// Check if found solution is correct (optional)
    virtual bool verify(const complex z[]);

    /// Return degree of polynomial F_i(z)
    virtual uint degree(uint i) const = 0;    

    /// Friends
    friend class HomotopyODE;

  protected:
    
    real tol; // Tolerance for Newton's method

  private:

    // Adjusted degree (limited by maxdegree)
    uint adjustedDegree(uint i);

    // Count the number of paths
    uint countPaths();

    // Compute component path numbers from global path number
    void computePath(uint m);

    // Compute solution with Newton's method from current starting point
    bool computeSolution(HomotopyODE& ode);

    // Save solution to file
    void saveSolution();

    // Randomize system G(z) = 0
    void randomize();

    // Evaluate right-hand side
    void feval(Vector& F, ComplexODE& ode);

    uint n;                // Size of system
    uint M;                // Number of paths
    uint maxiter;          // Maximum number of iterations
    uint maxpaths;         // Maximum number of paths
    uint maxdegree;        // Maximum degree for a single equation
    real divtol;           // Tolerance for divergence of homotopy path
    bool monitor;          // True if we should monitor the homotopy
    bool random;           // True if we should choose random initial data
    LinearSolver* solver;  // GMRES solver
    std::string filename;  // Filename for saving solutions  
    uint* mi;              // Array of local path numbers
    complex* ci;           // Array of constants for system G(z) = 0
    complex* tmp;          // Array used for temporary storage
    Vector x;              // Real-valued vector x corresponding to solution z of F(z) = 0
    Array<complex*> zs;    // Array of solutions
    Event degree_adjusted; // Message if degree has to be adjusted

  };

}

#endif
