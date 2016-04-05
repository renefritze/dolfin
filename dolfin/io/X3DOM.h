// Copyright (C) 2016 Quang T. Ha, Chris Richardson and Garth N. Wells
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

#ifndef __DOLFIN_X3DOM_H
#define __DOLFIN_X3DOM_H

#include <array>
#include <set>
#include <string>
#include <vector>
#include "pugixml.hpp"

namespace dolfin
{

  /// Class data to store X3DOM view parameters.
  struct X3DParameters
  {
    // Developer note: X3DParameters is declared outside the X3DOM
    // class because SWIG cannot wrap nested classes.

    /// X3DOM representation type
    enum class Representation {surface, surface_with_edges, wireframe};

    /// Constructor (with default parameter settings)
    X3DParameters()
      : representation(Representation::surface_with_edges),
        show_viewpoint_buttons(true),
        diffusive_colour({0.7, 0.7, 0.7}),
        emissive_colour({0.7, 0.7, 0.7}),
        specular_colour({0.2, 0.2, 0.2}),
        background_colour({1.0, 1.0, 1.0}),
        ambient_intensity(0.4),
        shininess(0.8),
        transparency(0.0)
    {
      // Do nothing
    }

    void set_background_colour(std::vector<double> rgb)
    {
      background_colour[0] = rgb[0];
      background_colour[0] = rgb[1];
      background_colour[0] = rgb[2];
    }

    std::array<double, 3> get_background_colour_array() const
    {
      return background_colour;
    }

    std::vector<double> get_background_colour() const
    {
      return std::vector<double>(background_colour.begin(),
                                 background_colour.end());
    }

    /// Surface, surface with edges or wireframe
    Representation representation;

    /// Toggle view point buttons
    bool show_viewpoint_buttons;

    // TODO: document
    //std::string diffusive_colour, emissive_colour, specular_colour;
    std::array<double , 3> diffusive_colour, emissive_colour, specular_colour,
      background_colour;

    // TODO: document
    double ambient_intensity, shininess, transparency;
  };


  /// This class implements output of meshes to X3DOM XML or HTML or
  /// string

  class X3DOM
  {
  public:

    /// Return X3D string for a Mesh, default colour and viewpoints
    static std::string str(const Mesh& mesh);

    /// Return X3D string for a Mesh, user-defined parameters
    static std::string str(const Mesh& mesh, X3DParameters paramemeters);

    /// Return HTML string with embedded X3D, default options
    static std::string html(const Mesh& mesh);

    /// Return HTML string with embedded X3D, user-defined
    static std::string html(const Mesh& mesh, X3DParameters parameters);

  private:

    // Check the colour vectors
    //static bool check_colour(const std::vector<double>& material_colour,
    //                         const std::array<double, 3> bg);

    // Add X3D doctype (an XML document should have no more than one
    // doc_type node)
    static void add_doctype(pugi::xml_node& xml_node);

    // Add X3D node and attributes, and return handle to node
    static pugi::xml_node add_x3d(pugi::xml_node& xml_node);

    // Add X3DOM mesh data to XML node
    static void x3dom_xml(pugi::xml_node& xml_node, const Mesh& mesh,
                          X3DParameters::Representation facet_type,
                          bool show_viewpoint_buttons,
                          const std::array<double, 3> bg);

    // Get mesh dimensions and viewpoint distance
    static std::vector<double> mesh_min_max(const Mesh& mesh);

    // Get list of vertex indices which are on surface
    static std::set<int> surface_vertex_indices(const Mesh& mesh);

    // Add mesh topology and geometry to XML, including either Facets
    // or Edges (depending on the facet_type flag). In 3D, only
    // include surface Facets/Edges.
    static void add_mesh(pugi::xml_node& xml_node, const Mesh& mesh,
                         X3DParameters::Representation facet_type);

    // Add header to XML document, adjusting field of view to the size
    // of the object
    static pugi::xml_node
    add_xml_header(pugi::xml_node& xml_node,
                   const std::vector<double>& xpos,
                   X3DParameters::Representation facet_type,
                   bool show_viewpoint_button,
                   const std::array<double, 3> bg);

    // Add control tags options for html
    static void add_viewpoint_control_option(pugi::xml_node& viewpoint_control,
                                             std::string viewpoint);

    // Add viewpoints to scene node
    static void
    add_viewpoint_xml_nodes(pugi::xml_node& xml_scene,
                            const std::vector<double>& xpos,
                            bool show_viewpoint_buttons);

    // Generate viewpoint nodes
    static void generate_viewpoint_nodes(pugi::xml_node& xml_scene,
                                         const size_t viewpoint,
					 const std::string center_of_rotation,
                                         const std::vector<double>& xpos);

    // Add shape node to XML document, and push the shape node to
    // first child
    static void add_shape_node(pugi::xml_node& x3d_scene,
                               X3DParameters::Representation facet_type);

    // Get a string representing a color palette (pal may be 0, 1 or 2)
    static std::string color_palette(const size_t pal);

    // Generate X3D string from facet_type
    static std::string x3d_str(X3DParameters::Representation facet_type);

  };

}

#endif
