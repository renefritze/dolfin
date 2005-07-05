// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-03-13
// Last changed: 2005

#ifndef __TERMINAL_LOGGER_H
#define __TERMINAL_LOGGER_H

#include <dolfin/constants.h>
#include <dolfin/GenericLogger.h>

namespace dolfin {

  class TerminalLogger : public GenericLogger {
  public:
	 
    TerminalLogger();
    ~TerminalLogger();
    
    void info    (const char* msg);
    void debug   (const char* msg, const char* location);
    void warning (const char* msg, const char* location);
    void error   (const char* msg, const char* location);
    void dassert (const char* msg, const char* location);
    void progress(const char* title, const char* label, real p);
    
    void update();
    void quit();
    bool finished();
    
    void progress_add    (Progress* p);
    void progress_remove (Progress *p);

  private:

    void indent();
    
  };

}
  
#endif
