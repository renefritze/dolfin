// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ELEMENT_DATA_H
#define __ELEMENT_DATA_H

#include <dolfin/NewList.h>
#include <dolfin/Element.h>
#include <dolfin/ElementTmpFile.h>

namespace dolfin {

  class ODE;
  class ElementBlock;

  /// ElementData is a container for Elements, which are organized
  /// in a sequence of ElementBlocks, each containing a list of
  /// Components, which each contain a list of Elements:
  ///
  ///     ElementData - ElementBlock - Component - Element
  ///
  /// A temporary file is used to store data whenever the amount of
  /// data exceeds the specified cache size.

  class ElementData {
  public:

    /// Constructor
    ElementData(int N);

    /// Destructor
    ~ElementData();

    /// Create a new element
    Element* createElement(Element::Type type, unsigned int q, unsigned int index,
			   real t0, real t1);
    
    /// Return element for given component at given time (null if not found)
    Element* element(unsigned int i, real t);

    /// Return last element for given component (null if not in memory)
    Element* last(unsigned int i);

    /// Notify that this might be a good time to move to next block
    void shift();

    /// Return number of components
    unsigned int size() const;

    /// Check if given time is within the range of data
    bool within(real t) const;

  private:

    // Find block for given time
    ElementBlock* findpos(real t);

    // Update interval
    void update(real t0, real t1);

    // Check if a new block does not fit into memory
    bool memfull();

    // Drop the last block (the one furthest from given interval)
    void droplast(real t0, real t1);

    // Size of system;
    int N;
    
    // List of element blocks
    NewList<ElementBlock*> blocks;
    typedef NewList<ElementBlock*>::iterator BlockIterator;

    // Current block
    ElementBlock* current;

    // Interval
    real t0, t1;

    // True if this block is empty
    bool empty;

    // Cache size
    unsigned int cache_size;

    // File for temporary storage of element data
    ElementTmpFile tmpfile;

  };

}

#endif
