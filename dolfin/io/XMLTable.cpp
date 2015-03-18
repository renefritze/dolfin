// Copyright (C) 2015 Jan Blechta
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.

#include "pugixml.hpp"
#include <dolfin/log/log.h>
#include <dolfin/log/Table.h>
#include "XMLTable.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void XMLTable::write(const Table& table, pugi::xml_node xml_node)
{
  // Temporaries
  auto it_double(table.dvalues.cbegin());
  auto it_string(table.values.cbegin());

  // Add table node
  pugi::xml_node table_node = xml_node.append_child("table");
  table_node.append_attribute("name") = table.name().c_str();

  // Loop over rows and columns
  for (std::size_t i = 0; i < table.rows.size(); ++i)
  {
    pugi::xml_node row_node = table_node.append_child("row");
    row_node.append_attribute("key") = table.rows[i].c_str();
    for (std::size_t j = 0; j < table.cols.size(); ++j)
    {
      pugi::xml_node col_node = row_node.append_child("col");
      col_node.append_attribute("key") = table.cols[j].c_str();
      const auto key = std::make_pair(table.rows[i], table.cols[j]);
      it_double = table.dvalues.find(key);
      if (it_double != table.dvalues.end())
      {
        col_node.append_attribute("type") = "double";
        col_node.append_attribute("value") = it_double->second;
      }
      else
      {
        it_string = table.values.find(key);
        if (it_string == table.values.end())
          dolfin_error("XMLTable.cpp",
                       "write XML output for table",
                       "Table is not rectangular, element(%u, %u) is missing",
                       i, j);
        col_node.append_attribute("type") = "string";
        col_node.append_attribute("value") = it_string->second.c_str();
      }
    }
  }
}
//-----------------------------------------------------------------------------
