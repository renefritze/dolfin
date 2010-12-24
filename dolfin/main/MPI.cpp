// Copyright (C) 2007 Magnus Vikstrøm.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007-2009.
// Modified by Anders Logg, 2007-2009.
// Modified by Ola Skavhaug, 2008-2009.
// Modified by Niclas Jansson, 2009.
//
// First added:  2007-11-30
// Last changed: 2010-11-09

#include <dolfin/log/dolfin_log.h>
#include <numeric>
#include "mpiutils.h"
#include "SubSystemsManager.h"
#include "MPI.h"

#ifdef HAS_MPI

#include <mpi.h>

using MPI::COMM_WORLD;

//-----------------------------------------------------------------------------
dolfin::MPICommunicator::MPICommunicator()
{
  MPI_Comm_dup(MPI_COMM_WORLD, &communicator);
}
//-----------------------------------------------------------------------------
dolfin::MPICommunicator::~MPICommunicator()
{
  MPI_Comm_free(&communicator);
}
//-----------------------------------------------------------------------------
MPI_Comm& dolfin::MPICommunicator::operator*()
{
  return communicator;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::process_number()
{
  SubSystemsManager::init_mpi();
  return static_cast<uint>(COMM_WORLD.Get_rank());
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::num_processes()
{
  SubSystemsManager::init_mpi();
  return static_cast<uint>(COMM_WORLD.Get_size());
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::is_broadcaster()
{
  // Always broadcast from processor number 0
  return num_processes() > 1 && process_number() == 0;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::is_receiver()
{
  // Always receive on processors with numbers > 0
  return num_processes() > 1 && process_number() > 0;
}
//-----------------------------------------------------------------------------
void dolfin::MPI::barrier()
{
  MPICommunicator comm;
  MPI_Barrier(*comm);
}
//-----------------------------------------------------------------------------
void dolfin::MPI::distribute(std::vector<uint>& values,
                             std::vector<uint>& partition)
{
  dolfin::distribute(values, partition);
}
//-----------------------------------------------------------------------------
void dolfin::MPI::distribute(std::vector<double>& values,
                             std::vector<uint>& partition)
{
  dolfin::distribute(values, partition);
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<uint>& values, uint sending_process)
{
  // Prepare receive buffer (size 1)
  int receive_buffer = 0;

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Prepare arguments differently depending on whether we're sending
  if (process_number() == sending_process)
  {
    // Check size of values
    if (values.size() != num_processes())
      error("Number of values to scatter must be equal to the number of processes.");

    // Prepare send buffer
    uint* send_buffer = new uint[values.size()];
    for (uint i = 0; i < values.size(); i++)
      send_buffer[i] = values[i];

    // Call MPI to send values
    MPI_Scatter(send_buffer,
                1,
                MPI_UNSIGNED,
                &receive_buffer,
                1,
                MPI_UNSIGNED,
                static_cast<int>(sending_process),
                *comm);

    // Cleanup
    delete [] send_buffer;
  }
  else
  {
    // Call MPI to receive values
    MPI_Scatter(0,
                0,
                MPI_UNSIGNED,
                &receive_buffer,
                1,
                MPI_UNSIGNED,
                static_cast<int>(sending_process),
                *comm);
  }

  // Collect values
  values.clear();
  values.push_back(receive_buffer);
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<std::vector<uint> >& values,
                          uint sending_process)
{
  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Receive buffer
  int recv_count = 0;
  uint* recv_buffer = 0;

  // Prepare arguments differently depending on whether we're sending
  if (process_number() == sending_process)
  {
    // Check size of values
    if (values.size() != num_processes())
      error("Number of values to scatter must be equal to the number of processes.");

    // Extract sizes and compute size of send buffer
    std::vector<uint> sizes;
    for (uint i = 0; i < values.size(); ++i)
      sizes.push_back(values[i].size());
    int send_buffer_size = std::accumulate(sizes.begin(), sizes.end(), 0);

    // Build send data
    uint* send_buffer = new uint[send_buffer_size];
    int* send_counts = new int[values.size()];
    int* send_offsets = new int[values.size()];
    send_offsets[0] = 0;
    uint offset = 0;
    for (uint i = 0; i < values.size(); ++i)
    {
      send_counts[i] = sizes[i];
      send_offsets[i] = offset;
      for (uint j = 0; j < values[i].size(); ++j)
        send_buffer[offset++] = values[i][j];
    }

    // Scatter number of values that will be scattered (note that sizes will be modified)
    scatter(sizes, sending_process);

    // Prepare receive buffer
    recv_count = sizes[0];
    recv_buffer = new uint[recv_count];

    // Call MPI to send values
    MPI_Scatterv(send_buffer,
                 send_counts,
                 send_offsets,
                 MPI_UNSIGNED,
                 recv_buffer,
                 recv_count,
                 MPI_UNSIGNED,
                 static_cast<int>(sending_process),
                 *comm);

    // Cleanup
    delete [] send_buffer;
    delete [] send_counts;
    delete [] send_offsets;
  }
  else
  {
    // Receive number of values that will be scattered
    std::vector<uint> sizes;
    scatter(sizes, sending_process);

    // Prepare receive buffer
    recv_count = sizes[0];
    recv_buffer = new uint[recv_count];

    // Call MPI to receive values
    MPI_Scatterv(0,
                 0,
                 0,
                 MPI_UNSIGNED,
                 recv_buffer,
                 recv_count,
                 MPI_UNSIGNED,
                 static_cast<int>(sending_process),
                 *comm);
  }

  // Copy values from receive buffer
  values.clear();
  values.resize(1);
  for (int i = 0; i < recv_count; ++i)
    values[0].push_back(recv_buffer[i]);

  // Cleanup
  delete [] recv_buffer;
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<std::vector<double> >& values,
                          uint sending_process)
{
  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Receive buffer
  int recv_count = 0;
  double* recv_buffer = 0;

  // Prepare arguments differently depending on whether we're sending
  if (process_number() == sending_process)
  {
    // Check size of values
    if (values.size() != num_processes())
      error("Number of values to scatter must be equal to the number of processes.");

    // Extract sizes and compute size of send buffer
    std::vector<uint> sizes;
    for (uint i = 0; i < values.size(); ++i)
      sizes.push_back(values[i].size());
    int send_buffer_size = std::accumulate(sizes.begin(), sizes.end(), 0);

    // Build send data
    double* send_buffer = new double[send_buffer_size];
    int* send_counts = new int[values.size()];
    int* send_offsets = new int[values.size()];
    send_offsets[0] = 0;
    uint offset = 0;
    for (uint i = 0; i < values.size(); ++i)
    {
      send_counts[i] = sizes[i];
      send_offsets[i] = offset;
      for (uint j = 0; j < values[i].size(); ++j)
        send_buffer[offset++] = values[i][j];
    }

    // Scatter number of values that will be scattered (note that sizes will be modified)
    scatter(sizes, sending_process);

    // Prepare receive buffer
    recv_count = sizes[0];
    recv_buffer = new double[recv_count];

    // Call MPI to send values
    MPI_Scatterv(send_buffer,
                 send_counts,
                 send_offsets,
                 MPI_DOUBLE,
                 recv_buffer,
                 recv_count,
                 MPI_DOUBLE,
                 static_cast<int>(sending_process),
                 *comm);

    // Cleanup
    delete [] send_buffer;
    delete [] send_counts;
    delete [] send_offsets;
  }
  else
  {
    // Receive number of values that will be scattered
    std::vector<uint> sizes;
    scatter(sizes, sending_process);

    // Prepare receive buffer
    recv_count = sizes[0];
    recv_buffer = new double[recv_count];

    // Call MPI to receive values
    MPI_Scatterv(0,
                 0,
                 0,
                 MPI_DOUBLE,
                 recv_buffer,
                 recv_count,
                 MPI_DOUBLE,
                 static_cast<int>(sending_process),
                 *comm);
  }

  // Copy values from receive buffer
  values.clear();
  values.resize(1);
  for (int i = 0; i < recv_count; ++i)
    values[0].push_back(recv_buffer[i]);

  // Cleanup
  delete [] recv_buffer;
}
//-----------------------------------------------------------------------------
std::vector<dolfin::uint> dolfin::MPI::gather(uint value)
{
  std::vector<uint> values(num_processes());
  values[process_number()] = value;
  gather(values);
  return values;
}
//-----------------------------------------------------------------------------
void dolfin::MPI::gather(std::vector<uint>& values)
{
  assert(values.size() == num_processes());

  // Prepare arrays
  uint send_value = values[process_number()];
  uint* received_values = new uint[values.size()];

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Call MPI
  MPI_Allgather(&send_value,     1, MPI_UNSIGNED,
                received_values, 1, MPI_UNSIGNED, *comm);

  // Copy values
  for (uint i = 0; i < values.size(); i++)
    values[i] = received_values[i];

  // Cleanup
  delete [] received_values;
}
//-----------------------------------------------------------------------------
void dolfin::MPI::gather(std::vector<double>& values)
{
  assert(values.size() == num_processes());

  // Prepare arrays
  double send_value = values[process_number()];
  double* received_values = new double[values.size()];

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Call MPI
  MPI_Allgather(&send_value,     1, MPI_DOUBLE,
                received_values, 1, MPI_DOUBLE, *comm);

  // Copy values
  for (uint i = 0; i < values.size(); i++)
    values[i] = received_values[i];

  // Cleanup
  delete [] received_values;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::global_maximum(uint size)
{
  uint recv_size = 0;
  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;
  MPI_Allreduce(&size, &recv_size, 1, MPI_UNSIGNED, MPI_MAX, *comm);
  return recv_size;
}
//-----------------------------------------------------------------------------
double dolfin::MPI::sum(double value)
{
  double recv_value = 0.0;
  MPICommunicator comm;
  MPI_Allreduce(&value, &recv_value, 1, MPI_DOUBLE, MPI_SUM, *comm);
  return recv_value;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::sum(uint value)
{
  uint recv_value = 0;
  MPICommunicator comm;
  MPI_Allreduce(&value, &recv_value, 1, MPI_UNSIGNED, MPI_SUM, *comm);
  return recv_value;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::global_offset(uint range, bool exclusive)
{
  uint offset = 0;

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Compute inclusive or exclusive partial reduction
  if (exclusive)
    MPI_Exscan(&range, &offset, 1, MPI_UNSIGNED, MPI_SUM, *comm);
  else
    MPI_Scan(&range, &offset, 1, MPI_UNSIGNED, MPI_SUM, *comm);

  return offset;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::send_recv(uint* send_buffer, uint send_size, uint dest,
                                    uint* recv_buffer, uint recv_size, uint source)
{
  MPI_Status status;

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Send and receive data
  MPI_Sendrecv(send_buffer, static_cast<int>(send_size), MPI_UNSIGNED, static_cast<int>(dest), 0,
               recv_buffer, static_cast<int>(recv_size), MPI_UNSIGNED, static_cast<int>(source),  0,
               *comm, &status);

  // Check number of received values
  int num_received = 0;
  MPI_Get_count(&status, MPI_UNSIGNED, &num_received);
  assert(num_received >= 0);

  return static_cast<uint>(num_received);
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::send_recv(double* send_buffer, uint send_size, uint dest,
                                    double* recv_buffer, uint recv_size, uint source)
{
  MPI_Status status;

  // Create communicator (copy of MPI_COMM_WORLD)
  MPICommunicator comm;

  // Send and receive data
  MPI_Sendrecv(send_buffer, static_cast<int>(send_size), MPI_DOUBLE, static_cast<int>(dest), 0,
               recv_buffer, static_cast<int>(recv_size), MPI_DOUBLE, static_cast<int>(source),  0,
               *comm, &status);

  // Check number of received values
  int num_received = 0;
  MPI_Get_count(&status, MPI_DOUBLE, &num_received);
  assert(num_received >= 0);

  return static_cast<uint>(num_received);
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint N)
{
  return local_range(process_number(), N);
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint process,
                                                               uint N)
{
  return local_range(process, N, num_processes());
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint process,
                                                               uint N,
                                                               uint num_processes)
{
  // Compute number of items per process and remainder
  const uint n = N / num_processes;
  const uint r = N % num_processes;

  // Compute local range
  std::pair<uint, uint> range;
  if (process < r)
  {
    range.first = process*(n + 1);
    range.second = range.first + n + 1;
  }
  else
  {
    range.first = process*n + r;
    range.second = range.first + n;
  }

  return range;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::index_owner(uint index, uint N)
{
  assert(index < N);

  // Get number of processes
  const uint _num_processes = num_processes();

  // Compute number of items per process and remainder
  const uint n = N / _num_processes;
  const uint r = N % _num_processes;

  // First r processes own n + 1 indices
  if (index < r * (n + 1))
    return index / (n + 1);

  // Remaining processes own n indices
  return r + (index - r * (n + 1)) / n;
}
//-----------------------------------------------------------------------------

#else

//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::process_number()
{
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::num_processes()
{
  return 1;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::is_broadcaster()
{
  return false;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::is_receiver()
{
  return false;
}
//-----------------------------------------------------------------------------
void dolfin::MPI::barrier()
{
  error("MPI::barrier() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::distribute(std::vector<uint>& values,
                             std::vector<uint>& partition)
{
  error("MPI::distribute() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::distribute(std::vector<double>& values,
                             std::vector<uint>& partition)
{
  error("MPI::distribute() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<uint>& values, uint sending_process)
{
  error("MPI::scatter() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<std::vector<uint> >& values,
                          uint sending_process)
{
  error("MPI::scatter() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::scatter(std::vector<std::vector<double> >& values,
                          uint sending_process)
{
  error("MPI::scatter() requires MPI.");
}
//-----------------------------------------------------------------------------
std::vector<dolfin::uint> dolfin::MPI::gather(uint value)
{
  error("MPI::gather() requires MPI.");
  return std::vector<uint>(1);
}
//-----------------------------------------------------------------------------
void dolfin::MPI::gather(std::vector<uint>& values)
{
  error("MPI::gather() requires MPI.");
}
//-----------------------------------------------------------------------------
void dolfin::MPI::gather(std::vector<double>& values)
{
  error("MPI::gather() requires MPI.");
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::global_maximum(uint size)
{
  return size;
}
//-----------------------------------------------------------------------------
double dolfin::MPI::sum(double value)
{
  return value;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::sum(uint value)
{
  return value;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::global_offset(uint range, bool exclusive)
{
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::send_recv(uint* send_buffer, uint send_size, uint dest,
                                    uint* recv_buffer, uint recv_size, uint source)
{
  error("MPI::send_recv() requires MPI.");
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::send_recv(double* send_buffer, uint send_size, uint dest,
                                    double* recv_buffer, uint recv_size, uint source)
{
  error("MPI::send_recv() requires MPI.");
  return 0;
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint N)
{
  return std::make_pair(0, N);
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint process,
                                                               uint N)
{
  if (process != 0 || num_processes() > 1)
    error("MPI is required for local_range with more than one process.");
  return std::make_pair(0, N);
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> dolfin::MPI::local_range(uint process,
                                                               uint N,
                                                               uint num_processes)
{
  if (process != 0 || num_processes > 1)
    error("MPI is required for local_range with more than one process.");
  return std::make_pair(0, N);
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::index_owner(uint i, uint N)
{
  assert(i < N);
  return 0;
}
//-----------------------------------------------------------------------------

#endif
