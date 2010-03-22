// Copyright (C) 2007 Magnus Vikstrøm.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
// Modified by Ola Skavhaug, 2008-2009.
// Modified by Anders Logg, 2008-2009.
// Modified by Niclas Jansson, 2009.
//
// First added:  2007-11-30
// Last changed: 2009-08-18

#ifndef __MPI_DOLFIN_WRAPPER_H
#define __MPI_DOLFIN_WRAPPER_H

#include <vector>
#include <dolfin/common/types.h>

#ifdef HAS_MPI
//#include <boost/mpi.hpp>
#include <mpi.h>
#endif

namespace dolfin
{

#ifdef HAS_MPI
  class MPICommunicator
  {

  public:

    /// Create communicator (copy of MPI_COMM_WORLD)
    MPICommunicator();

    /// Destructor
    ~MPICommunicator();

    /// Dereference operator
    MPI_Comm& operator*();

  private:
    MPI_Comm communicator;
  };
#endif

  /// This class provides utility functions for easy communcation with MPI.

  class MPI
  {
  public:

    /// Return proccess number
    static uint process_number();

    /// Return number of processes
    static uint num_processes();

    /// Determine whether we should broadcast (based on current parallel policy)
    static bool is_broadcaster();

    /// Determine whether we should receive (based on current parallel policy)
    static bool is_receiver();

    /// Set a barrier (synchronization point)
    static void barrier();

    // FIXME: Write documentation for this very fancy and versatile function!
    // FIXME: The mother of all MPI calls! It does everything anyone would ever
    //        need to do with MPI... :-)

    /// Distribute local arrays on all processors according to given partition
    static void distribute(std::vector<uint>& values, std::vector<uint>& partition);

    /// Distribute local arrays on all processors according to given partition
    static void distribute(std::vector<double>& values, std::vector<uint>& partition);

    // FIXME: Use common template function for uint and double scatter below

    /// Scatter values, one to each process
    static void scatter(std::vector<uint>& values, uint sending_process=0);

    /// Scatter values (wrapper for MPI_Scatterv)
    static void scatter(std::vector<std::vector<uint> >& values, uint sending_process=0);

    /// Scatter values (wrapper for MPI_Scatterv)
    static void scatter(std::vector<std::vector<double> >& values, uint sending_process=0);

    /// Gather values, one from each process (wrapper for MPI_Allgather)
    static std::vector<uint> gather(uint value);

    /// Gather values, one from each process (wrapper for MPI_Allgather)
    static void gather(std::vector<uint>& values);

    /// Gather values, one from each process (wrapper for MPI_Allgather)
    static void gather(std::vector<double>& values);

    // Commented out due to Boost MPI bug in Ubuntu Karmic
    /*
    /// Gather values, one from each process (wrapper for boost::mpi::all_gather)
    template<class T> static void gather_all(const T& in_value,
                                             std::vector<T>& out_values)
    {
      #ifdef HAS_MPI
      MPICommunicator mpi_comm;
      boost::mpi::communicator comm(*mpi_comm, boost::mpi::comm_duplicate);
      boost::mpi::all_gather(comm, in_value, out_values);
      #else
      out_values.clear();
      #endif
    }
    */

    /// Find global max value (wrapper for MPI_Allredue with MPI_MAX as
    /// reduction op)
    static uint global_maximum(uint size);

    /// Sum values and return sum
    static double sum(double value);
    static uint sum(uint value);

    /// Find global offset (index) (wrapper for MPI_(Ex)Scan with MPI_SUM as
    /// reduction op)
    static uint global_offset(uint range, bool exclusive);

    /// Send-receive and return number of received values (wrapper for MPI_Sendrecv)
    static uint send_recv(uint* send_buffer, uint send_size, uint dest,
                          uint* recv_buffer, uint recv_size, uint source);

    /// Send-receive and return number of received values (wrapper for MPI_Sendrecv)
    static uint send_recv(double* send_buffer, uint send_size, uint dest,
                          double* recv_buffer, uint recv_size, uint source);

    /// Return local range for local process, splitting [0, N - 1] into
    ///  num_processes() portions of almost equal size
    static std::pair<uint, uint> local_range(uint N);

    /// Return local range for given process, splitting [0, N - 1] into
    /// num_processes() portions of almost equal size
    static std::pair<uint, uint> local_range(uint process, uint N);

    /// Return which process owns index (inverse of local_range)
    static uint index_owner(uint index, uint N);

  };

}

#endif
