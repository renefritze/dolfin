#include <string>
#include <iostream>

#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/Grid.h>
#include <dolfin/Function.h>

#include <dolfin/File.h>
#include "GenericFile.h"
#include "XMLFile.h"
#include "MatlabFile.h"
#include "OctaveFile.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
File::File(const std::string& filename)
{
  if ( filename.rfind(".xml") != filename.npos )
	 file = new XMLFile(filename);
  else if ( filename.rfind(".xml.gz") != filename.npos )
	 file = new XMLFile(filename);
  else if ( filename.rfind(".m") != filename.npos )
	 file = new MatlabFile(filename);
  else{
	 file = 0;
	 std::cout << "Unknown file type for " << filename << std::endl;
	 exit(1);
  }
}
//-----------------------------------------------------------------------------
File::File(const std::string& filename, Type type)
{
  switch ( type ) {
  case XML:
	 file = new XMLFile(filename);
	 break;
  case MATLAB:
	 file = new MatlabFile(filename);
	 break;
  case OCTAVE:
	 file = new OctaveFile(filename);
	 break;
  default:
	 file = 0;
	 std::cout << "Unknown file type for " << filename << std::endl;
	 exit(1);
  }
}
//-----------------------------------------------------------------------------
File::~File()
{
  if ( file )
	 delete file;
  file = 0;
}
//-----------------------------------------------------------------------------
void File::operator>>(Vector& x)
{
  file->read();
  
  *file >> x;
}
//-----------------------------------------------------------------------------
void File::operator>>(Matrix& A)
{
  file->read();
  
  *file >> A;
}
//-----------------------------------------------------------------------------
void File::operator>>(Grid& grid)
{
  file->read();
  
  *file >> grid;
}
//-----------------------------------------------------------------------------
void File::operator>>(Function& u)
{
  file->read();
  
  *file >> u;
}
//-----------------------------------------------------------------------------
void File::operator<<(const Vector& x)
{
  file->write();
  
  *file << x;
}
//-----------------------------------------------------------------------------
void File::operator<<(const Matrix& A)
{
  file->write();
	 
  *file << A;
}
//-----------------------------------------------------------------------------
void File::operator<<(const Grid& grid)
{
  file->write();
  
  *file << grid;
}
//-----------------------------------------------------------------------------
void File::operator<<(const Function& u)
{
  file->write();
  
  *file << u;
}
//-----------------------------------------------------------------------------
