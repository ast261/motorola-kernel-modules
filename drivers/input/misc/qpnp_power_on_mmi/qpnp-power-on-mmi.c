/* Copyright (c) 2012-2018, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/regmap.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/spmi.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/input.h>
#include <linux/log2.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/input/qpnp-power-on.h>
#include <linux/power_supply.h>
#include <linux/reboot.h>

#define RESET_EXTRA_RESET_KUNPOW_REASON        BIT(9)

struct qpnp_pon_mmi {
	struct platform_device	*pdev;
	struct delayed_work	kpd_bark_work;
	u32 			kpd_bark_irq;
};

static bool mmi_kungpow_check(void)
{
	struct device_node *soc = of_find_node_by_path("/soc");
	struct device_node *kungpow = NULL;
	bool has_kungpow = false;
	if (soc)
		kungpow = of_find_node_by_name(soc, "kungpow_default");

	has_kungpow = (kungpow != NULL);

	of_node_put(kungpow);
	of_node_put(soc);

	return has_kungpow;
}

static void kpd_bark_work_func(struct work_struct *work)
{
	struct qpnp_pon_mmi *pon =
		container_of(work, struct qpnp_pon_mmi, kpd_bark_work.work);

	dev_err(&pon->pdev->dev, "HW User Reset! 2 sec to Reset!\n");
	qpnp_pon_store_extra_reset_info(RESET_EXTRA_RESET_KUNPOW_REASON,
		mmi_kungpow_check() ? 0 : RESET_EXTRA_RESET_KUNPOW_REASON);
	qpnp_pon_system_pwr_off(PON_POWER_OFF_HARD_RESET);
	kernel_halt();
}

static irqreturn_t qpnp_kpdpwr_bark_irq(int irq, void *_pon)
{
	struct qpnp_pon_mmi *pon = _pon;

	schedule_delayed_work(&pon->kpd_bark_work, msecs_to_jiffies(0));

	return IRQ_HANDLED;
}

static int qpnp_pon_mmi_probe(struct platform_device *pdev)
{
	struct qpnp_pon_mmi *pon;
	int rc;

	pon = devm_kzalloc(&pdev->dev, sizeof(struct qpnp_pon_mmi),
			   GFP_KERNEL);
	if (!pon)
		return -ENOMEM;

	pon->pdev = pdev;

	dev_set_drvdata(&pdev->dev, pon);

	INIT_DELAYED_WORK(&pon->kpd_bark_work, kpd_bark_work_func);
	pon->kpd_bark_irq = platform_get_irq_byname(pon->pdev, "kpdpwr-bark");
	if (pon->kpd_bark_irq < 0) {
		dev_err(&pon->pdev->dev,
			"Unable to get kpdpwr-bark irq\n");
		return -ENODEV;
	}

	dev_info(&pon->pdev->dev, "Request %d kpdpwr-bark  IRQ\n",
		pon->kpd_bark_irq);
	rc = devm_request_irq(&pon->pdev->dev, pon->kpd_bark_irq,
			      qpnp_kpdpwr_bark_irq,
			      IRQF_TRIGGER_RISING,
			      "qpnp_kpdpwr_bark", pon);
	if (rc < 0) {
		dev_err(&pon->pdev->dev,
			"Can't request %d IRQ\n",
			pon->kpd_bark_irq);
		return rc;
	}

	return 0;
}

static int qpnp_pon_mmi_remove(struct platform_device *pdev)
{
	struct qpnp_pon_mmi *pon = dev_get_drvdata(&pdev->dev);

	cancel_delayed_work_sync(&pon->kpd_bark_work);

	return 0;
}

static const struct of_device_id spmi_match_table[] = {
	{ .compatible = "qcom,qpnp-power-on-mmi", },
	{}
};

static struct platform_driver qpnp_pon_mmi_driver = {
	.driver		= {
		.name		= "qcom,qpnp-power-on-mmi",
		.of_match_table	= spmi_match_table,
	},
	.probe		= qpnp_pon_mmi_probe,
	.remove		= qpnp_pon_mmi_remove,
};

static int __init qpnp_pon_mmi_init(void)
{
	return platform_driver_register(&qpnp_pon_mmi_driver);
}
module_init(qpnp_pon_mmi_init);

static void __exit qpnp_pon_mmi_exit(void)
{
	return platform_driver_unregister(&qpnp_pon_mmi_driver);
}
module_exit(qpnp_pon_mmi_exit);

MODULE_DESCRIPTION("QPNP MMI PMIC POWER-ON driver");
MODULE_LICENSE("GPL v2");
