/*
 * BQ2589x battery charging driver
 *
 * Copyright (C) 2013 Texas Instruments
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>
#include <linux/mmi_discrete_charger_class.h>
#include <linux/seq_file.h>

#include "bq2589x_reg.h"

#define BQ2589x_MANUFACTURER		"Texas Instruments"
#define BQ25890_IRQ_PIN				"bq2589x_irq"
#define BQ2589X_STATUS_PLUGIN			0x0001
#define BQ2589X_STATUS_PG				0x0002
#define BQ2589X_STATUS_CHARGE_ENABLE	0x0004
#define BQ2589X_STATUS_FAULT			0x0008
#define BQ2589X_STATUS_EXIST			0x0100

static struct bq2589x *g_bq;
static DEFINE_MUTEX(bq2589x_i2c_lock);

enum bq2589x_status {
	STATUS_NOT_CHARGING,
	STATUS_PRE_CHARGING,
	STATUS_FAST_CHARGING,
	STATUS_TERMINATION_DONE,
};


static enum power_supply_usb_type bq2589x_usb_type[] = {
	POWER_SUPPLY_USB_TYPE_UNKNOWN,
	POWER_SUPPLY_USB_TYPE_SDP,
	POWER_SUPPLY_USB_TYPE_DCP,
	POWER_SUPPLY_USB_TYPE_CDP,
};

enum bq2589x_vbus_type {
	BQ2589X_VBUS_NONE,
	BQ2589X_VBUS_USB_SDP,
	BQ2589X_VBUS_USB_CDP, /*CDP for bq25890, Adapter for bq25892*/
	BQ2589X_VBUS_USB_DCP,
	BQ2589X_VBUS_MAXC,
	BQ2589X_VBUS_UNKNOWN,
	BQ2589X_VBUS_NONSTAND,
	BQ2589X_VBUS_OTG,
	BQ2589X_VBUS_TYPE_NUM,
};

enum bq2589x_part_no {
	BQ25890 = 0x03,
	BQ25892 = 0x00,
	BQ25895 = 0x07,
};

struct bq2589x_config {
	bool	enable_auto_dpdm;
	//bool    enable_12v;
	bool	enable_term;
	bool	enable_ico;
	bool	use_absolute_vindpm;

	int	charge_voltage;
	int	charge_current;
	int	term_current;
};

struct bq2589x_state {
	bool	vsys_stat;
	bool	therm_stat;
	bool	online;
	bool	chrg_en;
	bool	hiz_en;
	bool	term_en;
	bool	vbus_gd;

	u8		chrg_stat;
	u8		vbus_status;
	u8		chrg_type;
	u8		health;
	u8		chrg_fault;
	u8		ntc_fault;
};

struct bq2589x {
	enum	bq2589x_part_no part_no;

	unsigned int	status;

	bool	dpdm_enabled;
	bool	typec_apsd_rerun_done;
	bool	enabled;

	int		real_charger_type;
	int		revision;
	int		vbus_type;
	int		vbus_volt;
	int		vbat_volt;
	int		rsoc;
	int		chg_en_gpio;

	const char *chg_dev_name;

	struct charger_device *chg_dev;
	struct	device *dev;
	struct	i2c_client *client;
	struct	regulator *dpdm_reg;
	struct	mutex dpdm_lock;
	struct	bq2589x_state state;
	struct	mutex lock;

	struct	bq2589x_config  cfg;
	struct	work_struct irq_work;
	struct	delayed_work monitor_work;
	struct	delayed_work ico_work;
	struct	delayed_work pe_volt_tune_work;
	struct	delayed_work check_pe_tuneup_work;

	struct	power_supply_desc usb;
	struct	power_supply_desc wall;
	struct	power_supply *batt_psy;
	/*struct	power_supply *usb_psy;*/
	struct	power_supply *wall_psy;
	struct	power_supply_config usb_cfg;
	struct	power_supply_config wall_cfg;
};

struct pe_ctrl {
	bool enable;
	bool tune_up_volt;
	bool tune_down_volt;
	bool tune_done;
	bool tune_fail;
	int tune_count;
	int target_volt;
	int high_volt_level;/* vbus volt > this threshold means tune up successfully */
	int low_volt_level; /* vbus volt < this threshold means tune down successfully */
	int vbat_min_volt;  /* to tune up voltage only when vbat > this threshold */
};
static struct pe_ctrl pe;

static int bq2589x_read_byte(struct bq2589x *bq, u8 *data, u8 reg)
{
	int ret;

	mutex_lock(&bq2589x_i2c_lock);
	ret = i2c_smbus_read_byte_data(bq->client, reg);
	if (ret < 0) {
		dev_err(bq->dev, "failed to read 0x%.2x\n", reg);
		mutex_unlock(&bq2589x_i2c_lock);
		return ret;
	}

	*data = (u8)ret;
	mutex_unlock(&bq2589x_i2c_lock);

	return 0;
}

static int bq2589x_write_byte(struct bq2589x *bq, u8 reg, u8 data)
{
	int ret;
	mutex_lock(&bq2589x_i2c_lock);
	ret = i2c_smbus_write_byte_data(bq->client, reg, data);
	mutex_unlock(&bq2589x_i2c_lock);
	return ret;
}

static int bq2589x_update_bits(struct bq2589x *bq, u8 reg, u8 mask, u8 data)
{
	int ret;
	u8 tmp;

	ret = bq2589x_read_byte(bq, &tmp, reg);

	if (ret)
		return ret;

	tmp &= ~mask;
	tmp |= data & mask;

	return bq2589x_write_byte(bq, reg, tmp);
}

/*static enum bq2589x_vbus_type bq2589x_get_vbus_type(struct bq2589x *bq)
{
	u8 val = 0;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0)
		return 0;
	val &= BQ2589X_VBUS_STAT_MASK;
	val >>= BQ2589X_VBUS_STAT_SHIFT;

	return val;
}*/

static int bq2589x_enable_otg(struct bq2589x *bq)
{
	u8 val = BQ2589X_OTG_ENABLE << BQ2589X_OTG_CONFIG_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_OTG_CONFIG_MASK, val);

}

static int bq2589x_disable_otg(struct bq2589x *bq)
{
	u8 val = BQ2589X_OTG_DISABLE << BQ2589X_OTG_CONFIG_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_OTG_CONFIG_MASK, val);

}

/*static int bq2589x_set_otg_volt(struct bq2589x *bq, int volt)
{
	u8 val = 0;

	if (volt < BQ2589X_BOOSTV_BASE)
		volt = BQ2589X_BOOSTV_BASE;
	if (volt > BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB)
		volt = BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB;

	val = ((volt - BQ2589X_BOOSTV_BASE) / BQ2589X_BOOSTV_LSB) << BQ2589X_BOOSTV_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_0A, BQ2589X_BOOSTV_MASK, val);

}*/

static int bq2589x_set_otg_current(struct bq2589x *bq, int curr)
{
	u8 temp;

	if (curr == 500)
		temp = BQ2589X_BOOST_LIM_500MA;
	else if (curr == 700)
		temp = BQ2589X_BOOST_LIM_700MA;
	else if (curr == 1100)
		temp = BQ2589X_BOOST_LIM_1100MA;
	else if (curr == 1600)
		temp = BQ2589X_BOOST_LIM_1600MA;
	else if (curr == 1800)
		temp = BQ2589X_BOOST_LIM_1800MA;
	else if (curr == 2100)
		temp = BQ2589X_BOOST_LIM_2100MA;
	else if (curr == 2400)
		temp = BQ2589X_BOOST_LIM_2400MA;
	else
		temp = BQ2589X_BOOST_LIM_1300MA;

	return bq2589x_update_bits(bq, BQ2589X_REG_0A, BQ2589X_BOOST_LIM_MASK, temp << BQ2589X_BOOST_LIM_SHIFT);
}

static int bq2589x_enable_charger(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		bq->status |= BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}

static int bq2589x_disable_charger(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		bq->status &= ~BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}

