/*****************************************************************************
* Copyright(c) O2Micro, 2019. All rights reserved.
*       
* O2Micro [OZ8806] Source Code Reference Design
* File:[parameter.c]
*       
* This Source Code Reference Design for O2MICRO [OZ8806] access 
* ("Reference Design") is solely for the use of PRODUCT INTEGRATION REFERENCE ONLY, 
* and contains confidential and privileged information of O2Micro International 
* Limited. O2Micro shall have no liability to any PARTY FOR THE RELIABILITY, 
* SERVICEABILITY FOR THE RESULT OF PRODUCT INTEGRATION, or results from: (i) any 
* modification or attempted modification of the Reference Design by any party, or 
* (ii) the combination, operation or use of the Reference Design with non-O2Micro 
* Reference Design. Use of the Reference Design is at user's discretion to qualify 
* the final work result.
*****************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include "parameter.h"
#include "table.h"
#include "battery_config.h"


one_latitude_data_t			cell_temp_data[TEMPERATURE_DATA_NUM] = {                                                                                                               
			{681,   115}, {766,   113}, {865,   105},
			{980,   100}, {1113,   95}, {1266,   90},
			{1451,   85}, {1668,   80}, {1924,   75},
			{2228,   70}, {2588,   65}, {3020,   60},
			{3536,   55}, {4160,   50}, {4911,   45},
			{5827,   40}, {6940,   35}, {8313,   30},
			{10000,  25}, {12090,  20}, {14690,  15},
			{17960,  10}, {22050,   5},	{27280,   0},
			{33900,  -5}, {42470, -10}, {53410, -15},
			{67770, -20},
};


config_data_t config_data = {
       	.fRsense = 			O2_CONFIG_RSENSE,			
		.temp_pull_up 		= O2_TEMP_PULL_UP_R,			
		.temp_ref_voltage = O2_TEMP_REF_VOLTAGE,
		.dbCARLSB =			5,
		.dbCurrLSB =    	781	,
		.fVoltLSB =	  		250,	

		.design_capacity =			O2_CONFIG_CAPACITY,
		.charge_cv_voltage = 		OZ8806_VOLTAGE,
		.charge_end_current =		O2_CONFIG_EOC,
		.discharge_end_voltage =	OZ8806_EOD,
		.board_offset = 			O2_CONFIG_BOARD_OFFSET,	
		.debug = 1, 
};


/*****************************************************************************
 * bmu_init_parameter:
 *		this implements a interface for customer initiating bmu parameters
 * Return: NULL
 *****************************************************************************/
void bmu_init_parameter(parameter_data_t *parameter_customer)
{
    //如果选用88106，并且prg pin拉低，这一标志可以清0.以避免i2c通讯问题。
	//如果prg pin拉高，则这个标志需要写1，否则会导致电量不更新。此标志在
	//库中，默认拉高。
#ifdef CONFIG_OZ115_SUPPORT
	check_chip_function  = 1;
#else
   check_chip_function  = 0;
#endif
	parameter_customer->config = &config_data;
	parameter_customer->ocv = ocv_data;
	parameter_customer->temperature = cell_temp_data;
	parameter_customer->ocv_data_num = OCV_DATA_NUM;
	parameter_customer->cell_temp_num = TEMPERATURE_DATA_NUM;
	parameter_customer->charge_pursue_step = 10;		
 	parameter_customer->discharge_pursue_step = 6;		
	parameter_customer->discharge_pursue_th = 10;
	parameter_customer->wait_method = 2;

	//parameter_customer->BATT_CAPACITY 	= "/data/sCaMAH.dat";
	//parameter_customer->BATT_FCC 		= "/data/fcc.dat";
	//parameter_customer->OCV_FLAG 	= "/data/ocv_flag.dat";
	//parameter_customer->BATT_OFFSET 	= "/data/offset.dat";

	parameter_customer->fix_car_init = 0;
	parameter_customer->power_on_retry_times = 0;

	//if read/write file /data/sCaMAH.dat in userspace
	//please enable "bmu_sys_rw_kernel = 0"
#ifdef USERSPACE_READ_SAVED_CAP
	parameter_customer->bmu_sys_rw_kernel = 0;
#else
	parameter_customer->bmu_sys_rw_kernel = 1;
#endif
}
EXPORT_SYMBOL(bmu_init_parameter);


/*****************************************************************************
 * Description:
 *		bmu_init_gg
 * Return: NULL
 *****************************************************************************/
void bmu_init_gg(gas_gauge_t *gas_gauge)
{
	//gas_gauge->charge_table_num = CHARGE_DATA_NUM;
	gas_gauge->rc_x_num = XAxis;
	gas_gauge->rc_y_num = YAxis;
	gas_gauge->rc_z_num = ZAxis;

	gas_gauge->discharge_current_th = DISCH_CURRENT_TH;
	gas_gauge->fcc_data = config_data.design_capacity;

	pr_err("AAAA battery_id is %s\n", battery_id[0]);

	gas_gauge->ri = 18;
	gas_gauge->line_impedance = 10;
	gas_gauge->power_on_100_vol = O2_OCV_100_VOLTAGE;

#if 0
	//add this for oinom
	gas_gauge->lower_capacity_reserve = 5;
	gas_gauge->lower_capacity_soc_start =30;


	//gas_gauge->percent_10_reserve = 66;
#endif
}
EXPORT_SYMBOL(bmu_init_gg);


#ifdef OZ8806_API

#define BMU_INIT(x)  \
void bmu_init_##x(uint8_t *dest, uint32_t bytes)\
{\
	memcpy(dest, (uint8_t *)x, bytes);\
}\
EXPORT_SYMBOL(bmu_init_##x);\

//BMU_INIT(charge_data)
BMU_INIT(XAxisElement)
BMU_INIT(YAxisElement)
BMU_INIT(ZAxisElement)
BMU_INIT(RCtable)

#endif

void bmu_init_table(int **x, int **y, int **z, int **rc)
{
	*x = (int *)((uint8_t *)XAxisElement);
	*y = (int *)((uint8_t *)YAxisElement);
	*z = (int *)((uint8_t *)ZAxisElement);
	*rc = (int *)((uint8_t *)RCtable);
}
EXPORT_SYMBOL(bmu_init_table);

const char * get_table_version(void)
{
	return table_version;
}
EXPORT_SYMBOL(get_table_version);

