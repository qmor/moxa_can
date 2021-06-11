/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

/*
    tx.c

    SocketCAN example

    2012-07-31	Jason Chen
		new release
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

#define MXCAN_NAME		"mxcan0"
#define MXCAN_ID		5
#define MXCAN_DATA_LEN	8
#define MXCAN_DATA		"ABCDEFGH"
#define MXCAN_IS_EFR	1
#define MXCAN_IS_REMOTE	0

int	main(int argc,char *argv[])
{
	int tx_s = 0;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame = {0};
	int ret;

	tx_s = socket( PF_CAN, SOCK_RAW, CAN_RAW );
	if( tx_s < 0 ){
		perror( "socket" );
		return -1;
	}

	/* Get the interface index from the interface name of CAN bus */
	addr.can_family = PF_CAN;
	strcpy( ifr.ifr_name, MXCAN_NAME );
	if( ioctl( tx_s, SIOCGIFINDEX, &ifr )){
		perror( "ioctl" );
		close( tx_s );
		return -1;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	if( bind( tx_s, (struct sockaddr *)&addr, sizeof(addr) ) < 0){
		perror( "bind" );
		close( tx_s );
		return -1;
	}

	frame.can_id = MXCAN_ID;

	if( MXCAN_IS_EFR ){
		frame.can_id &= CAN_EFF_MASK;
		frame.can_id |= CAN_EFF_FLAG;
	} else {
		frame.can_id &= CAN_SFF_MASK;
	}

	if( MXCAN_IS_REMOTE  )
		frame.can_id |= CAN_RTR_FLAG;

	if( MXCAN_DATA_LEN <= 8 ){
		frame.can_dlc = MXCAN_DATA_LEN;
		memcpy( frame.data, MXCAN_DATA, MXCAN_DATA_LEN );
	} else {
		frame.can_dlc = 0;
	}

	ret = write( tx_s, &frame, sizeof(frame) );

	if( ret == -1 ){
		perror( "write" );
		close( tx_s );
		return -1;
	}

	printf( "TX: Data is sent.\n" );

	close( tx_s );

	return( 0 );
}

