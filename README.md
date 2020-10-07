# ZsunCrossCombile
A set of cross-compiled projects, designed to run on a MIPS machine. The projects have been tested both on a QEMU emulator and on a zSun device.


## Real Timer
A project to implement a low energy timer on an embeded zSun device. The project is getting the number of samples N and the period T of the timer. The system wakes up a process every T seconds and collects a timestamp. This loop is repeated for a total time of N times. 

Ιndicative results of the project could be found in the "results" folder. The results are separated based on the input arguments and contain runtime information both in a QEMU emulator and a real zSun device.

#### Run
To run...

#### Applications
This software is working in an embedded device either with a constant power suply or a battery, as its energy requirements are low. The software is able to begin a task periodically, relating the task with a timestamp. Adding functionality code to the process that collects the timestamps, the software could do any periodic action, keeping records as a history.

The software could give solutions like: 
- Periodically run an automation
- Periodically run a system check
- Periodically send notifications
- Periodically update a monitor

## Real Wifier
A project to implement a low energy WIFI scanner on an embeded zSun device. The project is getting the period T of the scanner. The system wakes up a process every T seconds and collects the available WIFI SSIDs related with a timestamp. This loop is repeated forever. 

The project uses PThreads to run parallel processes. You have to upload the PThreads library to the device, in order to run the project to the zSun.

Ιndicative results of the project could be found in the "results" folder. The results are separated based on the input arguments and contain runtime information both in a QEMU emulator and a real zSun device.

#### Run
To run...

#### Applications
This software is working in an embedded device either with a constant power suply or a battery, as its energy requirements are low. The software is able to track the WIFI footprints of an area. Applying WIFI transmitters to products, cars, wearables or anything else, the software could track the traffic of an area.

The software could give solutions like: 
- Product input / output
- Car traffic monitoring
- Human traffic monitoring
- Production line traffic monitoring

The Documentation and the repo is under construction... Check out soon!
