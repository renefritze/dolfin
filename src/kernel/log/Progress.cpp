// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Thanks to Jim Tilander for many helpful hints.

#include <stdio.h>

#include <dolfin/LogManager.h>
#include <dolfin/LoggerMacros.h>
#include <dolfin/Progress.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Progress::Progress(const char* title, int n)
{
  if ( n <= 0 )
	 dolfin_error("Number of steps for progress session must be positive.");

  this->title = new char[DOLFIN_WORDLENGTH];
  this->label = new char[DOLFIN_WORDLENGTH];

  sprintf(this->title, "%s", title);
  sprintf(this->label, "%s", "");

  p0 = 0.0;
  p1 = 0.0;
  
  i = 0;
  this->n = n;
}
//-----------------------------------------------------------------------------
Progress::Progress(const char* title)
{
  this->title = new char[DOLFIN_WORDLENGTH];
  this->label = new char[DOLFIN_WORDLENGTH];
  
  sprintf(this->title, "%s", title);
  sprintf(this->label, "%s", "");

  p0 = 0.0;
  p1 = 0.0;

  i = 0;
  n = 0;
}
//-----------------------------------------------------------------------------
Progress::~Progress()
{
  if ( title )
	 delete [] title;
  title = 0;

  if ( label )
	 delete [] label;
  label = 0;
}
//-----------------------------------------------------------------------------
void Progress::operator=(int i)
{
  if ( n == 0 )
	 dolfin_error("Cannot specify step number for progress session with unknown number of steps.");

  p1 = checkBounds(i);
  update();  
}
//-----------------------------------------------------------------------------
void Progress::operator=(real p)
{
  if ( n != 0 )
	 dolfin_error("Cannot specify value for progress session with given number of steps.");

  p1 = checkBounds(p);
  update();
}
//-----------------------------------------------------------------------------
void Progress::operator++()
{
  if ( n == 0 )
	 dolfin_error("Cannot step progress for session with unknown number of steps.");

  if ( i < (n-1) )
	 i++;

  p1 = checkBounds(i);  
  update();
}
//-----------------------------------------------------------------------------
void Progress::operator++(int)
{
  if ( n == 0 )
	 dolfin_error("Cannot step progress for session with unknown number of steps.");
  
  if ( i < (n-1) )
	 i++;

  p1 = checkBounds(i);
  update();
}
//-----------------------------------------------------------------------------
void Progress::update(int i, const char* format, ...)
{
  if ( n == 0 )
	 dolfin_error("Cannot specify step number for progress session with unknown number of steps.");

  va_list aptr;
  va_start(aptr, format);
  vsprintf(label, format, aptr);
  va_end(aptr);

  p1 = checkBounds(i);
  update();
}
//-----------------------------------------------------------------------------
void Progress::update(real p, const char* format, ...)
{
  if ( n != 0 )
	 dolfin_error("Cannot specify value for progress session with given number of steps.");

  va_list aptr;
  va_start(aptr, format);
  vsprintf(label, format, aptr);
  va_end(aptr);
  
  p1 = checkBounds(p);
  update();
}
//-----------------------------------------------------------------------------
real Progress::checkBounds(int i)
{
  if ( i < 0 )
	 return 0.0;
  if ( i >= n )
	 return 1.0;
  return ((real) i) / ((real) n);
}
//-----------------------------------------------------------------------------
real Progress::checkBounds(real p)
{
  if ( p > 1.0 )
	 return 1.0;
  if ( p < 0.0 )
	 return 0.0;
  return p;
}
//-----------------------------------------------------------------------------
void Progress::update()
{
  // Only update when the increase is significant
  if ( (p1 - p0) < DOLFIN_PROGRESS_STEP )
	 return;

  LogManager::log.progress(title, label, p1);
  p0 = p1;
}
//-----------------------------------------------------------------------------
