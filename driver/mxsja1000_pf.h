/* Copyright (C) MOXA Inc. All rights reserved.

   This is free software distributed under the terms of the
   GNU Public License.  See the file COPYING-GPL for details.
*/

/*
    mxsja1000_pf.h (derived from the platform/sja1000.h)

    Definitions for the CAN register

    2012-05-29	Jason Chen
		new release
*/

#ifndef _MXCAN_PLATFORM_SJA1000_H_
#define _MXCAN_PLATFORM_SJA1000_H_

/* clock divider register */
#define CDR_CLKOUT_MASK 0x07
#define CDR_CLK_OFF	0x08 /* Clock off (CLKOUT pin) */
#define CDR_RXINPEN	0x20 /* TX1 output is RX irq output */
#define CDR_CBP		0x40 /* CAN input comparator bypass */
#define CDR_PELICAN	0x80 /* PeliCAN mode */

/* output control register */
#define OCR_MODE_BIPHASE  0x00
#define OCR_MODE_TEST     0x01
#define OCR_MODE_NORMAL   0x02
#define OCR_MODE_CLOCK    0x03
#define OCR_MODE_MASK     0x07
#define OCR_TX0_INVERT    0x04
#define OCR_TX0_PULLDOWN  0x08
#define OCR_TX0_PULLUP    0x10
#define OCR_TX0_PUSHPULL  0x18
#define OCR_TX1_INVERT    0x20
#define OCR_TX1_PULLDOWN  0x40
#define OCR_TX1_PULLUP    0x80
#define OCR_TX1_PUSHPULL  0xc0
#define OCR_TX_MASK       0xfc
#define OCR_TX_SHIFT      2

struct sja1000_platform_data {
	u32 osc_freq;	/* CAN bus oscillator frequency in Hz */

	u8 ocr;		/* output control register */
	u8 cdr;		/* clock divider register */
};

#endif	/* !_MXCAN_PLATFORM_SJA1000_H_ */
