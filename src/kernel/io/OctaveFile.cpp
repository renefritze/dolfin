// Copyright (C) 2003-2006 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Erik Svensson, 2003.
// Modified by Garth N. Wells, 2006.
//
// First added:  2003-02-26
// Last changed: 2006-05-31

// FIXME: Use streams rather than stdio
#include <stdio.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/Array.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/MatlabFile.h>
#include <dolfin/OctaveFile.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
OctaveFile::OctaveFile(const std::string filename) : MFile(filename)
{
  type = "Octave";
}
//-----------------------------------------------------------------------------
OctaveFile::~OctaveFile()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void OctaveFile::operator<<(Matrix& A)
{
  // Octave file format for Matrix is not the same as the Matlab format,
  // since octave cannot handle sparse matrices.

  uint M = A.size(0);
  uint N = A.size(1);
  real* row = new real[N];
  
  FILE *fp = fopen(filename.c_str(), "a");
  fprintf(fp, "%s = zeros(%u, %u);\n", A.name().c_str(), M, N);
  
  for (uint i = 0; i < M; i++)
  {
    // Get nonzero entries
    int ncols = 0;
    Array<int> columns;
    Array<real> values;
    A.getRow(i, ncols, columns, values);

    // Write nonzero entries
    for (int pos = 0; pos < ncols; pos++)
      fprintf(fp, "%s(%d, %d) = %.16e;\n",
	      A.name().c_str(), (int)i + 1, columns[pos] + 1, values[pos]);
  }
  
  fclose(fp);
  delete [] row;
  
  cout << "Saved matrix " << A.name() << " (" << A.label()
       << ") to file " << filename << " in Octave format." << endl;
}
//-----------------------------------------------------------------------------
