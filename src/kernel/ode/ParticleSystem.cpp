// Copyright (C) 2004-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-01-19
// Last changed: 2005

#include <cmath>
#include <dolfin/dolfin_log.h>
#include <dolfin/ParticleSystem.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
ParticleSystem::ParticleSystem(unsigned int n, real T, unsigned int dim) : 
  ODE(2*dim*n, T), n(n), dim(dim), offset(0), u(0)
{
  // Check dimension
  if ( dim == 0 )
    dolfin_error("Dimension must be at least 1 for a particle system.");
  if ( dim > 3 )
    dolfin_error("Maximum allowed dimension is 3 for a particle system.");
  
  // Compute offset
  offset = dim*n;

  dolfin_info("Creating particle system of size %d (%d particles).", N, n);
}
//-----------------------------------------------------------------------------
ParticleSystem::~ParticleSystem()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
real ParticleSystem::x0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::y0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::z0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::vx0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::vy0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::vz0(unsigned int i)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::Fx(unsigned int i, real t)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::Fy(unsigned int i, real t)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::Fz(unsigned int i, real t)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::mass(unsigned int i, real t)
{
  return 1.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::k(unsigned int i)
{
  return default_timestep;
}
//-----------------------------------------------------------------------------
real ParticleSystem::u0(unsigned int i)
{
  if ( i < offset )
  {
    switch (i % dim) {
    case 0:
      return x0(i/dim);
      break;
    case 1:
      return y0(i/dim);
      break;
    case 2:
      return z0(i/dim);
      break;
    default:
      dolfin_error("Illegal dimension.");
    }
  }
  else
  {
    switch (i % dim) {
    case 0:
      return vx0((i-offset)/dim);
      break;
    case 1:
      return vy0((i-offset)/dim);
      break;
    case 2:
      return vz0((i-offset)/dim);
      break;
    default:
      dolfin_error("Illegal dimension.");
    }
  }  

  return 0.0;
}
//-----------------------------------------------------------------------------
void ParticleSystem::u0(uBlasVector& u)
{
  dolfin_error("Not implemented, needs to be updated for new ODE solver interface.");
}
//-----------------------------------------------------------------------------
real ParticleSystem::f(const real u[], real t, unsigned int i)
{
  // Return velocity
  if ( i < offset )
    return u[offset + i];

  // Subtract offset
  i -= offset;

  // Save pointer to solution vector
  this->u = u;

  // Compute force
  switch (i % dim) {
  case 0:
    return Fx(i/dim, t) / mass(i/dim, t);
    break;
  case 1:
    return Fy(i/dim, t) / mass(i/dim, t);
    break;
  case 2:
    return Fz(i/dim, t) / mass(i/dim, t);
    break;
  default:
    dolfin_error("Illegal dimension.");
  }
  
  return 0.0;
}
//-----------------------------------------------------------------------------
real ParticleSystem::timestep(unsigned int i)
{
  if ( i >= offset )
    i -= offset;

  i /= dim;

  return k(i);
}
//-----------------------------------------------------------------------------
real ParticleSystem::dist(unsigned int i, unsigned int j) const
{
  real dx = x(i) - x(j);
  real r = dx*dx;

  if ( dim > 1 )
  {
    real dy = y(i) - y(j);
    r += dy*dy;
  }

  if ( dim > 2 )
  {
    real dz = z(i) - z(j);
    r += dz*dz;
  }
 
  return sqrt(r);
}
//-----------------------------------------------------------------------------
