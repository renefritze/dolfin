// Copyright (C) 2003-2011 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Thanks to Jim Tilander for many helpful hints.
//
// Modified by Ola Skavhaug, 2007, 2009.
// Modified by Garth N. Wells, 2009.
//
// First added:  2003-03-13
// Last changed: 2011-01-21

#include <cstdarg>
#include <cstdlib>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>

#include <boost/scoped_array.hpp>
#include <dolfin/common/types.h>
#include <dolfin/common/constants.h>
#include <dolfin/common/Variable.h>
#include <dolfin/common/MPI.h>
#include <dolfin/parameter/Parameters.h>
#include "LogManager.h"
#include "log.h"

using namespace dolfin;

static boost::scoped_array<char> buffer(0);
static unsigned int buffer_size= 0;

// Buffer allocation
void allocate_buffer(std::string msg)
{
  // va_list, start, end require a char pointer of fixed size so we
  // need to allocate the buffer here. We allocate twice the size of
  // the format string and at least DOLFIN_LINELENGTH. This should be
  // ok in most cases.
  unsigned int new_size = std::max(static_cast<unsigned int>(2*msg.size()),
                                   static_cast<unsigned int>(DOLFIN_LINELENGTH));
  //static_cast<unsigned int>(DOLFIN_LINELENGTH));
  if (new_size > buffer_size)
  {
    buffer.reset(new char[new_size]);
    buffer_size = new_size;
  }
}

// Macro for parsing arguments
#define read(buffer, msg) \
  allocate_buffer(msg); \
  va_list aptr; \
  va_start(aptr, msg); \
  vsnprintf(buffer, buffer_size, msg.c_str(), aptr); \
  va_end(aptr);

//-----------------------------------------------------------------------------
void dolfin::info(std::string msg, ...)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.info(buffer.get());
}
//-----------------------------------------------------------------------------
void dolfin::info(int log_level, std::string msg, ...)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.info(buffer.get(), log_level);
}
//-----------------------------------------------------------------------------
void dolfin::info(const Variable& variable, bool verbose)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  info(variable.str(verbose));
}
//-----------------------------------------------------------------------------
void dolfin::info(const Parameters& parameters, bool verbose)
{
  // Need separate function for Parameters since we can't make Parameters
  // a subclass of Variable (gives cyclic dependencies)

  if (!LogManager::logger.is_active())
    return; // optimization
  info(parameters.str(verbose));
}
//-----------------------------------------------------------------------------
void dolfin::info_stream(std::ostream& out, std::string msg)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  std::ostream& old_out = LogManager::logger.get_output_stream();
  LogManager::logger.set_output_stream(out);
  LogManager::logger.info(msg);
  LogManager::logger.set_output_stream(old_out);
}
//-----------------------------------------------------------------------------
void dolfin::info_underline(std:: string msg, ...)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.info_underline(buffer.get());
}
//-----------------------------------------------------------------------------
void dolfin::warning(std::string msg, ...)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.warning(buffer.get());
}
//-----------------------------------------------------------------------------
void dolfin::error(std::string msg, ...)
{
  read(buffer.get(), msg);
  LogManager::logger.error(buffer.get());
}
//-----------------------------------------------------------------------------
void dolfin::begin(std::string msg, ...)
{
  if (!LogManager::logger.is_active())
    return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.begin(buffer.get());
}
//-----------------------------------------------------------------------------
void dolfin::begin(int log_level, std::string msg, ...)
{
  if (!LogManager::logger.is_active()) return; // optimization
  read(buffer.get(), msg);
  LogManager::logger.begin(buffer.get(), log_level);
}
//-----------------------------------------------------------------------------
void dolfin::end()
{
  if (!LogManager::logger.is_active())
    return; // optimization
  LogManager::logger.end();
}
//-----------------------------------------------------------------------------
void dolfin::logging(bool active)
{
  LogManager::logger.logging(active);
}
//-----------------------------------------------------------------------------
void dolfin::set_log_level(int level)
{
  LogManager::logger.set_log_level(level);
}
//-----------------------------------------------------------------------------
void dolfin::set_output_stream(std::ostream& out)
{
  LogManager::logger.set_output_stream(out);
}
//-----------------------------------------------------------------------------
int dolfin::get_log_level()
{
  return LogManager::logger.get_log_level();
}
//-----------------------------------------------------------------------------
void dolfin::summary(bool reset)
{
  // Optimization
  if (!LogManager::logger.is_active())
    return;

  // Only print summary for process 0
  if (MPI::process_number() != 0)
    return;

  LogManager::logger.summary(reset);
}
//-----------------------------------------------------------------------------
double dolfin::timing(std::string task, bool reset)
{
  return LogManager::logger.timing(task, reset);
}
//-----------------------------------------------------------------------------
void dolfin::not_working_in_parallel(std::string what)
{
  if (MPI::num_processes() > 1)
  {
    if (MPI::process_number() == 0)
    {
      std::string url("https://bugs.launchpad.net/dolfin");
      std::cout << what << " is not (yet) working in parallel. Consider filing a bug report at "
                << url << std::endl;
    }
    exit(10);
  }
}
//-----------------------------------------------------------------------------
void dolfin::check_equal(uint value,
                         uint valid_value,
                         std::string task,
                         std::string value_name)
{
  if (value == valid_value)
    return;

  std::stringstream s;
  s << "Unable to " << task
    << ". Reason: Illegal value " << value
    << " for " << value_name
    << ", should be " << valid_value
    << ".";

  error(s.str());
}
//-----------------------------------------------------------------------------
void dolfin::__debug(std::string file, unsigned long line,
                     std::string function, std::string format, ...)
{
  read(buffer.get(), format);
  std::ostringstream ost;
  ost << file << ":" << line << " in " << function << "()";
  std::string msg = std::string(buffer.get()) + " [at " + ost.str() + "]";
  LogManager::logger.__debug(msg);
}
//-----------------------------------------------------------------------------
