// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Thanks to Jim Tilander for many helpful hints.
//
// First added:  2003-03-13
// Last changed: 2005

#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdarg.h>
#include <dolfin/constants.h>
#include <dolfin/GenericLogger.h>

namespace dolfin
{

  class Logger
  {
  public:
    
    Logger();
    ~Logger();

    void info    (const char* msg);
    void info    (const char* format, va_list aptr);

    void debug   (const char* file, unsigned long line, const char* function, const char* format, ...);
    void warning (const char* file, unsigned long line, const char* function, const char* format, ...);
    void error   (const char* file, unsigned long line, const char* function, const char* format, ...);
    void dassert (const char* file, unsigned long line, const char* function, const char* format, ...);
    void progress(const char* title, const char* label, real p);

    void update();
    void quit();
    bool finished();

    void progress_add    (Progress* p);
    void progress_remove (Progress *p);

    void begin();
    void end();

    void active(bool state);

    void init(const char* type);

  private:

    void init();

    GenericLogger* log;
    char* buffer;
    char* location;
    bool state;

  };

}

#endif
