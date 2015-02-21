#ifndef __DOLFIN_LA_H
#define __DOLFIN_LA_H

// DOLFIN la interface

// Note that the order is important!

#include <dolfin/la/ublas.h>

#include <dolfin/la/LinearAlgebraObject.h>
#include <dolfin/la/GenericLinearOperator.h>

#include <dolfin/la/GenericTensor.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/GenericSparsityPattern.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/la/VectorSpaceBasis.h>
#include <dolfin/la/GenericLinearSolver.h>
#include <dolfin/la/GenericLUSolver.h>
#include <dolfin/la/GenericPreconditioner.h>

#include <dolfin/la/PETScOptions.h>
#include <dolfin/la/PETScObject.h>
#include <dolfin/la/PETScBaseMatrix.h>

#include <dolfin/la/uBLASFactory.h>

#include <dolfin/la/uBLASMatrix.h>
#include <dolfin/la/uBLASLinearOperator.h>

#include <dolfin/la/EigenMatrix.h>

#include <dolfin/la/PETScMatrix.h>
#include <dolfin/la/PETScLinearOperator.h>
#include <dolfin/la/PETScPreconditioner.h>

#include <dolfin/la/EigenKrylovSolver.h>
#include <dolfin/la/EigenLUSolver.h>
#include <dolfin/la/PETScKrylovSolver.h>
#include <dolfin/la/PETScLUSolver.h>
#include <dolfin/la/CholmodCholeskySolver.h>
#include <dolfin/la/UmfpackLUSolver.h>
#include <dolfin/la/MUMPSLUSolver.h>
#include <dolfin/la/PaStiXLUSolver.h>

#include <dolfin/la/STLMatrix.h>
#include <dolfin/la/CoordinateMatrix.h>
#include <dolfin/la/EigenVector.h>
#include <dolfin/la/uBLASVector.h>
#include <dolfin/la/PETScVector.h>

#include <dolfin/la/SparsityPattern.h>

#include <dolfin/la/GenericLinearAlgebraFactory.h>
#include <dolfin/la/DefaultFactory.h>
#include <dolfin/la/EigenFactory.h>
#include <dolfin/la/PETScUserPreconditioner.h>
#include <dolfin/la/PETScFactory.h>
#include <dolfin/la/PETScCuspFactory.h>
#include <dolfin/la/STLFactory.h>
#include <dolfin/la/SLEPcEigenSolver.h>
#include <dolfin/la/uBLASSparseMatrix.h>
#include <dolfin/la/uBLASDenseMatrix.h>
#include <dolfin/la/uBLASPreconditioner.h>
#include <dolfin/la/uBLASKrylovSolver.h>
#include <dolfin/la/uBLASILUPreconditioner.h>
#include <dolfin/la/Vector.h>
#include <dolfin/la/Matrix.h>
#include <dolfin/la/Scalar.h>
#include <dolfin/la/LinearSolver.h>
#include <dolfin/la/KrylovSolver.h>
#include <dolfin/la/LUSolver.h>
#include <dolfin/la/solve.h>
#include <dolfin/la/test_nullspace.h>
#include <dolfin/la/BlockVector.h>
#include <dolfin/la/BlockMatrix.h>
#include <dolfin/la/LinearOperator.h>

#endif
