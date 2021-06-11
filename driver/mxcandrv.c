/* Copyright (C) MOXA Inc. All rights reserved.

   This is free software distributed under the terms of the
   GNU Public License.  See the file COPYING-GPL for details.
*/

/*
    mxcandrv.c

    Routines to support Moxa CAN card.

    2012-05-29	Jason Chen
		new release
*/

/*
 * Derived from the ems_pci.c driver:
 *	Copyright (C) 2007 Wolfgang Grandegger <wg@grandegger.com>
 *	Copyright (C) 2008 Markus Plessing <plessing@ems-wuensche.com>
 *	Copyright (C) 2008 Sebastian Haas <haas@ems-wuensche.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the version 2 of the GNU General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/io.h>

#include "mxdev.h"
#include "mxsja1000.h"

MODULE_DESCRIPTION("Socket-CAN driver for Moxa CP602 series CAN card");
MODULE_SUPPORTED_DEVICE("Moxa CP602 series CAN card");
MODULE_LICENSE("GPL v2");

#define CLR_BYTE				0x00
#define DONT_CARE				0xFF
#define DRV_NAME				"mxcandrv"

// CP602 uses 16MHz and Intel mode
#define MOXA_PCI_CAN_CLOCK		(16000000 / 2)

#define MOXA_PCI_OCR			(OCR_TX0_PUSHPULL | OCR_TX1_PUSHPULL | OCR_MODE_NORMAL )
#define MOXA_PCI_CDR			(CDR_PELICAN | CDR_CBP)

#define MOXA_PCI_MAX_CHAN		2
#define MOXA_PCI_BASE_BAR		2
#define MOXA_PCI_CONF_SIZE		512
#define MOXA_PCI_CAN_CTRL_SIZE	128 /* memory size for each controller */

#if 0
#define mxprint(...) printk("mx: "__VA_ARGS__)
#else
#define mxprint(...)
#endif

#include <linux/version.h>
#define	VERSION_CODE(ver,rel,seq)	((ver << 16) | (rel << 8) | seq)

struct moxa_pci_card {
	int version;
	int channels;

	struct pci_dev *pci_dev;
	struct net_device *net_dev[MOXA_PCI_MAX_CHAN];

	void __iomem *base_addr;
};

struct moxa_pci_can_info {
	const char * model_name;
	int channel_count;
};


#if (LINUX_VERSION_CODE < VERSION_CODE(3,8,0))
static struct moxa_pci_can_info moxa_can_cp602u __devinitdata = {
		"CP602U", 2
};

static struct moxa_pci_can_info moxa_can_cp602e __devinitdata = {
		"CP602E", 2
};

static struct moxa_pci_can_info moxa_can_cb602 __devinitdata = {
		"CB602", 2
};
#else
static struct moxa_pci_can_info moxa_can_cp602u = {
		"CP602U", 2
};

static struct moxa_pci_can_info moxa_can_cp602e = {
		"CP602E", 2
};

static struct moxa_pci_can_info moxa_can_cb602 = {
		"CB602", 2
};
#endif

//static DEFINE_PCI_DEVICE_TABLE(moxa_pci_tbl) = {
static const struct pci_device_id moxa_pci_tbl[] = {
	/* CP602U */
	{PCI_VENDOR_ID_MOXA, 0x6020, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (kernel_ulong_t)&moxa_can_cp602u},
	/* CP602E */
	{PCI_VENDOR_ID_MOXA, 0x6021, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (kernel_ulong_t)&moxa_can_cp602e},
	/* CB602 */
	{PCI_VENDOR_ID_MOXA, 0x6022, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (kernel_ulong_t)&moxa_can_cb602},
	{0,}
};

MODULE_DEVICE_TABLE(pci, moxa_pci_tbl);

/*
 * Helper to read internal registers from card logic (not CAN)
 */
static u8 moxa_pci_read_reg(const struct sja1000_priv *priv, int port)
{
	mxprint("%s\n", __FUNCTION__);
	return readb(priv->reg_base + port);
}

static void moxa_pci_write_reg(const struct sja1000_priv *priv,
				 int port, u8 val)
{
	mxprint("%s\n", __FUNCTION__);
	writeb(val, priv->reg_base + port);
}

static void moxa_pci_del_card(struct pci_dev *pdev)
{
	struct moxa_pci_card *card = pci_get_drvdata(pdev);
	struct net_device *dev;
	int i = 0;

	mxprint("%s IN\n", __FUNCTION__);

	for (i = 0; i < card->channels; i++) {
		dev = card->net_dev[i];

		if (!dev)
			continue;

		dev_info(&pdev->dev, "Removing %s.\n", dev->name);
		mx_unregister_sja1000dev(dev);
		mx_free_sja1000dev(dev);
	}

	if (card->base_addr != NULL)
		pci_iounmap(card->pci_dev, card->base_addr);

	kfree(card);

	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);

	mxprint("%s OUT\n", __FUNCTION__);
}