/* interfaces that can be called by other module */
int bq2589x_adc_start(struct bq2589x *bq, bool oneshot)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_02);
	if (ret < 0) {
		dev_err(bq->dev, "%s failed to read register 0x02:%d\n", __func__, ret);
		return ret;
	}

	if (((val & BQ2589X_CONV_RATE_MASK) >> BQ2589X_CONV_RATE_SHIFT) == BQ2589X_ADC_CONTINUE_ENABLE)
		return 0; /*is doing continuous scan*/
	if (oneshot)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_START_MASK, BQ2589X_CONV_START << BQ2589X_CONV_START_SHIFT);
	else
		ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK,  BQ2589X_ADC_CONTINUE_ENABLE << BQ2589X_CONV_RATE_SHIFT);
	return ret;
}

int bq2589x_adc_stop(struct bq2589x *bq)
{
	return bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_DISABLE << BQ2589X_CONV_RATE_SHIFT);
}


int bq2589x_adc_read_battery_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0E);
	if (ret < 0) {
		dev_err(bq->dev, "read battery voltage failed :%d\n", ret);
		return ret;
	} else{
		volt = BQ2589X_BATV_BASE + ((val & BQ2589X_BATV_MASK) >> BQ2589X_BATV_SHIFT) * BQ2589X_BATV_LSB ;
		return volt;
	}
}


int bq2589x_adc_read_sys_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0F);
	if (ret < 0) {
		dev_err(bq->dev, "read system voltage failed :%d\n", ret);
		return ret;
	} else{
		volt = BQ2589X_SYSV_BASE + ((val & BQ2589X_SYSV_MASK) >> BQ2589X_SYSV_SHIFT) * BQ2589X_SYSV_LSB ;
		return volt;
	}
}

int bq2589x_adc_read_vbus_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_11);
	if (ret < 0) {
		dev_err(bq->dev, "read vbus voltage failed :%d\n", ret);
		return ret;
	} else{
		volt = BQ2589X_VBUSV_BASE + ((val & BQ2589X_VBUSV_MASK) >> BQ2589X_VBUSV_SHIFT) * BQ2589X_VBUSV_LSB ;
		return volt;
	}
}

int bq2589x_adc_read_temperature(struct bq2589x *bq)
{
	uint8_t val;
	int temp;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_10);
	if (ret < 0) {
		dev_err(bq->dev, "read temperature failed :%d\n", ret);
		return ret;
	} else{
		temp = BQ2589X_TSPCT_BASE + ((val & BQ2589X_TSPCT_MASK) >> BQ2589X_TSPCT_SHIFT) * BQ2589X_TSPCT_LSB ;
		return temp;
	}
}

int bq2589x_adc_read_charge_current(struct bq2589x *bq)
{
	uint8_t val;
	int curr;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_12);
	if (ret < 0) {
		dev_err(bq->dev, "read charge current failed :%d\n", ret);
		return ret;
	} else{
		curr = (int)(BQ2589X_ICHGR_BASE + ((val & BQ2589X_ICHGR_MASK) >> BQ2589X_ICHGR_SHIFT) * BQ2589X_ICHGR_LSB) ;
		return curr;
	}
}

int bq2589x_set_chargecurrent(struct bq2589x *bq, int curr)
{
	u8 ichg;

	ichg = (curr - BQ2589X_ICHG_BASE)/BQ2589X_ICHG_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_04, BQ2589X_ICHG_MASK, ichg << BQ2589X_ICHG_SHIFT);

}

int bq2589x_set_term_current(struct bq2589x *bq, int curr)
{
	u8 iterm;

	iterm = (curr - BQ2589X_ITERM_BASE) / BQ2589X_ITERM_LSB;

	return bq2589x_update_bits(bq, BQ2589X_REG_05, BQ2589X_ITERM_MASK, iterm << BQ2589X_ITERM_SHIFT);
}


int bq2589x_set_prechg_current(struct bq2589x *bq, int curr)
{
	u8 iprechg;

	iprechg = (curr - BQ2589X_IPRECHG_BASE) / BQ2589X_IPRECHG_LSB;

	return bq2589x_update_bits(bq, BQ2589X_REG_05, BQ2589X_IPRECHG_MASK, iprechg << BQ2589X_IPRECHG_SHIFT);
}

int bq2589x_set_chargevoltage(struct bq2589x *bq, int volt)
{
	u8 val;

	val = (volt - BQ2589X_VREG_BASE)/BQ2589X_VREG_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_06, BQ2589X_VREG_MASK, val << BQ2589X_VREG_SHIFT);
}


int bq2589x_set_input_volt_limit(struct bq2589x *bq, int volt)
{
	u8 val;
	val = (volt - BQ2589X_VINDPM_BASE) / BQ2589X_VINDPM_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_0D, BQ2589X_VINDPM_MASK, val << BQ2589X_VINDPM_SHIFT);
}

int bq2589x_set_input_current_limit(struct bq2589x *bq, int curr)
{
	u8 val;

	if (curr < BQ2589X_IINLIM_BASE)
		val = 0;
	else if (curr > BQ2589X_IINLIM_MAX)
		val = 0x3F;
	else
		val = (curr - BQ2589X_IINLIM_BASE) / BQ2589X_IINLIM_LSB;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_IINLIM_MASK, val << BQ2589X_IINLIM_SHIFT);
}


int bq2589x_set_vindpm_offset(struct bq2589x *bq, int offset)
{
	u8 val;

	val = (offset - BQ2589X_VINDPMOS_BASE)/BQ2589X_VINDPMOS_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_01, BQ2589X_VINDPMOS_MASK, val << BQ2589X_VINDPMOS_SHIFT);
}

int bq2589x_get_charging_status(struct bq2589x *bq)
{
	u8 val = 0;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0) {
		dev_err(bq->dev, "%s Failed to read register 0x0b:%d\n", __func__, ret);
		return ret;
	}
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;
	return val;
}

void bq2589x_set_otg(struct bq2589x *bq, int enable)
{
	int ret;

	if (enable) {
		ret = bq2589x_enable_otg(bq);
		if (ret < 0) {
			dev_err(bq->dev, "%s:Failed to enable otg-%d\n", __func__, ret);
			return;
		}
	} else{
		ret = bq2589x_disable_otg(bq);
		if (ret < 0)
			dev_err(bq->dev, "%s:Failed to disable otg-%d\n", __func__, ret);
	}
}

int bq2589x_set_watchdog_timer(struct bq2589x *bq, u8 timeout)
{
	return bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_WDT_MASK, (u8)((timeout - BQ2589X_WDT_BASE) / BQ2589X_WDT_LSB) << BQ2589X_WDT_SHIFT);
}

int bq2589x_disable_watchdog_timer(struct bq2589x *bq)
{
	u8 val = BQ2589X_WDT_DISABLE << BQ2589X_WDT_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_WDT_MASK, val);
}

int bq2589x_reset_watchdog_timer(struct bq2589x *bq)
{
	u8 val = BQ2589X_WDT_RESET << BQ2589X_WDT_RESET_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_WDT_RESET_MASK, val);
}

int bq2589x_force_dpdm(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_FORCE_DPDM << BQ2589X_FORCE_DPDM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_FORCE_DPDM_MASK, val);
	if (ret)
		return ret;

	return 0;
}

int bq2589x_reset_chip(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_RESET << BQ2589X_RESET_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_14, BQ2589X_RESET_MASK, val);
	return ret;
}

int bq2589x_enter_ship_mode(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_BATFET_OFF << BQ2589X_BATFET_DIS_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
	return ret;

}

int bq2589x_enter_hiz_mode(struct bq2589x *bq)
{
	u8 val = BQ2589X_HIZ_ENABLE << BQ2589X_ENHIZ_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);

}

int bq2589x_exit_hiz_mode(struct bq2589x *bq)
{

	u8 val = BQ2589X_HIZ_DISABLE << BQ2589X_ENHIZ_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);

}

int bq2589x_get_hiz_mode(struct bq2589x *bq, u8 *state)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_00);
	if (ret)
		return ret;
	*state = (val & BQ2589X_ENHIZ_MASK) >> BQ2589X_ENHIZ_SHIFT;

	return 0;
}

int bq2589x_pumpx_enable(struct bq2589x *bq, int enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_PUMPX_ENABLE << BQ2589X_EN_PUMPX_SHIFT;
	else
		val = BQ2589X_PUMPX_DISABLE << BQ2589X_EN_PUMPX_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_04, BQ2589X_EN_PUMPX_MASK, val);

	return ret;
}

