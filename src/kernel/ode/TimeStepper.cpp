// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <dolfin/dolfin_log.h>
#include <dolfin/ODE.h>
#include <dolfin/RHS.h>
#include <dolfin/Partition.h>
#include <dolfin/TimeSlabData.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/TimeStepper.h>

using namespace dolfin;

void storeSolution(TimeSlab &slab, TimeSlabData &data,
		   std::vector<std::pair<real, Vector> > &solution)
{
  Vector Ui(data.size());

  if(slab.timeslabs.size() == 0)
  {
    real t = slab.endtime();

    for (int i = 0; i < data.size(); i++)
    {
      Component &c = data.component(i);

      real value = c(t);
      dolfin::cout << "U(" << t << ", " << i << "): " << value << dolfin::endl;

      Ui(i) = value;
    }

    solution.push_back(std::pair<real, Vector>(t, Ui));
    
  }
  else
  {
    for(std::vector<TimeSlab *>::iterator it = slab.timeslabs.begin();
	it != slab.timeslabs.end(); it++)
    {
      TimeSlab *s = *it;

      storeSolution(*s, data, solution);
    }
  }
}


//-----------------------------------------------------------------------------
void TimeStepper::solve(ODE& ode, real t0, real t1)
{
  // Get size of the system
  int N = ode.size();

  // Create time slab data
  TimeSlabData data(ode);

  // Create partition
  Partition partition(N, 0.1);

  // Create right-hand side
  RHS f(ode, data);

  // Temporary way of storing solution
  std::vector<std::pair<real, Vector> > solution;
  Vector Ui(f.size());
  solution.push_back(std::pair<real, Vector>(t0, ode.u0));
  
  // Start time
  real t = t0;

  // The time-stepping loop
  Progress p("Time-stepping");
  while ( true ) {

    dolfin_debug("stepping2");

    // Create time slab
    TimeSlab timeslab(t, t1, f, data, partition, 0);

    dolfin_debug2("Built Slab: %lf-%lf",
		  timeslab.starttime(), timeslab.endtime());

    // Iterate a couple of times on the time slab
    for (int i = 0; i < 3; i++)
      timeslab.update(f, data);

    // Store solution
    storeSolution(timeslab, data, solution);

    // Check if we are done
    if ( timeslab.finished() )
      break;

    // Update time
    t = timeslab.endtime();

    // Update partition with new time steps
    //partition.update(data, 0);


    /*
    dolfin_debug("solution");
    for (int i = 0; i < data.size(); i++)
    {
      Component &c = data.component(i);

      real value = c(t);
      dolfin::cout << "U(" << t << ", " << i << "): " << value << dolfin::endl;

      Ui(i) = value;
    }

    solution.push_back(std::pair<real, Vector>(t, Ui));
    */


    data.shift();
  }

  // Temporary way of storing solution

  std::ofstream os;
  os.open("U.m", std::ios::out);

  typedef std::vector<std::pair<real, Vector> >::iterator dataiterator;

  os << "t" << " = [" << std::endl;

  for(dataiterator i = solution.begin(); i != solution.end(); i++)
  {
    real &arg = (*i).first;

    os << arg << "; ";
  }
  os << "]" << std::endl;


  os << "U" << " = [" << std::endl;
  for(dataiterator i = solution.begin(); i != solution.end(); i++)
  {
    Vector &res = (*i).second;

    os << "[";
    for(int k = 0; k < res.size(); k++)
    {
      os << res(k) << ";";
    }
    os << "]";
    os << " ";
  }
  os << "]" << std::endl;

}
//-----------------------------------------------------------------------------
