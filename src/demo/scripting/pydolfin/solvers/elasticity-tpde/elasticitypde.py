from dolfin import *

class ElasticityPDE(TimeDependentPDE):
    def __init__(self, mesh, f, u0, v0, bc, k, T, t):
        
        self.t = t

        self.U = Function(Vector(), mesh)
        self.V = Function(Vector(), mesh)

        forms = import_formfile("Elasticity.form")
        #import elasticitytdform as forms

        self.aelast = forms.ElasticityBilinearForm()
        self.Lelast = forms.ElasticityLinearForm(self.U, f)

        self.U.init(mesh, self.aelast.trial())
        self.V.init(mesh, self.aelast.trial())

        self.N = self.U.vector().size() + self.V.vector().size()

        TimeDependentPDE.__init__(self, self.aelast, self.Lelast, mesh,
                                  bc, self.N, k, T)

        self.xtmp = Vector(self.V.vector().size())

        # Initial values

        self.U.interpolate(u0)
        self.V.interpolate(v0)


        self.M = Matrix()
        self.m = Vector()

        FEM_assemble(self.a(), self.M, mesh)
        FEM_applyBC(self.M, self.mesh(), self.a().trial(), self.bc())

        FEM_lump(self.M, self.m)

        self.solutionfile = File("solution.pvd")
        self.sampleperiod = T / 100.0
        self.lastsample = 0.0

    def save(self, U, t):

        if(t == 0.0):
            self.U.vector().copy(self.x, 0, 0, self.U.vector().size())
            self.solutionfile << U

        while(self.lastsample + self.sampleperiod < t):
            self.lastsample = min(t, self.lastsample + self.sampleperiod)
            self.U.vector().copy(self.x, 0, 0, self.U.vector().size())
            self.solutionfile << U

    def fu(self, x, dotx, t):

        #print "x: "
        #x.disp()

        self.t.assign(t)

        self.U.vector().copy(x, 0, 0, self.U.vector().size())
        self.V.vector().copy(x, 0, self.U.vector().size(),
                             self.V.vector().size())

        # U

        dotx.copy(x, 0, self.V.vector().size(), self.U.vector().size())

        # V

        dolfin_log(False)
        FEM_assemble(self.L(), self.xtmp, self.mesh())
        FEM_applyBC(self.xtmp, self.mesh(), self.a().trial(), self.bc())
        dolfin_log(True)

        self.xtmp.div(self.m)

        dotx.copy(self.xtmp, self.xtmp.size(), 0, self.xtmp.size())

        #print "dotx: "
        #dotx.disp()
