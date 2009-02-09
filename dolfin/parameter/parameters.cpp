// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2006.
// Modified by Benjamin Kehlet, 2009
//
// First added:  2005-12-19
// Last changed: 2009-02-02

#include <dolfin/log/LogManager.h>
#include "ParameterSystem.h"
#include "parameters.h"

//-----------------------------------------------------------------------------
dolfin::Parameter dolfin::dolfin_get(std::string key)
{
  if (key == "debug level")
    return Parameter(LogManager::logger.getDebugLevel());
  return ParameterSystem::parameters.get(key);
}
//-----------------------------------------------------------------------------
void dolfin::dolfin_set(std::string key, Parameter value)
{
  // Special cases: pass on to log system
  if (key == "debug level")
  {
    LogManager::logger.setDebugLevel(value);
    return;
  }
  else if (key == "output destination")
  {
    LogManager::logger.setOutputDestination(value);
    return;
  }

  ParameterSystem::parameters.set(key, value);
}
//-----------------------------------------------------------------------------
void dolfin::dolfin_set(std::string key, std::ostream& stream)
{
  if (key == "output destination"){
    LogManager::logger.setOutputDestination(stream);
    {
        if (key == "output destination"){
              LogManager::logger.setOutputDestination(stream);
                }
          else
                error("Only key 'output destination' can take a stream as value.");
    }
    
  }
  else
    error("Only key 'output destination' can take a stream as value.");
}
//-----------------------------------------------------------------------------
void dolfin::dolfin_add(std::string key, Parameter value)
{
  ParameterSystem::parameters.add(key, value);
}
//-----------------------------------------------------------------------------
bool dolfin::dolfin_changed(std::string key) 
{
  return ParameterSystem::parameters.changed(key);
}
//-----------------------------------------------------------------------------
