/*****************************************************************************
* Copyright 2010 Texas Instruments Corporation, All Rights Reserved.
* TI makes NO WARRANTY as to software products, which are supplied "AS-IS"
*****************************************************************************/

/*****************************************************************************
*     This code is automatically generated from bqfs/dffs file.              *
*          DO NOT MODIFY THIS FILE DIRECTLY                                  *
*****************************************************************************/

//this file is generated from Lenovo_golden_atl_4000mha-5hom.gm.fs at Mon Jun 24 14:49:06 2019

#ifndef __ATL_BQFS_FILE__
#define __ATL_BQFS_FILE__

#include "bqfs_cmd_type.h"

const bqfs_cmd_t atl_bqfs_image[] = {
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x01, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x26, 0x04}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x02, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x02, 0x02}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x13, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 1100},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x02, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x02, 0x26, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xA5}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x02, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xA5}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x24, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x00, 0x19, 0x28, 0x63, 0x5F, 0xFF, 0x62, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x69}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x24, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x69}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x31, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x0A, 0x0F, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xDF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x31, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xDF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x40, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x64, 0x71, 0x0F, 0x9F, 0x23, 0x00, 0x00, 0x14, 0x04, 0x00, 0x09, 0x04, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x0E}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x40, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x0E}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x44, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x00, 0x32, 0x01, 0xC2, 0x30, 0x00, 0x03, 0x08, 0x98, 0x01, 0x00, 0x3C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xF9}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x44, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xF9}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x01, 0xF4, 0x00, 0x1E, 0xC8, 0x14, 0x08, 0x00, 0x3C, 0x0E, 0x10, 0x00, 0x0A, 0x46, 0x05, 0x14, 0x05, 0x0F, 0x03, 0x20, 0x7F, 0xFF, 0x00, 0xF0, 0x46, 0x50, 0x18, 0x01, 0x90, 0x00, 0x64, 0x19}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xE4}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xE4}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xDC, 0x5C, 0x60, 0x00, 0x7D, 0x00, 0x04, 0x03, 0x19, 0x25, 0x0F, 0x14, 0x0A, 0x78, 0x60, 0x28, 0x01, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x43, 0x80, 0x04, 0x01, 0x14, 0x00, 0x08, 0x0B, 0xB8, 0x01}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xDB}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xDB}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x02}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x2C, 0x0A, 0x01, 0x0A, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x64, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0xD0, 0x01, 0x03, 0x5A, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x47}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x50, 0x02}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x47}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x51, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x00, 0xA7, 0x00, 0x64, 0x00, 0xFA, 0x00, 0x3C, 0x3C, 0x01, 0xB3, 0xB3, 0x01, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x8A}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x51, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x8A}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x52, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x41, 0xB2, 0x00, 0x00, 0x00, 0x81, 0x0F, 0xA0, 0x13, 0x60, 0x0C, 0x80, 0x00, 0xC8, 0x00, 0x32, 0x00, 0x14, 0x03, 0xE8, 0x01, 0x00, 0x85, 0x00, 0x0A, 0xFF, 0xCC, 0xFF, 0xEF, 0x00, 0x02, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x99}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x52, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x99}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x53, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x09, 0xEE, 0x11, 0x38, 0xDB, 0xDD, 0xE0, 0xE1, 0xE1, 0xE1, 0xE1, 0xE3, 0xE3, 0xE4, 0xE6, 0xE6, 0xE6, 0xE9, 0xEB, 0xE8, 0xE8, 0xE5, 0xE5, 0xF0, 0xF1, 0xF5, 0xF6, 0xF7, 0xF6, 0xFB, 0xF9, 0xFD}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x2F}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x53, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x2F}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x53, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xFC, 0xF4, 0xF7, 0xF0, 0xF2, 0xEA, 0xFA, 0xFB, 0xFB, 0xDB, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x01}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x53, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x01}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x54, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xFC, 0x0C, 0x03, 0x17, 0x02, 0x16, 0xFE, 0x09, 0x0B, 0x01, 0xF9, 0x07, 0x0D, 0xF7, 0x0F, 0x11, 0x07, 0x04, 0x0B, 0x15, 0x08, 0xFE, 0x09, 0x01, 0x02, 0xF1, 0xFA, 0x0D, 0xF1, 0xF3, 0x9F, 0xA5}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x31}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x54, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x31}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x54, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xF8, 0xD6, 0x44, 0xFA, 0x11, 0xFF, 0x28, 0xAB, 0xA0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xF0}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x54, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xF0}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x55, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xFF, 0x85, 0x00, 0x19, 0xF3, 0xF5, 0xE2, 0x36, 0xFF, 0x0A, 0x02, 0x01, 0xE7, 0x01, 0xEC, 0xA3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x5F}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x55, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x5F}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6C, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xFE, 0x75, 0x00, 0x09, 0x1B, 0x1D, 0x61, 0xA3, 0x23, 0x90, 0x04, 0x0A, 0xEE, 0x0B, 0x18, 0x48, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xAE}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6C, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xAE}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x59, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x00, 0x1D, 0x00, 0x1C, 0x00, 0x1C, 0x00, 0x1F, 0x00, 0x24, 0x00, 0x15, 0x00, 0x19, 0x00, 0x14, 0x00, 0x15, 0x00, 0x15, 0x00, 0x14, 0x00, 0x17, 0x00, 0x1E, 0x00, 0x6A, 0x02, 0xEF, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x57}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x59, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x57}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6D, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x0F, 0xF6, 0x0E, 0xE5, 0x0E, 0xB0, 0x11, 0x26, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xEF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6D, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xEF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x68, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x02, 0x00, 0x00, 0x00, 0xD4, 0x86, 0x4A, 0xC6, 0xB4, 0xC2, 0x6E, 0x2B, 0x03, 0x7C, 0x01, 0x48, 0xFD, 0xA3, 0xF6, 0x75, 0x12, 0x58, 0x2D, 0xB7, 0x00, 0x00, 0x00, 0x00, 0xCB, 0xD4, 0x1A, 0x05}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xA5}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x68, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xA5}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x68, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x1C, 0x98, 0x02, 0xD3, 0xFF, 0xB9, 0x30, 0xEF, 0x05, 0x11, 0x05, 0x01, 0x00, 0x00, 0x10, 0x01, 0x00, 0x3C, 0x00, 0x50, 0x3C, 0x00, 0x64, 0x3C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xEA}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x68, 0x01}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xEA}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x69, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x00, 0x01, 0x0B, 0xD6, 0x7F, 0x73, 0x1A, 0x6D, 0x94, 0x0A, 0x4C, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x41}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x69, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x41}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6B, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0xEF, 0x05, 0x11, 0x05, 0x01, 0x00, 0x00, 0x10, 0x01, 0x00, 0x3C, 0x00, 0x50, 0x3C, 0x00, 0x64, 0x3C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x5B}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x6B, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0x5B}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x70, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x40,
		.data		= {.bytes = {0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
		.data_len	= 32,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xFF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 10},
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x3E,
		.data		= {.bytes = {0x70, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_C,
		.addr		= 0xAA,
		.reg		= 0x60,
		.data		= {.bytes = {0xFF}},
		.data_len	= 1,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x00, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_W,
		.addr		= 0xAA,
		.reg		= 0x00,
		.data		= {.bytes = {0x42, 0x00}},
		.data_len	= 2,
	},
	{
		.cmd_type	= CMD_X,
		.data		= {.delay = 2000},
	},
};
//end of const bqfs_cmd_t bqfs_image[]
#endif
