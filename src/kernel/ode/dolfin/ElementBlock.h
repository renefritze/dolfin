// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ELEMENT_BLOCK_H
#define __ELEMENT_BLOCK_H

#include <dolfin/NewArray.h>
#include <dolfin/Component.h>
#include <dolfin/Element.h>

namespace dolfin {

  /// ElementBlock is a container for Elements, which are organized
  /// in a list of Components, each containing a list of Elements.

  class ElementBlock {
  public:

    /// Constructor
    ElementBlock(int N);

    /// Destructor
    ~ElementBlock();

    /// Create a new element
    Element* createElement(Element::Type type, real t0, real t1, int q, int index);

    /// Return element for given component at given time
    Element* element(unsigned int, real t);
    
    /// Return last element for given component
    Element* last(unsigned int i);

    /// Return number of components
    unsigned int size() const;

  private:

    NewArray<Component> components;

  };

}

#endif
