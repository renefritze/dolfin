// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __OPEN_DX_FILE_H
#define __OPEN_DX_FILE_H

#include <string>
#include <stdio.h>
#include <dolfin/constants.h>
#include <dolfin/Event.h>
#include <dolfin/Array.h>
#include <dolfin/GenericFile.h>

namespace dolfin
{
    
  class OpenDXFile : public GenericFile
  {
  public:
    
    OpenDXFile(const std::string filename);
    ~OpenDXFile();
    
    // Input
    
    // Output
    
    void operator<< (Mesh& mesh);
    void operator<< (Function& u);

  private:

    void writeHeader   (FILE* fp);
    void writeMesh     (FILE* fp, Mesh& mesh);
    void writeMeshData (FILE* fp, Mesh& mesh);
    void writeFunction (FILE* fp, Function& u);
    void writeSeries   (FILE* fp, Function& u);
  
    void removeSeries  (FILE* fp);

    // Data for each frame
    class Frame {
    public:

      Frame(real time);
      ~Frame();
      
      real time;

    };

    // Frame data
    Array<Frame> frames;

    // Start position for latest time series
    long series_pos;

    // Check if we should save each mesh
    bool save_each_mesh;

    // Events
    Event event_saving_mesh;
    Event event_saving_function;
    
  };
  
}

#endif
