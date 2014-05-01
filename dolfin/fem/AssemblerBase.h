// Copyright (C) 2007-2009 Anders Logg
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Garth N. Wells, 2007-2008.
// Modified by Ola Skavhaug, 2008.
//
// First added:  2007-01-17
// Last changed: 2013-09-19

#ifndef __ASSEMBLER_BASE_H
#define __ASSEMBLER_BASE_H

#include <string>
#include <utility>
#include <vector>
#include <dolfin/common/types.h>
#include <dolfin/log/log.h>

namespace dolfin
{

  // Forward declarations
  class GenericTensor;
  class Form;

  /// This class provides type compatible with boolean used for announcing
  /// deprecation warning for deprecated bool member variable

  // TODO: Remove when not needed.
  class bool_deprecated
  {
  public:

    bool_deprecated(const bool& value, const std::string& what,
                    const std::string& ver_deprecated,
                    const std::string& ver_removed,
                    const std::string& reason)
      : _value(value), _what(what),
        _ver_deprecated(ver_deprecated),
        _ver_removed(ver_removed), _reason(reason)
    { }

    bool_deprecated(const bool_deprecated& that)
      : _value(that._value), _what(that._what),
        _ver_deprecated(that._ver_deprecated),
        _ver_removed(that._ver_removed), _reason(that._reason)
    { }

    ~bool_deprecated() { }

    bool_deprecated& operator=(const bool_deprecated& that)
    { 
      _value          = that._value;
      _what           = that._what;
      _ver_deprecated = that._ver_deprecated;
      _ver_removed    = that._ver_removed;
      _reason         = that._reason;
      return *this;
    }

    bool_deprecated& operator=(const bool& value)
    {
      deprecation(_what,
                  _ver_deprecated,
                  _ver_removed,
                  _reason);
      _value = value;
      return *this;
    }

    operator bool() const { return _value; }

  private:

    bool _value;

    std::string _what;
    std::string _ver_deprecated;
    std::string _ver_removed;
    std::string _reason;

  };

  /// This class provides some common functions used in
  /// assembler classes.

  class AssemblerBase
  {
  public:

    // Check form
    AssemblerBase() :
      reset_sparsity(true, "Parameter reset_sparsity of assembler",
                     "1.4", "1.5", "Parameter reset_sparsity of assembler"
                     " is no longer used. Tensor is reset iff empty()."),
      add_values(false),
      finalize_tensor(true),
      keep_diagonal(false) {}

    /// reset_sparsity (bool)
    ///     Deprecated. Sparsity pattern of the tensor is reset
    ///     iff the tensor is empty().
    bool_deprecated reset_sparsity;

    /// add_values (bool)
    ///     Default value is false.
    ///     This controls whether values are added to the given
    ///     tensor or if it is zeroed prior to assembly.
    bool add_values;

    /// finalize_tensor (bool)
    ///     Default value is true.
    ///     This controls whether the assembler finalizes the
    ///     given tensor after assembly is completed by calling
    ///     A.apply().
    bool finalize_tensor;

    /// keep_diagonal (bool)
    ///     Default value is false.
    ///     This controls whether the assembler enures that a diagonal
    ///     entry exists in an assembled matrix. It may be removed
    ///     if the matrix is finalised.
    bool keep_diagonal;

    // Initialize global tensor
    void init_global_tensor(GenericTensor& A, const Form& a);

  protected:

    // Check form
    static void check(const Form& a);

    // Pretty-printing for progress bar
    static std::string progress_message(std::size_t rank, std::string integral_type);

  };

}

#endif