int bq2589x_pumpx_increase_volt(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_PUMPX_UP << BQ2589X_PUMPX_UP_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_PUMPX_UP_MASK, val);

	return ret;

}

int bq2589x_pumpx_increase_volt_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_09);
	if (ret)
		return ret;

	if (val & BQ2589X_PUMPX_UP_MASK)
		return 1;   /* not finished*/
	else
		return 0;   /* pumpx up finished*/

}

int bq2589x_pumpx_decrease_volt(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_PUMPX_DOWN << BQ2589X_PUMPX_DOWN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_PUMPX_DOWN_MASK, val);

	return ret;

}

int bq2589x_pumpx_decrease_volt_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_09);
	if (ret)
		return ret;

	if (val & BQ2589X_PUMPX_DOWN_MASK)
		return 1;   /* not finished*/
	else
		return 0;   /* pumpx down finished*/

}

static int bq2589x_force_ico(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_FORCE_ICO << BQ2589X_FORCE_ICO_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_FORCE_ICO_MASK, val);

	return ret;
}

static int bq2589x_check_force_ico_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_14);
	if (ret)
		return ret;

	if (val & BQ2589X_ICO_OPTIMIZED_MASK)
		return 1;  /*finished*/
	else
		return 0;   /* in progress*/
}

static int bq2589x_enable_term(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_TERM_ENABLE << BQ2589X_EN_TERM_SHIFT;
	else
		val = BQ2589X_TERM_DISABLE << BQ2589X_EN_TERM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_EN_TERM_MASK, val);

	return ret;
}

static int bq2589x_enable_auto_dpdm(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_AUTO_DPDM_ENABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;
	else
		val = BQ2589X_AUTO_DPDM_DISABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_AUTO_DPDM_EN_MASK, val);

	return ret;

}

static int bq2589x_use_absolute_vindpm(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_FORCE_VINDPM_ENABLE << BQ2589X_FORCE_VINDPM_SHIFT;
	else
		val = BQ2589X_FORCE_VINDPM_DISABLE << BQ2589X_FORCE_VINDPM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_0D, BQ2589X_FORCE_VINDPM_MASK, val);

	return ret;

}

static int bq2589x_enable_ico(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_ICO_ENABLE << BQ2589X_ICOEN_SHIFT;
	else
		val = BQ2589X_ICO_DISABLE << BQ2589X_ICOEN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_ICOEN_MASK, val);

	return ret;

}


static int bq2589x_read_idpm_limit(struct bq2589x *bq)
{
	uint8_t val;
	int curr;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_13);
	if (ret < 0) {
		dev_err(bq->dev, "read vbus voltage failed :%d\n", ret);
		return ret;
	} else{
		curr = BQ2589X_IDPM_LIM_BASE + ((val & BQ2589X_IDPM_LIM_MASK) >> BQ2589X_IDPM_LIM_SHIFT) * BQ2589X_IDPM_LIM_LSB ;
		return curr;
	}
}

/*static bool bq2589x_is_charge_done(struct bq2589x *bq)
{
	int ret;
	u8 val;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0) {
		dev_err(bq->dev, "%s:read REG0B failed :%d\n", __func__, ret);
		return false;
	}
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;

	return (val == BQ2589X_CHRG_STAT_CHGDONE);
}*/

int bq2589x_set_maxcharge_en(struct bq2589x *bq, bool enable)
{
	u8 val;

	if (enable)
		val = BQ2589X_MAXC_ENABLE << BQ2589X_MAXCEN_SHIFT;
	else
		val = BQ2589X_MAXC_DISABLE << BQ2589X_MAXCEN_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_MAXCEN_MASK, val);
}

static int bq2589x_get_vindpm_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0D);
	if (ret < 0) {
		dev_err(bq->dev, "read vindpm failed :%d\n", ret);
		return ret;
	} else{
		volt = BQ2589X_VINDPM_BASE + ((val & BQ2589X_VINDPM_MASK) >> BQ2589X_VINDPM_SHIFT) * BQ2589X_VINDPM_LSB ;
		return volt;
	}
}

static int bq2589x_sync_state(struct bq2589x *bq, struct bq2589x_state *state)
{
	u8 chrg_stat, volt_stat, fault;
	u8 chrg_param_0,chrg_param_1,chrg_param_2;
	int ret;

	ret = bq2589x_read_byte(bq, &chrg_stat, BQ2589X_REG_0B);
	if (ret < 0){
		ret = bq2589x_read_byte(bq, &chrg_stat, BQ2589X_REG_0B);
		if (ret < 0){
			dev_err(bq->dev, "read bq2589x REG 0B failed :%d\n", ret);
			return ret;
		}
	}

	ret = bq2589x_read_byte(bq, &volt_stat, BQ2589X_REG_0E);
	if (ret < 0){
		dev_err(bq->dev, "read bq2589x REG 0E failed :%d\n", ret);
		return ret;
	}
	state->chrg_type = (chrg_stat & BQ2589X_VBUS_STAT_MASK) >> BQ2589X_VBUS_STAT_SHIFT;
	state->chrg_stat = (chrg_stat & BQ2589X_CHRG_STAT_MASK) >> BQ2589X_CHRG_STAT_SHIFT;
	state->online = (chrg_stat & BQ2589X_PG_STAT_MASK) >> BQ2589X_PG_STAT_SHIFT;
	state->vsys_stat = (chrg_stat & BQ2589X_VSYS_STAT_MASK) >> BQ2589X_VSYS_STAT_SHIFT;
	state->therm_stat =  (volt_stat & BQ2589X_THERM_STAT_MASK) >> BQ2589X_THERM_STAT_SHIFT;

	dev_info(bq->dev, "%s chrg_stat =%d,chrg_type =%d online = %d\n",__func__,state->chrg_stat,state->chrg_type,state->online);

	ret = bq2589x_read_byte(bq, &fault, BQ2589X_REG_0C);
	if (ret < 0){
		dev_err(bq->dev, "read bq2589x REG 0C failed :%d\n", ret);
		return ret;
	}
	state->chrg_fault = fault;
	state->ntc_fault = (fault & BQ2589X_FAULT_NTC_MASK) >> BQ2589X_FAULT_NTC_SHIFT;
	state->health = state->ntc_fault;

	ret = bq2589x_read_byte(bq, &chrg_param_0, BQ2589X_REG_00);
	if (ret < 0){
		dev_err(bq->dev, "read bq2589x REG 00 failed :%d\n", ret);
		return ret;
	}
	state->hiz_en = (chrg_param_0 & BQ2589X_ENHIZ_MASK) >> BQ2589X_ENHIZ_SHIFT;

	ret = bq2589x_read_byte(bq, &chrg_param_1, BQ2589X_REG_07);
	if (ret < 0){
		dev_err(bq->dev, "read bq2589x REG 07 failed :%d\n", ret);
		return ret;
	}
	state->term_en = (chrg_param_1 & BQ2589X_EN_TERM_MASK) >> BQ2589X_EN_TERM_SHIFT;

	ret = bq2589x_read_byte(bq, &chrg_param_2, BQ2589X_REG_11);
	if (ret < 0){
		dev_err(bq->dev, "read bq2589x REG 11 failed :%d\n", ret);
		return ret;
	}
	state->vbus_gd = (chrg_param_2 & BQ2589X_VBUS_GD_MASK) >> BQ2589X_VBUS_GD_SHIFT;

	return 0;
}

static int bq2589x_init_device(struct bq2589x *bq)
{
	int ret;

	/*common initialization*/
	bq2589x_disable_watchdog_timer(bq);

	/*disable maxcharge en to allow qc2.0 detection*/
	bq2589x_set_maxcharge_en(bq, false);

	bq2589x_enable_auto_dpdm(bq, bq->cfg.enable_auto_dpdm);
	bq2589x_enable_term(bq, bq->cfg.enable_term);
	bq2589x_enable_ico(bq, bq->cfg.enable_ico);
	/*force use absolute vindpm if auto_dpdm not enabled*/
	if (!bq->cfg.enable_auto_dpdm)
		bq->cfg.use_absolute_vindpm = true;
	bq2589x_use_absolute_vindpm(bq, bq->cfg.use_absolute_vindpm);


	ret = bq2589x_set_vindpm_offset(bq, 600);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set vindpm offset:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_term_current(bq, bq->cfg.term_current);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set termination current:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_chargevoltage(bq, bq->cfg.charge_voltage);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set charge voltage:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_chargecurrent(bq, bq->cfg.charge_current);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set charge current:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_enable_charger(bq);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to enable charger:%d\n", __func__, ret);
		return ret;
	}

	bq2589x_adc_start(bq, false);

	ret = bq2589x_pumpx_enable(bq, 1);
	if (ret) {
		dev_err(bq->dev, "%s:Failed to enable pumpx:%d\n", __func__, ret);
		return ret;
	}

	bq2589x_set_watchdog_timer(bq, 160);

	return ret;
}

