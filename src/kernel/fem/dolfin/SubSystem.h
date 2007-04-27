// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-04-24
// Last changed: 2007-04-27

#ifndef __SUB_SYSTEM_H
#define __SUB_SYSTEM_H

#include <ufc.h>
#include <dolfin/Array.h>

namespace dolfin
{

  /// This class represents a sub system that may be specified as a
  /// recursively nested sub system of some given system.
  ///
  /// The sub system is specified by an array of indices. For example,
  /// the array [3, 0, 2] specifies sub system 2 of sub system 0 of
  /// sub system 3.

  class SubSystem
  {
  public:

    /// Create empty sub system (no sub systems)
    SubSystem();

    /// Create given sub system (one level)
    SubSystem(uint sub_system);

    /// Create given sub sub system (two levels)
    SubSystem(uint sub_system, uint sub_sub_system);

    /// Create sub system for given array (n levels)
    SubSystem(const Array<uint>& sub_system);

    /// Copy constructor
    SubSystem(const SubSystem& sub_system);
    
    /// Return number of levels for nested sub system
    uint depth() const;
    
    /// Extract sub finite element of given finite element
    const ufc::finite_element* extractFiniteElement
    (const ufc::finite_element& finite_element) const;

    /// Extract sub dof map of given dof map
    const ufc::dof_map* extractDofMap
    (const ufc::dof_map& dof_map) const;

  private:

    // Recursively extract sub finite element
    static const ufc::finite_element* extractFiniteElement
    (const ufc::finite_element& finite_element, const Array<uint>& sub_system);

    // Recursively extract sub dof map
    static const ufc::dof_map* extractDofMap
    (const ufc::dof_map& dof_map, const Array<uint>& sub_system);

    // The array specifying the sub system
    Array<uint> sub_system;
    
  };

}

#endif
