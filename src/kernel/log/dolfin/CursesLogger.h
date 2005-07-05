// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-03-26
// Last changed: 2005

#ifndef __CURSES_LOGGER_H
#define __CURSES_LOGGER_H

#ifndef NO_CURSES

#include <curses.h>

#include <dolfin/constants.h>
#include <dolfin/Buffer.h>
#include <dolfin/Progress.h>
#include <dolfin/GenericLogger.h>

namespace dolfin {
  
  class CursesLogger : public GenericLogger {
  public:

    CursesLogger();
    ~CursesLogger();
    
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
    void progress_flush  ();

  private:

    enum State { RUNNING, PAUSED, ERROR, ABOUT, HELP, FINISHED, QUIT };

    bool running;  // True while running, will remain false when first set to false
    bool waiting;  // True if waiting for input, ignore alarm
    bool updating; // True if we are currently updating, ignore extra calls

    State state;      // State (what to display)

    int lines;        // Number of lines
    int cols;         // Number of columns
    int offset;       // Start position for buffer (depends on the number of progress bars)

    Progress** pbars; // List of progress bars
    int* ptimes;      // Remaining time to display progress bars
    WINDOW *win;      // Pointer to the terminal
    Buffer buffer;    // Buffer
    char* guiinfo;    // Message from the curses interface (not program)
    char* tmp;        // Temporary storage

    void initColors();

    void updateInternal();

    void updateRunning (char c);
    void updatePaused  (char c);
    void updateError   (char c);
    void updateAbout   (char c);
    void updateHelp    (char c);
    void updateFinished(char c);

    void killProgram();
    
    void setSignals();
    
    void setInfo(const char* msg);
    
    void clearLines();
    void clearBuffer();
    void clearLine(int line, int col);

    bool getYesNo();
    void getAnyKey();
    
    void drawTitle();
    void drawProgress();
    void drawBuffer();
    void drawAbout();
    void drawHelp();
    void drawInfo();
    void drawCommands();
    
    void redraw();

    void indent(int _level);
    
  };
  
}

#endif

#endif
