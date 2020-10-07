# EmbeddedCrossCompile
A set of cross-compiled projects, designed to run on a MIPS machine. The projects have been tested both on a QEMU emulator and on a zSun device.

In order to cross-compile the source code you will need a toolchain, to build the platform specific executables. In order to upload and run the executables to the embedded system you will need to install a basic middleware. [OpenWrt](https://github.com/Emeryth/openwrt-zsun) contain both. It gives a toolchain for platform specific builds (including MIPS) and could be installed in the embedded system (including zSun) to upload and run executables.

In order to test the executables to an emulator, you will need [QEMU](https://www.qemu.org/). This software is used to build an emulator that runs as a virtual machine.

## Real Timer
A project to implement a low energy timer on an embeded zSun device. The project is getting the number of samples N and the period T of the timer. The system wakes up a process every T seconds and collects a timestamp. This loop is repeated for a total time of N times. 

Ιndicative results of the project could be found in the "results" folder. The results are separated based on the input arguments and contain runtime information both in a QEMU emulator and a real zSun device.


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

#### Applications
This software is working in an embedded device either with a constant power suply or a battery, as its energy requirements are low. The software is able to track the WIFI footprints of an area. Applying WIFI transmitters to products, cars, wearables or anything else, the software could track the traffic of an area.

The software could give solutions like: 
- Product input / output
- Car traffic monitoring
- Human traffic monitoring
- Production line traffic monitoring

## Compile
After seting up the OpenWrt and with the Cross-Compile Toolchain, to compile run:

``` 
$ ./mips-openwrt-linux-gcc source.c -o name
```

where "source.c" is the source code file and "name" the name of the executable file.

## Run

#### zSun
After installing and setting up OpenWrt, with zSun connected, to send an executable run:

```
$ sudo scp name root@192.168.1.1:/root
```

where "name" is the name of the executable and "192.168.1.1" is the IP adress of the zSun.

#### QEMU
After installing and setting up QEMU, to start the virtual machine run:

```
$ qemu-system-mips -kernel ~/openwrt-zsun-zsun/bin/malta/openwrt-malta-be-vmlinux-initramfs.elf -redir tcp:2222::22 -nographic

```

Then, to send an executable run:

```
$ sudo scp -P 2222 name root@192.168.1.72:/root
```

where "name" is the name of the executable and "192.168.1.72" is the IP adress of the emulator.
