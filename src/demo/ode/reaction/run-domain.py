#!/usr/bin/python
#
# Copyright (C) 2005 Anders Logg.
# Licensed under the GNU GPL Version 2.
#
# Run benchmarks for test problem and collect results

from os import system
from commands import getoutput
from checkerror import checkerror
from shutil import copy

def run_set(method, solver, tolmax, k0, kmax, T, gamma, Nstart):
    "Run set of tests for given data."

    # Open file for storing benchmark results
    file = open("domain.log", "a")
    
    # Write results header
    file.write("\n%s/%s gamma = %.3e\n" % (method, solver, gamma))
    file.write("---------------------------------\n")
    file.write("\nTOL\tError\t\tCPU time\tSteps\t\tIterations\tIndex\tN\n")
    file.write("--------------------------------------------------------------------------------------\n")
    
#    for tol in [tolmax, 0.5*tolmax, 0.1*tolmax, 0.05*tolmax]:
#    for tol in [0.5*tolmax]:

    tol = 1.0 * tolmax

#    for N in [1.0 * Nstart, 10 * Nstart, 100 * Nstart]:
    for N in [1 * Nstart, 2 * Nstart, 4 * Nstart, 8 * Nstart, 16 * Nstart]:
#    for N in [1.0 * Nstart, 10 * Nstart]:

        L = 5.0 * N / Nstart
    
        # Run program
        print "Running %s/%s at tolerance %.1e N %d" % (method, solver, tol, N)
        output = \
            getoutput("./dolfin-ode-reaction %s %s %e %e %e %e %e %d %e %s" % \
                (method, solver, tol, k0, kmax, T, gamma, N, L, \
                 "bench-parameters.xml"))

        # Check if we got any solution
        if len(output.split("Solution stopped")) > 1:
            file.write("Unable to compute solution\n")
            continue

        # Parse output
        cputime = output.split("computed in ")[1].split(" ")[0]
        global_iter = output.split("iterations per step: ")[1].split("\n")[0]
        local_iter = output.split("per global iteration: ")[1].split("\n")[0]
        steps = output.split("(macro) time steps: ")[1].split("\n")[0]
        rejected = output.split("rejected time steps: ")[1].split("\n")[0]
        if method == "mcg":
            index = output.split("efficiency index: ")[1].split("\n")[0]
        else:
            index = 1.0
        size = output.split("ODE of size")[1].split(".")[0]

        # Store end values
        copy("solution.data", "solution-%s-N-%d.data" % (method, N))

        # Compute error
        error = 0.0

        if(method == "mcg"):
            error = checkerror("solution-mcg-N-%d.data" % N, \
                               "solution-cg-N-%d.data" % N)


#        if gamma == 100.0 and N == 1000:
#            error = checkerror("solution.data", "reference-gamma-100.data")
#        elif gamma == 1000.0 and N == 1000:
#            error = checkerror("solution.data", "reference-gamma-1000.data")
#        else:
#            print "Don't have a reference solution for gamma = " + str(gamma)
#            error = 0.0


        # Write results to file
        file.write("%.1e\t%.3e\t%s\t\t%s (%s)\t%s (%s)\t%s %s\n" % \
                   (tol, error, cputime, steps, rejected, global_iter, local_iter, index, size))

        file.flush()

    # Close file
    file.close()

# Move old domain.log to bench.bak
system("mv domain.log domain.bak")

# Run sets of benchmarks for gamma = 100
#run_set("cg",  "newton",      1e-3, 1e-2, 1e-2, 3.0, 100.0, 1000)
#run_set("cg",  "fixed-point", 1e-3, 1e-2, 1e-2, 3.0, 100.0, 1000)
#run_set("mcg", "newton",      1e-3, 1e-2, 1e-2, 3.0, 100.0, 1000)
#run_set("mcg", "fixed-point", 1e-3, 1e-2, 1e-2, 3.0, 100.0, 1000)

# Run sets of benchmarks for gamma = 1000
# (cg/newton temporarily disabled, takes too long)
#run_set("cg",  "newton",      1e-6, 1e-5, 1e-2, 1.0, 1000.0, 1000)
run_set("cg",  "fixed-point", 1e-6, 1e-5, 1e-2, 1.0, 1000.0, 1000)
run_set("mcg", "newton",      1e-6, 1e-5, 1e-2, 1.0, 1000.0, 1000)
run_set("mcg", "fixed-point", 1e-6, 1e-5, 1e-2, 1.0, 1000.0, 1000)

# Print results
system("cat domain.log")

