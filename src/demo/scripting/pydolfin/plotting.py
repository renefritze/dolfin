"Demonstrate plotting from within DOLFIN"

from dolfin import *


# Example of a user-defined scalar function
class ScalarFunction(Function):

    def eval(self, x, i):
        return x[0]*x[1] + 1.0 / (x[0] + 0.1)

f = ScalarFunction()

# Example of a user-defined vector function
class VectorFunction(Function):

    def __init__(self):
        Function.__init__(self, 2)
    
    def eval(self, x, i):
        if i == 0:
            return -x[1]**2
        else:
            return x[0]

class VectorFunction3D(Function):
    def __init__(self):
        Function.__init__(self, 3)

    def eval(self, x, i):
        if i == 0:
            return -x[1]**2
        elif i == 1:
            return x[2]**2
        else:
            return x[0]**2


g = VectorFunction()


# Attach a mesh to the functions
mesh = UnitSquare(16, 16)
f.attach(mesh)
g.attach(mesh)

# Plot mesh
plot(mesh)

# Plot scalar function
plot(f)

# Plot vector function
plot(g)

# Plot mesh displaced by vector function
plot(mesh, g)

mesh = UnitCube(16,16,16)
f.attach(mesh)
g = VectorFunction3D()
g.attach(mesh)

# Plot mesh
plot(mesh)

# Plot scalar function
plot(f)

# Plot vector function
plot(g)

# Plot mesh displaced by vector function
plot(mesh, g)