/*
static int bq2589x_charge_status(struct bq2589x *bq)
{
	u8 val = 0;

	bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;
	switch (val) {
	case BQ2589X_CHRG_STAT_FASTCHG:
		return POWER_SUPPLY_CHARGE_TYPE_FAST;
	case BQ2589X_CHRG_STAT_PRECHG:
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
	case BQ2589X_CHRG_STAT_CHGDONE:
	case BQ2589X_CHRG_STAT_IDLE:
		return POWER_SUPPLY_CHARGE_TYPE_NONE;
	default:
		return POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
	}
}*/

static int bq2589x_power_supply_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	struct bq2589x *bq = power_supply_get_drvdata(psy);
	struct bq2589x_state state = bq->state;
	u8 chrg_status = bq2589x_get_charging_status(bq);

	switch (psp) {
	case POWER_SUPPLY_PROP_MANUFACTURER:
		val->strval = BQ2589x_MANUFACTURER;
		break;
	case POWER_SUPPLY_PROP_MODEL_NAME:
		if (bq->part_no== BQ25890)
			val->strval = "BQ25890";
		else if (bq->part_no== BQ25892)
			val->strval = "BQ25892";
		else if (bq->part_no== BQ25895)
			val->strval = "BQ25895";
		else
			val->strval = "UNKNOWN";
		break;
	case POWER_SUPPLY_PROP_STATUS:
		if (!state.online)
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		else if (chrg_status == STATUS_NOT_CHARGING)
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
		else if (chrg_status == STATUS_PRE_CHARGING || chrg_status == STATUS_FAST_CHARGING)
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
		else if (chrg_status == STATUS_TERMINATION_DONE)
			val->intval = POWER_SUPPLY_STATUS_FULL;
		else
			val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = state.online;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		if (state.chrg_fault & 0xF8)
			val->intval = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
		else
			val->intval = POWER_SUPPLY_HEALTH_GOOD;

		switch (state.health) {
			case 6: /* 110 - TS Hot */
				val->intval = POWER_SUPPLY_HEALTH_HOT;
				break;
			case 2: /* 010 - TS Warm */
				val->intval = POWER_SUPPLY_HEALTH_WARM;
				break;
			case 3: /* 011 - TS Cool */
				val->intval = POWER_SUPPLY_HEALTH_COOL;
				break;
			case 5: /* 101 - TS Cold */
				val->intval = POWER_SUPPLY_HEALTH_COLD;
				break;
		}
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = bq2589x_adc_read_vbus_volt(bq) * 1000;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = bq2589x_adc_read_charge_current(bq);
		break;
	case POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT:
		val->intval = bq2589x_read_idpm_limit(bq);
		break;
	case POWER_SUPPLY_PROP_INPUT_VOLTAGE_LIMIT:
		val->intval = bq2589x_get_vindpm_volt(bq);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		switch (chrg_status) {
			case STATUS_PRE_CHARGING:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
				break;
			case STATUS_FAST_CHARGING:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_FAST;
				break;
			case STATUS_TERMINATION_DONE:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
				break;
			case STATUS_NOT_CHARGING:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
				break;
			default:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
		}
		break;
    case POWER_SUPPLY_PROP_USB_TYPE:
		switch (state.chrg_type) {
			case 0: /* 000 - No Input */
				val->intval = POWER_SUPPLY_USB_TYPE_UNKNOWN;
				break;
			case 1: /* 001 - USB Host SDP */
				val->intval = POWER_SUPPLY_USB_TYPE_SDP;
				break;
			case 2: /* 010 - USB CDP */
				val->intval = POWER_SUPPLY_USB_TYPE_CDP;
				break;
			case 3: /* 011 - USB DCP */
				val->intval = POWER_SUPPLY_USB_TYPE_DCP;
				break;
			case 4: /* 100 - Adjustable High Voltage DCP */
				val->intval = POWER_SUPPLY_TYPE_USB_HVDCP;
				break;
			case 5: /* 101 - Unkown Adapter */
				val->intval = POWER_SUPPLY_USB_TYPE_UNKNOWN;
				break;
			case 6: /* 110 - Non-Standard Adapter */
				val->intval = POWER_SUPPLY_TYPE_USB_FLOAT;
				break;
			default:
				val->intval = POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
		}
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = state.vbus_gd;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int bq2589x_power_supply_set_property(struct power_supply *psy,
					enum power_supply_property psp,
					const union power_supply_propval *val)
{
	struct bq2589x *bq = power_supply_get_drvdata(psy);
	int ret = -EINVAL;

	switch (psp) {
		case POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT:
			ret = bq2589x_set_input_current_limit(bq, val->intval);
			break;
		case POWER_SUPPLY_PROP_INPUT_VOLTAGE_LIMIT:
			ret = bq2589x_set_input_volt_limit(bq, val->intval);
			break;
		default:
			return -EINVAL;
	}

	return ret;
}
static int bq2589x_power_supply_prop_is_writeable(struct power_supply *psy,
					enum power_supply_property psp)
{
	switch (psp) {
		case POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT:
		case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE:
		case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT:
		case POWER_SUPPLY_PROP_PRECHARGE_CURRENT:
		case POWER_SUPPLY_PROP_CHARGE_TERM_CURRENT:
			return true;
		default:
			return false;
	}
}

static enum power_supply_property bq2589x_power_supply_props[] = {
	POWER_SUPPLY_PROP_MANUFACTURER,
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	POWER_SUPPLY_PROP_INPUT_VOLTAGE_LIMIT,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_USB_TYPE,
	POWER_SUPPLY_PROP_PRESENT
};

static char *bq2589x_charger_supplied_to[] = {
	"battery",
};

static const struct power_supply_desc bq2589x_power_supply_desc = {
	.name = "charger",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.usb_types = bq2589x_usb_type,
	.num_usb_types = ARRAY_SIZE(bq2589x_usb_type),
	.properties = bq2589x_power_supply_props,
	.num_properties = ARRAY_SIZE(bq2589x_power_supply_props),
	.get_property = bq2589x_power_supply_get_property,
	.set_property = bq2589x_power_supply_set_property,
	.property_is_writeable = bq2589x_power_supply_prop_is_writeable,
};

static int bq2589x_psy_register(struct bq2589x *bq)
{
	struct power_supply_config psy_cfg = { .drv_data = bq, };

	psy_cfg.supplied_to = bq2589x_charger_supplied_to;
	psy_cfg.num_supplicants = ARRAY_SIZE(bq2589x_charger_supplied_to);

	bq->wall_psy = devm_power_supply_register(bq->dev, &bq2589x_power_supply_desc, &psy_cfg);
	if (IS_ERR(bq->wall_psy))
		return -EINVAL;

	return 0;
}

static void bq2589x_psy_unregister(struct bq2589x *bq)
{
	power_supply_unregister(bq->wall_psy);
}

static ssize_t bq2589x_show_registers(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	u8 addr;
	u8 val;
	u8 tmpbuf[300];
	int len;
	int idx = 0;
	int ret ;

	idx = snprintf(buf, PAGE_SIZE, "%s:\n", "Charger 1");
	for (addr = 0x0; addr <= 0x14; addr++) {
		ret = bq2589x_read_byte(g_bq, &val, addr);
		if (ret == 0) {
			len = snprintf(tmpbuf, PAGE_SIZE - idx,"Reg[0x%.2x] = 0x%.2x\n", addr, val);
			memcpy(&buf[idx], tmpbuf, len);
			idx += len;
		}
	}

	return idx;
}

static DEVICE_ATTR(registers, S_IRUGO, bq2589x_show_registers, NULL);

static struct attribute *bq2589x_attributes[] = {
	&dev_attr_registers.attr,
	NULL,
};

static const struct attribute_group bq2589x_attr_group = {
	.attrs = bq2589x_attributes,
};


static int bq2589x_parse_dt(struct device *dev, struct bq2589x *bq)
{
	int ret;
	struct device_node *np = dev->of_node;

	if (of_property_read_string(np, "charger_name", &bq->chg_dev_name) < 0) {
		bq->chg_dev_name = "master_chg";
		dev_err(bq->dev,"%s: no charger name\n", __func__);
	}

	bq->chg_en_gpio = of_get_named_gpio(np, "bq2589x_en-gpio", 0);
	if (gpio_is_valid(bq->chg_en_gpio))
	{
		ret = devm_gpio_request(bq->dev, bq->chg_en_gpio, "bq25890 chg en pin");
		if (ret) {
			dev_err(bq->dev, "%s: %d gpio request failed\n", __func__, bq->chg_en_gpio);
			return ret;
		}
		gpio_direction_output(bq->chg_en_gpio,0);//default enable charge
	}

	ret = of_property_read_u32(np, "ti,bq2589x,vbus-volt-high-level", &pe.high_volt_level);
	if (ret)
		return ret;

	ret = of_property_read_u32(np, "ti,bq2589x,vbus-volt-low-level", &pe.low_volt_level);
	if (ret)
		return ret;

	ret = of_property_read_u32(np, "ti,bq2589x,vbat-min-volt-to-tuneup", &pe.vbat_min_volt);
	if (ret)
		return ret;

	bq->cfg.enable_auto_dpdm = of_property_read_bool(np, "ti,bq2589x,enable-auto-dpdm");
	bq->cfg.enable_term = of_property_read_bool(np, "ti,bq2589x,enable-termination");
	bq->cfg.enable_ico = of_property_read_bool(np, "ti,bq2589x,enable-ico");
	bq->cfg.use_absolute_vindpm = of_property_read_bool(np, "ti,bq2589x,use-absolute-vindpm");

	ret = of_property_read_u32(np, "ti,bq2589x,charge-voltage",&bq->cfg.charge_voltage);
	if (ret)
		return ret;

	ret = of_property_read_u32(np, "ti,bq2589x,charge-current",&bq->cfg.charge_current);
	if (ret)
		return ret;

	ret = of_property_read_u32(np, "ti,bq2589x,term-current",&bq->cfg.term_current);
	if (ret)
		return ret;

	return 0;
}

static int bq2589x_detect_device(struct bq2589x *bq)
{
	int ret;
	u8 data;

	ret = bq2589x_read_byte(bq, &data, BQ2589X_REG_14);
	if (ret == 0) {
		bq->part_no = (data & BQ2589X_PN_MASK) >> BQ2589X_PN_SHIFT;
		bq->revision = (data & BQ2589X_DEV_REV_MASK) >> BQ2589X_DEV_REV_SHIFT;
	}

	return ret;
}

static int bq2589x_read_batt_rsoc(struct bq2589x *bq)
{
	union power_supply_propval ret = {0,};

	if (!bq->batt_psy)
		bq->batt_psy = power_supply_get_by_name("bms");

	if (bq->batt_psy) {
		power_supply_get_property(bq->batt_psy, POWER_SUPPLY_PROP_CAPACITY, &ret);
		return ret.intval;
	} else {
		return 50;
	}
}

int bq2589x_get_usb_present(struct bq2589x *bq)
{
	uint8_t val;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_11);
	if (ret < 0) {
		dev_err(bq->dev, "read vbus BQ2589X_REG_11 failed :%d\n", ret);
		return ret;
	}

	bq->state.vbus_gd = (val & BQ2589X_VBUS_GD_MASK) >> BQ2589X_VBUS_GD_SHIFT;
	return ret;
}

