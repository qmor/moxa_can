/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

/*
    rx.c

    SocketCAN example

    2012-07-31	Jason Chen
		new release
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>

#define MXCAN_NAME		"mxcan1"

int	main(int argc,char *argv[])
{
	int rx_s = 0;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame = {0};
	int ret;

	rx_s = socket( PF_CAN, SOCK_RAW, CAN_RAW );
	if( rx_s < 0 ){
		perror( "socket" );
		return -1;
	}

	/* Get the interface index from the interface name of CAN bus */
	addr.can_family = PF_CAN;
	strcpy( ifr.ifr_name, MXCAN_NAME );
	if( ioctl( rx_s, SIOCGIFINDEX, &ifr )){
		perror( "ioctl" );
		close( rx_s );
		return -1;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	if( bind( rx_s, (struct sockaddr *)&addr, sizeof(addr) ) < 0){
		perror( "bind" );
		close( rx_s );
		return -1;
	}

	ret = read( rx_s, &frame, sizeof(struct can_frame) );

	if( ret < 0 ){
		perror( "read" );
		close( rx_s );
		return -1;

	} else {

		char buf[80] = {0};
		int i;

		/* Print the received data */
		if( frame.can_id & CAN_EFF_FLAG )
			ret = sprintf( buf, "<0x%08x> ", frame.can_id & CAN_EFF_MASK );
		else
			ret = sprintf( buf, "<0x%03x> ", frame.can_id & CAN_SFF_MASK );

		ret += sprintf( buf + ret, "[%d] ", frame.can_dlc );

		for( i = 0; i < frame.can_dlc; i++ ){
			ret += sprintf( buf + ret, "%02x ", frame.data[i] );
		}
		if( frame.can_id & CAN_RTR_FLAG )
			ret += sprintf( buf + ret, "remote request" );

		printf( "RX: %s\n", buf );
	}

	close( rx_s );

	return( 0 );
}

