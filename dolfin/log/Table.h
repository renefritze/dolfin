// Copyright (C) 2008-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-07-19
// Last changed: 2009-05-08

#ifndef __TABLE_H
#define __TABLE_H

#include <vector>
#include <set>
#include <map>

#include <dolfin/common/types.h>

namespace dolfin
{

  class TableEntry;

  /// This class provides storage and pretty-printing for tables.
  /// Example usage:
  ///
  ///   Table table("Timings");
  ///
  ///   table("uBLAS",  "Assemble") = 0.010;
  ///   table("uBLAS",  "Solve")    = 0.020;
  ///   table("PETSc",  "Assemble") = 0.011;
  ///   table("PETSc",  "Solve")    = 0.019;
  ///   table("Epetra", "Assemble") = 0.012;
  ///   table("Epetra", "Solve")    = 0.018;
  ///
  ///   table.print();

  class Table
  {
  public:

    /// Create empty table
    Table(std::string title="");

    /// Destructor
    ~Table();

    /// Return table entry
    TableEntry operator() (std::string row, std::string col);

    /// Set value of table entry
    void set(std::string row, std::string col, int value);

    /// Set value of table entry
    void set(std::string row, std::string col, uint value);

    /// Set value of table entry
    void set(std::string row, std::string col, double value);

    /// Set value of table entry
    void set(std::string row, std::string col, std::string value);

    /// Get value of table entry
    std::string get(std::string row, std::string col) const;

    /// Return table title
    std::string title() const;

    /// Assignment operator
    const Table& operator= (const Table& table);

    /// Print table, rounding small numbers to zero
    void print() const;

  private:

    // Table title
    std::string _title;

    // Rows
    std::vector<std::string> rows;
    std::set<std::string> row_set;

    // Columns
    std::vector<std::string> cols;
    std::set<std::string> col_set;

    // Table values
    std::map<std::pair<std::string, std::string>, std::string> values;

  };

  /// This class represents an entry in a Table

  class TableEntry
  {
  public:

    /// Create table entry
    TableEntry(std::string row, std::string col, Table& table);

    /// Destructor
    ~TableEntry();

    /// Assign value to table entry
    const TableEntry& operator= (uint value);

    /// Assign value to table entry
    const TableEntry& operator= (int value);

    /// Assign value to table entry
    const TableEntry& operator= (double value);

    /// Assign value to table entry
    const TableEntry& operator= (std::string value);

    /// Cast to entry value
    operator std::string() const;

  private:

    // Row
    std::string row;

    // Column
    std::string col;

    // Table
    Table& table;

  };

}

#endif