static bq2589x_reuqest_dpdm(struct bq2589x *bq, bool enable)
{
	int ret = 0;

	/* fetch the DPDM regulator */
	if (!bq->dpdm_reg && of_get_property(bq->dev->of_node, "dpdm-supply", NULL)) {
		bq->dpdm_reg = devm_regulator_get(bq->dev, "dpdm");
		if (IS_ERR(bq->dpdm_reg)) {
			ret = PTR_ERR(bq->dpdm_reg);
			dev_err(bq->dev, "Couldn't get dpdm regulator ret=%d\n", ret);
			bq->dpdm_reg = NULL;
			return ret;
		}
	}

	mutex_lock(&bq->dpdm_lock);
	if (enable) {
		if (bq->dpdm_reg && !bq->dpdm_enabled) {
		dev_err(bq->dev, "enabling DPDM regulator\n");
		ret = regulator_enable(bq->dpdm_reg);
		if (ret < 0)
			dev_err(bq->dev, "Couldn't enable dpdm regulator ret=%d\n", ret);
		else
			bq->dpdm_enabled = true;
		}
	} else {
		if (bq->dpdm_reg && bq->dpdm_enabled) {
			dev_err(bq->dev, "disabling DPDM regulator\n");
			ret = regulator_disable(bq->dpdm_reg);
			if (ret < 0)
				dev_err(bq->dev, "Couldn't disable dpdm regulator ret=%d\n", ret);
			else
				bq->dpdm_enabled = false;
		}
	}
	mutex_unlock(&bq->dpdm_lock);

	return ret;
}

void bq2589x_set_ilim_enable(struct bq2589x *bq, bool enable)
{
	int ret;
	u8 val;

	if (enable)
		val = BQ2589X_ENILIM_ENABLE << BQ2589X_ENILIM_SHIFT;
	else
		val = BQ2589X_ENILIM_DISABLE << BQ2589X_ENILIM_SHIFT;

	dev_info(bq->dev, "%s:set ILIM %s\n", __func__, enable ? "enabled":"disabled");

	ret = bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENILIM_MASK, val);
	if (ret < 0)
		dev_err(bq->dev, "fail to set ILIM pin ret=%d\n", ret);
}

static bool bq2589x_is_rerun_apsd_complete(struct bq2589x *bq)
{
	int ret;
	u8 val = 0;
	bool result = false;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_02);
	if (ret)
		return false;

	result = !(val & BQ2589X_FORCE_DPDM_MASK);
	pr_info("%s:rerun apsd %s", __func__, result ? "complete" : "not complete");
	return result;
}

static int bq2589x_rerun_apsd_if_required(struct bq2589x *bq)
{
	int ret = 0;
	ret = bq2589x_get_usb_present(bq);
	if (ret)
		return ret;

	if (!bq->state.vbus_gd)
		return 0;

	ret = bq2589x_reuqest_dpdm(bq, true);
	if (ret < 0)
		dev_err(bq->dev,"%s:cannot enable DPDM ret=%d\n",__func__, ret);

	bq2589x_force_dpdm(bq);

	bq->typec_apsd_rerun_done = true;

	dev_info(bq->dev,"rerun apsd done\n");
	return 0;
}

static void bq2589x_adjust_absolute_vindpm(struct bq2589x *bq)
{
	u16 vbus_volt;
	u16 vindpm_volt;
	int ret;

	ret = bq2589x_disable_charger(bq);
	if (ret < 0) {
		dev_err(bq->dev,"%s:failed to disable charger\n",__func__);
		/*return;*/
	}
	/* wait for new adc data */
	msleep(1000);
	vbus_volt = bq2589x_adc_read_vbus_volt(bq);
	ret = bq2589x_enable_charger(bq);
	if (ret < 0) {
		dev_err(bq->dev, "%s:failed to enable charger\n",__func__);
		return;
	}

	if (vbus_volt < 6000)
		vindpm_volt = vbus_volt - 600;
	else
		vindpm_volt = vbus_volt - 1200;
	ret = bq2589x_set_input_volt_limit(bq, vindpm_volt);
	if (ret < 0)
		dev_err(bq->dev, "%s:Set absolute vindpm threshold %d Failed:%d\n", __func__, vindpm_volt, ret);
	else
		dev_info(bq->dev, "%s:Set absolute vindpm threshold %d successfully\n", __func__, vindpm_volt);

}

