#!/usr/bin/env python
#
# Copyright (C) 2006 Anders Logg.
# Licensed under the GNU LGPL Version 2.1.
#
# Run benchmark for mesh

from os import system
from time import time

# Log file
logfile = "bench.log"

# Run benchmark
tic = time()
system("./dolfin-bench-mesh")
toc = time() - tic

# Save timing to log file
system("pkg-config --modversion dolfin >> " + logfile)
system("date >> " + logfile)
file = open(logfile, "a")
file.write("CPU time: %g\n" % toc)
file.write("----------------------------------------\n")
file.close()
