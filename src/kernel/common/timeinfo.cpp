// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <time.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/utils.h>
#include <dolfin/timeinfo.h>

using namespace dolfin;

namespace dolfin
{
  clock_t __tic_time;
  char datestring[DOLFIN_WORDLENGTH];
}

//-----------------------------------------------------------------------------
const char* dolfin::date()
{
  time_t t = time(0);
  sprintf(datestring, "%s", ctime(&t));
  remove_newline(datestring);
  return datestring;
}
//-----------------------------------------------------------------------------
void dolfin::tic()
{
  dolfin::__tic_time = clock();
}
//-----------------------------------------------------------------------------
real dolfin::toc()
{
  clock_t __toc_time = clock();

  real elapsed_time = ((real) (__toc_time - __tic_time)) / CLOCKS_PER_SEC;

  return elapsed_time;
}
//-----------------------------------------------------------------------------
real dolfin::tocd()
{
  real elapsed_time = toc();

  cout << "Elapsed time: " << elapsed_time << " seconds" << endl;

  return elapsed_time;
}
//-----------------------------------------------------------------------------
