// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ECONOMY_H
#define __ECONOMY_H

#include <dolfin.h>

using namespace dolfin;

/// Base class for economies

class Economy : public Homotopy
{
public:

  Economy(unsigned int m, unsigned int n) : Homotopy(n), m(m), n(n), a(0), w(0), tmp0(0), tmp1(0)
  {
    a = new real * [m];
    w = new real * [m];
    for (unsigned int i = 0; i < m; i++)
    {
      a[i] = new real[n];
      w[i] = new real[n];
      for (unsigned int j = 0; j < n; j++)
      {
	a[i][j] = dolfin::rand();
	w[i][j] = dolfin::rand();
      }
    }

    tmp0 = new complex[m];
    tmp1 = new complex[m];
    for (unsigned int i = 0; i < m; i++)
    {
      tmp0[i] = 0.0;
      tmp1[i] = 0.0;
    }
  }

  ~Economy()
  {
    for (unsigned int i = 0; i < m; i++)
    {
      delete [] a[i];
      delete [] w[i];
    }
    delete [] a;
    delete [] w;
    delete [] tmp0;
    delete [] tmp1;
  }

  void disp()
  {
    cout << "Utility parameters:" << endl;
    for (unsigned int i = 0; i < m; i++)
    {
      cout << "  trader " << i << ":";
      for (unsigned int j = 0; j < n; j++)
	cout << " " << a[i][j];
      cout << endl;
    }

    cout << "Initial endowments:" << endl;
    for (unsigned int i = 0; i < m; i++)
    {
      cout << "  trader " << i << ":";
      for (unsigned int j = 0; j < n; j++)
	cout << " " << w[i][j];
      cout << endl;
    }
  }

  // Number of traders
  unsigned int m;

  // Number of goods
  unsigned int n;
  
  // Matrix of traders' preferences
  real** a;
  
  // Matrix of traders' initial endowments
  real** w;

protected:

  // Compute sum of elements
  complex sum(const complex x[]) const
  {
    complex sum = 0.0;
    for (unsigned int j = 0; j < n; j++)
      sum += x[j];
    return sum;
  }

  // Compute scalar product x . y
  complex dot(const real x[], const complex y[]) const
  {
    complex sum = 0.0;
    for (unsigned int j = 0; j < n; j++)
      sum += x[j] * y[j];
    return sum;
  }

  // Compute special scalar product x . y.^b
  complex bdot(const real x[], const complex y[], real b) const
  {
    complex sum = 0.0;
    for (unsigned int j = 0; j < n; j++)
      sum += x[j] * std::pow(y[j], b);
    return sum;
  }

  // Compute special scalar product x . y. z^b
  complex bdot(const real x[], const complex y[], const complex z[], real b) const
  {
    complex sum = 0.0;
    for (unsigned int j = 0; j < n; j++)
      sum += x[j] * y[j] * std::pow(z[j], b);
    return sum;
  }
  
  // Display values
  void disp(const real x[], const char* name)
  {
    dolfin::cout << name << " = [";
    for (unsigned int j = 0; j < n; j++)
      dolfin::cout << x[j] << " ";
    dolfin::cout << "]" << endl;
  }

  // Display values
  void disp(const complex z[], const char* name)
  {
    dolfin::cout << name << " = [";
    for (unsigned int j = 0; j < n; j++)
      dolfin::cout << z[j] << " ";
    dolfin::cout << "]" << endl;
  }
  
  complex* tmp0; // Temporary storage for scalar products
  complex* tmp1; // Temporary storage for scalar products
  
};

#endif
