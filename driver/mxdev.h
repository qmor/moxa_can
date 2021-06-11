/* Copyright (C) MOXA Inc. All rights reserved.

   This is free software distributed under the terms of the
   GNU Public License.  See the file COPYING-GPL for details.
*/

/*
    mxdev.h

    Definitions for the CAN network device driver interface

    2012-05-29	Jason Chen
		new release
*/

/*
 * Derived from the linux/can/dev.h
 *
 * Definitions for the CAN network device driver interface
 *
 * Copyright (C) 2006 Andrey Volkov <avolkov@varma-el.com>
 *               Varma Electronics Oy
 *
 * Copyright (C) 2008 Wolfgang Grandegger <wg@grandegger.com>
 *
 * Send feedback to <socketcan-users@lists.berlios.de>
 */

#ifndef MXCAN_DEV_H
#define MXCAN_DEV_H

#include "mxcan.h"
#include "mxnetlink.h"
#include "mxerror.h"

/*
 * CAN mode
 */
enum can_mode {
	CAN_MODE_STOP = 0,
	CAN_MODE_START,
	CAN_MODE_SLEEP
};

#define DEFAULT_BITRATE 250000

/*
 * CAN common private data
 */
struct can_priv {
	struct can_device_stats can_stats;

	struct can_bittiming bittiming;
	struct can_bittiming_const *bittiming_const;
	struct can_clock clock;

	enum can_state state;
	u32 ctrlmode;
	u32 ctrlmode_supported;

	int restart_ms;
	struct timer_list restart_timer;

	int (*do_set_bittiming)(struct net_device *dev);
	int (*do_set_mode)(struct net_device *dev, enum can_mode mode);
	int (*do_get_state)(const struct net_device *dev,
			    enum can_state *state);
	int (*do_get_berr_counter)(const struct net_device *dev,
				   struct can_berr_counter *bec);

	unsigned int echo_skb_max;
	struct sk_buff **echo_skb;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
struct net_device *dev;
#endif
};

/*
 * get_can_dlc(value) - helper macro to cast a given data length code (dlc)
 * to __u8 and ensure the dlc value to be max. 8 bytes.
 *
 * To be used in the CAN netdriver receive path to ensure conformance with
 * ISO 11898-1 Chapter 8.4.2.3 (DLC field)
 */
#define get_can_dlc(i)	(min_t(__u8, (i), 8))

/* Drop a given socketbuffer if it does not contain a valid CAN frame. */
static inline int can_dropped_invalid_skb(struct net_device *dev,
					  struct sk_buff *skb)
{
	const struct can_frame *cf = (struct can_frame *)skb->data;

	if (unlikely(skb->len != sizeof(*cf) || cf->can_dlc > 8)) {
		kfree_skb(skb);
		dev->stats.tx_dropped++;
		return 1;
	}

	return 0;
}

struct net_device *mx_alloc_candev(int sizeof_priv, unsigned int echo_skb_max);
void mx_free_candev(struct net_device *dev);

int mx_open_candev(struct net_device *dev);
void mx_close_candev(struct net_device *dev);

int mx_register_candev(struct net_device *dev);
void mx_unregister_candev(struct net_device *dev);

void mx_can_bus_off(struct net_device *dev);

void mx_can_put_echo_skb(struct sk_buff *skb, struct net_device *dev,
		      unsigned int idx);
void mx_can_get_echo_skb(struct net_device *dev, unsigned int idx);
void mx_can_free_echo_skb(struct net_device *dev, unsigned int idx);

struct sk_buff *mx_alloc_can_skb(struct net_device *dev, struct can_frame **cf);
struct sk_buff *mx_alloc_can_err_skb(struct net_device *dev,
				  struct can_frame **cf);

#endif /* MXCAN_DEV_H */
