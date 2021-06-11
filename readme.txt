=============================================================================
          MOXA CAN Family Device Driver Installation Guide
          for Linux Kernel 2.6.31 or newer version
          Copyright (C) 2012, Moxa Inc.
=============================================================================
Date: 10/19/2012

Content

1. Introduction
2. System Requirements
3. Installation
   3.1 Hardware Installation
   3.2 Driver Files   
   3.3 Device Naming Convention
   3.4 Module Driver Configuration
4. CAN Interface Operation
   4.1 Displaying the CAN Interfaces
   4.2 Setting Bitrate
   4.3 Setting the CAN Interfaces Up or Down 
   4.4 Restarting from Bus-Off State
   4.5 The procfs Directory
5. Utilities and Test Procedure
   5.1 Utilities
   5.2 Test Procedure
   5.3 Examples
6. Troubleshooting
   6.1 Unable to restart the CAN interface
   6.2 Installing the Moxa CAN Driver in Kernel 2.6.31

-----------------------------------------------------------------------------
1. Introduction

   The CAN family Linux driver supports the following CAN boards:

   - 2-port CAN boards:
     CP-602E-I, CP-602U-I, CB-602I

   This driver and installation procedure have been developed upon Linux Kernel
   3.x. This driver supports Intel x86/x64 hardware platforms. In order 
   to maintain compatibility, this version has also been properly tested with 
   Ubuntu Linux. However, if compatibility problem occurs, please contact Moxa
   at support@moxa.com.

   All the drivers are published in the form of source code under the GNU General
   Public License in this version. Please refer to the GNU General Public License
   announcement in each source code file for more details.

   Visit www.moxa.com to download the latest driver. 

   This version of driver can be installed as loadable module (module driver).
   Before you install the driver, please refer to the hardware installation
   procedure in the User's Manual.

   The user should be familiar with the following documents:
   - Linux SocketCAN
   - Kernel-HOWTO

-----------------------------------------------------------------------------
2. System Requirements
   - Hardware platform: Intel x86/x64 machine
   - Kernel version: 2.6.31 or newer
   - GCC version 4.6.1 or later

-----------------------------------------------------------------------------
3. Installation

   3.1 Hardware Installation

       PCI IRQ Sharing
       -----------
       Each port within the same multiport board shares the same IRQ. Up to
       four Moxa CAN PCI family boards can be installed together on one system
       and share the same IRQ.


   3.2 Driver Files

       The driver file may be obtained from the CD-ROM or website. The first step
       is to copy the driver file "driv_linux_CAN_vx.x_build_yymmddhh.tgz"
       into the specified directory, e.g. /moxa. Follow the executed commands below.

       # cd /
       # mkdir moxa
       # cd /moxa
       # cp /mnt/cdrom/<driver directory>/driv_linux_CAN_vx.x_build_yymmddhh.tgz ./
       # tar -xvzf driv_linux_CAN_vx.x_build_yymmddhh.tgz

       Note: xx=version, yy=year, mm=month, dd=day, hh=hour 

   3.3 Device Naming Convention

       Device naming rule
       -----------------------------------------------
       The following is the naming convention for CAN boards as assigned by system:

       Board Num.    Interface Name 
       1st board     mxcan0, mxcan1
       2nd board     mxcan2, mxcan3
       3rd board     mxcan4, mxcan5
       4th board     mxcan6, mxcan7       

       You will find all the driver files in /moxa/mxcan. The following installation
       procedure depends on the model you'd like to run the driver.

       
   3.4 Module Driver Configuration

       ------------- Preparing to Use the MOXA Driver -------------------- 
 
       3.4.1 Building the MOXA Driver
          
             Before using the MOXA driver, you will need to compile all the source
             code. This step needs to be executed only once.
             However, if you modify the source code, you will need to recompile the
             source code, and perform this step again.
                    
             Find "Makefile" in /moxa/mxcan, then run

             # make;

       3.4.2 Installing the MOXA Driver
          
             To install the MOXA driver, you have to execute the command below
             in /moxa/mxcan directory. The driver will then be installed in the system
             directory.

             # make install

       ------------- Loading the MOXA Driver --------------------  

       3.4.3 To load the MOXA driver, execute the command below.  

             # modprobe mxcandrv

             This command will activate the module driver. You may run "lsmod" to
             check if "mxcandrv" is activated.

             To simplify the processes above, we provided a single step to build, 
             install, and load the MOXA driver. You may execute the ./mxinstall in
             /moxa/mxcan to start using the product.

             # ./mxinstall

       ------------- Loading the MOXA Driver on Boot --------------------  

       3.4.4 From the above description, you may manually execute
             "modprobe mxcandrv" to activate this driver and run "rmmod mxcandrv"
             to remove it. However, it is better to have a boot time configuration
             to eliminate manual operation. Boot time configuration may differ
             with each distribution. The following steps may apply to common
             distributions: 
          
             # vi /etc/modules.conf 
              or
             # vi /etc/modules

             Append the following line to the file.

             mxcandrv

             Reboot and check if mxcandrv has been activated by the "lsmod" command.

       ------------- Unloading the MOXA Driver Manually -------------------- 

       3.4.5 Execute the command below to unload the MOXA driver manually

             # modprobe -r mxcandrv

             This command will deactivate the module driver. You may run "lsmod" to 
             check if "mxcandrv" is deactivated or not. If the driver is marked as a
             permanent driver, it means the kernel prevented dynamic unload of modules.
             Please refer to kernel documents for detailed information.

       ------------- Cleaning the MOXA Driver -------------------- 

       3.4.6 Execute the command below to clean the files.

             # make clean

       ------------- Uninstalling the MOXA Driver and Utilities --------------------

       3.4.7 The MOXA driver and utilities will be removed from the system
             respectively after executing the command below in /moxa/mxcan.

             # make uninstall

             If the driver is marked as a permanent driver, reboot the system to
             remove the driver from memory.


