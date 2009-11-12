// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-11-11
// Last changed: 2009-11-11

#ifndef __TIME_SERIES_H
#define __TIME_SERIES_H

#include <string>
#include <dolfin/common/Variable.h>

namespace dolfin
{

  // Forward declarations
  class GenericVector;
  class Mesh;

  /// This class stores a time series of objects to file(s) in a
  /// binary format which is efficient for reading and writing.
  ///
  /// When objects are retrieved, the object stored at the time
  /// closest to the given time will be used.

  class TimeSeries : public Variable
  {
  public:

    /// Create empty time series
    TimeSeries(std::string name);

    /// Destructor
    ~TimeSeries();

    /// Store vector at given time
    void store(const GenericVector& vector, double t);

    /// Store mesh at given time
    void store(const Mesh& mesh, double t);

    /// Retrieve vector at given time
    void retrieve(GenericVector& vector, double t) const;

    /// Retrieve mesh at given time
    void retrieve(Mesh& mesh, double t) const;

    /// Return filename
    static std::string filename(std::string series_name, std::string type_name, uint index);

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  private:

    // Name of series
    std::string _series_name;

    // List of times
    std::vector<double> _vector_times;
    std::vector<double> _mesh_times;

  };

}

#endif
