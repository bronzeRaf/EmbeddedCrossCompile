%script to create error of real scan times
clear step rr res2


% title:
% timestep = 2 seconds
% add in the queue time (us)


% scan time error (us)
% 
% X axis
% SSID index
% Y axis
% time to be added (usec)


% min max average in every case needed

step = 10;

stop = step*29;

rr = (0:step:stop)';

rr = rr*10e5;

realTime10 = abs(real10-rr);

% plot(realTime)