-----------------------------------------------------------------------------
4. CAN Interface Operation

   4.1 Displaying the CAN Interfaces
  
       # ip link show

   4.2 Setting Bitrate

       Type the following to set the interface as bitrate 50000. When the driver
       is loaded for the first time, the bitrate is 0.
          
       # ip link set mxcanX type can bitrate 50000

   4.3 Setting the CAN Interfaces Up or Down

       Users may set the interface up or down. Set the interface up to receive and
       transmit data. Set the interface down to configure the settings. The bitrate
       should be set correctly before setting the interface up. 

       # ip link set mxcanX up
        or
       # ifconfig mxcanX up

       Type the following to set the interface down.

       # ip link set mxcanX down
         or
       # ifconfig mxcanX down

   4.4 Restarting from Bus-Off State

       If the CAN bus detects too many errors, it will enter the bus-off state.
       Set an auto recovery time from the bus-off state with the following command:
       
       # ip link set mxcanX type can restart-ms 100
      
       Or manually restart the interface.
       
       # ip link set mxcanX type can restart

       Note that a restart will also create a CAN error frame.

   4.5 The procfs Directory

       The processes and system information is provided by the CAN bus core in
       the following files. Users may need to load the CAN filter manually to access
       these files.
       
       # modprobe can
       
       Use similar usage to read the information. 

       # cat /proc/net/can/rcvlist_all

       rcvlist_all - list for unfiltered entries
       rcvlist_eff - list for single extended frame (EFF) entries
       rcvlist_err - list for error frames masks
       rcvlist_fil - list for mask/value filters
       rcvlist_inv - list for mask/value filters (inverse semantic)
       rcvlist_sff - list for single standard frame (SFF) entries

       Additional files found in /proc/net/can:

       stats       - SocketCAN core statistics
       reset_stats - manual statistic reset
       version     - prints the SocketCAN core version and the ABI version

-----------------------------------------------------------------------------
5. Utilities and Test Procedure

   5.1 Utilities

       The canutils is a set of popular SocketCAN tools. You may download 
       and test the CAN interface as well.

       http://www.pengutronix.de/software/socket-can/download/canutils/

       To install the canutils, you need libsocketcan library in your system.

       http://www.pengutronix.de/software/libsocketcan/download/

       The contents of canutils are listed below.

       canconfig - configure a CAN bus interface
       candump   - dump messages from the CAN bus interface to stdout
       canecho   - loop back received CAN messages to the interface
       cansend   - send data on a CAN interface

   5.2 Test Procedure

       When the CAN driver is installed, type the following to test a 2-port CAN card.

       # modprobe mxcandrv
       # ip link set mxcan0 type can bitrate 50000
       # ip link set mxcan0 up
       # ip link set mxcan1 type can bitrate 50000
       # ip link set mxcan1 up
       # candump mxcan0

       Loop back mxcan0 and mxcan1, open another terminal, and type

       # cansend mxcan1 1 2 3 -i 50

       The caddump will display the following messages:

       # <0x032> [3] 01 02 03
 
   5.3 Examples

       User may handle the CAN bus raw data by program. There are some examples
       in /moxa/mxcan/utility/examples. However the Moxa CAN bus driver is based on
       SocketCAN technology. User may refer to the Linux source document for
       detailed information. (linux/Documentation/networking/can.txt)      

-----------------------------------------------------------------------------
6. Troubleshooting

   6.1 Unable to restart the CAN interface

       If the CAN bus status is not in bus-off, the driver will not restart the
       interface. Check the bus state with the following command.

       # ip -detail link show mxcan0

       3: mxcan0: <NOARP,UP,LOWER_UP,ECHO> mtu 16 qdisc pfifo_fast state UNKNOWN
          qlen 10 link/can
          can state BUS-OFF (berr-counter tx 0 rx 0) restart-ms 0
          ^^^^^^^^^^^^^^^^^

   6.2 Installing the Moxa CAN Driver in Kernel 2.6.31

       Linux Kernel has supported the SocketCAN device driver in the mainline
       since version 2.6.31. However, a decent version of iproute2 library
       should be installed for proper operation. Otherwise, you will
       encounter an error when you set bitrate to CAN interface. 
       
       Some distributions don't install the suitable iproute2 as default. You can
       type the following command to check the version of iproute2.
       
       # ip -V
       ip utility, iproute2-ss091226
      
       For earlier versions, such as ip utility, iproute2-ss090324,
       you will need to upgrade to iproute2-2.6.31 or the latest version. Download
       iproute2-2.6.31.tar.bz2 from the Linux Kernel repository, then compile and
       install it into your distribution. Please refer to kernel documents for
       detailed information.
