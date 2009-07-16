// Copyright (C) 2008-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Benjamin Kehlet, 2008.
//
// First added:  2008-10-02
// Last changed: 2009-05-31
//
// This file provides utilities for working with variable-precision
// floating-point numbers. It defines a datatype "real" which defaults
// to double, but may be changed to higher-precision representations,
// for example using GMP.

#ifndef __REAL_H
#define __REAL_H

#ifdef HAS_GMP
#include <gmpxx.h>
#endif

#include "types.h"
#include <dolfin/log/log.h>
#include <cmath>
#include <iostream>

namespace dolfin
{

  // Real type
#ifdef HAS_GMP
  typedef mpf_class real;
#else
  typedef double real;
#endif

  // Store the epsilon value
  extern real _real_epsilon;

  // Set precision and initialize extended precision
  void dolfin_set_precision(uint prec);

  //Store the epsilon value
  extern real _real_epsilon;
  extern bool _real_initialized;

  // Convert to double (if necessary)
  inline double to_double(real x)
  {
#ifdef HAS_GMP
    return x.get_d();
#else
    return x;
#endif
  }

  // Convert to real (if necessary)
  inline real to_real(double x)
  { return real(x); }

  // Absolute value
  inline real real_abs(real x)
  { return x >= 0.0 ? x : -1.0*x; }

  // Maximum
  inline real real_max(const real& x, const real& y)
  { return x > y ? x : y; }

  // Minimum
  inline real real_min(const real& x, const real& y)
  { return x < y ? x : y; }

  // Power function
  inline real real_pow(const real& x, uint y)
  {
#ifdef HAS_GMP
    real res;
    mpf_pow_ui(res.get_mpf_t(), x.get_mpf_t(), y);
    return res;
#else
    return std::pow(to_double(x), to_double(y));
#endif
  }
  
  // Power function
  inline real pow(const real& x, const real& y)
  {
#ifdef HAS_GMP
    error("Multiprecision pow function not implemented.");
    return 0.0;
#else
    return std::pow(to_double(x), to_double(y));
#endif
  }

  inline int isnormal(const real& x) {
#ifdef HAS_GMP
    // NOTE: Not implemented. 
    // GMP has no notion of infinity or NaN
    return 1;
#else
    return std::isnormal(to_double(x));
#endif
  }

  // Power function (note: not full precision!)
  inline real real_pow(const real& x, const real& y)
  { return std::pow(to_double(x), to_double(y)); }

 /// Square root
  real real_sqrt(const real& a);

  /// Compute pi
  real real_pi();

  /// Compute matrix exponential using Pade approximation og degree p
  void real_mat_exp(uint n, real* res, const real* A, const uint p=6);

  double real_frexp(int* exp, const real& x);
  /// Exponential function (note: not full precision!)
  inline real real_exp(real x)
  { return to_real(exp(to_double(x))); }

  /// Logarithmic function (note: not full precision!)
  inline real real_log(const real& x)
  { return to_real(log(to_double(x))); }

  // Get computed epsilon
  inline const real& real_epsilon() {return _real_epsilon;}

  // Get precision in decimal digits
  // Usefull when writing with full precision to text (ascii) files
  int real_decimal_prec();

  // Set array to given array (copy values)
  inline void real_set(uint n, real* x, const real* y)
  { for (uint i = 0; i < n; i++) x[i] = y[i]; }

  // Set array to given number
  inline void real_set(uint n, real* x, const real& value)
  { for (uint i = 0; i < n; i++) x[i] = value; }

  // Set array to zero
  inline void real_zero(uint n, real* x)
  { for (uint i = 0; i < n; i++) x[i] = 0.0; }

  // Add array, x += y
  inline void real_add(uint n, real* x, const real* y)
  { for (uint i = 0; i < n; i++) x[i] += y[i]; }

  // Subtract array, x -= y
  inline void real_sub(uint n, real* x, const real* y)
  { for (uint i = 0; i < n; i++) x[i] -= y[i]; }

  // Add multiple of array, x += a*y (AXPY operation)
  inline void real_axpy(uint n, real* x, const real& a, const real* y)
  { for (uint i = 0; i < n; i++) x[i] += a*y[i]; }

  // Multiply array with given number
  inline void real_mult(uint n, real* x, const real& a)
  { for (uint i = 0; i < n; i++) x[i] *= a; }

  // Compute inner products of array
  inline real real_inner(uint n, const real* x, const real* y)
  { real sum = 0.0; for (uint i = 0; i < n; i++) sum += x[i]*y[i]; return sum; }

  // Compute maximum absolute value of array
  inline real real_max_abs(uint n, const real* x)
  { real _max = 0.0; for (uint i = 0; i < n; i++) _max = real_max(real_abs(x[i]), _max); return _max; }

  // Matrix multiplication
  inline void real_mat_prod(uint n, real* res, const real* A, const real* B) 
  {
    //Compute Precond*A and Precond*b with extended precision
    for (uint i=0; i < n; ++i)
    {
      for (uint j = 0; j < n; ++j)
      {
	res[i + n*j] = 0.0;
	for (uint k = 0; k < n; ++k)
	{
	  res[i+n*j] += A[i + n*k]* B[k + n*j];
	}
      }
    }
  }


  // Matrix multiplication A = A * B
  inline void real_mat_prod_inplace(uint n, real* A, const real* B)
  {
    real tmp[n*n];
    real_set(n*n, tmp, A);
    real_mat_prod(n, A, tmp, B);
  }

  // Set A (a multippel of) to identity
  inline void real_identity(uint n, real* A, real value=1.0)
  {
    real_zero(n*n, A);

    for (uint i=0; i < n; ++i)
      A[i*n+i] = value;
  }
}

#endif
