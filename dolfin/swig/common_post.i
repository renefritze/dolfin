//%template(STLVectorDirichletBCPtr) std::vector<dolfin::DirichletBC *>;
//%template(STLVectorBoundaryConditionPtr) std::vector<dolfin::BoundaryCondition *>;
//%template(STLVectorFunctionPtr) std::vector<dolfin::Function *>;
//%template(STLVectorFunctionSpacePtr) std::vector<dolfin::FunctionSpace *>;
//%template(STLVectorUInt) std::vector<dolfin::uint>;
//%template(STLVectorDouble) std::vector<double>;
//%template(STLVectorString) std::vector<std::string>;
//%template(STLPairUInt) std::pair<dolfin::uint,dolfin::uint>;

/*%template(BOOSTUnorderSetUInt) boost::unordered_set<dolfin::uint>;*/

%extend dolfin::Variable
{
  std::string __str__() const
  {
    return self->str(false);
  }
}
