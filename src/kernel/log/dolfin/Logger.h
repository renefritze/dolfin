// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Thanks to Jim Tilander for many helpful hints.

#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdarg.h>
#include <dolfin/constants.h>
#include <dolfin/GenericLogger.h>

namespace dolfin {

  class Logger {
  public:
	 
	 Logger();
	 ~Logger();

	 void info    (const char* msg);
	 void info    (const char* format, va_list aptr);
	 
	 void debug   (const char* file, unsigned long line, const char* function, const char* format, ...);
	 void warning (const char* file, unsigned long line, const char* function, const char* format, ...);
	 void error   (const char* file, unsigned long line, const char* function, const char* format, ...);

	 void progress(const char* title, const char* label, real p);
	 
	 void start();
	 void end();
	 
  private:

	 GenericLogger* log;
	 char* buffer;
	 char* location;
	 
  };

}
  
#endif
