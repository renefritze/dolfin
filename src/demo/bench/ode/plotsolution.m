% Copyright (C) 2005 Johan Jansson and Anders Logg.
% Licensed under the GNU GPL Version 2.
%
% First added:  2005-04-11
% Last changed: 2005

% Load data
solutionu
solutionv
timesteps
residual

% Get minimum and maximum for u and v
m = length(u);
vmin = 0; vmax = 0;
umin = 0; umax = 0;
kmin = 0; kmax = 0;
rmin = 0; rmax = 0;
for i = 1:m
  umin = min(umin, min(u{i})); umax = max(umax, max(u{i}));
  vmin = min(vmin, min(v{i})); vmax = max(vmax, max(v{i}));
  kmin = min(kmin, min(k{i})); kmax = max(kmax, max(k{i}));
  rmin = min(rmin, min(r{i})); rmax = max(rmax, max(r{i}));
end

% Plot solution
clf
for i = 1:m
  
  subplot(2, 4, 1)
  pdesurf(points, cells, u{i})
  axis([0 1 0 1 umin umax])
  xlabel('x')
  ylabel('y')
  zlabel('u')
  
  subplot(2, 4, 2)
  pdesurf(points, cells, v{i})
  axis([0 1 0 1 vmin vmax])
  xlabel('x')
  ylabel('y')
  zlabel('v')
  
  subplot(2, 4, 3)
  pdesurf(points, cells, k{i})
  axis([0 1 0 1 kmin kmax])
  xlabel('x')
  ylabel('y')
  zlabel('k')
  
  subplot(2, 4, 4)
  pdesurf(points, cells, r{i})
  axis([0 1 0 1 rmin rmax])
  xlabel('x')
  ylabel('y')
  zlabel('r')
  
  subplot(2, 4, 5)
  pdesurf(points, cells, u{i})
  view(2)
  xlabel('x')
  ylabel('y')
  title('u')
  
  subplot(2, 4, 6)
  pdesurf(points, cells, v{i})
  view(2)
  xlabel('x')
  ylabel('y')
  title('v')

  subplot(2, 4, 7)
  pdesurf(points, cells, k{i})
  view(2)
  xlabel('x')
  ylabel('y')
  title('k')
  
  subplot(2, 4, 8)
  pdesurf(points, cells, r{i})
  view(2)
  xlabel('x')
  ylabel('y')
  title('r')
  
  disp('Press any key to continue')
  pause
    
end
