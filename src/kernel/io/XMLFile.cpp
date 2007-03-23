// Copyright (C) 2002-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Erik Svensson 2003.
// Modified by Garth N. Wells 2006.
// Modified by Ola Skavhaug 2006.
// Modified by Magnus Vikstrom 2007.
//
// First added:  2002-12-03
// Last changed: 2007-03-21

#include <stdarg.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/Array.h>
#include <dolfin/Vector.h>
#include <dolfin/Matrix.h>
#include <dolfin/Mesh.h>
#include <dolfin/Graph.h>
#include <dolfin/MeshFunction.h>
#include <dolfin/Function.h>
#include <dolfin/FiniteElement.h>
#include <dolfin/FiniteElementSpec.h>
#include <dolfin/Parameter.h>
#include <dolfin/ParameterList.h>

#include <dolfin/XMLObject.h>
#include <dolfin/XMLVector.h>
#include <dolfin/XMLMatrix.h>
#include <dolfin/XMLMesh.h>
#include <dolfin/XMLMeshFunction.h>
#include <dolfin/XMLFunction.h>
#include <dolfin/XMLFiniteElementSpec.h>
#include <dolfin/XMLParameterList.h>
#include <dolfin/XMLBLASFormData.h>
#include <dolfin/XMLGraph.h>
#include <dolfin/XMLFile.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLFile::XMLFile(const std::string filename) : GenericFile(filename),
					       header_written(false),
					       mark(0)
{
  type = "XML";
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
void XMLFile::operator>>(Mesh& mesh)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMesh(mesh);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(MeshFunction<int>& meshfunction)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMeshFunction(meshfunction);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(MeshFunction<unsigned int>& meshfunction)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMeshFunction(meshfunction);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(MeshFunction<double>& meshfunction)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMeshFunction(meshfunction);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(MeshFunction<bool>& meshfunction)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLMeshFunction(meshfunction);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Function& f)
{
  // We are cheating here. Instead of actually parsing the XML for
  // Function data nested inside <function></function>, we just ignore
  // the nesting and look for the first occurence of the data which
  // might be outide of <function></function>

  // Read the vector
  Vector* x = new Vector();
  *this >> *x;

  // Read the mesh
  Mesh* mesh = new Mesh();
  *this >> *mesh;

  // Read the finite element specification
  FiniteElementSpec spec;
  *this >> spec;

  // Create a finite element
  FiniteElement* element = FiniteElement::makeElement(spec);

  // Read the function
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLFunction(f);
  parseFile();
  
  // Attach the data
  f.init(*mesh, *element);
  f.attach(*x, true);
  f.attach(*mesh, true);
  f.attach(*element, true);
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(FiniteElementSpec& spec)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLFiniteElementSpec(spec);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(ParameterList& parameters)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLParameterList(parameters);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(BLASFormData& blas)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLBLASFormData(blas);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator>>(Graph& graph)
{
  if ( xmlObject )
    delete xmlObject;
  xmlObject = new XMLGraph(graph);
  parseFile();
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Vector& x)
{
  // Open file
  FILE* fp = openFile();
  
  // Write vector in XML format
  fprintf(fp, "  <vector size=\" %u \"> \n", x.size() );
  
  for (unsigned int i = 0; i < x.size(); i++) 
  {
    // FIXME: This is a slow way to acces PETSc vectors. Need a fast way 
    //        which is consistent for different vector types.
    real temp = x(i);
    fprintf(fp, "    <entry row=\"%u\" value=\"%.15g\"/>\n", i, temp);
    if ( i == (x.size() - 1))
      fprintf(fp, "  </vector>\n");
  }
  
  // Close file
  closeFile(fp);
  
  dolfin_info("Saved vector %s (%s) to file %s in DOLFIN XML format.",
	      x.name().c_str(), x.label().c_str(), filename.c_str());
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Matrix& A)
{
  // Open file
  FILE *fp = openFile();
  
  // Write matrix in XML format
  fprintf(fp, "  <matrix rows=\"%u\" columns=\"%u\">\n", A.size(0), A.size(1));
        
  int ncols = 0;
  Array<int> columns;
  Array<real> values;

  for (unsigned int i = 0; i < A.size(0); i++)
  {
    A.getRow(i, ncols, columns, values);
    if ( ncols > 0 )
      fprintf(fp, "    <row row=\"%u\" size=\"%i\">\n", i, ncols);
    for (int pos = 0; pos < ncols; pos++)
    {
      unsigned int j = columns[pos];
      real aij = values[pos];
      fprintf(fp, "      <entry column=\"%u\" value=\"%.15g\"/>\n", j, aij);
    }
    if ( ncols > 0 )
      fprintf(fp, "    </row>\n");
  }
  fprintf(fp, "  </matrix>\n");

  // Close file
  closeFile(fp);

  dolfin_info("Saved vector %s (%s) to file %s in DOLFIN XML format.",
	      A.name().c_str(), A.label().c_str(), filename.c_str());
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Mesh& mesh)
{
  // Open file
  FILE *fp = openFile();
  
  // Get cell type
  CellType::Type cell_type = mesh.type().cellType();

  // Write mesh in XML format
  fprintf(fp, "  <mesh celltype=\"%s\" dim=\"%u\">\n",
          CellType::type2string(cell_type).c_str(), mesh.geometry().dim());

  fprintf(fp, "    <vertices size=\"%u\">\n", mesh.numVertices());
  
  for(VertexIterator v(mesh); !v.end(); ++v)
  {
    Point p = v->point();

    switch ( mesh.geometry().dim() ) {
    case 1:
      fprintf(fp, "      <vertex index=\"%u\" x=\"%g\"/>\n",
              v->index(), p.x());
      break;
    case 2:
      fprintf(fp, "      <vertex index=\"%u\" x=\"%g\" y=\"%g\"/>\n",
              v->index(), p.x(), p.y());
      break;
    case 3:
      fprintf(fp, "      <vertex index=\"%u\" x=\"%g\" y=\"%g\" z=\"%g\" />\n",
              v->index(), p.x(), p.y(), p.z());
      break;
    default:
      dolfin_error("The XML mesh file format only supports 1D, 2D and 3D meshes.");
    }
  }

  fprintf(fp, "    </vertices>\n");
  fprintf(fp, "    <cells size=\"%u\">\n", mesh.numCells());

  for (CellIterator c(mesh); !c.end(); ++c)
  {
    uint* vertices = c->entities(0);
    dolfin_assert(vertices);

    switch ( cell_type )
    {
    case CellType::interval:
      fprintf(fp, "      <interval index=\"%u\" v0=\"%u\" v1=\"%u\"/>\n",
	      c->index(), vertices[0], vertices[1]);
      break;
    case CellType::triangle:
      fprintf(fp, "      <triangle index=\"%u\" v0=\"%u\" v1=\"%u\" v2=\"%u\"/>\n",
	      c->index(), vertices[0], vertices[1], vertices[2]);
      break;
    case CellType::tetrahedron:
      fprintf(fp, "      <tetrahedron index=\"%u\" v0=\"%u\" v1=\"%u\" v2=\"%u\" v3=\"%u\"/>\n",
              c->index(), vertices[0], vertices[1], vertices[2], vertices[3]);
      break;
    default:
      dolfin_error1("Unknown cell type: %u.", cell_type);
    }
  }

  fprintf(fp, "    </cells>\n");
  fprintf(fp, "  </mesh>\n");
 
  // Close file
  closeFile(fp);

  cout << "Saved mesh to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(MeshFunction<int>& meshfunction)
{
  // Open file
  FILE *fp = openFile();
  
  // Write mesh in XML format
  fprintf(fp, "  <meshfunction type=\"int\" dim=\"%u\" size=\"%u\">\n",
          meshfunction.dim(), meshfunction.size());
  
  Mesh& mesh = meshfunction.mesh();
  for(MeshEntityIterator e(mesh, meshfunction.dim()); !e.end(); ++e)
  {
      fprintf(fp, "    <entity index=\"%u\" value=\"%d\"/>\n",
              e->index(), meshfunction(*e));
  }

  fprintf(fp, "  </meshfunction>\n");
 
  // Close file
  closeFile(fp);
  
  cout << "Saved mesh function to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(MeshFunction<unsigned int>& meshfunction)
{
  // Open file
  FILE *fp = openFile();
  
  // Write mesh in XML format
  fprintf(fp, "  <meshfunction type=\"uint\" dim=\"%u\" size=\"%u\">\n",
          meshfunction.dim(), meshfunction.size());
  
  Mesh& mesh = meshfunction.mesh();
  for(MeshEntityIterator e(mesh, meshfunction.dim()); !e.end(); ++e)
  {
      fprintf(fp, "    <entity index=\"%u\" value=\"%d\"/>\n",
              e->index(), meshfunction(*e));
  }

  fprintf(fp, "  </meshfunction>\n");
 
  // Close file
  closeFile(fp);
  
  cout << "Saved mesh function to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(MeshFunction<double>& meshfunction)
{
  // Open file
  FILE *fp = openFile();
  
  // Write mesh in XML format
  fprintf(fp, "  <meshfunction type=\"double\" dim=\"%u\" size=\"%u\">\n",
          meshfunction.dim(), meshfunction.size());

  Mesh& mesh = meshfunction.mesh();
  for(MeshEntityIterator e(mesh, meshfunction.dim()); !e.end(); ++e)
  {
      fprintf(fp, "    <entity index=\"%u\" value=\"%g\"/>\n",
              e->index(), meshfunction(*e));
  }

  fprintf(fp, "  </meshfunction>\n");
 
  // Close file
  closeFile(fp);
  
  cout << "Saved mesh function to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(MeshFunction<bool>& meshfunction)
{
  // Open file
  FILE *fp = openFile();
  
  // Write mesh in XML format
  fprintf(fp, "  <meshfunction type=\"bool\" dim=\"%u\" size=\"%u\">\n",
          meshfunction.dim(), meshfunction.size());

  Mesh& mesh = meshfunction.mesh();
  std::string value;
  for (MeshEntityIterator e(mesh, meshfunction.dim()); !e.end(); ++e)
  {
    value = (meshfunction(*e) ? "true" : "false");
    fprintf(fp, "    <entity index=\"%u\" value=\"%s\"/>\n",
              e->index(), value.c_str());
  }

  fprintf(fp, "  </meshfunction>\n");
 
  // Close file
  closeFile(fp);
  
  cout << "Saved mesh function to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Function& f)
{
  // Can only write discrete functions
  if ( f.type() != Function::discrete )
    dolfin_error("Only discrete functions can be saved to file.");

  // Open file
  FILE *fp = openFile();
  
  // Begin function
  fprintf(fp, "  <function> \n");

  // Close file
  closeFile(fp);
  
  // Write the vector
  *this << f.vector();

  // Write the mesh
  *this << f.mesh();

  // Write the finite element specification
  FiniteElementSpec spec = f.element().spec();
  *this << spec;

  // Open file
  fp = openFile();

  // End function
  fprintf(fp, "  </function> \n");

  // Close file
  closeFile(fp);

  cout << "Saved function " << f.name() << " (" << f.label()
       << ") to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(FiniteElementSpec& spec)
{
  // Open file
  FILE *fp = openFile();
  
  // Write element in XML format
  fprintf(fp, "  <finiteelement type=\"%s\" shape=\"%s\" degree=\"%u\" vectordim=\"%u\"/>\n",
  	  spec.type().c_str(), spec.shape().c_str(), spec.degree(), spec.vectordim());
  
  // Close file
  closeFile(fp);

  cout << "Saved finite element specification" << spec
       << " to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(ParameterList& parameters)
{
  // Open file
  FILE *fp = openFile();

  // Write parameter list in XML format
  fprintf(fp, "  <parameters>\n" );

  for (ParameterList::const_iterator it = parameters.parameters.begin(); it != parameters.parameters.end(); ++it)
  {
    const Parameter parameter = it->second;

    switch ( parameter.type() )
    {
    case Parameter::type_int:
      fprintf(fp, "    <parameter name=\"%s\" type=\"int\" value=\"%d\"/>\n",
	      it->first.c_str(), static_cast<int>(parameter));
      break;
    case Parameter::type_real:
      fprintf(fp, "    <parameter name=\"%s\" type=\"real\" value=\"%.16e\"/>\n",
	      it->first.c_str(), static_cast<real>(parameter));
      break;
    case Parameter::type_bool:
      if ( static_cast<bool>(parameter) )
	fprintf(fp, "    <parameter name=\"%s\" type=\"bool\" value=\"true\"/>\n",
		it->first.c_str());
      else
	fprintf(fp, "    <parameter name=\"%s\" type=\"bool\" value=\"false\"/>\n",
		it->first.c_str());
      break;
    case Parameter::type_string:
      fprintf(fp, "    <parameter name=\"%s\" type=\"string\" value=\"%s\"/>\n",
	      it->first.c_str(), static_cast<std::string>(parameter).c_str());
      break;
    default:
      ; // Do nothing
    }

  }
  
  fprintf(fp, "  </parameters>\n" );

  // Close file
  closeFile(fp);

  cout << "Saved parameters to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
void XMLFile::operator<<(Graph& graph)
{
  // Open file
  FILE *fp = openFile();
  
  // Get graph type and number of vertices, edges and arches
  uint num_vertices = graph.numVertices();

  // Write graph in XML format
  fprintf(fp, "  <graph type=\"%s\">\n", graph.typestr().c_str());

  // Get connections (outgoing edges), offsets and weigts
  const uint* connections = graph.connectivity();
  const uint* offsets = graph.offsets();
  const uint* edge_weights = graph.edgeWeights();
  const uint* vertex_weights = graph.vertexWeights();

  dolfin_assert(connections);
  dolfin_assert(offsets);
  dolfin_assert(edge_weights);
  dolfin_assert(vertex_weights);
  
  // Write vertice header 
  fprintf(fp, "    <vertices size=\"%u\">\n", graph.numVertices());

  // Vertices
  for(uint i=0; i<num_vertices; ++i)
  {
	  fprintf(fp, 
          "      <vertex index=\"%u\" num_edges=\"%u\" weight=\"%u\"/>\n", i,
          graph.numEdges(i), vertex_weights[i]);
	  
  }
  fprintf(fp, "    </vertices>\n");

  fprintf(fp, "    <edges size=\"%u\">\n", graph.numEdges());
  // Edges
  for(uint i=0; i<num_vertices; ++i)
  {
    for(uint j=offsets[i]; j<offsets[i] + graph.numEdges(i); ++j)
    {
      // In undirected graphs an edge (v1, v2) is the same as edge (v2, v1)
      // and should not be stored twice
      if ( graph.type() == Graph::directed || i < connections[j] )
        fprintf(fp, 
        "      <edge v1=\"%u\" v2=\"%u\" weight=\"%u\"/>\n",
        i, connections[j], edge_weights[j]);
    }
  }
  fprintf(fp, "    </edges>\n");
  fprintf(fp, "  </graph>\n");
  
  // Close file
  closeFile(fp);

  cout << "Saved graph to file " << filename << " in XML format." << endl;
}
//-----------------------------------------------------------------------------
FILE* XMLFile::openFile()
{
  // Open file
  FILE *fp = fopen(filename.c_str(), "r+");

  // Step to position before previously written footer
  //printf("Stepping to position: %ld\n", mark);
  fseek(fp, mark, SEEK_SET);
  fflush(fp);
  
  // Write DOLFIN XML format header
  if ( !header_written )
  {
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n" );
    fprintf(fp, "<dolfin xmlns:dolfin=\"http://www.fenics.org/dolfin/\">\n" );
    
    header_written = true;
  }

  return fp;
}
//-----------------------------------------------------------------------------
void XMLFile::closeFile(FILE* fp)
{
  // Get position in file before writing footer
  mark = ftell(fp);
  //printf("Position in file before writing footer: %ld\n", mark);

  // Write DOLFIN XML format footer
  if ( header_written )
    fprintf(fp, "</dolfin>\n");

  // Close file
  fclose(fp);
}
//-----------------------------------------------------------------------------
void XMLFile::parseFile()
{
  // Notify that file is being opened
  xmlObject->open(filename);

  // Parse file using the SAX interface
  parseSAX();
  
  // Notify that file is being closed
  if ( !xmlObject->close() )
    dolfin_error("Unable to find data in XML file.");
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
  // Do nothing
}
//-----------------------------------------------------------------------------
void dolfin::sax_end_document(void *ctx)
{
  // Do nothing
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
