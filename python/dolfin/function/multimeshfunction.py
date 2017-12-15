# -*- coding: utf-8 -*-
"""This module handles the MultiMeshFunction class in Python.
"""
# Copyright (C) 2017 Jørgen Schartum Dokken
#
# This file is part of DOLFIN.
#
# DOLFIN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DOLFIN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
#

import ufl
import dolfin.cpp as cpp
import numpy

from dolfin.function.multimeshfunctionspace import MultiMeshFunctionSpace
from dolfin.function.function import Function

class MultiMeshFunction(ufl.Coefficient):
    """This class represents a multimeshfunction
    :math:`u_h=(u_{h,1}\cross \dots u_{h,N}` in a finite
    element multimeshfunction space
    :math:`V_h=V_{h,1}\cross \dots V_{h,N}`, given by

    .. math::

        u_{h,j}=  \sum_{i=1}^n U_i \phi_{i,j},

    where :math:`\{\phi_{i,j}\}_{i=1}^n` is a basis for :math:`V_{h,j}`,
    and :math:`U` is a vector of expansion coefficients for
    :math:`u_h`.

    *Arguments*
        There is a maximum of two arguments. The first argument must be a
        :py:class:`MultiMeshFunctionSpace
	 <dolfin.cpp.function.MultiMeshFunctionSpace>`.

	The second argument must be a GenericVector and is intended for library
	use only.

    *Examples*
        Create a MultiMeshFunction:

        - from a :py:class:`MultiMeshFunctionSpace
          <dolfin.cpp.function.MultiMeshFunctionSpace>` ``V``

          .. code-block:: python

              f = MultiMeshFunction(V)


        - from a :py:class:`MultiMeshFunctionSpace
          <dolfin.cpp.function.MultiMeshFunctionSpace>` ``V`` and a
          :py:class:`GenericVector <dolfin.cpp.GenericVector>` ``v``

          *Warning: this constructor is intended for internal libray use only.*

          .. code-block:: python

              g = MultiMeshFunction(V, v)

    """

    def __init__(self, *args, **kwargs):
        """Initialize MultiMeshFunction."""
        # Initial quick check for valid arguments (other checks
        # sprinkled below)
        if len(args) == 0:
            raise TypeError("expected 1 or more arguments")
        # Type switch on argument types
        if isinstance(args[0], MultiMeshFunction):
            other = args[0]
            if len(args) == 1:
                # Copy constructor used to be here
                raise RuntimeError("Use 'MultiMeshFunction.copy(deepcopy=True)' for copying.")
            else:
                raise NotImplementedError
        elif isinstance(args[0], cpp.function.MultiMeshFunction):
            raise NotImplementedError
        elif isinstance(args[0], MultiMeshFunctionSpace):
            V = args[0]
            # If initialising from a FunctionSpace
            if len(args) == 1:
                # If passing only the FunctionSpace
                self._cpp_object = cpp.function.MultiMeshFunction(V._cpp_object)
                ufl.Coefficient.__init__(self, V._parts[0].ufl_function_space(),
                                         count=self._cpp_object.id())
            elif len(args) == 2:
                other = args[1]
                if isinstance(other, cpp.function.MultiMeshFunction):
                    raise NotImplementedError
                else:
                    self.cpp_object = cpp.function.MultiMeshFunction.__init__(self, V, other)
                    ufl.Coefficient.__init__(self, V._parts[0].ufl_function_space(),
                                 count=self._cpp_object.id())

            else:
                raise TypeError("too many arguments")

            # Keep a reference of the functionspace with additional attributes
            self._V = V
        else:
            raise TypeError("expected a MultiMeshFunctionSpace or a MultiMeshFunction as argument 1")


    def function_space(self):
        return self._V

    def assign(self, rhs):
        """
        Parameters: 
            rhs: A dolfin.MultiMeshFunction
        """
        # Assign a MultiMeshFunction into a MultiMeshFunction
        if isinstance(rhs, MultiMeshFunction):
            self.vector()[:]= rhs.vector()[:]
        else:
            raise TypeError("expected a MultiMeshFunction as argument.")

    def part(self, i, deepcopy=False):
        f = Function(self._cpp_object.part(i, deepcopy))
        f.rename(self._cpp_object.name(), self._cpp_object.label())
        return f

    def parts(self, deepcopy=False):
        """
        Generator for MultiMeshFunction
        """
        for part in range(self._V._cpp_object.multimesh().num_parts()):
            yield self.part(part, deepcopy)

    def interpolate(self, v):
        """
        Interpolate function.

        *Arguments*
            v
              a :py:class:`MultiMeshFunction <dolfin.functions.function.MultimeshFunction>` or
              an :py:class:`Expression <dolfin.functions.expression.Expression'>
        *Example of usage*
            .. code-block:: python

                V = MultiMeshFunctionSpace(multimesh, "Lagrange", 1)
                v = MultiMeshFunction(V)
                w = Expression("sin(pi*x[0])")
                v.interpolate(w)
        """
        # Developer note: This version involves a lot of copying
        # and should be changed at some point.
        # Developer note: Interpolate does not set inactive dofs to zero,
        # and should be fixed
        # Check argument
        if isinstance(v, ufl.Coefficient):
            for i, vp in enumerate(self.parts(deepcopy=True)):
                vp.interpolate(v)
                self.assign_part(i, vp)

        elif isinstance(v, MultiMeshFunction):
            # Same multimesh required for interpolation
            # Developer note: Is this test necessary?
            if  self._V.multimesh().id() != v._V.multimesh().id():
                raise RuntimeError("MultiMeshFunctions must live on same MultiMesh")
            for i, vp in enumerate(self.parts(deepcopy=True)):
                vmm = v.part(i, deepcopy=True)
                vp.interpolate(vmm)
                self.assign_part(i, vp)
        else:
            raise TypeError("Expected an Expression or a MultiMeshFunction.")

        # Set inactive dofs to zero
        for part in range(self._V.num_parts()):
            dofs = self._V.dofmap().inactive_dofs(self._V.multimesh(),part)
            self.vector()[dofs]=0
