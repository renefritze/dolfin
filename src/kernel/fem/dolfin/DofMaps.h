// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-01-17
// Last changed: 2007-03-01

#ifndef __DOF_MAPS_H
#define __DOF_MAPS_H

#include <map>
#include <vector>
#include <string>
#include <ufc.h>

#include <dolfin/constants.h>

namespace dolfin
{

  class DofMap;
  class Mesh;

  /// Storage for (precomputed) dof maps

  class DofMaps
  {
  public:
    
    /// Create empty set of dof maps
    DofMaps();

    /// Destructor
    ~DofMaps();

    /// Update set of dof maps for given form
    void update(const ufc::form& form, Mesh& mesh);
    
    /// Return number of dof maps
    uint size() const;
    
    /// Return dof map for argument function i
    const DofMap& operator[] (uint i) const;
    
  private:

    // Cached precomputed dof maps
    std::map<const std::string, DofMap*> dof_map_cache;

    // Array of dof maps for current form
    std::vector<DofMap*> dof_maps;

  };

}

#endif
