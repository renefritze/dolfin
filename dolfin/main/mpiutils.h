// Copyright (C) 2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-12-09
// Last changed: 2008-12-09
//
// Template utilities for MPI class placed here so it will not clutter
// the MPI class.

#ifndef __MPI_UTILS_H
#define __MPI_UTILS_H

#include <vector>
#include <dolfin/log/log.h>
#include <dolfin/common/types.h>
#include "MPI.h"

#ifdef HAS_MPI

#include <mpi.h>

namespace dolfin
{

  /// Distribute local arrays on all processors according to given partition
  template <typename T>
  void distribute(std::vector<T>& values, const std::vector<uint> partition)
  {
    dolfin_assert(values.size() == partition.size());
    dolfin_assert(values.size() > 0);

    // Get number of processes and process number
    const uint num_processes = MPI::num_processes();
    const uint process_number = MPI::process_number();

    // Sort out data that should be sent to other processes
    std::vector<std::vector<T> > send_data(num_processes);
    for (uint i = 0; i < values.size(); i++)
    {
      // Get process number data should be sent to
      const uint p = partition[i];
      dolfin_assert(p < send_data.size());

      // Append data to array for process p
      send_data[p].push_back(values[i]);
    }

    // Store local data (don't send)
    values.clear();
    const std::vector<T>& local_values = send_data[process_number];
    for (uint i = 0; i < local_values.size(); i++)
      values.push_back(local_values[i]);

    // Determine size of send buffer
    uint send_buffer_size = 0;
    for (uint p = 0; p < send_data.size(); p++)
      send_buffer_size = std::max(send_buffer_size, send_data[p].size());

    // Determine size of receive buffer (same for all processes)
    uint recv_buffer_size = 0;
    for (uint p = 0; p < send_data.size(); p++)
    {
      uint send_size = send_data[p].size();
      MPI_Reduce(&send_size, &recv_buffer_size, 1, MPI_UNSIGNED, MPI_MAX, p, MPI_COMM_WORLD);
    }

    // Allocate memory for send and receive buffers
    dolfin_assert(send_buffer_size > 0);
    dolfin_assert(recv_buffer_size > 0);
    T* send_buffer = new T[send_buffer_size];
    T* recv_buffer = new T[recv_buffer_size];

    // Exchange data
    for (uint i = 1; i < send_data.size(); i++)
    {
      // We receive data from process p - i (i steps to the left)
      const int source = (process_number - i + num_processes) % num_processes;

      // We send data to process p + i (i steps to the right)
      const int dest = (process_number + i) % num_processes;
    
      // Copy data to send buffer
      for (uint j = 0; j < send_data[dest].size(); j++)
        send_buffer[j] = send_data[dest][j];

      // Send and receive data
      const uint num_received = MPI::send_recv(send_buffer, send_data[dest].size(), dest,
                                               recv_buffer, send_buffer_size,       source);

      // Copy data from receive buffer
      dolfin_assert(num_received <= recv_buffer_size);
      for (uint j = 0; j < num_received; j++)
        values.push_back(recv_buffer[j]);
    }

    // Clean up
    delete [] send_buffer;
    delete [] recv_buffer;
  }

}

#else

namespace dolfin
{

  /// Distribute local arrays on all processors according to given partition
  template <typename T>
  void distribute(std::vector<T>& values, const std::vector<uint> partition)
  {
    error("Distribution of partitioned values requires MPI.");
  }

}

#endif

#endif