static void bq2589x_adapter_in_func(struct bq2589x *bq)
{
	int ret = 0;

	if ((bq->vbus_type == BQ2589X_VBUS_USB_SDP ||
		bq->vbus_type == BQ2589X_VBUS_NONSTAND ||
		bq->vbus_type == BQ2589X_VBUS_UNKNOWN)
		&& (!bq->typec_apsd_rerun_done)) {
		dev_err(bq->dev, "rerun apsd for 0x%x\n", bq->vbus_type);
		bq2589x_rerun_apsd_if_required(bq);
		return;
	}

	switch (bq->vbus_type) {
		case BQ2589X_VBUS_MAXC:
			dev_info(bq->dev, "%s:HVDCP adapter plugged in\n", __func__);
			bq->real_charger_type = POWER_SUPPLY_TYPE_USB_HVDCP;
			bq->chg_dev->noti.hvdcp_done = true;
			schedule_delayed_work(&bq->ico_work, 0);
			break;
		case BQ2589X_VBUS_USB_CDP:
			dev_info(bq->dev, "%s:CDP plugged in\n", __func__);
			bq->real_charger_type = POWER_SUPPLY_TYPE_USB_CDP;
			break;
		case BQ2589X_VBUS_USB_DCP:
			dev_info(bq->dev, "%s:DCP adapter plugged in\n", __func__);
			bq->real_charger_type = POWER_SUPPLY_TYPE_USB_DCP;
			schedule_delayed_work(&bq->check_pe_tuneup_work, 0);
			break;
		case BQ2589X_VBUS_USB_SDP:
			dev_info(bq->dev, "%s:SDP plugged in\n", __func__);
			bq->real_charger_type = POWER_SUPPLY_TYPE_USB;
			break;
		case BQ2589X_VBUS_UNKNOWN:
			dev_err(bq->dev, "%s:Unkown adpater plugged in\n", __func__);
		case BQ2589X_VBUS_NONSTAND:
			dev_info(bq->dev, "%s:NON STAND plugged in\n", __func__);
			bq->real_charger_type = POWER_SUPPLY_TYPE_USB_FLOAT;
			break;
		default:
			dev_info(bq->dev, "%s:Other adapter %d plugged in\n", __func__, bq->vbus_type);
			bq->real_charger_type = POWER_SUPPLY_TYPE_UNKNOWN;
			schedule_delayed_work(&bq->ico_work, 0);
			break;
	}

	if (bq->cfg.use_absolute_vindpm) {
		ret = bq2589x_set_input_volt_limit(bq, 4600);
		if (ret < 0)
			dev_err(bq->dev,"%s:reset vindpm threshold to 4600 failed:%d\n",__func__,ret);
		else
			dev_info(bq->dev,"%s:reset vindpm threshold to 4600 successfully\n",__func__);
	}

	if (pe.enable) {
		schedule_delayed_work(&bq->monitor_work, 0);
	}

	bq->chg_dev->noti.apsd_done = true;
	bq->typec_apsd_rerun_done = false;
	charger_dev_notify(bq->chg_dev);

}

static void bq2589x_adapter_out_func(struct bq2589x *bq)
{
	int ret;

	ret = bq2589x_set_input_volt_limit(bq, 4600);
	if (ret < 0)
		dev_err(bq->dev,"%s:reset vindpm threshold to 4600 failed:%d\n",__func__,ret);
	else
		dev_info(bq->dev,"%s:reset vindpm threshold to 4600 successfully\n",__func__);

	if (pe.enable) {
		cancel_delayed_work_sync(&bq->monitor_work);
	}

	bq->typec_apsd_rerun_done = false;
	bq->chg_dev->noti.apsd_done = false;
	bq->chg_dev->noti.hvdcp_done = false;
	bq->real_charger_type = POWER_SUPPLY_TYPE_UNKNOWN;
	bq2589x_reuqest_dpdm(bq, false);
	charger_dev_notify(bq->chg_dev);
}

static void bq2589x_ico_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, ico_work.work);
	int ret;
	int idpm;
	u8 status;
	static bool ico_issued;

	if (!ico_issued) {
		ret = bq2589x_force_ico(bq);
		if (ret < 0) {
			schedule_delayed_work(&bq->ico_work, HZ); /* retry 1 second later*/
			dev_info(bq->dev, "%s:ICO command issued failed:%d\n", __func__, ret);
		} else {
			ico_issued = true;
			schedule_delayed_work(&bq->ico_work, 3 * HZ);
			dev_info(bq->dev, "%s:ICO command issued successfully\n", __func__);
		}
	} else {
		ico_issued = false;
		ret = bq2589x_check_force_ico_done(bq);
		if (ret) {/*ico done*/
			ret = bq2589x_read_byte(bq, &status, BQ2589X_REG_13);
			if (ret == 0) {
				idpm = ((status & BQ2589X_IDPM_LIM_MASK) >> BQ2589X_IDPM_LIM_SHIFT) * BQ2589X_IDPM_LIM_LSB + BQ2589X_IDPM_LIM_BASE;
				dev_info(bq->dev, "%s:ICO done, result is:%d mA\n", __func__, idpm);
			}
		}
	}
}

static void bq2589x_check_pe_tuneup_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, check_pe_tuneup_work.work);

	if (!pe.enable) {
		schedule_delayed_work(&bq->ico_work, 0);
		return;
	}

	bq->vbat_volt = bq2589x_adc_read_battery_volt(bq);
	bq->rsoc = bq2589x_read_batt_rsoc(bq);

	if (bq->vbat_volt > pe.vbat_min_volt && bq->rsoc < 95) {
		dev_info(bq->dev, "%s:trying to tune up vbus voltage\n", __func__);
		pe.target_volt = pe.high_volt_level;
		pe.tune_up_volt = true;
		pe.tune_down_volt = false;
		pe.tune_done = false;
		pe.tune_count = 0;
		pe.tune_fail = false;
		schedule_delayed_work(&bq->pe_volt_tune_work, 0);
	} else if (bq->rsoc >= 95) {
		schedule_delayed_work(&bq->ico_work, 0);
	} else {
		/* wait battery voltage up enough to check again */
		schedule_delayed_work(&bq->check_pe_tuneup_work, 2*HZ);
	}
}

static void bq2589x_tune_volt_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, pe_volt_tune_work.work);
	int ret = 0;
	static bool pumpx_cmd_issued;

	bq->vbus_volt = bq2589x_adc_read_vbus_volt(bq);

	dev_info(bq->dev, "%s:vbus voltage:%d, Tune Target Volt:%d\n", __func__, bq->vbus_volt, pe.target_volt);

	if ((pe.tune_up_volt && bq->vbus_volt > pe.target_volt) ||
	(pe.tune_down_volt && bq->vbus_volt < pe.target_volt)) {
		dev_info(bq->dev, "%s:voltage tune successfully\n", __func__);
		pe.tune_done = true;
		bq2589x_adjust_absolute_vindpm(bq);
		if (pe.tune_up_volt)
			schedule_delayed_work(&bq->ico_work, 0);
		return;
	}

	if (pe.tune_count > 10) {
		dev_info(bq->dev, "%s:voltage tune failed,reach max retry count\n", __func__);
		pe.tune_fail = true;
		bq2589x_adjust_absolute_vindpm(bq);

		if (pe.tune_up_volt)
			schedule_delayed_work(&bq->ico_work, 0);
		return;
	}

	if (!pumpx_cmd_issued) {
		if (pe.tune_up_volt)
			ret = bq2589x_pumpx_increase_volt(bq);
		else if (pe.tune_down_volt)
			ret =  bq2589x_pumpx_decrease_volt(bq);
		if (ret) {
			schedule_delayed_work(&bq->pe_volt_tune_work, HZ);
		} else {
			dev_info(bq->dev, "%s:pumpx command issued.\n", __func__);
			pumpx_cmd_issued = true;
			pe.tune_count++;
			schedule_delayed_work(&bq->pe_volt_tune_work, 3*HZ);
		}
	} else {
		if (pe.tune_up_volt)
			ret = bq2589x_pumpx_increase_volt_done(bq);
		else if (pe.tune_down_volt)
			ret = bq2589x_pumpx_decrease_volt_done(bq);
		if (ret == 0) {
			dev_info(bq->dev, "%s:pumpx command finishedd!\n", __func__);
			bq2589x_adjust_absolute_vindpm(bq);
			pumpx_cmd_issued = 0;
		}
		schedule_delayed_work(&bq->pe_volt_tune_work, HZ);
	}
}


