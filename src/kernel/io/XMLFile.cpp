// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Updates by Erik Svensson 2003

#include <stdarg.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/Grid.h>
#include <dolfin/Function.h>

#include <dolfin/XMLFile.h>
#include <dolfin/XMLObject.h>
#include <dolfin/XMLVector.h>
#include <dolfin/XMLMatrix.h>
#include <dolfin/XMLGrid.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLFile::XMLFile(const std::string filename) : GenericFile(filename)
{
  xmlObject = 0;
}
//-----------------------------------------------------------------------------
XMLFile::~XMLFile()
{
  if ( xmlObject )
    delete xmlObject;
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Vector& x)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLVector(x);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Matrix& A)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMatrix(A);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Grid& grid)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLGrid(grid);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Function& u)
{
  dolfin_warning("Cannot read functions from XML files.");
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Vector& x)
{
  // Open file
  FILE *fp = fopen(filename.c_str(), "a");
  
  // Write vector in XML format
  fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n\n" );  
  fprintf(fp, " <dolfin xmlns:dolfin=\"http://www.phi.chalmers.se/dolfin/\"> \n" );
  fprintf(fp, "\t  <vector size=\" %i \"> \n", x.size() );
  
  for (int i = 0; i < x.size(); i++) {
    fprintf(fp, "\t\t  <element row=\"%i\" value=\"%f\"/>\n", i, x(i) );
    if ( i == (x.size() - 1))
      fprintf(fp, "\t </vector>\n");
  }
  
  fprintf(fp, "</dolfin>\n");
  
  // Close file
  fclose(fp);
  
  // Increase the number of times we have saved the vector
  ++x;
  
  cout << "Saved vector " << x.name() << " (" << x.label()
       << ") to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Matrix& A)
{
  // Open file
  FILE *fp = fopen(filename.c_str(), "a");
  
  // Write matrix in sparse XML format
  fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n\n" );
  fprintf(fp, " <dolfin xmlns:dolfin=\"http://www.phi.chalmers.se/dolfin/\"> \n" );
  
  switch( A.type() ) {
  case Matrix::sparse:

    fprintf(fp, "\t <sparsematrix rows=\"%i\" columns=\"%i\">\n", A.size(0), A.size(1) );
                                                                                                                             
    for (int i = 0; i < A.size(0); i++) {
      fprintf(fp, "\t\t <row row=\"%i\" size=\"%i\">\n", i, A.rowsize(i));
      for (int pos = 0; !A.endrow(i, pos); pos++) {
	int j = 0;
	real aij = A(i, j, pos);
        fprintf(fp, "\t\t\t <element column=\"%i\" value=\"%f\"/>\n", j, aij);
        if ( i == (A.size(0) - 1) && pos == (A.rowsize(i)-1)){
           fprintf(fp, "\t\t </row>\n");
           fprintf(fp, "\t </sparsematrix>\n");
        }
        else if ( pos == (A.rowsize(i)-1))
          fprintf(fp, "\t\t </row>\n");
      }
    }
    break;

  case Matrix::dense:
    dolfin_error("Cannot write dense matrices to XML files.");
    break;
  default:
    dolfin_error("Unknown matrix type.");
  }
  
  fprintf(fp, "</dolfin>\n");
                                                                                                                             
  // Close file
  fclose(fp);
                                                                                                                             
  // Increase the number of times we have saved the matrix
  ++A;
                                                                                                                             
  cout << "Saved matrix " << A.name() << " (" << A.label()
       << ") to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Grid& Grid)
{
  dolfin_warning("Cannot write grids to XML files.");
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Function& u)
{
  dolfin_warning("Cannot write functions to XML files.");
}
//-----------------------------------------------------------------------------
void XMLFile::parseFile()
{
  // Write a message
  xmlObject->reading(filename);

  // Parse file using the SAX interface
  parseSAX();
  
  // Check that we got the data
  if ( !xmlObject->dataOK() )
    dolfin_error("Unable to find data in XML file.");
  
  // Write a message
  xmlObject->done();
}
//-----------------------------------------------------------------------------
void XMLFile::parseSAX()
{
  // Set up the sax handler. Note that it is important that we initialise
  // all (24) fields, even the ones we don't use!
  xmlSAXHandler sax = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  // Set up handlers for parser events
  sax.startDocument = sax_start_document;
  sax.endDocument   = sax_end_document;
  sax.startElement  = sax_start_element;
  sax.endElement    = sax_end_element;
  sax.warning       = sax_warning;
  sax.error         = sax_error;
  sax.fatalError    = sax_fatal_error;
  
  // Parse file
  xmlSAXUserParseFile(&sax, (void *) xmlObject, filename.c_str());
}
//-----------------------------------------------------------------------------
// Callback functions for the SAX interface
//-----------------------------------------------------------------------------
void dolfin::sax_start_document(void *ctx)
{
  
}
//-----------------------------------------------------------------------------
void dolfin::sax_end_document(void *ctx)
{
  
}
//-----------------------------------------------------------------------------
void dolfin::sax_start_element(void *ctx,
			       const xmlChar *name, const xmlChar **attrs)
{
  ( (XMLObject *) ctx )->startElement(name, attrs);
}
//-----------------------------------------------------------------------------
void dolfin::sax_end_element(void *ctx, const xmlChar *name)
{
  ( (XMLObject *) ctx )->endElement(name);
}
//-----------------------------------------------------------------------------
void dolfin::sax_warning(void *ctx, const char *msg, ...)
{
  va_list args;
  
  va_start(args, msg);
  dolfin_info_aptr(msg, args);
  dolfin_warning("Incomplete XML data.");
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::sax_error(void *ctx, const char *msg, ...)
{
  va_list args;
  
  va_start(args, msg);
  dolfin_info_aptr(msg, args);
  dolfin_error("Illegal XML data.");
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::sax_fatal_error(void *ctx, const char *msg, ...)
{
  va_list args;
  
  va_start(args, msg);
  dolfin_info_aptr(msg, args);
  dolfin_error("Illegal XML data.");
  va_end(args);
}
//-----------------------------------------------------------------------------
