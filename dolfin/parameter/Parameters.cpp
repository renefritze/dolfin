// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Hake, 2009
// Modified by Garth N. Wells, 2009
//
// First added:  2009-05-08
// Last changed: 2009-09-05

#include <sstream>
#include <boost/program_options.hpp>
#include <dolfin/log/log.h>
#include <dolfin/log/LogStream.h>
#include <dolfin/log/Table.h>
#include <dolfin/common/utils.h>
#include "Parameter.h"
#include "Parameters.h"

using namespace dolfin;
namespace po = boost::program_options;

// Typedef of iterators for convenience
typedef std::map<std::string, Parameter*>::iterator parameter_iterator;
typedef std::map<std::string, Parameter*>::const_iterator const_parameter_iterator;
typedef std::map<std::string, Parameters*>::iterator parameter_set_iterator;
typedef std::map<std::string, Parameters*>::const_iterator const_parameter_set_iterator;

//-----------------------------------------------------------------------------
Parameters::Parameters(std::string key) : _key(key)
{
  // Check that key name is allowed
  Parameter::check_key(key);
}
//-----------------------------------------------------------------------------
Parameters::~Parameters()
{
  clear();
}
//-----------------------------------------------------------------------------
Parameters::Parameters(const Parameters& parameters)
{
  *this = parameters;
}
//-----------------------------------------------------------------------------
std::string Parameters::name() const
{
  return _key;
}
//-----------------------------------------------------------------------------
void Parameters::rename(std::string key)
{
  Parameter::check_key(key);
  _key = key;
}
//-----------------------------------------------------------------------------
void Parameters::clear()
{
  // Delete parameters
  for (parameter_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
    delete it->second;
  _parameters.clear();

  // Delete parameter sets
  for (parameter_set_iterator it = _parameter_sets.begin(); it != _parameter_sets.end(); ++it)
    delete it->second;
  _parameter_sets.clear();

  // Reset key
  _key = "";
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, int value)
{
  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new IntParameter(key, value);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, int value,
                        int min_value, int max_value)
{
  // Add parameter
  add(key, value);

  // Set range
  Parameter* p = find_parameter(key);
  assert(p);
  p->set_range(min_value, max_value);
}
//-----------------------------------------------------------------------------
#ifdef HAS_GMP
void Parameters::add(std::string key, real value)
{
  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new RealParameter(key, value);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, real value,
                        real min_value, real max_value)
{
  // Add parameter
  add(key, value);

  // Set range
  Parameter* p = find_parameter(key);
  assert(p);
  p->set_range(min_value, max_value);
}
#endif
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, double value)
{
  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new RealParameter(key, to_real(value));
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, double value,
                        double min_value, double max_value)
{
  // Add parameter
  add(key, to_real(value));

  // Set range
  Parameter* p = find_parameter(key);
  assert(p);
  p->set_range(to_real(min_value), to_real(max_value));
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, std::string value)
{
  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new StringParameter(key, value);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, const char* value)
{
  // This version is needed to avoid having const char* picked up by
  // the add function for bool parameters.

  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new StringParameter(key, value);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, std::string value, std::set<std::string> range)
{
  // Add parameter
  add(key, value);

  // Set range
  Parameter* p = find_parameter(key);
  assert(p);
  p->set_range(range);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, const char* value, std::set<std::string> range)
{
  // This version is needed to avoid having const char* picked up by
  // the add function for bool parameters.

  // Add parameter
  add(key, value);

  // Set range
  Parameter* p = find_parameter(key);
  assert(p);
  p->set_range(range);
}
//-----------------------------------------------------------------------------
void Parameters::add(std::string key, bool value)
{
  // Check key name
  if (find_parameter(key))
    error("Unable to add parameter \"%s\", already defined.", key.c_str());

  // Add parameter
  _parameters[key] = new BoolParameter(key, value);
}
//-----------------------------------------------------------------------------
void Parameters::add(const Parameters& parameters)
{
  // Check key name
  if (find_parameter_set(parameters.name()))
    error("Unable to add parameter set \"%s\", already defined.",
          parameters.name().c_str());

  // Add parameter set
  Parameters* p = new Parameters("");
  *p = parameters;
  _parameter_sets[parameters.name()] = p;
}
//-----------------------------------------------------------------------------
void Parameters::parse(int argc, char* argv[])
{
  info("Parsing command-line arguments...");

  // Add list of allowed options to po::options_description
  po::options_description desc("Allowed options");
  add_parameter_set_to_po(desc, *this);

  // Add help option
  desc.add_options()("help", "show help text");

  // Read command-line arguments into po::variables_map
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // FIXME: Should we exit after printing help text?

  // Show help text
  if (vm.count("help"))
  {
    std::stringstream s;
    s << desc;
    info(s.str());
    exit(1);
  }

  // Read values from the parsed variable map
  read_vm(vm, *this);
}
//-----------------------------------------------------------------------------
void Parameters::update(const Parameters& parameters)
{
  // Update the parameters
  for (const_parameter_iterator it = parameters._parameters.begin();
       it != parameters._parameters.end(); ++it)
  {
    // Get parameters
    const Parameter& other = *it->second;
    Parameter* self = find_parameter(other.key());

    // Skip parameters not in this parameter set (no new parameters added)
    if (!self)
    {
      warning("Ignoring unknown parameter \"%s\" in parameter set \"%s\" when updating parameter set \"%s\".",
              other.key().c_str(), parameters.name().c_str(), name().c_str());
      continue;
    }

    // Set value (will give an error if the type is wrong)
    if (other.type_str() == "int")
      *self = static_cast<int>(other);
    else if (other.type_str() == "real")
      *self = static_cast<double>(other);
    else if (other.type_str() == "bool")
      *self = static_cast<bool>(other);
    else if (other.type_str() == "string")
      *self = static_cast<std::string>(other);
    else
      error("Unable to use parameter \"%s\", unknown type: \"%s\".",
            other.key().c_str(), other.type_str().c_str());
  }

  // Update nested parameter sets
  for (const_parameter_set_iterator it = parameters._parameter_sets.begin(); it != parameters._parameter_sets.end(); ++it)
  {
    (*this)[it->first].update(*it->second);
  }
}
//-----------------------------------------------------------------------------
Parameter& Parameters::operator() (std::string key)
{
  Parameter* p = find_parameter(key);
  if (!p)
    error("Unable to access parameter \"%s\" in parameter set \"%s\", parameter not defined.",
          key.c_str(), this->name().c_str());
  return *p;
}
//-----------------------------------------------------------------------------
const Parameter& Parameters::operator() (std::string key) const
{
  Parameter* p = find_parameter(key);
  if (!p)
    error("Unable to access parameter \"%s\" in parameter set \"%s\", parameter not defined.",
          key.c_str(), this->name().c_str());
  return *p;
}
//-----------------------------------------------------------------------------
Parameters& Parameters::operator[] (std::string key)
{
  Parameters* p = find_parameter_set(key);
  if (!p)
    error("Unable to access parameter \"%s\" in parameter set \"%s\", parameter set not defined.",
          key.c_str(), this->name().c_str());
  return *p;
}
//-----------------------------------------------------------------------------
const Parameters& Parameters::operator[] (std::string key) const
{
  Parameters* p = find_parameter_set(key);
  if (!p)
    error("Unable to access parameter \"%s\" in parameter set \"%s\", parameter set not defined.",
          key.c_str(), this->name().c_str());
  return *p;
}
//-----------------------------------------------------------------------------
const Parameters& Parameters::operator= (const Parameters& parameters)
{
  // Clear all parameters
  clear();

  // Note: We're relying on the default copy constructors for the
  // Parameter subclasses here to do their work, which they should
  // since they don't use any dynamically allocated data.

  // Copy key
  _key = parameters._key;

  // Copy parameters
  for (const_parameter_iterator it = parameters._parameters.begin();
       it != parameters._parameters.end(); ++it)
  {
    const Parameter& p = *it->second;
    Parameter* q = 0;
    if (p.type_str() == "int")
      q = new IntParameter(dynamic_cast<const IntParameter&>(p));
    else if (p.type_str() == "real")
      q = new RealParameter(dynamic_cast<const RealParameter&>(p));
    else if (p.type_str() == "bool")
      q = new BoolParameter(dynamic_cast<const BoolParameter&>(p));
    else if (p.type_str() == "string")
      q = new StringParameter(dynamic_cast<const StringParameter&>(p));
    else
      error("Unable to copy parameter from parameter set \"%s\" to parameter set \"%s\", unknown type: \"%s\".",
            parameters.name().c_str(), name().c_str(), p.type_str().c_str());
    _parameters[p.key()] = q;
  }

  // Copy parameter sets
  for (const_parameter_set_iterator it = parameters._parameter_sets.begin();
       it != parameters._parameter_sets.end(); ++it)
  {
    const Parameters& p = *it->second;
    _parameter_sets[p.name()] = new Parameters(p);
  }

  return *this;
}
//-----------------------------------------------------------------------------
void Parameters::get_parameter_keys(std::vector<std::string>& keys) const
{
  keys.reserve(_parameters.size());
  for (const_parameter_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
    keys.push_back(it->first);
}
//-----------------------------------------------------------------------------
void Parameters::get_parameter_set_keys(std::vector<std::string>& keys) const
{
  keys.reserve(_parameter_sets.size());
  for (const_parameter_set_iterator it = _parameter_sets.begin(); it != _parameter_sets.end(); ++it)
    keys.push_back(it->first);
 }
//-----------------------------------------------------------------------------
std::string Parameters::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    if (_parameters.size() == 0 && _parameter_sets.size() == 0)
    {
      s << name() << indent("(empty)");
      return s.str();
    }

    Table t(_key);
    for (const_parameter_iterator it = _parameters.begin();
         it != _parameters.end(); ++it)
    {
      Parameter* p = it->second;
      t(p->key(), "type") = p->type_str();
      t(p->key(), "value") = p->value_str();
      t(p->key(), "range") = p->range_str();
      t(p->key(), "access") = p->access_count();
      t(p->key(), "change") = p->change_count();
    }
    s << indent(t.str());

    for (const_parameter_set_iterator it = _parameter_sets.begin();
         it != _parameter_sets.end(); ++it)
      s << "\n\n" << indent(it->second->str(verbose));
  }
  else
  {
    s << "<Parameter set containing "
      << _parameters.size() << " parameters and "
      << _parameter_sets.size() << " nested parameter sets>";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
void Parameters::add_parameter_set_to_po(po::options_description& desc,
                                         const Parameters &parameters,
                                         std::string base_name) const
{
  for (const_parameter_iterator it = parameters._parameters.begin();
       it != parameters._parameters.end(); ++it)
  {
    const Parameter& p = *it->second;
    std::string param_name(base_name + p.key());
    if (p.type_str() == "int")
      desc.add_options()(param_name.c_str(), po::value<int>(), p.description().c_str());
    else if (p.type_str() == "double")
      desc.add_options()(param_name.c_str(), po::value<double>(), p.description().c_str());
    else if (p.type_str() == "string")
      desc.add_options()(param_name.c_str(), po::value<std::string>(), p.description().c_str());
  }

  for (const_parameter_set_iterator it = parameters._parameter_sets.begin(); it != parameters._parameter_sets.end(); ++it)
  {
    add_parameter_set_to_po(desc, *it->second, base_name + it->first + ".");
  }
}
//-----------------------------------------------------------------------------
void Parameters::read_vm(po::variables_map& vm, Parameters &parameters, std::string base_name)
{
  // Read values from po::variables_map
  for (parameter_iterator it = parameters._parameters.begin();
       it != parameters._parameters.end(); ++it)
  {
    Parameter& p = *it->second;
    std::string param_name(base_name + p.key());
    if (p.type_str() == "int")
    {
      const po::variable_value& v = vm[param_name];
      if (!v.empty())
        p = v.as<int>();
    }
    else if (p.type_str() == "double")
    {
      const po::variable_value& v = vm[param_name];
      if (!v.empty())
        p = v.as<double>();
    }
    else if (p.type_str() == "string")
    {
      const po::variable_value& v = vm[param_name];
      if (!v.empty())
        p = v.as<std::string>();
    }
  }

  for (parameter_set_iterator it = parameters._parameter_sets.begin(); it != parameters._parameter_sets.end(); ++it)
  {
    read_vm(vm, *it->second, base_name + it->first + ".");
  }
}
//-----------------------------------------------------------------------------
Parameter* Parameters::find_parameter(std::string key) const
{
  const_parameter_iterator p = _parameters.find(key);
  if (p == _parameters.end())
    return 0;
  return p->second;
}
//-----------------------------------------------------------------------------
Parameters* Parameters::find_parameter_set(std::string key) const
{
  const_parameter_set_iterator p = _parameter_sets.find(key);
  if (p == _parameter_sets.end())
    return 0;
  return p->second;
}
//-----------------------------------------------------------------------------