static void bq2589x_monitor_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, monitor_work.work);
	u8 status = 0;
	int ret;
	int chg_current;

	bq2589x_reset_watchdog_timer(bq);

	bq->rsoc = bq2589x_read_batt_rsoc(bq);

	bq->vbus_volt = bq2589x_adc_read_vbus_volt(bq);
	bq->vbat_volt = bq2589x_adc_read_battery_volt(bq);
	chg_current = bq2589x_adc_read_charge_current(bq);

	dev_info(bq->dev, "%s:vbus volt:%d,vbat volt:%d,charge current:%d\n", __func__,bq->vbus_volt,bq->vbat_volt,chg_current);

	ret = bq2589x_read_byte(bq, &status, BQ2589X_REG_13);
	if (ret == 0 && (status & BQ2589X_VDPM_STAT_MASK))
		dev_info(bq->dev, "%s:VINDPM occurred\n", __func__);
	if (ret == 0 && (status & BQ2589X_IDPM_STAT_MASK))
		dev_info(bq->dev, "%s:IINDPM occurred\n", __func__);

	if (bq->vbus_type == BQ2589X_VBUS_USB_DCP && bq->vbus_volt > pe.high_volt_level &&
	bq->rsoc > 95 && !pe.tune_down_volt) {
		pe.tune_down_volt = true;
		pe.tune_up_volt = false;
		pe.target_volt = pe.low_volt_level;
		pe.tune_done = false;
		pe.tune_count = 0;
		pe.tune_fail = false;
		schedule_delayed_work(&bq->pe_volt_tune_work, 0);
	}

	/* read temperature,or any other check if need to decrease charge current*/
	schedule_delayed_work(&bq->monitor_work, 10 * HZ);
}



static void bq2589x_charger_irq_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, irq_work);
	struct bq2589x_state state;
	u8 status = 0;
	u8 fault = 0;
	u8 charge_status = 0;
	int ret;
	bool reapsd_complete = false;

	ret = bq2589x_sync_state(bq, &state);
	mutex_lock(&bq->lock);
	bq->state = state;
	mutex_unlock(&bq->lock);

	/* Read STATUS and FAULT registers */
	ret = bq2589x_read_byte(bq, &status, BQ2589X_REG_0B);
	if (ret)
		return;

	ret = bq2589x_read_byte(bq, &fault, BQ2589X_REG_0C);
	if (ret)
		return;

	bq->vbus_type = (status & BQ2589X_VBUS_STAT_MASK) >> BQ2589X_VBUS_STAT_SHIFT;

	if (state.vbus_gd && (bq->vbus_type == BQ2589X_VBUS_NONE)){
		dev_info(bq->dev, "BC1.2 detect is not done.\n");
		bq2589x_reuqest_dpdm(bq, true);
	}

	if(state.vbus_gd && state.online
		&& (bq->typec_apsd_rerun_done == true)
		&& (bq->vbus_type != BQ2589X_VBUS_NONE)) {
		reapsd_complete = bq2589x_is_rerun_apsd_complete(bq);
	}

	if (((bq->vbus_type == BQ2589X_VBUS_NONE) || (bq->vbus_type == BQ2589X_VBUS_OTG)) && (bq->status & BQ2589X_STATUS_PLUGIN)) {
		dev_info(bq->dev, "%s:adapter removed\n", __func__);
		bq->status &= ~BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_out_func(bq);
	} else if ((bq->vbus_type != BQ2589X_VBUS_NONE) && (bq->vbus_type != BQ2589X_VBUS_OTG)
			&& (!(bq->status & BQ2589X_STATUS_PLUGIN) || (reapsd_complete == true))
			&& state.online) {
		dev_info(bq->dev, "%s:adapter plugged in\n", __func__);
		bq->status |= BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_in_func(bq);
	}


	if ((status & BQ2589X_PG_STAT_MASK) && !(bq->status & BQ2589X_STATUS_PG))
		bq->status |= BQ2589X_STATUS_PG;
	else if (!(status & BQ2589X_PG_STAT_MASK) && (bq->status & BQ2589X_STATUS_PG))
		bq->status &= ~BQ2589X_STATUS_PG;

	if (fault && !(bq->status & BQ2589X_STATUS_FAULT))
		bq->status |= BQ2589X_STATUS_FAULT;
	else if (!fault && (bq->status & BQ2589X_STATUS_FAULT))
		bq->status &= ~BQ2589X_STATUS_FAULT;

	charge_status = (status & BQ2589X_CHRG_STAT_MASK) >> BQ2589X_CHRG_STAT_SHIFT;
	if (charge_status == BQ2589X_CHRG_STAT_IDLE)
		dev_info(bq->dev, "%s:not charging\n", __func__);
	else if (charge_status == BQ2589X_CHRG_STAT_PRECHG)
		dev_info(bq->dev, "%s:precharging\n", __func__);
	else if (charge_status == BQ2589X_CHRG_STAT_FASTCHG)
		dev_info(bq->dev, "%s:fast charging\n", __func__);
	else if (charge_status == BQ2589X_CHRG_STAT_CHGDONE)
		dev_info(bq->dev, "%s:charge done!\n", __func__);

	if (fault)
		dev_info(bq->dev, "%s:charge fault:%02x\n", __func__,fault);
}

static int bq2589x_irq_probe(struct bq2589x *bq)
{
	struct gpio_desc *irq;

	irq = devm_gpiod_get(bq->dev, BQ25890_IRQ_PIN, GPIOD_IN);
	if (IS_ERR(irq)) {
		dev_err(bq->dev, "Could not probe irq pin.\n");
		return PTR_ERR(irq);
	}

	return gpiod_to_irq(irq);
}

static irqreturn_t bq2589x_charger_interrupt(int irq, void *data)
{
	struct bq2589x *bq = data;

	schedule_work(&bq->irq_work);
	return IRQ_HANDLED;
}

static int bq2589x_get_real_charger_type(struct charger_device *chg_dev, int *chg_type)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);

	*chg_type = bq->real_charger_type;

	return 0;
}

static int bq2589x_set_icl(struct charger_device *chg_dev, u32 uA)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	uA /= 1000;
	dev_info(bq->dev,"%s set icl curr = %d mA\n", __func__, uA);

	return bq2589x_set_input_current_limit(bq, uA);
}

static int bq2589x_get_icl(struct charger_device *chg_dev, u32 *uA)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);

	*uA = bq2589x_read_idpm_limit(bq)*1000;

	dev_info(bq->dev,"%s get icl curr = %d uA\n", __func__, *uA);

	return 0;
}

static int bq2589x_enable_hw_jeita(struct charger_device *chg_dev, bool en)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	dev_warn(bq->dev,"%s cannot set jeita state to %s\n", __func__, en ? "enabled":"disabled");

	return en;
}

static int bq2589x_set_otg_enable(struct charger_device *chg_dev, bool enable)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	u8 val;

	if (enable)
		val = BQ2589X_OTG_ENABLE << BQ2589X_OTG_CONFIG_SHIFT;
	else
		val = BQ2589X_OTG_DISABLE << BQ2589X_OTG_CONFIG_SHIFT;

	dev_info(bq->dev, "%s: %s otg\n", __func__, enable ? "enable" : "disable");

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_OTG_CONFIG_MASK, val);
}

static int bq2589x_set_boost_current_limit(struct charger_device *chg_dev, u32 uA)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	int ret = 0;
	uA /= 1000;
	ret = bq2589x_set_otg_current(bq, uA);

	dev_info(bq->dev,"%s set boost current limit = %d mA, %s\n", __func__, uA, ret ? "failed" : "success");

	return ret;
}

static int bq2589x_enable_charging(struct charger_device *chg_dev, bool enable)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;
	else
		val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);

	if (ret == 0) {
		if (enable)
			bq->status |= BQ2589X_STATUS_CHARGE_ENABLE;
		else
			bq->status &= ~BQ2589X_STATUS_CHARGE_ENABLE;
	}

	dev_info(bq->dev,"%s, %s charging %s\n", __func__, enable ? "enable" : "disable", ret == 0 ? "success":"failed");

	return ret;
}

