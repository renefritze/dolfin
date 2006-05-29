# Import solution
from solution import *

# Import matplotlib
from pylab import *

# Plot first component of solution
subplot(211)
plot(t, u[:,0])
grid(True)
xlabel('t')
ylabel('V(t)')
title('Courtemanche')

# Plot time steps
subplot(212)
plot(t, k[:,0])
grid(True)
xlabel('t')
ylabel('k(t)')

# Display value at end time
print "V(T) = " + str(u[len(u) - 1, 0])

# Show plot
show()
