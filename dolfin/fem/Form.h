// Copyright (C) 2007-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
// Modified by Martin Alnes, 2008.
//
// First added:  2007-04-02
// Last changed: 2011-01-29

#ifndef __FORM_H
#define __FORM_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/types.h>

// Forward declaration
namespace ufc
{
  class form;
}

namespace dolfin
{

  class FunctionSpace;
  class GenericFunction;
  class Mesh;

  /// Base class for UFC code generated by FFC for DOLFIN with option -l

  class Form
  {
  public:

    /// Create form of given rank with given number of coefficients
    Form(dolfin::uint rank, dolfin::uint num_coefficients);

    /// Create form (constructor used from Python interface)
    Form(const ufc::form& ufc_form,
         const std::vector<const FunctionSpace*>& function_spaces,
         const std::vector<const GenericFunction*>& coefficients);

    /// Destructor
    virtual ~Form();

    /// Return rank of form (bilinear form = 2, linear form = 1, functional = 0, etc)
    uint rank() const;

    /// Return number of coefficients
    uint num_coefficients() const;

    /// Set mesh, necessary for functionals when there are no function spaces
    void set_mesh(const Mesh& mesh);

    /// Set mesh, necessary for functionals when there are no function spaces
    void set_mesh(boost::shared_ptr<const Mesh> mesh);

    /// Return mesh
    const Mesh& mesh() const;

    /// Return function space for given argument
    const boost::shared_ptr<const FunctionSpace> function_space(uint i) const;

    /// Return function spaces for arguments
    std::vector<boost::shared_ptr<const FunctionSpace> > function_spaces() const;

    /// Set coefficient with given number
    void set_coefficient(uint i, const GenericFunction& coefficient);

    /// Set coefficient with given number (shared pointer version)
    void set_coefficient(uint i, boost::shared_ptr<const GenericFunction> coefficient);

    /// Set coefficient with given name
    void set_coefficient(std::string name, const GenericFunction& coefficient);

    /// Set coefficient with given name (shared pointer version)
    void set_coefficient(std::string name, boost::shared_ptr<const GenericFunction> coefficient);

    /// Set all coefficients in given map, possibly a subset
    void set_coefficients(std::map<std::string, const GenericFunction*> coefficients);

    /// Set all coefficients in given map, possibly a subset  (shared pointer version)
    void set_coefficients(std::map<std::string, boost::shared_ptr<const GenericFunction> > coefficients);

    /// Return coefficient with given number
    const GenericFunction& coefficient(uint i) const;

    /// Return coefficient with given name
    const GenericFunction& coefficient(std::string name) const;

    /// Return all coefficients
    std::vector<const GenericFunction*> coefficients() const;

    /// Return the number of the coefficient with this name
    virtual dolfin::uint coefficient_number(const std::string & name) const;

    /// Return the name of the coefficient with this number
    virtual std::string coefficient_name(dolfin::uint i) const;

    /// Return UFC form
    const ufc::form& ufc_form() const;

    /// Return UFC form shared pointer
    boost::shared_ptr<const ufc::form> ufc_form_shared_ptr() const;

    /// Check function spaces and coefficients
    void check() const;

    /// Friends
    friend class LinearPDE;
    friend class NonlinearPDE;
    friend class VariationalProblem;

  protected:

    // The mesh (needed for functionals when we don't have any spaces)
    boost::shared_ptr<const Mesh> _mesh;

    // Function spaces (one for each argument)
    std::vector<boost::shared_ptr<const FunctionSpace> > _function_spaces;

    // Coefficients
    std::vector<boost::shared_ptr<const GenericFunction> > _coefficients;

    // The UFC form
    boost::shared_ptr<const ufc::form> _ufc_form;

  };

}

#endif
