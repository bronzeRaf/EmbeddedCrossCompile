# ZsunCrossCombile
A set of cross-compiled project, designed to run on a MIPS machine. The project have been tested both on a QEMU emulator and on a zSun device.


## Real Timer
A project to implement a low energy timer on an embeded zSun device. The project is getting the number of samples N and the period T of the timer. The system wakes up a process every T seconds and collects a timestamp. This loop is repeated for a total time of N times.

## Real Time WIFI footprint scanner
A project to implement a low energy WIFI scanner on an embeded zSun device. The project is getting the period T of the scanner. The system wakes up a process every T seconds and collects the available WIFI SSIDs related with a timestamp. This loop is repeated forever. The project uses PThreads to run parallel processes. You have to upload the PThreads library to the device, in orfer to run the project to the zSun.

The Documentation and the repo is under construction... Check out soon!
