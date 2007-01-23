import sys
import datetime
import Numeric
import LinearAlgebra
from dolfin import *
from dolfinmodules import *
import transform

class DiffPDE(TimeDependentPDE):
    def __init__(self, mesh, f, E, nu, nuvval, nupval, yld, bc, T, v0, rho):

        self.U = Function(Vector(), mesh)
        self.V = Function(Vector(), mesh)
        self.S = Function(Vector(), mesh)
        self.Snorm = Function(Vector(), mesh)

        lmbdaval = E * nu / ((1 + nu) * (1 - 2 * nu))
        muval = E / (2 * (1 + nu))

        self.lmbda = Function(lmbdaval)
        self.mu = Function(muval)
        self.nuv = Function(nuvval)
        self.nuplast = Function(nupval)
        self.yld = yld

        diff_forms = import_formfile("ElasticityUpdatedEq.form")
        #import elasticityupdatedeq as diff_forms

        stress_forms = import_formfile("ElasticityUpdatedStress.form")
        #import elasticityupdatedstress as stress_forms

        self.amass = diff_forms.ElasticityUpdatedEqBilinearForm(rho)

        #self.Ldiff = diff_forms.ElasticityUpdatedEqLinearForm(self.S, self.V,
        #                                                      self.nuv)
        self.Ldiff = diff_forms.ElasticityUpdatedEqLinearForm(self.S, self.V,
                                                              f, self.nuv)


        self.Lstress = \
        stress_forms.ElasticityUpdatedStressLinearForm(self.V,
                                                       self.S,
                                                       self.Snorm,
                                                       self.lmbda,
                                                       self.mu,
                                                       self.nuplast)


        self.amassstress = \
            stress_forms.ElasticityUpdatedStressBilinearForm()

        self.U.init(mesh, self.amass.trial())
        self.V.init(mesh, self.amass.trial())
        self.S.init(mesh, self.Ldiff.element(0))
        self.Snorm.init(mesh, self.Lstress.element(3))

        N = self.U.vector().size() + self.V.vector().size() + \
            self.S.vector().size()

        TimeDependentPDE.__init__(self, self.amass, self.Ldiff,
                                  mesh, bc, N, T)

        self.v0 = v0
        
        self.E = E
        self.nu = nu
        
        self.solutionfile = File("solution.pvd")
        self.counter = 0
        self.filecounter = 0
        self.fcount = 0

        self.xu = self.U.vector()
        self.xv = self.V.vector()
        self.xS = self.S.vector()
        self.xSnorm = self.Snorm.vector()
    
        self.dotxu = Vector(self.xu.size())
        self.dotxv = Vector(self.xv.size())
        self.dotxS = Vector(self.xS.size())
        
        self.M = Matrix()
        self.m = Vector()
        self.Msigma = Matrix()
        self.msigma = Vector()
        
        self.xtmp = Vector(self.xu.size())
    
        # Initialize and compute coefficients

        # Mass matrix
        FEM_assemble(self.a(), self.M, self.mesh())
        FEM_lump(self.M, self.m)

        print "m:"
        self.m.disp()

        #ElasticityUpdatedSolver_initmsigma(self.msigma, self.S.element(),
        #                                   self.mesh())

        FEM_assemble(self.amassstress, self.Msigma, self.mesh())
        FEM_lump(self.Msigma, self.msigma)

        self.msigma.mult(1.5)
                
        # U
        ElasticityUpdatedSolver_initu0(self.xu, self.U.element(), self.mesh())

        # V
        ElasticityUpdatedSolver_finterpolate(self.v0, self.V, self.mesh())
        dolfin_log(False)
        FEM_applyBC(self.xv, self.mesh(), self.V.element(), self.bc())
        dolfin_log(True)

        #print "xv:"
        #self.xv.disp()

        ElasticityUpdatedSolver_plasticity(self.xS,
                                           self.xSnorm, self.yld,
                                           self.Lstress.element(3),
                                           self.mesh())
        

        # Initial values for ODE
        # Gather into x
        self.x.copy(self.xu, 0, 0, self.xu.size())
        self.x.copy(self.xv, self.xu.size(), 0, self.xv.size())

        print "xu:"
        self.xu.disp()

        print "xv:"
        self.xv.disp()



    def init(self, U):
        print "Python init"
        
    def preparestep(self):
        1

    def prepareiteration(self):
        1

    def fu(self, x, dotx, t):
        #print "Python fu"

        self.xu.copy(self.x, 0, 0, self.dotxu.size())
        self.xv.copy(self.x, 0, self.dotxu.size(), self.dotxv.size())
        self.xS.copy(self.x, 0, self.dotxv.size() + self.dotxu.size(), \
                     self.dotxS.size())

        # Mesh
        ElasticityUpdatedSolver_deform(self.mesh(), self.U)

        # U
        self.dotxu.copy(self.xv, 0, 0, self.xv.size())

        # V
        dolfin_log(False)
        FEM_assemble(self.L(), self.dotxv, self.mesh())
        FEM_applyBC(self.dotxv, self.mesh(), self.V.element(), self.bc())
        dolfin_log(True)
    
        self.dotxv.div(self.m)

        # S
        ElasticityUpdatedSolver_plasticity(self.xS,
                                           self.xSnorm, self.yld,
                                           self.Lstress.element(3),
                                           self.mesh())        

        dolfin_log(False)
        FEM_assemble(self.Lstress, self.dotxS, self.mesh())
        dolfin_log(True)

        self.dotxS.div(self.msigma)

        # Gather into dotx
        self.dotx.copy(self.dotxu, 0, 0, self.dotxu.size())
        self.dotx.copy(self.dotxv, self.dotxu.size(), 0, self.dotxv.size())
        self.dotx.copy(self.dotxS, self.dotxu.size() + self.dotxv.size(), \
                       0, self.dotxS.size())

        self.fcount += 1

    def save(self, U, t):

        if((self.counter % (3)) == 0):
            print "t: ", t
            self.solutionfile << U
            self.filecounter += 1

            Smax = 0.0
            dotSmax = 0.0
            for i in range(0, self.xS.size()):
                Smax = max(Smax, self.xS[i])
                dotSmax = max(dotSmax, self.dotxS[i])

            print "stress: "
            print Smax

            print "dot stress: "
            print dotSmax

        self.counter += 1
