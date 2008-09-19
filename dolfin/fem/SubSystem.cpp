// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-04-24
// Last changed: 2007-05-14

#include <dolfin/log/log.h>
#include <dolfin/fem/FiniteElement.h>
#include "SubSystem.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
SubSystem::SubSystem()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
SubSystem::SubSystem(uint sub_system)
{
  this->sub_system.push_back(sub_system);
}
//-----------------------------------------------------------------------------
SubSystem::SubSystem(uint sub_system, uint sub_sub_system)
{
  this->sub_system.push_back(sub_system);
  this->sub_system.push_back(sub_sub_system);
}
//-----------------------------------------------------------------------------
SubSystem::SubSystem(const Array<uint>& sub_system) : sub_system(sub_system)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
SubSystem::SubSystem(const SubSystem& sub_system)
{
  this->sub_system = sub_system.sub_system;
}
//-----------------------------------------------------------------------------
const SubSystem& SubSystem::operator= (const SubSystem& sub_system)
{
  this->sub_system = sub_system.sub_system;
  return *this;
}
//-----------------------------------------------------------------------------
dolfin::uint SubSystem::depth() const
{
  return sub_system.size();
}
//-----------------------------------------------------------------------------
FiniteElement* SubSystem::extractFiniteElement(const FiniteElement& finite_element) const
{
  // Recursively extract sub element
  FiniteElement* sub_finite_element = extractFiniteElement(finite_element, sub_system);
  message(2, "Extracted finite element for sub system: %s", sub_finite_element->signature().c_str());
  
  return sub_finite_element;
}
//-----------------------------------------------------------------------------
FiniteElement* SubSystem::extractFiniteElement
      (const FiniteElement& finite_element, const Array<uint>& sub_system)
{
  // Check if there are any sub systems
  if (finite_element.num_sub_elements() == 0)
    error("Unable to extract sub system (there are no sub systems).");

  // Check that a sub system has been specified
  if (sub_system.size() == 0)
    error("Unable to extract sub system (no sub system specified).");
  
  // Check the number of available sub systems
  if (sub_system[0] >= finite_element.num_sub_elements())
    error("Unable to extract sub system %d (only %d sub systems defined).",
                  sub_system[0], finite_element.num_sub_elements());
  
  // Create sub system
  FiniteElement* sub_element = finite_element.create_sub_element(sub_system[0]);
  
  // Return sub system if sub sub system should not be extracted
  if (sub_system.size() == 1)
    return sub_element;

  // Otherwise, recursively extract the sub sub system
  Array<uint> sub_sub_system;
  for (uint i = 1; i < sub_system.size(); i++)
    sub_sub_system.push_back(sub_system[i]);
  FiniteElement* sub_sub_element = extractFiniteElement(*sub_element, sub_sub_system);
  delete sub_element;

  return sub_sub_element;
}
//-----------------------------------------------------------------------------
