/* -*- C -*- */
// Copyright (C) 2009 Johan Hake
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
// First added:  2009-10-07
// Last changed: 2012-08-09

//=============================================================================
// SWIG directives for the DOLFIN nls kernel module (pre)
//
// The directives in this file are applied _before_ the header files of the
// modules has been loaded.
//=============================================================================

//-----------------------------------------------------------------------------
// Add director classes
//-----------------------------------------------------------------------------
%feature("director") dolfin::NonlinearProblem;
%feature("director") dolfin::OptimisationProblem;
%feature("director") dolfin::NewtonSolver;

//-----------------------------------------------------------------------------
// Director typemap for dolfin::GenericMatrix&
//-----------------------------------------------------------------------------
%typemap(directorin, fragment="NoDelete") dolfin::GenericMatrix& {
  // director in dolfin::GenericMatrix&
  std::shared_ptr< dolfin::GenericMatrix > *smartresult = new std::shared_ptr< dolfin::GenericMatrix >(reference_to_no_delete_pointer($1_name));
  $input = SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(std::shared_ptr< dolfin::GenericMatrix > *), SWIG_POINTER_OWN);
}

//-----------------------------------------------------------------------------
// Director typemap for dolfin::GenericVector&
//-----------------------------------------------------------------------------
%typemap(directorin, fragment="NoDelete") dolfin::GenericVector& {
  // director in dolfin::GenericVector&
  std::shared_ptr< dolfin::GenericVector > *smartresult = new std::shared_ptr< dolfin::GenericVector >(reference_to_no_delete_pointer($1_name));
  $input = SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(std::shared_ptr< dolfin::GenericVector > *), SWIG_POINTER_OWN);
}

//-----------------------------------------------------------------------------
// Director typemap for const dolfin::GenericVector&
//-----------------------------------------------------------------------------
%typemap(directorin, fragment="NoDelete") const dolfin::GenericVector& {
  // director in const dolfin::GenericVector&
  std::shared_ptr< const dolfin::GenericVector > *smartresult = new std::shared_ptr< const dolfin::GenericVector >(reference_to_no_delete_pointer($1_name));
  $input = SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(std::shared_ptr< dolfin::GenericVector > *), SWIG_POINTER_OWN);
}

//-----------------------------------------------------------------------------
// Director typemap for std::shared_ptr<const dolfin::GenericVector>
//-----------------------------------------------------------------------------
%typemap(directorin) std::shared_ptr<const dolfin::GenericMatrix> {
  // director in std::shared_ptr<const dolfin::GenericVector>&
  std::shared_ptr< const dolfin::GenericMatrix > *smartresult = new std::shared_ptr< const dolfin::GenericMatrix >($1_name);
  $input = SWIG_NewPointerObj(%as_voidptr(smartresult), $descriptor(std::shared_ptr< dolfin::GenericMatrix > *), SWIG_POINTER_OWN);
}
