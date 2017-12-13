/*
 * cs35l36-tables.c -- CS35L36 ALSA SoC audio driver
 *
 * Copyright 2016 Cirrus Logic, Inc.
 *
 * Author: Brian Austin <brian.austin@cirrus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "cs35l36.h"

struct reg_default cs35l36_reg[CS35L36_MAX_CACHE_REG] = {
	{CS35L36_TESTKEY_CTRL,			0x00000000},
	{CS35L36_USERKEY_CTL,			0x00000000},
	{CS35L36_OTP_CTRL1,			0x00002460},
	{CS35L36_OTP_CTRL2,			0x00000000},
	{CS35L36_OTP_CTRL3,			0x00000000},
	{CS35L36_OTP_CTRL4,			0x00000000},
	{CS35L36_OTP_CTRL5,			0x00000000},
	{CS35L36_PAC_CTL1,			0x00000004},
	{CS35L36_PAC_CTL2,			0x00000000},
	{CS35L36_PAC_CTL3,			0x00000000},
	{CS35L36_PWR_CTRL1,			0x00000000},
	{CS35L36_PWR_CTRL2,			0x00003321},
	{CS35L36_PWR_CTRL3,			0x01000010},
	{CS35L36_CTRL_OVRRIDE,			0x00000002},
	{CS35L36_AMP_OUT_MUTE,			0x00000000},
	{CS35L36_OTP_TRIM_STATUS,		0x00000000},
	{CS35L36_DISCH_FILT,			0x00000000},
	{CS35L36_PROTECT_REL_ERR,		0x00000000},
	{CS35L36_PAD_INTERFACE,			0x00000038},
	{CS35L36_PLL_CLK_CTRL,			0x00000010},
	{CS35L36_GLOBAL_CLK_CTRL,		0x00000003},
	{CS35L36_ADC_CLK_CTRL,			0x00000000},
	{CS35L36_SWIRE_CLK_CTRL,		0x00000000},
	{CS35L36_SP_SCLK_CLK_CTRL,		0x00000000},
	{CS35L36_MDSYNC_EN,			0x00000000},
	{CS35L36_MDSYNC_TX_ID,			0x00000000},
	{CS35L36_MDSYNC_PWR_CTRL,		0x00000000},
	{CS35L36_MDSYNC_DATA_TX,		0x00000000},
	{CS35L36_MDSYNC_TX_STATUS,		0x00000002},
	{CS35L36_MDSYNC_RX_STATUS,		0x00000000},
	{CS35L36_MDSYNC_ERR_STATUS,		0x00000000},
	{CS35L36_BSTCVRT_VCTRL1,		0x00000000},
	{CS35L36_BSTCVRT_VCTRL2,		0x00000001},
	{CS35L36_BSTCVRT_PEAK_CUR,		0x0000004A},
	{CS35L36_BSTCVRT_SFT_RAMP,		0x00000003},
	{CS35L36_BSTCVRT_COEFF,			0x00002424},
	{CS35L36_BSTCVRT_SLOPE_LBST,		0x00005800},
	{CS35L36_BSTCVRT_SW_FREQ,		0x00010000},
	{CS35L36_BSTCVRT_DCM_CTRL,		0x00002001},
	{CS35L36_BSTCVRT_DCM_MODE_FORCE,	0x00000000},
	{CS35L36_BSTCVRT_OVERVOLT_CTRL,		0x00000130},
	{CS35L36_VPI_LIMIT_MODE,		0x00000000},
	{CS35L36_VPI_LIMIT_MINMAX,		0x00003000},
	{CS35L36_VPI_VP_THLD,			0x00101010},
	{CS35L36_VPI_TRACK_CTRL,		0x00000000},
	{CS35L36_VPI_TRIG_MODE_CTRL,		0x00000000},
	{CS35L36_VPI_TRIG_STEPS,		0x00000000},
	{CS35L36_VI_SPKMON_FILT,		0x00000003},
	{CS35L36_VI_SPKMON_GAIN,		0x00000909},
	{CS35L36_VI_SPKMON_IP_SEL,		0x00000000},
	{CS35L36_DTEMP_WARN_THLD,		0x00000002},
	{CS35L36_DTEMP_STATUS,			0x00000000},
	{CS35L36_VPVBST_FS_SEL,			0x00000001},
	{CS35L36_VPVBST_VP_CTRL,		0x000001C0},
	{CS35L36_VPVBST_VBST_CTRL,		0x000001C0},
	{CS35L36_ASP_TX_PIN_CTRL,		0x00000028},
	{CS35L36_ASP_RATE_CTRL,			0x00090000},
	{CS35L36_ASP_FORMAT,			0x00000002},
	{CS35L36_ASP_FRAME_CTRL,		0x00180018},
	{CS35L36_ASP_TX1_TX2_SLOT,		0x00010000},
	{CS35L36_ASP_TX3_TX4_SLOT,		0x00030002},
	{CS35L36_ASP_TX5_TX6_SLOT,		0x00050004},
	{CS35L36_ASP_TX7_TX8_SLOT,		0x00070006},
	{CS35L36_ASP_RX1_SLOT,			0x00000000},
	{CS35L36_ASP_RX_TX_EN,			0x00000000},
	{CS35L36_ASP_RX1_SEL,			0x00000008},
	{CS35L36_ASP_TX1_SEL,			0x00000018},
	{CS35L36_ASP_TX2_SEL,			0x00000019},
	{CS35L36_ASP_TX3_SEL,			0x00000028},
	{CS35L36_ASP_TX4_SEL,			0x00000029},
	{CS35L36_ASP_TX5_SEL,			0x00000020},
	{CS35L36_ASP_TX6_SEL,			0x00000000},
	{CS35L36_SWIRE_P1_TX1_SEL,		0x00000018},
	{CS35L36_SWIRE_P1_TX2_SEL,		0x00000019},
	{CS35L36_SWIRE_P2_TX1_SEL,		0x00000028},
	{CS35L36_SWIRE_P2_TX2_SEL,		0x00000029},
	{CS35L36_SWIRE_P2_TX3_SEL,		0x00000020},
	{CS35L36_SWIRE_DP1_FIFO_CFG,		0x0000001B},
	{CS35L36_SWIRE_DP2_FIFO_CFG,		0x0000001B},
	{CS35L36_SWIRE_DP3_FIFO_CFG,		0x0000001B},
	{CS35L36_SWIRE_PCM_RX_DATA,		0x00000000},
	{CS35L36_SWIRE_FS_SEL,			0x00000001},
	{CS35L36_AMP_DIG_VOL_CTRL,		0x00008000},
	{CS35L36_VPBR_CFG,			0x02AA1905},
	{CS35L36_VBBR_CFG,			0x02AA1905},
	{CS35L36_VPBR_STATUS,			0x00000000},
	{CS35L36_VBBR_STATUS,			0x00000000},
	{CS35L36_OVERTEMP_CFG,			0x00000001},
	{CS35L36_AMP_ERR_VOL,			0x00000000},
	{CS35L36_CLASSH_CFG,			0x000B0405},
	{CS35L36_CLASSH_FET_DRV_CFG,		0x00000111},
	{CS35L36_NG_CFG,			0x0000000B},
	{CS35L36_AMP_GAIN_CTRL,			0x00000273},
	{CS35L36_PWM_MOD_IO_CTRL,		0x00000000},
	{CS35L36_PWM_MOD_STATUS,		0x00000000},
	{CS35L36_DAC_MSM_CFG,			0x00000000},
	{CS35L36_AMP_SLOPE_CTRL,		0x00000B00},
	{CS35L36_AMP_PDM_VOLUME,		0x00000000},
	{CS35L36_AMP_PDM_RATE_CTRL,		0x00000000},
	{CS35L36_PDM_CH_SEL,			0x00000000},
	{CS35L36_AMP_NG_CTRL,			0x0000212F},
	{CS35L36_PDM_HIGHFILT_CTRL,		0x00000000},
	{CS35L36_PAC_INT0_CTRL,			0x00000001},
	{CS35L36_PAC_INT1_CTRL,			0x00000001},
	{CS35L36_PAC_INT2_CTRL,			0x00000001},
	{CS35L36_PAC_INT3_CTRL,			0x00000001},
	{CS35L36_PAC_INT4_CTRL,			0x00000001},
	{CS35L36_PAC_INT5_CTRL,			0x00000001},
	{CS35L36_PAC_INT6_CTRL,			0x00000001},
	{CS35L36_PAC_INT7_CTRL,			0x00000001},
};

bool cs35l36_readable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case CS35L36_SW_RESET:
	case CS35L36_SW_REV:
	case CS35L36_HW_REV:
	case CS35L36_TESTKEY_CTRL:
	case CS35L36_USERKEY_CTL:
	case CS35L36_OTP_MEM30:
	case CS35L36_OTP_CTRL1:
	case CS35L36_OTP_CTRL2:
	case CS35L36_OTP_CTRL3:
	case CS35L36_OTP_CTRL4:
	case CS35L36_OTP_CTRL5:
	case CS35L36_PAC_CTL1:
	case CS35L36_PAC_CTL2:
	case CS35L36_PAC_CTL3:
	case CS35L36_DEVICE_ID:
	case CS35L36_FAB_ID:
	case CS35L36_REV_ID:
	case CS35L36_PWR_CTRL1:
	case CS35L36_PWR_CTRL2:
	case CS35L36_PWR_CTRL3:
	case CS35L36_CTRL_OVRRIDE:
	case CS35L36_AMP_OUT_MUTE:
	case CS35L36_OTP_TRIM_STATUS:
	case CS35L36_DISCH_FILT:
	case CS35L36_PROTECT_REL_ERR:
	case CS35L36_PAD_INTERFACE:
	case CS35L36_PLL_CLK_CTRL:
	case CS35L36_GLOBAL_CLK_CTRL:
	case CS35L36_ADC_CLK_CTRL:
	case CS35L36_SWIRE_CLK_CTRL:
	case CS35L36_SP_SCLK_CLK_CTRL:
	case CS35L36_MDSYNC_EN:
	case CS35L36_MDSYNC_TX_ID:
	case CS35L36_MDSYNC_PWR_CTRL:
	case CS35L36_MDSYNC_DATA_TX:
	case CS35L36_MDSYNC_TX_STATUS:
	case CS35L36_MDSYNC_RX_STATUS:
	case CS35L36_MDSYNC_ERR_STATUS:
	case CS35L36_BSTCVRT_VCTRL1:
	case CS35L36_BSTCVRT_VCTRL2:
	case CS35L36_BSTCVRT_PEAK_CUR:
	case CS35L36_BSTCVRT_SFT_RAMP:
	case CS35L36_BSTCVRT_COEFF:
	case CS35L36_BSTCVRT_SLOPE_LBST:
	case CS35L36_BSTCVRT_SW_FREQ:
	case CS35L36_BSTCVRT_DCM_CTRL:
	case CS35L36_BSTCVRT_DCM_MODE_FORCE:
	case CS35L36_BSTCVRT_OVERVOLT_CTRL:
	case CS35L36_BST_TST_MANUAL:
	case CS35L36_BST_ANA2_TEST:
	case CS35L36_VPI_LIMIT_MODE:
	case CS35L36_VPI_LIMIT_MINMAX:
	case CS35L36_VPI_VP_THLD:
	case CS35L36_VPI_TRACK_CTRL:
	case CS35L36_VPI_TRIG_MODE_CTRL:
	case CS35L36_VPI_TRIG_STEPS:
	case CS35L36_VI_SPKMON_FILT:
	case CS35L36_VI_SPKMON_GAIN:
	case CS35L36_VI_SPKMON_IP_SEL:
	case CS35L36_DTEMP_WARN_THLD:
	case CS35L36_DTEMP_STATUS:
	case CS35L36_VPVBST_FS_SEL:
	case CS35L36_VPVBST_VP_CTRL:
	case CS35L36_VPVBST_VBST_CTRL:
	case CS35L36_ASP_TX_PIN_CTRL:
	case CS35L36_ASP_RATE_CTRL:
	case CS35L36_ASP_FORMAT:
	case CS35L36_ASP_FRAME_CTRL:
	case CS35L36_ASP_TX1_TX2_SLOT:
	case CS35L36_ASP_TX3_TX4_SLOT:
	case CS35L36_ASP_TX5_TX6_SLOT:
	case CS35L36_ASP_TX7_TX8_SLOT:
	case CS35L36_ASP_RX1_SLOT:
	case CS35L36_ASP_RX_TX_EN:
	case CS35L36_ASP_RX1_SEL:
	case CS35L36_ASP_TX1_SEL:
	case CS35L36_ASP_TX2_SEL:
	case CS35L36_ASP_TX3_SEL:
	case CS35L36_ASP_TX4_SEL:
	case CS35L36_ASP_TX5_SEL:
	case CS35L36_ASP_TX6_SEL:
	case CS35L36_SWIRE_P1_TX1_SEL:
	case CS35L36_SWIRE_P1_TX2_SEL:
	case CS35L36_SWIRE_P2_TX1_SEL:
	case CS35L36_SWIRE_P2_TX2_SEL:
	case CS35L36_SWIRE_P2_TX3_SEL:
	case CS35L36_SWIRE_DP1_FIFO_CFG:
	case CS35L36_SWIRE_DP2_FIFO_CFG:
	case CS35L36_SWIRE_DP3_FIFO_CFG:
	case CS35L36_SWIRE_PCM_RX_DATA:
	case CS35L36_SWIRE_FS_SEL:
	case CS35L36_AMP_DIG_VOL_CTRL:
	case CS35L36_VPBR_CFG:
	case CS35L36_VBBR_CFG:
	case CS35L36_VPBR_STATUS:
	case CS35L36_VBBR_STATUS:
	case CS35L36_OVERTEMP_CFG:
	case CS35L36_AMP_ERR_VOL:
	case CS35L36_CLASSH_CFG:
	case CS35L36_CLASSH_FET_DRV_CFG:
	case CS35L36_NG_CFG:
	case CS35L36_AMP_GAIN_CTRL:
	case CS35L36_PWM_MOD_IO_CTRL:
	case CS35L36_PWM_MOD_STATUS:
	case CS35L36_DAC_MSM_CFG:
	case CS35L36_AMP_SLOPE_CTRL:
	case CS35L36_AMP_PDM_VOLUME:
	case CS35L36_AMP_PDM_RATE_CTRL:
	case CS35L36_PDM_CH_SEL:
	case CS35L36_AMP_NG_CTRL:
	case CS35L36_PDM_HIGHFILT_CTRL:
	case CS35L36_INT1_STATUS:
	case CS35L36_INT2_STATUS:
	case CS35L36_INT3_STATUS:
	case CS35L36_INT4_STATUS:
	case CS35L36_INT1_RAW_STATUS:
	case CS35L36_INT2_RAW_STATUS:
	case CS35L36_INT3_RAW_STATUS:
	case CS35L36_INT4_RAW_STATUS:
	case CS35L36_INT1_MASK:
	case CS35L36_INT2_MASK:
	case CS35L36_INT3_MASK:
	case CS35L36_INT4_MASK:
	case CS35L36_INT1_EDGE_LVL_CTRL:
	case CS35L36_INT3_EDGE_LVL_CTRL:
	case CS35L36_PAC_INT_STATUS:
	case CS35L36_PAC_INT_RAW_STATUS:
	case CS35L36_PAC_INT_FLUSH_CTRL:
	case CS35L36_PAC_INT0_CTRL:
	case CS35L36_PAC_INT1_CTRL:
	case CS35L36_PAC_INT2_CTRL:
	case CS35L36_PAC_INT3_CTRL:
	case CS35L36_PAC_INT4_CTRL:
	case CS35L36_PAC_INT5_CTRL:
	case CS35L36_PAC_INT6_CTRL:
	case CS35L36_PAC_INT7_CTRL:
	case 0x00007064:
	case 0x00007850:
	case 0x00007854:
	case 0x00007858:
	case 0x0000785C:
	case 0x00007860:
	case 0x00007864:
	case 0x00007868:
	case 0x00007848:
	case 0x00002088:
	case 0x00003014:
	case 0x00003854:
	case 0x00003008:
	case 0x00007418:
	case 0x00002D10:
	case 0x0000410C:
	case 0x00006E08:
	case 0x00006454:
		return true;
	default:
		if (reg >= CS35L36_PAC_PMEM_WORD0 &&
			reg <= CS35L36_PAC_PMEM_WORD1023)
			return true;
		else
			return false;
	}
}

bool cs35l36_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case CS35L36_SW_RESET:
	case CS35L36_SW_REV:
	case CS35L36_HW_REV:
	case CS35L36_DEVICE_ID:
	case CS35L36_FAB_ID:
	case CS35L36_REV_ID:
	case CS35L36_INT1_STATUS:
	case CS35L36_INT2_STATUS:
	case CS35L36_INT3_STATUS:
	case CS35L36_INT4_STATUS:
	case CS35L36_INT1_RAW_STATUS:
	case CS35L36_INT2_RAW_STATUS:
	case CS35L36_INT3_RAW_STATUS:
	case CS35L36_INT4_RAW_STATUS:
	case CS35L36_INT1_MASK:
	case CS35L36_INT2_MASK:
	case CS35L36_INT3_MASK:
	case CS35L36_INT4_MASK:
	case CS35L36_INT1_EDGE_LVL_CTRL:
	case CS35L36_INT3_EDGE_LVL_CTRL:
	case CS35L36_PAC_INT_STATUS:
	case CS35L36_PAC_INT_RAW_STATUS:
	case CS35L36_PAC_INT_FLUSH_CTRL:
		return true;
	default:
		if (reg >= CS35L36_PAC_PMEM_WORD0 &&
			reg <= CS35L36_PAC_PMEM_WORD1023)
			return true;
		else
			return false;
	}
}

const int cs35l36_a0_pac_patch[] = {
	0x02000802, 0x00090308, 0x02000300, 0x00000b08,
	0x02000000, 0x00001308, 0x02000000, 0x00001b08,
	0x02000000, 0x00002308, 0x02000000, 0xf9752b08,
	0x12437880, 0x43789601, 0x78770512, 0xe8f5e242,
	0x02000022, 0x00004308, 0x02000000, 0x00004b08,
	0x02000000, 0x00005308, 0x02000000, 0x00005b08,
	0x02000000, 0x00006308, 0x02000000, 0x05126b08,
	0x9e04124d, 0x12870512, 0x05122705, 0x00fb8091,
	0x02000000, 0x00008308, 0x02000000, 0x00008b08,
	0x02000000, 0x00009308, 0x02000000, 0x00009b08,

	0x02000000, 0x0000a308, 0x02000000, 0xb443ab08,
	0x12127880, 0x0f00a201, 0x1278ffff, 0x787d0112,
	0x54041212, 0x1278ffe4, 0xc3980412, 0x706c0112,
	0x22017f03, 0xe730b6e5, 0x7fb653e1, 0x7822007f,
	0x9601123a, 0x05123a78, 0x02002277, 0x01bbeb08,
	0x8a828906, 0x5022e083, 0xbb22e702, 0x22e302fe,
	0x838a8289, 0xe82293e4, 0xcca4f08f, 0x2ca4f08b,
	0xf08ee9fc, 0x8afc2ca4, 0x2ca4edf0, 0xf08eeafc,
	0xf0a8cda4, 0x2da4f08b, 0xf02538cc, 0xf08fe9fd,
	0x35cd2ca4, 0x8eebfcf0, 0xa9fea4f0, 0xf08febf0,

	0xf0c5cfa4, 0xfe39cd2e, 0xeafc3ce4, 0x35ce2da4,
	0x3ce4fdf0, 0x9feb22fc, 0x9eeaf0f5, 0x9de9f042,
	0x64ecf042, 0x8064c880, 0x22f04598, 0xf0f59feb,
	0xf0429eea, 0xf0429de9, 0xf0459ce8, 0x08fce222,
	0xe208fde2, 0xffe208fe, 0x08fbe222, 0xe208f9e2,
	0xcbe208fa, 0xf2ec22f8, 0x08f2ed08, 0xef08f2ee,
	0x83d022f2, 0x93e482d0, 0x017408f2, 0x7408f293,
	0x08f29302, 0xf2930374, 0xe2730474, 0xfae208fb,
	0x22f9e208, 0x08fbe2fa, 0x25f9e208, 0xe218f2f0,
	0x22f23aca, 0xea08f2eb, 0xf2e908f2, 0xe47f7822,

	0x75fdd8f6, 0x02026381, 0x6e000224, 0xf8a393e4,
	0x40a393e4, 0x0180f603, 0xf4df08f2, 0x93e42980,
	0x0754f8a3, 0xc3c80c24, 0x0f54c433, 0x83c82044,
	0x56f40440, 0xf6460180, 0x0b80e4df, 0x08040201,
	0x80402010, 0xe4eb0290, 0x6093017e, 0x54ffa3bc,
	0x09e5303f, 0xe4fe1f54, 0x0160a393, 0xc054cf0e,
	0xa860e025, 0x93e4b840, 0x93e4faa3, 0x93e4f8a3,
	0x82c5c8a3, 0x83c5cac8, 0xc8a3f0ca, 0xcac882c5,
	0xdfca83c5, 0x80e7dee9, 0xcdefcdbe, 0xedcceecc,
	0xe4ff7f24, 0xf5828f3c, 0x64ffe083, 0xef0a6080,

	0x05600864, 0x708864ef, 0x2d09745c, 0xfc3ce4fd,
	0xf20f78e4, 0x0f78f208, 0xe208fee2, 0x1794c3ff,
	0x500094ee, 0xe025ef45, 0x0824e025, 0x24e6f8ff,
	0x241a60fb, 0xca217005, 0xedc9caec, 0xcd017bc9,
	0x0412cdef, 0xcceacce5, 0x80cde9cd, 0xf804ef0b,
	0xe608fee6, 0x670512ff, 0x24e21078, 0xe218f201,
	0x80f20034, 0x12047fb1, 0x20220804, 0x0005085c,
	0x20000000, 0x20000291, 0x30000194, 0x30000110,
	0x3900010c, 0x4100124e, 0x41000660, 0x41000168,
	0x4300026a, 0x44000260, 0x44000248, 0x4400024c,

	0x6e000150, 0x6e001530, 0x74001e48, 0x74000218,
	0x7400031c, 0x70000236, 0x30000268, 0x39000117,
	0x4400011a, 0x44000268, 0x11810280, 0x16780000,
	0xe4960112, 0x807dfeff, 0x121e78fc, 0x12d38901,
	0x03406c01, 0x7f22017f, 0x4244ef04, 0x9a0512ff,
	0x01121678, 0xdf00127d, 0x78047fe4, 0x9804121e,
	0x78070112, 0x54041222, 0x04122578, 0x12267874,
	0x0000a201, 0x1a78e800, 0x787d0112, 0x9601122a,
	0x786cb575, 0x80041229, 0x04122d78, 0xae00128c,
	0xee2e7822, 0xf2ef08f2, 0x08f2ec08, 0x828bf2ed,

	0x82af838a, 0xfce483ae, 0x79fafbfd, 0x12c3f880,
	0x03505601, 0xae22017f, 0xef82af83, 0xeeffff24,
	0x78feff34, 0x6e041232, 0x04123578, 0x26b57574,
	0x04123078, 0x12397867, 0x2e788c04, 0x78670412,
	0x80041239, 0x22ae0012, 0xac43afc2, 0xf0ab5304,
	0xe5ab42ef, 0x8094c3ab, 0xab430540, 0x43038002,
	0x427801ab, 0x7ff2ace5, 0x4a041228, 0xabe54278,
	0x12297ff2, 0x42784a04, 0x7ff20174, 0x7d287e24,
	0x12007c00, 0x87432e00, 0x287e2202, 0x007c007d,
	0x222e0012, 0xffff24ef, 0xfeff34ee, 0xfdff34ed,

	0xfcff34ec, 0xe2960102, 0xffe208fe, 0xfce43678,
	0x960102fd, 0x18b1f5e2, 0x18b2f5e2, 0x22b3f5e2,
	0x18a1f5e2, 0x18a2f5e2, 0x22a3f5e2, 0x18a4f5e2,
	0x18a5f5e2, 0x22a6f5e2, 0x02fcfdfe, 0xffe48901,
	0x7810ab75, 0xa2011200, 0x00040000, 0x01120478,
	0x000000a2, 0x7a017b20, 0xcf477900, 0xeacecfe9,
	0xe4007cce, 0x121a78fd, 0x04789601, 0x787d0112,
	0x9601121e, 0x01120078, 0x4e03127d, 0x7f0560ef,
	0x0804120c, 0x12087822, 0x04edd401, 0x08ffe6f8,
	0xcf0b78e6, 0xf2ef08f2, 0xf80324ed, 0xf975ffe6,

	0xe20b7884, 0xe208fcf5, 0xd3effdf5, 0x11400094,
	0x75e40878, 0x011201f0, 0xee0012c4, 0x801fe8f5,
	0x120878e9, 0x7522bb01, 0xac4330ab, 0xe5427802,
	0x287ff2ac, 0x007d287e, 0x0012007c, 0x7442782e,
	0x247ff201, 0x007d287e, 0x0002007c, 0x01ac752e,
	0x78abf5e4, 0xf2ace542, 0x287e287f, 0x007c007d,
	0xe42e0012, 0x78228ef5, 0x08f2ee0d, 0xe218f2ef,
	0xe208faf5, 0xe222fbf5, 0xe208faf5, 0xe208fbf5,
	0xe208fcf5, 0x7522fdf5, 0x007e20ab, 0x0202477f,
	0xe2117869, 0x8743f204, 0x44ef2202, 0x22f9f580,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0xd2000000, 0x030802d5,
};