#if 0
static void moxa_pci_card_reset(struct moxa_pci_card *card)
{
	mxprint("%s IN\n", __FUNCTION__);

	/* Request board reset */
	writeb(0, card->base_addr);

	mxprint("%s OUT\n", __FUNCTION__);
}
#endif

/*
 * Probe PCI device for Moxa CAN signature and register each available
 * CAN channel to SJA1000 Socket-CAN subsystem.
 */

#if (LINUX_VERSION_CODE < VERSION_CODE(3,8,0))
static int __devinit moxa_pci_add_card(struct pci_dev *pdev,
					const struct pci_device_id *ent)
#else
static int moxa_pci_add_card(struct pci_dev *pdev,
					const struct pci_device_id *ent)
#endif
{
	struct sja1000_priv *priv;
	struct net_device *dev;
	struct moxa_pci_card *card;
	struct moxa_pci_can_info *mxinfo;
	int conf_size, base_bar;
	int err, i;

	mxprint("%s IN\n", __FUNCTION__);

	mxinfo = (struct moxa_pci_can_info *) ent->driver_data;

	if(mxinfo == NULL){
		dev_err(&pdev->dev, "Unable to extract the information of moxa can card.\n");
		mxprint("%s OUT\n", __FUNCTION__);
		return -EINVAL;
	}

	if(mxinfo->channel_count > MOXA_PCI_MAX_CHAN){
		dev_err(&pdev->dev, "MOXA_PCI_MAX_CHAN error\n");
		mxprint("%s OUT\n", __FUNCTION__);
		return -EINVAL;
	}

	/* Enabling PCI device */
	if (pci_enable_device(pdev) < 0) {
		dev_err(&pdev->dev, "Enabling PCI device failed\n");
		mxprint("%s OUT\n", __FUNCTION__);
		return -ENODEV;
	}

	/* Allocating card structures to hold addresses, ... */
	card = kzalloc(sizeof(struct moxa_pci_card), GFP_KERNEL);
	if (card == NULL) {
		dev_err(&pdev->dev, "Unable to allocate memory\n");
		pci_disable_device(pdev);
		mxprint("%s OUT\n", __FUNCTION__);
		return -ENOMEM;
	}

	pci_set_drvdata(pdev, card);

	card->pci_dev = pdev;

	card->channels = 0;

	base_bar = MOXA_PCI_BASE_BAR;
	conf_size = MOXA_PCI_CONF_SIZE;

	card->base_addr = pci_iomap(pdev, base_bar, conf_size);
	if (card->base_addr == NULL) {
		err = -ENOMEM;
		goto failure_cleanup;
	}

	/* Detect available channels */
	for (i = 0; i < mxinfo->channel_count; i++) {

		dev = mx_alloc_sja1000dev(0);
		if (dev == NULL) {
			err = -ENOMEM;
			goto failure_cleanup;
		}

		card->net_dev[i] = dev;
		priv = netdev_priv(dev);
		priv->priv = card;
		priv->irq_flags = IRQF_SHARED;

		dev->irq = pdev->irq;
		priv->reg_base = card->base_addr + (i * MOXA_PCI_CAN_CTRL_SIZE);

		priv->read_reg  = moxa_pci_read_reg;
		priv->write_reg = moxa_pci_write_reg;

		/* Check if channel is present */
		priv->can.clock.freq = MOXA_PCI_CAN_CLOCK;
		priv->ocr = MOXA_PCI_OCR;
		priv->cdr = MOXA_PCI_CDR;

		SET_NETDEV_DEV(dev, &pdev->dev);

		/* Register SJA1000 device */
		err = mx_register_sja1000dev(dev);
		if (err) {
			dev_err(&pdev->dev, "Registering device failed "
							"(err=%d)\n", err);
			mx_free_sja1000dev(dev);
			goto failure_cleanup;
		}

		card->channels++;

		dev_info(&pdev->dev, "Moxa CAN board %s - %s Channel #%d at 0x%p, irq %d\n",
				mxinfo->model_name, dev->name, i + 1, priv->reg_base, dev->irq);

	}

	return 0;

failure_cleanup:
	dev_err(&pdev->dev, "Error: %d. Cleaning Up.\n", err);

	moxa_pci_del_card(pdev);

	mxprint("%s OUT\n", __FUNCTION__);

	return err;
}

static struct pci_driver moxa_pci_driver = {
	.name = DRV_NAME,
	.id_table = moxa_pci_tbl,
	.probe = moxa_pci_add_card,
	.remove = moxa_pci_del_card,
};
static int __init moxa_pci_init(void)
{
	//moxa_pci_driver.id_table = moxa_pci_tbl;


	int res;
	mxprint("%s IN\n", __FUNCTION__);

	res = pci_register_driver(&moxa_pci_driver);

	mxprint("%s OUT\n", __FUNCTION__);
	return res;
}

static void __exit moxa_pci_exit(void)
{
	mxprint("%s IN\n", __FUNCTION__);

	pci_unregister_driver(&moxa_pci_driver);

	mxprint("%s OUT\n", __FUNCTION__);
}

module_init(moxa_pci_init);
module_exit(moxa_pci_exit);


