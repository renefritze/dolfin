// Copyright (C) 2007-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Magnus Vikstrom, 2008.
// Modified by Anders Logg, 2008.
//
// First added:  2007-03-13
// Last changed: 2008-07-31

#include <algorithm>
#include <iostream>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/main/MPI.h>
#include "SparsityPattern.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
SparsityPattern::SparsityPattern(uint M, uint N) : range(0)
{
  uint dims[2];
  dims[0] = M;
  dims[1] = N;
  init(2, dims);
}
//-----------------------------------------------------------------------------
SparsityPattern::SparsityPattern(uint M) : range(0)
{
  uint dims[2];
  dims[0] = M;
  dims[1] = 0;
  init(1, dims);
}
//-----------------------------------------------------------------------------
  SparsityPattern::SparsityPattern() : range(0)
{
  dim[0] = 0;
  dim[1] = 0;
  sparsity_pattern.clear();
  o_sparsity_pattern.clear();
}
//-----------------------------------------------------------------------------
SparsityPattern::~SparsityPattern()
{
  if(range)
    delete [] range;
}
//-----------------------------------------------------------------------------
void SparsityPattern::init(uint rank, const uint* dims)
{
  dolfin_assert(rank <= 2);
  dim[0] = dim[1] = 0;
  for (uint i = 0; i < rank; ++i)
    dim[i] = dims[i];
  sparsity_pattern.clear();
  sparsity_pattern.resize(dim[0]);
}
//-----------------------------------------------------------------------------
void SparsityPattern::pinit(uint rank, const uint* dims)
{
  dolfin_assert(rank <= 2);
  dim[0] = dim[1] = 0;
  for (uint i = 0; i < rank; ++i)
    dim[i] = dims[i];
  sparsity_pattern.clear();
  sparsity_pattern.resize(dim[0]);
  o_sparsity_pattern.clear();
  o_sparsity_pattern.resize(dim[0]);
  init_range();
}
//-----------------------------------------------------------------------------
void SparsityPattern::insert(uint m, const uint* rows, uint n, const uint* cols)
{
  for (uint i = 0; i < m; ++i)
    for (uint j = 0; j < n; ++j)
    {
      bool inserted = false;
      uint k = 0;
      while(k < sparsity_pattern[rows[i]].size() && !inserted)
      {
        if(cols[j] == sparsity_pattern[rows[i]][k])
          inserted = true;
        ++k;
      }
      if(!inserted)
        sparsity_pattern[rows[i]].push_back(cols[j]);
    }
}
//-----------------------------------------------------------------------------
void SparsityPattern::pinsert(const uint* num_rows, const uint * const * rows)
{
  error("SparsityPattern::pinsert needs to be updated");

  uint process = dolfin::MPI::process_number();

  for (unsigned int i = 0; i<num_rows[0];++i)
  {
    const uint global_row = rows[0][i];
    // If not in a row "owned" by this processor
    if(global_row < range[process] || global_row >= range[process+1])
      continue;
    for (unsigned int j = 0; j<num_rows[1];++j)
    {
      const uint global_col = rows[1][j];
      // On the off-diagonal
      if(global_col < range[process] || global_col >= range[process+1])
        o_sparsity_pattern[rows[0][i]].insert(rows[1][j]);
      // On the diagonal
//      else
//        sparsity_pattern[rows[0][i]].insert(rows[1][j]);
    }
  }
}
//-----------------------------------------------------------------------------
void SparsityPattern::sort() const
{
  std::vector< std::vector<uint> >::iterator row;
  for(row = sparsity_pattern.begin(); row != sparsity_pattern.end(); ++row)
    std::sort(row->begin(), row->end());
}
//-----------------------------------------------------------------------------
dolfin::uint SparsityPattern::size(uint n) const
{
  dolfin_assert(n < 2);
  return dim[n];
}
//-----------------------------------------------------------------------------
void SparsityPattern::numNonZeroPerRow(uint nzrow[]) const
{
  if ( dim[1] == 0 )
    error("Non-zero entries per row can be computed for matrices only.");

  if ( sparsity_pattern.size() == 0 )
    error("Sparsity pattern has not been computed.");

  // Compute number of nonzeros per row
  std::vector< std::vector<uint> >::const_iterator row;
  for(row = sparsity_pattern.begin(); row != sparsity_pattern.end(); ++row)
    nzrow[row - sparsity_pattern.begin()] = row->size();
}
//-----------------------------------------------------------------------------
void SparsityPattern::numNonZeroPerRow(uint process_number, uint d_nzrow[], uint o_nzrow[]) const
{
  if ( dim[1] == 0 )
    error("Non-zero entries per row can be computed for matrices only.");

  if ( sparsity_pattern.size() == 0 )
    error("Sparsity pattern has not been computed.");

  // Compute number of nonzeros per row diagonal and off-diagonal
  uint offset = range[process_number];
  for(uint i=0; i+offset<range[process_number+1]; ++i)
  {
    d_nzrow[i] = sparsity_pattern[i+offset].size();
    o_nzrow[i] = o_sparsity_pattern[i+offset].size();
  }
}
//-----------------------------------------------------------------------------
dolfin::uint SparsityPattern::numNonZeroPerRowMax() const
{
  if ( dim[1] == 0 )
    error("Total non-zeros entries can be computed for matrices only.");

  if ( sparsity_pattern.size() == 0 )
    error("Sparsity pattern has not been computed.");

  // Compute total number of nonzeros per row
  uint nzrow_max = 0;
  std::vector< std::vector<uint> >::const_iterator set;
  for(set = sparsity_pattern.begin(); set != sparsity_pattern.end(); ++set)
    nzrow_max = std::max(nzrow_max, (dolfin::uint)set->size());
  return nzrow_max;
}
//-----------------------------------------------------------------------------
dolfin::uint SparsityPattern::numNonZero() const
{
  if ( dim[1] == 0 )
    error("Total non-zeros entries can be computed for matrices only.");

  if ( sparsity_pattern.size() == 0 )
    error("Sparsity pattern has not been computed.");

  // Compute total number of nonzeros per row
  uint nz = 0;
  std::vector< std::vector<uint> >::const_iterator set;
  for(set = sparsity_pattern.begin(); set != sparsity_pattern.end(); ++set)
    nz += set->size();
  return nz;
}
//-----------------------------------------------------------------------------
void SparsityPattern::disp() const
{
  if ( dim[1] == 0 )
    warning("Only matrix sparsity patterns can be displayed.");

  // Sort pattern
  sort();

  std::vector< std::vector<uint> >::const_iterator row;
  std::vector<uint>::const_iterator element;

  for(row = sparsity_pattern.begin(); row != sparsity_pattern.end(); ++row)
  {
    cout << "Row " << endl;
    for(element = row->begin(); element != row->end(); ++element)
      cout << *element << " ";
    cout << endl;
  }
}
//-----------------------------------------------------------------------------
void SparsityPattern::process_range(uint process_number, uint local_range[])
{
  local_range[0] = range[process_number];
  local_range[1] = range[process_number + 1];
}
//-----------------------------------------------------------------------------
dolfin::uint SparsityPattern::numLocalRows(uint process_number) const
{
  return range[process_number + 1] - range[process_number];
}
//-----------------------------------------------------------------------------
void SparsityPattern::init_range()
{
  uint num_procs = dolfin::MPI::num_processes();
  range = new uint[num_procs+1];
  range[0] = 0;

  for(uint p=0; p<num_procs; ++p)
    range[p+1] = range[p] + dim[0]/num_procs + ((dim[0]%num_procs) > p ? 1 : 0);
}
//-----------------------------------------------------------------------------
