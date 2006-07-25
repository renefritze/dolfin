#include <dolfin.h>

// FIXME: Temporary, only works for PETSc data structures

/// Assemble linear form
namespace dolfin
{

class FEMM
  {

 public:
static void assemble(LinearForm& L, GenericVector& b,
	      Mesh& mesh)
{
  FEM::assemble(L, b, mesh);
}
  };

}

/*

void assemble(dolfin::BilinearForm& a, dolfin::LinearForm& L,
	      dolfin::GenericMatrix& A, dolfin::GenericVector& b,
	      dolfin::Mesh& mesh, dolfin::BoundaryCondition& bc)
{
  dolfin::FEM::assemble(a, L, A, b, mesh, bc);
}

void assemble(dolfin::LinearForm& L, dolfin::Vector& b, dolfin::Mesh& mesh)
{
  dolfin::FEM::assemble(L, b, mesh);
}

void assemble(dolfin::BilinearForm& a, dolfin::Matrix& A, dolfin::Mesh& mesh)
{
  dolfin::FEM::assemble(a, A, mesh);
}

void applyBC(dolfin::Vector& b, dolfin::Mesh& mesh,
	     dolfin::FiniteElement& element, dolfin::BoundaryCondition& bc)
{
  dolfin::FEM::applyBC(b, mesh, element, bc);
}

void applyBC(dolfin::Matrix& A, dolfin::Mesh& mesh,
	     dolfin::FiniteElement& element, dolfin::BoundaryCondition& bc)
{
  dolfin::FEM::applyBC(A, mesh, element, bc);
}

*/
