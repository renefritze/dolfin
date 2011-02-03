// Copyright (C) 2011 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2011-01-30
// Last changed: 2011-02-03

#ifndef __HIERARCHICAL_H
#define __HIERARCHICAL_H

#include <boost/shared_ptr.hpp>

#include <dolfin/log/dolfin_log.h>
#include "NoDeleter.h"

namespace dolfin
{

  /// This class provides storage and data access for hierarchical
  /// classes; that is, classes where an object may have a child
  /// and a parent.
  ///
  /// Note to developers: each subclass of Hierarchical that
  /// implements an assignment operator must call the base class
  /// assignment operator (Hierarchical<Foo>::operator=(foo);) at the
  /// beginning of operator=.

  template<class T>
  class Hierarchical
  {
  public:

    /// Constructor
    Hierarchical(T& self) : _self(reference_to_no_delete_pointer(self)) {}

    /// Destructor
    ~Hierarchical() {}

    /// Return depth of the hierarchy; that is, the total number of
    /// objects in the hierarchy linked to the current object via
    /// child-parent relationships, including the object itself.
    ///
    /// *Returns*
    ///     uint
    ///         The depth of the hierarchy.
    uint depth() const
    {
      uint d = 1;
      for (boost::shared_ptr<const T> it = coarse_shared_ptr(); it->_child; it = it->_child)
        d++;
      return d;
    }

    /// Check if the object has a parent.
    ///
    /// *Returns*
    ///     bool
    ///         The return value is true iff the object has a parent.
    bool has_parent() const
    { return _parent; }

    /// Check if the object has a child.
    ///
    /// *Returns*
    ///     bool
    ///         The return value is true iff the object has a child.
    bool has_child() const
    { return _child; }

    /// Return parent in hierarchy. An error is thrown if the object
    /// has no parent.
    ///
    /// *Returns*
    ///     _Object_
    ///         The parent object.
    T& parent()
    {
      if (!_parent)
        error("Object has no parent in hierarchy.");
      return *_parent;
    }

    /// Return parent in hierarchy (const version).
    const T& parent() const
    {
      if (!_parent)
        error("Object has no parent in hierarchy.");
      return *_parent;
    }

    /// Return shared pointer to parent. A zero pointer is returned if
    /// the object has no parent.
    ///
    /// *Returns*
    ///     shared_ptr<T>
    ///         The parent object.
    boost::shared_ptr<T> parent_shared_ptr()
    { return _parent; }

    /// Return shared pointer to parent (const version).
    boost::shared_ptr<const T> parent_shared_ptr() const
    { return _parent; }

    /// Return child in hierarchy. An error is thrown if the object
    /// has no child.
    ///
    /// *Returns*
    ///     _T_
    ///         The child object.
    T& child()
    {
      if (!_child)
        error("Object has no child in hierarchy.");
      return *_child;
    }

    /// Return child in hierarchy (const version).
    const T& child() const
    {
      if (!_child)
        error("Object has no child in hierarchy.");
      return *_child;
    }

    /// Return shared pointer to child. A zero pointer is returned if
    /// the object has no child.
    ///
    /// *Returns*
    ///     shared_ptr<T>
    ///         The child object.
    boost::shared_ptr<T> child_shared_ptr()
    { return _child; }

    /// Return shared pointer to child (const version).
    boost::shared_ptr<const T> child_shared_ptr() const
    { return _child; }

    /// Return coarsest object in hierarchy.
    ///
    /// *Returns*
    ///     _T_
    ///         The coarse object.
    T& coarse()
    {
      return *coarse_shared_ptr();
    }

    /// Return coarsest object in hierarchy (const version).
    const T& coarse() const
    {
      return *coarse_shared_ptr();
    }

    /// Return shared pointer to coarsest object in hierarchy.
    ///
    /// *Returns*
    ///     _T_
    ///         The coarse object.
    boost::shared_ptr<T> coarse_shared_ptr()
    {
      boost::shared_ptr<T> it = _self;
      for (; it->_parent; it = it->_parent);
      return it;
    }

    /// Return shared pointer to coarsest object in hierarchy (const version).
    boost::shared_ptr<const T> coarse_shared_ptr() const
    {
      boost::shared_ptr<const T> it = _self;
      for (; it->_parent; it = it->_parent);
      return it;
    }

    /// Return finest object in hierarchy.
    ///
    /// *Returns*
    ///     _T_
    ///         The fine object.
    T& fine()
    {
      return *fine_shared_ptr();
    }

    /// Return finest object in hierarchy (const version).
    const T& fine() const
    {
      return *fine_shared_ptr();
    }

    /// Return shared pointer to finest object in hierarchy.
    ///
    /// *Returns*
    ///     _T_
    ///         The fine object.
    boost::shared_ptr<T> fine_shared_ptr()
    {
      boost::shared_ptr<T> it = _self;
      for (; it->_child; it = it->_child);
      return it;
    }

    /// Return shared pointer to finest object in hierarchy (const version).
    boost::shared_ptr<const T> fine_shared_ptr() const
    {
      boost::shared_ptr<const T> it = _self;
      for (; it->_child; it = it->_child);
      return it;
    }

    /// Set parent
    void set_parent(boost::shared_ptr<T> parent)
    { _parent = parent; }

    /// Set child
    void set_child(boost::shared_ptr<T> child)
    { _child = child; }

    /// Assignment operator
    const Hierarchical& operator= (const Hierarchical& hierarchical)
    {
      // Assignment to object part of a hierarchy not allowed as it
      // would either require a very exhaustive logic to handle or
      // completely mess up child-parent relations and data ownership.
      const uint d = depth();
      if (d > 1)
      {
        error("Unable to assign, object is part of a hierarchy (depth = %d).", d);
      }

      return *this;
    }

    /// Function useful for debugging the hierarchy
    void _debug() const
    {
      info("Debugging hierarchical object:");
      cout << "  depth           = " << depth() << endl;
      cout << "  has_parent()    = " << has_parent() << endl;
      info("  _parent.get()   = %x", _parent.get());
      info("  _parent.count() = %d", _parent.use_count());
      cout << "  has_child()     = " << has_parent() << endl;
      info("  _child.get()    = %x", _parent.get());
      info("  _child.count()  = %d", _parent.use_count());
    }

  private:

    // The object itself
    boost::shared_ptr<T> _self;

    // Parent and child in hierarchy
    boost::shared_ptr<T> _parent;
    boost::shared_ptr<T> _child;

  };

}

#endif
