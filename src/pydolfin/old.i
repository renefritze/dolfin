// Stuff I have removed, don't know if it's useful /Anders

%pythoncode
%{
def set(name, val):
  if(isinstance(val, bool)):
    glueset_bool(name, val)
  else:
    glueset(name, val)
%}

%extend dolfin::TimeDependent {
  TimeDependent(double *t)
  {
    TimeDependent* td = new TimeDependent();
    td->sync(*t);
    return td;
  }
  void sync(double* t)
  {
    self->sync(*t);
  }
}

%pythoncode
%{
def set(name, val):
  if(isinstance(val, bool)):
    glueset_bool(name, val)
  else:
    glueset(name, val)
%}

# FIXME: These renames don't seem to work.
%rename(fmono) dolfin::ODE::f(const dolfin::uBlasVector&, dolfin::real, dolfin::uBlasVector&);
%rename(fmulti) dolfin::ODE::f(const dolfin::uBlasVector&, dolfin::real, dolfin::uint);
