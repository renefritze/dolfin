from dolfin import *
from math import *

class Simple(ODE):
    def __init__(self):
        print "Simple ctor"
        ODE.__init__(self, 1)
        self.T = 1.0

    def u0(self, i):
        print "Evaluating Python u0 function: "
        return 10.0

    def fmono(self, u, t, y):
        print "Evaluating Python fmono function: "
        print "u: " + str(realArray_getitem(u,0))
        print "foo"

    def fmulti(self, u, t, i):
        print "Evaluating Python fmulti function: "
        print "u: " + str(realArray_getitem(u,0))
        print "i: " + str(i)
        return 1.0
        
        
#dolfin_set("method", "cg");

settings = Settings()

ode = Simple()
print "ODE size: " + str(ode.size())
N = ode.size()

utest = new_realArray(N)
for i in range(0,N):
    realArray_setitem(utest,i,ode.u0(i))
print "utest: "
for i in range(0,N):
    print realArray_getitem(utest,i),



#ode.solve()
