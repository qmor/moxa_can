=============================================================================
          MOXA CAN Family Device Driver Examples
          Copyright (C) 2012, Moxa Inc.
=============================================================================
Date: 07/31/2012

-----------------------------------------------------------------------------
   Introduction

   These examples demonstrate the basic SocketCAN programming to transmit and
   receive data from mxcan0 to mxcan1.


-----------------------------------------------------------------------------
   Build the examples
   
   # make
   
   or clean them
   
   # make clean 

   
-----------------------------------------------------------------------------
   Usage

   Use following command to check if the same message is printed.

   $ ./rx &
   $ ./tx
   TX: Data is sent.
   RX: <0x00000005> [8] 41 42 43 44 45 46 47 48 
   $_ 


   