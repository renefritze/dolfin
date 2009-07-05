// Instantiate io template functions 

// Input
%template(__rshift__) dolfin::File::operator>> <GenericVector>;
%template(__rshift__) dolfin::File::operator>> <GenericMatrix>;
%template(__rshift__) dolfin::File::operator>> <Mesh>;
%template(__rshift__) dolfin::File::operator>> <LocalMeshData>;
%template(__rshift__) dolfin::File::operator>> <MeshFunction<int> >;
%template(__rshift__) dolfin::File::operator>> <MeshFunction<unsigned int> >;
%template(__rshift__) dolfin::File::operator>> <MeshFunction<double> >;
%template(__rshift__) dolfin::File::operator>> <MeshFunction<bool> >;
%template(__rshift__) dolfin::File::operator>> <Sample>;
%template(__rshift__) dolfin::File::operator>> <NewParameters>;
%template(__rshift__) dolfin::File::operator>> <FunctionPlotData>;

// Output
%template(__lshift__) dolfin::File::operator<< <GenericVector>;
%template(__lshift__) dolfin::File::operator<< <GenericMatrix>;
%template(__lshift__) dolfin::File::operator<< <Mesh>;
%template(__lshift__) dolfin::File::operator<< <LocalMeshData>;
%template(__lshift__) dolfin::File::operator<< <MeshFunction<int> >;
%template(__lshift__) dolfin::File::operator<< <MeshFunction<unsigned int> >;
%template(__lshift__) dolfin::File::operator<< <MeshFunction<double> >;
%template(__lshift__) dolfin::File::operator<< <MeshFunction<bool> >;
%template(__lshift__) dolfin::File::operator<< <Sample>;
%template(__lshift__) dolfin::File::operator<< <NewParameters>;
%template(__lshift__) dolfin::File::operator<< <Function>;
%template(__lshift__) dolfin::File::operator<< <FunctionPlotData>;