static int bq2589x_set_charging_current(struct charger_device *chg_dev, u32 uA)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	int ret = 0;
	uA /= 1000;
	ret = bq2589x_set_chargecurrent(bq, uA);

	dev_info(bq->dev,"%s set charging curr = %d mA, %s\n", __func__, uA, ret ? "failed" : "success");

	return ret;
}

static int bq2589x_set_charging_voltage(struct charger_device *chg_dev, u32 uV)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	int ret = 0;
	uV /= 1000;
	ret = bq2589x_set_chargevoltage(bq, uV);

	dev_info(bq->dev,"%s set charging volt = %d mV, %s\n", __func__, uV, ret ? "failed" : "success");

	return ret;
}

static int bq2589x_is_charging_halted(struct charger_device *chg_dev, bool *en)
{
	int ret = 0;
	int chrg_stat = 0;
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	chrg_stat = bq2589x_get_charging_status(bq);

	switch(chrg_stat) {
	case STATUS_NOT_CHARGING:
	case STATUS_TERMINATION_DONE:
		*en = true;
		break;
	case STATUS_PRE_CHARGING:
	case STATUS_FAST_CHARGING:
		*en = false;
		break;
	default:
		break;
	}

	return ret;
}

static int bq2589x_dump_registers(struct charger_device *chg_dev, struct seq_file *m)
{
	struct bq2589x *bq = dev_get_drvdata(&chg_dev->dev);
	u8 addr;
	u8 val;
	int ret;

	for(addr = 0x0; addr<=0x14; addr++) {
		ret = bq2589x_read_byte(bq, &val, addr);
		if (ret)
			continue;
		seq_printf(m, "%s Reg[0x%.2x] = 0x%.2x\n", __func__, addr, val);

	}

	return ret;
}

static const struct charger_properties bq2589x_chg_props = {
	.alias_name = "bq25890",
};

static struct charger_ops bq2589x_chg_ops = {
	.get_real_charger_type = bq2589x_get_real_charger_type,
	.set_input_current = bq2589x_set_icl,
	.get_input_current = bq2589x_get_icl,
	.enable_hw_jeita = bq2589x_enable_hw_jeita,
	.enable_otg = bq2589x_set_otg_enable,
	.set_boost_current_limit = bq2589x_set_boost_current_limit,
	.enable_charging = bq2589x_enable_charging,
	.set_charging_current = bq2589x_set_charging_current,
	.set_constant_voltage = bq2589x_set_charging_voltage,
	.is_charge_halted = bq2589x_is_charging_halted,

	.dump_registers = bq2589x_dump_registers,

};

static int bq2589x_charger_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct bq2589x *bq;
	int ret;

	bq = devm_kzalloc(dev, sizeof(struct bq2589x), GFP_KERNEL);
	if (!bq) {
		dev_err(dev, "%s: out of memory\n", __func__);
		return -ENOMEM;
	}

	bq->dev = dev;
	bq->client = client;
	i2c_set_clientdata(client, bq);

	ret = bq2589x_detect_device(bq);
	if (!ret && bq->part_no == BQ25890) {
		bq->status |= BQ2589X_STATUS_EXIST;
		dev_info(dev, "%s: charger device bq25890 detected, revision:%d\n", __func__, bq->revision);
	} else {
		dev_info(dev, "%s: no bq25890 charger device found:%d\n", __func__, ret);
		return -ENODEV;
	}

	bq->batt_psy = power_supply_get_by_name("bms");

	g_bq = bq;

	if (client->dev.of_node)
		bq2589x_parse_dt(&client->dev, bq);

	bq->chg_dev = charger_device_register(bq->chg_dev_name,
					      &client->dev, bq,
					      &bq2589x_chg_ops,
					      &bq2589x_chg_props);
	if (IS_ERR_OR_NULL(bq->chg_dev)) {
		ret = PTR_ERR(bq->chg_dev);
		return ret;
	}

	ret = bq2589x_init_device(bq);
	if (ret) {
		dev_err(dev, "device init failure: %d\n", ret);
		goto err_0;
	}

	if (client->irq <= 0)
		client->irq = bq2589x_irq_probe(bq);

	if (client->irq < 0) {
		dev_err(dev, "No irq resource found.\n");
		return client->irq;
	}

	ret = bq2589x_psy_register(bq);
	if (ret)
		goto err_0;

	/*MMI_STOPSHIP PMIC:add disable ILIM pin because of hw is not ready */
	bq2589x_set_ilim_enable(bq, false);

	INIT_WORK(&bq->irq_work, bq2589x_charger_irq_workfunc);
	INIT_DELAYED_WORK(&bq->monitor_work, bq2589x_monitor_workfunc);
	INIT_DELAYED_WORK(&bq->ico_work, bq2589x_ico_workfunc);
	INIT_DELAYED_WORK(&bq->pe_volt_tune_work, bq2589x_tune_volt_workfunc);
	INIT_DELAYED_WORK(&bq->check_pe_tuneup_work, bq2589x_check_pe_tuneup_workfunc);


	ret = sysfs_create_group(&bq->dev->kobj, &bq2589x_attr_group);
	if (ret) {
		dev_err(dev, "failed to register sysfs. err: %d\n", ret);
		goto err_irq;
	}

	ret = devm_request_threaded_irq(dev, client->irq, NULL,
				bq2589x_charger_interrupt, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, BQ25890_IRQ_PIN, bq);
	if (ret) {
		dev_err(dev, "%s:Request IRQ %d failed: %d\n", __func__, client->irq, ret);
		goto err_irq;
	} else {
		dev_info(dev, "%s:irq = %d\n", __func__, client->irq);
	}

	pe.enable = false;

	bq2589x_rerun_apsd_if_required(bq);

	return 0;

err_irq:
	cancel_work_sync(&bq->irq_work);
	cancel_delayed_work_sync(&bq->ico_work);
	cancel_delayed_work_sync(&bq->check_pe_tuneup_work);
	if (pe.enable) {
		cancel_delayed_work_sync(&bq->pe_volt_tune_work);
		cancel_delayed_work_sync(&bq->monitor_work);
	}

err_0:
	g_bq = NULL;
	return ret;
}

static void bq2589x_charger_shutdown(struct i2c_client *client)
{
	struct bq2589x *bq = i2c_get_clientdata(client);

	dev_info(bq->dev, "%s: shutdown\n", __func__);

	/*set CONV_RATE 0 to exits continuous conversion mode*/
	bq2589x_adc_stop(bq);

	bq2589x_psy_unregister(bq);

	sysfs_remove_group(&bq->dev->kobj, &bq2589x_attr_group);
	cancel_work_sync(&bq->irq_work);
	cancel_delayed_work_sync(&bq->ico_work);
	cancel_delayed_work_sync(&bq->check_pe_tuneup_work);
	if (pe.enable) {
		cancel_delayed_work_sync(&bq->pe_volt_tune_work);
		cancel_delayed_work_sync(&bq->monitor_work);
	}

	devm_free_irq(&client->dev, client->irq, bq);

	/*release charging enable gpio to avoid power leakage.*/
	if(gpio_is_valid(bq->chg_en_gpio)){
		devm_gpio_free(&client->dev, bq->chg_en_gpio);
	}
	g_bq = NULL;
}

static struct of_device_id bq2589x_charger_match_table[] = {
	{.compatible = "ti,bq2589x-1",},
	{},
};


static const struct i2c_device_id bq2589x_charger_id[] = {
	{ "bq2589x-1", BQ25890 },
	{},
};

MODULE_DEVICE_TABLE(i2c, bq2589x_charger_id);

static struct i2c_driver bq2589x_charger_driver = {
	.driver		= {
		.name	= "bq2589x-1",
		.of_match_table = bq2589x_charger_match_table,
	},
	.id_table	= bq2589x_charger_id,

	.probe		= bq2589x_charger_probe,
	.shutdown	= bq2589x_charger_shutdown,
};

module_i2c_driver(bq2589x_charger_driver);

MODULE_DESCRIPTION("TI BQ2589x Charger Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Texas Instruments");