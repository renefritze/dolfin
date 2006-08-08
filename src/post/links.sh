#!/bin/sh

cd ../../
TOPLEVEL=`pwd`

# Function for creating symbolic links to Octave scripts
link_octave() {
    echo $1
    ln -sf $TOPLEVEL/src/utils/octave/pdeplot.m $1
    ln -sf $TOPLEVEL/src/utils/octave/pdemesh.m $1
    ln -sf $TOPLEVEL/src/utils/octave/pdesurf.m $1
}

# Function for creating symbolic links to OpenDX programs
link_dx() {
    ln -sf $TOPLEVEL/src/utils/opendx/dolfin.net $1
    ln -sf $TOPLEVEL/src/utils/opendx/dolfin.cfg $1
}

# Function for creating symbolic link to plotslab.m
link_plotslab() {
    ln -sf $TOPLEVEL/src/utils/matlab/plotslab.m $1
}

# Create links to plotslab script
#link_plotslab src/demo/ode/stiff

# Create links to Octave scripts
link_octave src/demo/solvers/elasticity
link_octave src/demo/solvers/wave

# Create links to meshes
ln -sf $TOPLEVEL/data/meshes/gear.xml.gz     src/demo/pde/elasticity
ln -sf $TOPLEVEL/data/meshes/dolfin-2.xml.gz src/demo/pde/stokes/taylor-hood
ln -sf $TOPLEVEL/data/meshes/dolfin-2.xml.gz src/demo/pde/stokes/stabilized
ln -sf $TOPLEVEL/data/meshes/dolfin-2.xml.gz src/demo/pde/convection-diffusion
