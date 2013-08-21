/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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

#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/irqs-7xxx.h>
#include "devices-msm7x2xa.h"
#include "board-msm7627a.h"
#include <linux/hardware_self_adapt.h>
/*< DTS2012020400396 zhangyu 20120206 begin */
/*< DTS2011121001804 yuguangcai 20111227 begin */
/*< DTS2012012901317 yuguangcai 20120131 begin */
#ifdef CONFIG_HUAWEI_CAMERA
/* < DTS2012031403324 zhouqiwei 20120314 begin */
#define S5K5CA_IS_NOT_ON 0 
/*we use the variable to sign whether s5k5ca is on*/
static int s5k5ca_is_on = S5K5CA_IS_NOT_ON;
/* DTS2012031403324 zhouqiwei 20120314 end > */
#endif
/* DTS2012012901317 yuguangcai 20120131 end > */
/* DTS2011121001804 yuguangcai 20111227 end > */
/* DTS2012020400396 zhangyu 20120206 end > */
/* <DTS2012041003722 sibingsong 20120410 begin */
/*< DTS2012021702010 yuguangcai 20120217 begin */
#ifdef CONFIG_HUAWEI_CAMERA
char  back_camera_name[128];
char  front_camera_name[128];
#endif
/* DTS2012021702010 yuguangcai 20120217 end > */
/* DTS2012041003722 sibingsong 20120410 end> */
#ifdef CONFIG_MSM_CAMERA_V4L2
static uint32_t camera_off_gpio_table[] = {
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static uint32_t camera_on_gpio_table[] = {
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = GPIO_CAM_GP_LED_EN1,
	._fsrc.ext_driver_src.led_flash_en = GPIO_CAM_GP_LED_EN2,
};
#endif

static struct regulator_bulk_data regs_camera[] = {
	{ .supply = "msme1", .min_uV = 1800000, .max_uV = 1800000 },
	{ .supply = "gp2",   .min_uV = 2850000, .max_uV = 2850000 },
	{ .supply = "usb2",  .min_uV = 1800000, .max_uV = 1800000 },
};

static void msm_camera_vreg_config(int vreg_en)
{
	int rc = vreg_en ?
		regulator_bulk_enable(ARRAY_SIZE(regs_camera), regs_camera) :
		regulator_bulk_disable(ARRAY_SIZE(regs_camera), regs_camera);

	if (rc)
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, vreg_en ? "en" : "dis", rc);
}

static int config_gpio_table(uint32_t *table, int len)
{
	int rc = 0, i = 0;

	for (i = 0; i < len; i++) {
		rc = gpio_tlmm_config(table[i], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s not able to get gpio\n", __func__);
			for (i--; i >= 0; i--)
				gpio_tlmm_config(camera_off_gpio_table[i],
							GPIO_CFG_ENABLE);
			break;
		}
	}
	return rc;
}

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data;
/* TODO: static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data; */
static int config_camera_on_gpios_rear(void)
{
	int rc = 0;

	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa())
		msm_camera_vreg_config(1);

	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_rear(void)
{
	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa())
		msm_camera_vreg_config(0);

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}

static int config_camera_on_gpios_front(void)
{
	int rc = 0;

	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa())
		msm_camera_vreg_config(1);

	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
			"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_front(void)
{
	if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa())
		msm_camera_vreg_config(0);

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}

struct msm_camera_device_platform_data msm_camera_device_data_csi1 = {
	.camera_gpio_on  = config_camera_on_gpios_rear,
	.camera_gpio_off = config_camera_off_gpios_rear,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 192000000,
	.csid_core = 1,
	.is_csic = 1,
};

struct msm_camera_device_platform_data msm_camera_device_data_csi0 = {
	.camera_gpio_on  = config_camera_on_gpios_front,
	.camera_gpio_off = config_camera_off_gpios_front,
	.ioclk.mclk_clk_rate = 24000000,
	.ioclk.vfe_clk_rate  = 192000000,
	.csid_core = 0,
};

#ifdef CONFIG_DW9712_ACT
static struct i2c_board_info s5k4e1_actuator_i2c_info = {
	I2C_BOARD_INFO("dw9712_act", 0x8C >> 1),
};

static struct msm_actuator_info s5k4e1_actuator_info = {
	.board_info     = &s5k4e1_actuator_i2c_info,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable     = 1,
};
#endif

#ifdef CONFIG_S5K4E1
static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k4e1 = {
	.mount_angle	= 90,
	.sensor_reset	= GPIO_CAM_GP_CAMIF_RESET_N,
	.sensor_pwd	= 85,
	.vcm_pwd	= GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable	= 1,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name    = "s5k4e1",
	.sensor_reset_enable = 1,
	.pdata                  = &msm_camera_device_data_csi1,
	.flash_data             = &flash_s5k4e1,
	.sensor_platform_info   = &sensor_board_info_s5k4e1,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
#ifdef CONFIG_DW9712_ACT
	.actuator_info = &s5k4e1_actuator_info
#endif
};
#endif

#ifdef CONFIG_MT9E013
static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9e013 = {
	.mount_angle	= 90,
	.sensor_reset	= 0,
	.sensor_pwd	= 85,
	.vcm_pwd	= 1,
	.vcm_enable	= 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
	.sensor_name    = "mt9e013",
	.sensor_reset_enable = 1,
	.pdata                  = &msm_camera_device_data_csi1,
	.flash_data             = &flash_mt9e013,
	.sensor_platform_info   = &sensor_board_info_mt9e013,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
};
#endif

#ifdef CONFIG_IMX072
static struct msm_camera_sensor_platform_info imx072_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_imx072 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_imx072_data = {
	.sensor_name    = "imx072",
	.sensor_reset_enable = 1,
	.sensor_reset   = GPIO_CAM_GP_CAMIF_RESET_N, /* TODO 106,*/
	.sensor_pwd             = 85,
	.vcm_pwd                = GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable             = 1,
	.pdata                  = &msm_camera_device_data_csi1,
	.flash_data             = &flash_imx072,
	.sensor_platform_info = &imx072_sensor_7627a_info,
	.csi_if                 = 1
};

static struct platform_device msm_camera_sensor_imx072 = {
	.name   = "msm_camera_imx072",
	.dev    = {
		.platform_data = &msm_camera_sensor_imx072_data,
	},
};
#endif

#ifdef CONFIG_WEBCAM_OV9726
static struct msm_camera_sensor_flash_data flash_ov9726 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov9726 = {
	.mount_angle	= 90,
	.sensor_reset	= GPIO_CAM_GP_CAM1MP_XCLR,
	.sensor_pwd	= 85,
	.vcm_pwd	= 1,
	.vcm_enable	= 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data = {
	.sensor_name    = "ov9726",
	.sensor_reset_enable = 0,
	.pdata                  = &msm_camera_device_data_csi0,
	.flash_data             = &flash_ov9726,
	.sensor_platform_info   = &sensor_board_info_ov9726,
	.csi_if                 = 1,
	.camera_type = FRONT_CAMERA_2D,
};
#endif

static void __init msm7x27a_init_cam(void)
{
	platform_device_register(&msm7x27a_device_csic0);
	platform_device_register(&msm7x27a_device_csic1);
	platform_device_register(&msm7x27a_device_clkctl);
	platform_device_register(&msm7x27a_device_vfe);
}

static struct i2c_board_info i2c_camera_devices[] = {
	#ifdef CONFIG_S5K4E1
	{
		I2C_BOARD_INFO("s5k4e1", 0x36),
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV9726
	{
		I2C_BOARD_INFO("ov9726", 0x10),
		.platform_data = &msm_camera_sensor_ov9726_data,
	},
	#endif
	#ifdef CONFIG_IMX072
	{
		I2C_BOARD_INFO("imx072", 0x34),
	},
	#endif
	#ifdef CONFIG_MT9E013
	{
		I2C_BOARD_INFO("mt9e013", 0x6C >> 2),
		.platform_data = &msm_camera_sensor_mt9e013_data,
	},
	#endif
	{
		I2C_BOARD_INFO("sc628a", 0x6E),
	},
};
#else
static uint32_t camera_off_gpio_table[] = {
	/* < DTS2011123001368 zhouqiwei 20111219 begin */
	GPIO_CFG(8, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET For mt9v113 */
	/* DTS2011123001368 zhouqiwei 20111219 end > */	
	/*< DTS2011122201199 yuguangcai 20120326 begin */
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
	/* DTS2011122201199 yuguangcai 20120326 end > */
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	GPIO_CFG(32, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for M660 camera*/
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for U8185 camera*/
	/* DTS2012012901317 yuguangcai 20120131 end > */
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */

	/* Camera I2C config like normal gpio with value 0 when camera turn off */
	GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), 
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD*/
	/* DTS2011121001804 yuguangcai 20111227 end > */
	/* < DTS2011123001368 zhouqiwei 20111219 begin */
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for mt9v113 */
	/* DTS2011123001368 zhouqiwei 20111219 end > */
};

/*< DTS2011082302680   songxiaoming 20110823 begin */
/* add the camera reset gpio 49*/
static uint32_t camera_on_gpio_table[] = {
/* <DTS2011083104712 yuguangcai 20110908 begin */
	/* < DTS2011123001368 zhouqiwei 20111219 begin */
	GPIO_CFG(8,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET For mt9v113 */
	/* DTS2011123001368 zhouqiwei 20111219 end > */
	/*< DTS2011122201199 yuguangcai 20120326 begin */
	/*increase the driving capability of  MCLK*/
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA), /* MCLK */
	/* DTS2011122201199 yuguangcai 20120326 end > */
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	GPIO_CFG(32, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for M660 camera*/
	GPIO_CFG(37, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /*PWD for U8185 camera*/
	/* DTS2012012901317 yuguangcai 20120131 end > */
	GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* RESET */

    /* < DTS2011121307435 zhouqiwei 20111215 begin */
    GPIO_CFG(7, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* VCM_PWD */
    /* DTS2011121307435 zhouqiwei 20111215 end > */
	/* Camera I2C config to I2C when camera start */
	GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA), 
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	GPIO_CFG(119, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD */
	/* DTS2011121001804 yuguangcai 20111227 end > */
	/* < DTS2011123001368 zhouqiwei 20111219 begin */
	GPIO_CFG(120, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), /* PWD for mt9v113 */
	/* DTS2011123001368 zhouqiwei 20111219 end > */
/* DTS2011110301689 zhangyu 20111108 end > */
/* DTS2011083104712 yuguangcai 20110908 end> */
};
/* DTS2011082302680   songxiaoming 20110823 end > */
/* DTS2012020400396 zhangyu 20120206 end > */

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = GPIO_CAM_GP_LED_EN1,
	._fsrc.ext_driver_src.led_flash_en = GPIO_CAM_GP_LED_EN2,
};
#endif

/*< DTS2012020400396 zhangyu 20120206 begin */
static struct regulator_bulk_data regs_camera[] = {
	{ .supply = "wlan2", .min_uV = 1300000, .max_uV = 1300000 },
	{ .supply = "bt",   .min_uV = 2850000, .max_uV = 2850000 },
};
/* DTS2012020400396 zhangyu 20120206 end > */

/* < DTS2012050205820 zhouqiwei 20120502 begin */
static struct regulator_bulk_data regs_camera_WIFI_QUALCOMM[] = {
	{ .supply = "wlan2", .min_uV = 1300000, .max_uV = 1300000 },
	{ .supply = "usim2",   .min_uV = 2850000, .max_uV = 2850000 },
};

static struct regulator_bulk_data regs_camera_HW_DS[] = {
	{ .supply = "wlan2", .min_uV = 1300000, .max_uV = 1300000 },
};
/* DTS2012050205820 zhouqiwei 20120502 end > */
static void qrd1_camera_gpio_cfg(void)
{

	int rc = 0;

	rc = gpio_request(QRD_GPIO_CAM_5MP_SHDN_EN, "ov5640");
	if (rc < 0)
		pr_err("%s: gpio_request---GPIO_CAM_5MP_SHDN_EN failed!",
				__func__);


	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_5MP_SHDN_EN, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable Power Down gpio for main"
				"camera!\n", __func__);
		gpio_free(QRD_GPIO_CAM_5MP_SHDN_EN);
	}


	rc = gpio_request(QRD_GPIO_CAM_5MP_RESET, "ov5640");
	if (rc < 0) {
		pr_err("%s: gpio_request---GPIO_CAM_5MP_RESET failed!",
				__func__);
		gpio_free(QRD_GPIO_CAM_5MP_SHDN_EN);
	}


	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_5MP_RESET, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable reset gpio for main camera!\n",
				__func__);
		gpio_free(QRD_GPIO_CAM_5MP_RESET);
	}

	rc = gpio_request(QRD_GPIO_CAM_3MP_PWDN, "ov7692");
	if (rc < 0)
		pr_err("%s: gpio_request---GPIO_CAM_3MP_PWDN failed!",
				__func__);

	rc = gpio_tlmm_config(GPIO_CFG(QRD_GPIO_CAM_3MP_PWDN, 0,
				GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable Power Down gpio for front"
				"camera!\n", __func__);
		gpio_free(QRD_GPIO_CAM_3MP_PWDN);
	}

	gpio_direction_output(QRD_GPIO_CAM_5MP_SHDN_EN, 1);
	gpio_direction_output(QRD_GPIO_CAM_5MP_RESET, 1);
	gpio_direction_output(QRD_GPIO_CAM_3MP_PWDN, 1);
}

/* < DTS2012050205820 zhouqiwei 20120502 begin */
static void msm_camera_vreg_config(int vreg_en)
{
	int rc = 0;

	if (vreg_en)
	{		
		if (WIFI_QUALCOMM == get_hw_wifi_device_type())
		{
			if (HW_DS != get_hw_ds_type())
			{
				rc = regulator_bulk_enable(ARRAY_SIZE(regs_camera_WIFI_QUALCOMM), 
				regs_camera_WIFI_QUALCOMM);
			}
			else
			{
				rc = regulator_bulk_enable(ARRAY_SIZE(regs_camera_HW_DS), 
				regs_camera_HW_DS);
			}
		}
		else
		{
				rc = regulator_bulk_enable(ARRAY_SIZE(regs_camera), 
				regs_camera);
		}
	} 
	else 
	{
		if (WIFI_QUALCOMM == get_hw_wifi_device_type())
		{
			if (HW_DS != get_hw_ds_type())
			{
				rc = regulator_bulk_disable(ARRAY_SIZE(regs_camera_WIFI_QUALCOMM), 
				regs_camera_WIFI_QUALCOMM);
			}
			else
			{
				rc = regulator_bulk_disable(ARRAY_SIZE(regs_camera_HW_DS), 
				regs_camera_HW_DS);
			}
		}
		else
		{
			rc = regulator_bulk_disable(ARRAY_SIZE(regs_camera), regs_camera);
		}
	}

	if (rc)
	pr_err("%s: could not %sable regulators: %d\n",
			__func__, vreg_en ? "en" : "dis", rc);

/* DTS2012050205820 zhouqiwei 20120502 end > */
}

static int config_gpio_table(uint32_t *table, int len)
{
	int rc = 0, i = 0;

	for (i = 0; i < len; i++) {
		rc = gpio_tlmm_config(table[i], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s not able to get gpio\n", __func__);
			for (i--; i >= 0; i--)
				gpio_tlmm_config(camera_off_gpio_table[i],
							GPIO_CFG_ENABLE);
			break;
		}
	}
	return rc;
}

/*< DTS2012020400396 zhangyu 20120206 begin */
/*< DTS2011082302680   songxiaoming 20110823 begin */
/* <DTS2011083104712 yuguangcai 20110908 begin */
#ifdef CONFIG_HUAWEI_CAMERA
/*< DTS2011121001804 yuguangcai 20111227 begin */
/*< DTS2012012901317 yuguangcai 20120131 begin */
/* < DTS2012031403324 zhouqiwei 20120314 begin */
/* set and get the value of s5k5ca_is_on*/
static void set_s5k5ca_is_on(int s5k5ca_probe_success)
{
	s5k5ca_is_on = s5k5ca_probe_success ;
}

static int get_s5k5ca_is_on(void)
{
	return  s5k5ca_is_on;
}
/* DTS2012031403324 zhouqiwei 20120314 end > */
#endif
/* DTS2012012901317 yuguangcai 20120131 end > */
/* DTS2011121001804 yuguangcai 20111227 end > */
static int config_camera_on_gpios_rear(void)
{
	int rc = 0;
/*< DTS2011121001804 yuguangcai 20111227 begin */
/*< DTS2012012901317 yuguangcai 20120131 begin */
/*delete some lines for power enable*/
/* DTS2012012901317 yuguangcai 20120131 end > */
/* DTS2011121001804 yuguangcai 20111227 end > */
	
	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
		"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_rear(void)
{
/*< DTS2011121001804 yuguangcai 20111227 begin */
/*< DTS2012012901317 yuguangcai 20120131 begin */
/*delete some lines for power disable*/
/* DTS2012012901317 yuguangcai 20120131 end > */
/* DTS2011121001804 yuguangcai 20111227 end > */

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}

static int config_camera_on_gpios_front(void)
{
	int rc = 0;

	/*< DTS2012012901317 yuguangcai 20120131 begin */
	/*delete one line for power enable*/
	/* DTS2012012901317 yuguangcai 20120131 end > */

	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("%s: CAMSENSOR gpio table request"
			"failed\n", __func__);
		return rc;
	}

	return rc;
}

static void config_camera_off_gpios_front(void)
{
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	/*delete one line for power disable*/
	/* DTS2012012901317 yuguangcai 20120131 end > */

	config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));
}
/* DTS2011083104712 yuguangcai 20110908 end> */
/* DTS2011082302680   songxiaoming 20110823 end > */

struct msm_camera_device_platform_data msm_camera_device_data_rear = {
	.camera_gpio_on		= config_camera_on_gpios_rear,
	.camera_gpio_off	= config_camera_off_gpios_rear,
	.ioext.csiphy		= 0xA1000000,
	.ioext.csisz		= 0x00100000,
	.ioext.csiirq		= INT_CSI_IRQ_1,
	.ioclk.mclk_clk_rate	= 24000000,
	.ioclk.vfe_clk_rate	= 192000000,
	.ioext.appphy		= MSM_CLK_CTL_PHYS,
	.ioext.appsz  = MSM_CLK_CTL_SIZE,
/*< DTS2011111602756 yuguangcai 20111206 begin */
#ifdef CONFIG_HUAWEI_CAMERA 	
	.get_board_support_flash = board_support_flash,
#endif
/* DTS2011111602756 yuguangcai 20111206 end > */
};
/* DTS2012020400396 zhangyu 20120206 end > */

struct msm_camera_device_platform_data msm_camera_device_data_front = {
	.camera_gpio_on		= config_camera_on_gpios_front,
	.camera_gpio_off	= config_camera_off_gpios_front,
	.ioext.csiphy		= 0xA0F00000,
	.ioext.csisz		= 0x00100000,
	.ioext.csiirq		= INT_CSI_IRQ_0,
	.ioclk.mclk_clk_rate	= 24000000,
	.ioclk.vfe_clk_rate	= 192000000,
	.ioext.appphy		= MSM_CLK_CTL_PHYS,
	.ioext.appsz		= MSM_CLK_CTL_SIZE,
};

/*< DTS2012020400396 zhangyu 20120206 begin */
/* < DTS2011091701690  zhangyu 20110917 begin */
#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
static struct msm_camera_sensor_platform_info s5k4e1_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
	/*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    = (char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset_enable = 1,
	.sensor_reset   = 49,
	.sensor_pwd             = 119,
	.vcm_pwd                = 7,
	.vcm_enable             = 1,
	.pdata                  = &msm_camera_device_data_rear,
	.flash_data             = &flash_s5k4e1,
	.sensor_platform_info   = &s5k4e1_sensor_7627a_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	.csi_if                 = 1,
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	/*back camera is not slave_sensor, below as the same*/
	.slave_sensor           = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};

static struct platform_device msm_camera_sensor_s5k4e1 = {
	.name   = "msm_camera_s5k4e1",
	.dev    = {
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
};
#endif
/* DTS2011091701690 zhangyu 20110917 end > */

#ifdef CONFIG_IMX072
static struct msm_camera_sensor_platform_info imx072_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_imx072 = {
	.flash_type	     = MSM_CAMERA_FLASH_LED,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_imx072_data = {
	.sensor_name		= "imx072",
	.sensor_reset_enable	= 1,
	.sensor_reset		= GPIO_CAM_GP_CAMIF_RESET_N, /* TODO 106,*/
	.sensor_pwd	     = 85,
	.vcm_pwd		= GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable	     = 1,
	.pdata			= &msm_camera_device_data_rear,
	.flash_data	     = &flash_imx072,
	.sensor_platform_info	= &imx072_sensor_7627a_info,
	.csi_if			= 1
};

static struct platform_device msm_camera_sensor_imx072 = {
	.name   = "msm_camera_imx072",
	.dev    = {
		.platform_data = &msm_camera_sensor_imx072_data,
	},
};
#endif

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data;
#ifdef CONFIG_WEBCAM_OV9726
static struct msm_camera_sensor_platform_info ov9726_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_ov9726 = {
	.flash_type	     = MSM_CAMERA_FLASH_NONE,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data = {
	.sensor_name		= "ov9726",
	.sensor_reset_enable	= 0,
	.sensor_reset		= GPIO_CAM_GP_CAM1MP_XCLR,
	.sensor_pwd	     = 85,
	.vcm_pwd		= 1,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_front,
	.flash_data	     = &flash_ov9726,
	.sensor_platform_info   = &ov9726_sensor_7627a_info,
	.csi_if			= 1
};

static struct platform_device msm_camera_sensor_ov9726 = {
	.name   = "msm_camera_ov9726",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov9726_data,
	},
};
#else
static inline void msm_camera_vreg_init(void) { }
#endif

#ifdef CONFIG_MT9E013
static struct msm_camera_sensor_platform_info mt9e013_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

/*< DTS2011082302680   songxiaoming 20110823 begin */
/* <DTS2011083104712 yuguangcai 20110908 begin */
#ifdef CONFIG_HUAWEI_SENSOR_MT9E013
static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
	/*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    = (char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9e013,
	.sensor_platform_info   = &mt9e013_sensor_7627a_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	.csi_if         = 1,
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	.slave_sensor   = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};
#endif
/* DTS2011083104712 yuguangcai 20110908 end> */
/* DTS2011082302680   songxiaoming 20110823 end > */

static struct platform_device msm_camera_sensor_mt9e013 = {
	.name      = "msm_camera_mt9e013",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9e013_data,
	},
};
#endif
/*< DTS2011092106898   yuguangcai 20110924 begin */
#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
static struct msm_camera_sensor_platform_info mt9p017_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9p017 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9p017_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
	/*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    =(char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9p017,
	.sensor_platform_info   = &mt9p017_sensor_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	.csi_if         = 1,
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	.slave_sensor   = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};

static struct platform_device msm_camera_sensor_mt9p017 = {
	.name      = "msm_camera_mt9p017",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9p017_data,
	},
};
#endif
/* DTS2011092106898   yuguangcai 20110924 end > */

/*< DTS2011092104220   yuguangcai 20110921 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K5CA
static struct msm_camera_sensor_platform_info s5k5ca_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_s5k5ca = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k5ca_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
    /*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    = (char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_s5k5ca,
	.sensor_platform_info   = &s5k5ca_sensor_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	/* < DTS2012031403324 zhouqiwei 20120314 begin */
	.set_s5k5ca_is_on = set_s5k5ca_is_on,
	/* DTS2012031403324 zhouqiwei 20120314 end > */
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	.slave_sensor   = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};

static struct platform_device msm_camera_sensor_s5k5ca = {
	.name      = "msm_camera_s5k5ca",
	.dev       = {
		.platform_data = &msm_camera_sensor_s5k5ca_data,
	},
};
#endif
/* DTS2011092104220   yuguangcai 20110921 end > */
/*< DTS2011100802771   songxiaoming 20111009 begin */ 
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9T113
static struct msm_camera_sensor_platform_info mt9t113_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9t113 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9t113_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
	/*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    =(char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9t113,
	.sensor_platform_info   = &mt9t113_sensor_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	/* < DTS2012031403324 zhouqiwei 20120314 begin */
	/*delete one line*/
	/* DTS2012031403324 zhouqiwei 20120314 end > */
	.csi_if         = 1,
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	.slave_sensor   = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};

static struct platform_device msm_camera_sensor_mt9t113 = {
	.name      = "msm_camera_mt9t113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9t113_data,
	},
};
#endif
/* DTS2011100802771   songxiaoming 20111009 end > */
/*< DTS2011112401319 yuguangcai 20111124 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9D113
static struct msm_camera_sensor_platform_info mt9d113_sensor_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_mt9d113 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9d113_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
    /*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    = (char*)back_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 49,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 119,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_rear,
	.flash_data     = &flash_mt9d113,
	.sensor_platform_info   = &mt9d113_sensor_info,
	/*< DTS2011121001804 yuguangcai 20111227 begin */
	.csi_if         = 1,
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	.slave_sensor   = 0
	/* DTS2011121001804 yuguangcai 20111227 end > */
};

static struct platform_device msm_camera_sensor_mt9d113 = {
	.name      = "msm_camera_mt9d113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9d113_data,
	},
};
#endif
/* DTS2011112401319 yuguangcai 20111124 end > */
/* < DTS2011123001368 zhouqiwei 20111219 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
/* <DTS2012031303643 sibingsong 20120321 begin */
static struct msm_camera_sensor_platform_info mt9v113_sensor_info = {
	.mount_angle = 270
};
/* DTS2012031303643 sibingsong 20120321 end> */

static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type =MSM_CAMERA_FLASH_NONE,
	.flash_src  = &msm_flash_src
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
    /* <DTS2012041003722 sibingsong 20120410 begin */
    /*< DTS2012021702010 yuguangcai 20120217 begin */
	.sensor_name    =(char*)front_camera_name,
	/* DTS2012021702010 yuguangcai 20120217 end > */
    /* DTS2012041003722 sibingsong 20120410 end> */
	.sensor_reset   = 8,
	.sensor_reset_enable = 1,
	.sensor_pwd     = 120,
	.vcm_pwd        = 7,
	.vcm_enable     = 1,
	.pdata          = &msm_camera_device_data_front,
	.flash_data     = &flash_mt9v113,
	.sensor_platform_info   = &mt9v113_sensor_info,
	.csi_if         = 1,
	/*< DTS2012012901317 yuguangcai 20120131 begin */
	.vreg_enable_func = msm_camera_vreg_config,
	.vreg_disable_func = msm_camera_vreg_config,
	/* DTS2012012901317 yuguangcai 20120131 end > */
	/* < DTS2012031403324 zhouqiwei 20120314 begin */
	.get_s5k5ca_is_on = get_s5k5ca_is_on,
	/* DTS2012031403324 zhouqiwei 20120314 end > */
	.slave_sensor   = 1
};

static struct platform_device msm_camera_sensor_mt9v113 = {
	.name      = "msm_camera_mt9v113",
	.dev       = {
		.platform_data = &msm_camera_sensor_mt9v113_data,
	},
};
#endif
/* DTS2011123001368 zhouqiwei 20111219 end > */
/* <DTS2011112400871 sunwenyong 20111124 begin */
/* driver for hw device detect */
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
static struct platform_device huawei_device_detect = {
	.name = "hw-dev-detect",
	.id   =-1,
};
#endif

#ifdef CONFIG_OV5640
static struct msm_camera_sensor_platform_info ov5640_sensor_info = {
	.mount_angle    = 90
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5640 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED,
	._fsrc.led_src.led_name = "flashlight",
	._fsrc.led_src.led_name_len = 10,
};

static struct msm_camera_sensor_flash_data flash_ov5640 = {
	.flash_type     = MSM_CAMERA_FLASH_LED,
	.flash_src      = &msm_flash_src_ov5640,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5640_data = {
	.sensor_name	    = "ov5640",
	.sensor_reset_enable    = 1,
	.sensor_reset	   = QRD_GPIO_CAM_5MP_RESET,
	.sensor_pwd	     = QRD_GPIO_CAM_5MP_SHDN_EN,
	.vcm_pwd		= 0,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_rear,
	.flash_data	     = &flash_ov5640,
	.sensor_platform_info   = &ov5640_sensor_info,
	.csi_if		 = 1,
};

static struct platform_device msm_camera_sensor_ov5640 = {
	.name   = "msm_camera_ov5640",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov5640_data,
	},
};
#endif

#ifdef CONFIG_WEBCAM_OV7692_QRD
static struct msm_camera_sensor_platform_info ov7692_sensor_7627a_info = {
	.mount_angle = 90
};

static struct msm_camera_sensor_flash_data flash_ov7692 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov7692_data = {
	.sensor_name	    = "ov7692",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = 0,
	.sensor_pwd	     = QRD_GPIO_CAM_3MP_PWDN,
	.vcm_pwd		= 0,
	.vcm_enable	     = 0,
	.pdata			= &msm_camera_device_data_front,
	.flash_data	     = &flash_ov7692,
	.sensor_platform_info   = &ov7692_sensor_7627a_info,
	.csi_if		 = 1,
};

static struct platform_device msm_camera_sensor_ov7692 = {
	.name   = "msm_camera_ov7692",
	.dev    = {
		.platform_data = &msm_camera_sensor_ov7692_data,
	},
};
#endif

static struct i2c_board_info i2c_camera_devices[] = {
	/* < DTS2011091701690  zhangyu 20110917 begin */
	#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
	{
		I2C_BOARD_INFO("s5k4e1", 0x6E >> 1),
	},
	{
		I2C_BOARD_INFO("s5k4e1_af", 0x18 >> 1),
	},
	#endif
	/* DTS2011091701690 zhangyu 20110917 end > */
	#ifdef CONFIG_WEBCAM_OV9726
	{
		I2C_BOARD_INFO("ov9726", 0x10),
	},
	#endif
	#ifdef CONFIG_IMX072
	{
		I2C_BOARD_INFO("imx072", 0x34),
	},
	#endif
	#ifdef CONFIG_MT9E013
	{
		I2C_BOARD_INFO("mt9e013", 0x6C >> 2),
	},
	#endif
	{
		I2C_BOARD_INFO("sc628a", 0x6E),
	},
	/*< DTS2011092106898   yuguangcai 20110924 begin */
	#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
	{
		I2C_BOARD_INFO("mt9p017", 0x6C),
	},
	#endif
	/* DTS2011092106898   yuguangcai 20110924 end > */
	/*< DTS2011092104220   yuguangcai 20110921 begin */
	#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K5CA
	{
		I2C_BOARD_INFO("s5k5ca", 0x2D),
	},
	#endif
	/* DTS2011092104220   yuguangcai 20110921 end > */
    /*< DTS2011100802771   songxiaoming 20111009 begin */
	/*< DTS2011101402197   songxiaoming 20111014 begin */
	#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9T113
	{
		/* mt9t113 real i2c address is 0x3C*/
		I2C_BOARD_INFO("mt9t113", 0x3C),
	},
	#endif
	/* DTS2011101402197   songxiaoming 20111014 end > */
    /* DTS2011100802771   songxiaoming 20111009 end > */
	/*< DTS2011112401319 yuguangcai 20111124 begin */
	#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9D113
	{
		I2C_BOARD_INFO("mt9d113", 0x3C >> 1),
	},
	#endif
	/* DTS2011112401319 yuguangcai 20111124 end > */
/* < DTS2011123001368 zhouqiwei 20111219 begin */
	#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
	{
		I2C_BOARD_INFO("mt9v113", 0x3D),
	},
	#endif
/* DTS2011123001368 zhouqiwei 20111219 end > */
};
/* DTS2012020400396 zhangyu 20120206 end > */

static struct i2c_board_info i2c_camera_devices_qrd[] = {
	#ifdef CONFIG_OV5640
	{
		I2C_BOARD_INFO("ov5640", 0x78 >> 1),
	},
	#endif
	#ifdef CONFIG_WEBCAM_OV7692_QRD
	{
		I2C_BOARD_INFO("ov7692", 0x78),
	},
	#endif
};

static struct platform_device *camera_devices_msm[] __initdata = {
/* <DTS2012032603420 sibingsong 20120326 begin */
#ifdef CONFIG_HUAWEI_SENSOR_S5K4E1
/* DTS2012032603420 sibingsong 20120326 end> */
	&msm_camera_sensor_s5k4e1,
#endif
#ifdef CONFIG_IMX072
	&msm_camera_sensor_imx072,
#endif
#ifdef CONFIG_WEBCAM_OV9726
	&msm_camera_sensor_ov9726,
#endif
#ifdef CONFIG_MT9E013
	&msm_camera_sensor_mt9e013,
#endif
/*< DTS2011092106898   yuguangcai 20110924 begin */
#ifdef CONFIG_HUAWEI_SENSOR_MT9P017
	&msm_camera_sensor_mt9p017,
#endif
/* DTS2011092106898   yuguangcai 20110924 end > */
/*< DTS2011092104220   yuguangcai 20110921 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_S5K5CA
	&msm_camera_sensor_s5k5ca,
#endif
/*< DTS2011100802771   songxiaoming 20111009 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9T113
	&msm_camera_sensor_mt9t113,
#endif
/* DTS2011100802771   songxiaoming 20111009 end > */
/*< DTS2011112401319 yuguangcai 20111124 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9D113
	&msm_camera_sensor_mt9d113,
#endif
/* DTS2011112401319 yuguangcai 20111124 end > */

/* DTS2011092104220   yuguangcai 20110921 end > */
/* < DTS2011123001368 zhouqiwei 20111219 begin */
#ifdef CONFIG_HUAWEI_CAMERA_SENSOR_MT9V113
    &msm_camera_sensor_mt9v113,
#endif
/* DTS2011123001368 zhouqiwei 20111219 end > */
};

static struct platform_device *camera_devices_qrd[] __initdata = {
#ifdef CONFIG_OV5640
	&msm_camera_sensor_ov5640,
#endif
#ifdef CONFIG_WEBCAM_OV7692_QRD
	&msm_camera_sensor_ov7692,
#endif
};
#endif

enum {
	SX150X_CAM,
};
/* <DTS2012041003722 sibingsong 20120410 begin */
/*< DTS2012021000399 yuguangcai 20120211 begin */
#ifdef CONFIG_HUAWEI_CAMERA
static void camera_sensor_pwd_config(void)
{
	/*distinguish the camera pwd gpio by lcd_interface_type*/
	if(LCD_IS_RGB == get_hw_lcd_interface_type())
	{
		int gpio_pwd = 37;
		/*camera pwd gpio for M660 is 32*/
		if(machine_is_msm7x27a_M660())
		{
			gpio_pwd = 32;
		}
		pr_err("camera sensor pwd gpio is %d\n",gpio_pwd);
		
		/*config the camera pwd gpio*/
		//msm_camera_sensor_mt9e013_data.sensor_pwd = gpio_pwd;
		msm_camera_sensor_mt9p017_data.sensor_pwd = gpio_pwd;
		msm_camera_sensor_s5k4e1_data.sensor_pwd = gpio_pwd;
		msm_camera_sensor_s5k5ca_data.sensor_pwd = gpio_pwd;
		msm_camera_sensor_mt9t113_data.sensor_pwd = gpio_pwd;
		/* <DTS2012041003722 sibingsong 20120410 begin */
		/* delete a line */
		/* DTS2012041003722 sibingsong 20120410 end> */
	}
}
#endif
/* DTS2012021000399 yuguangcai 20120211 end > */
/* DTS2012041003722 sibingsong 20120410 end> */
static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CAM]    = {
		.gpio_base	      = GPIO_CAM_EXPANDER_BASE,
		.oscio_is_gpo	   = false,
		.io_pullup_ena	  = 0,
		.io_pulldn_ena	  = 0,
		.io_open_drain_ena      = 0x23,
		.irq_summary	    = -1,
	},
};

static struct i2c_board_info cam_exp_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data  = &sx150x_data[SX150X_CAM],
	},
};

static void __init register_i2c_devices(void)
{
	i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				cam_exp_i2c_info,
				ARRAY_SIZE(cam_exp_i2c_info));
}

void __init msm7627a_camera_init(void)
{
	int rc;
/* <DTS2012041003722 sibingsong 20120410 begin */
/*< DTS2012021000399 yuguangcai 20120211 begin */
#ifdef CONFIG_HUAWEI_CAMERA
		/*before camera probe, config the camera pwd gpio*/
		camera_sensor_pwd_config();
#endif
/* DTS2012021000399 yuguangcai 20120211 end > */
/* DTS2012041003722 sibingsong 20120410 end> */
#ifndef CONFIG_MSM_CAMERA_V4L2
	if (machine_is_msm7627a_qrd1()) {
		qrd1_camera_gpio_cfg();
		platform_add_devices(camera_devices_qrd,
				ARRAY_SIZE(camera_devices_qrd));
	} else
		platform_add_devices(camera_devices_msm,
				ARRAY_SIZE(camera_devices_msm));
#endif
	if (!machine_is_msm7627a_qrd1())
		register_i2c_devices();
/* < DTS2012050205820 zhouqiwei 20120502 begin */
	if (WIFI_QUALCOMM == get_hw_wifi_device_type())
	{
		if (HW_DS != get_hw_ds_type())
		{
			rc = regulator_bulk_get(NULL, 
			ARRAY_SIZE(regs_camera_WIFI_QUALCOMM), regs_camera_WIFI_QUALCOMM);

			if (rc) {
				pr_err("%s: could not get regs_camera_WIFI_QUALCOMM: %d\n", __func__, rc);
					return;
			}

			rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_camera_WIFI_QUALCOMM), regs_camera_WIFI_QUALCOMM);

			if (rc) {
				pr_err("%s: could not set regs_camera_WIFI_QUALCOMM: %d\n", __func__, rc);
				return;
			}
		}
		else
		{
			rc = regulator_bulk_get(NULL, 
			ARRAY_SIZE(regs_camera_HW_DS), regs_camera_HW_DS);

			if (rc) {
				pr_err("%s: could not get regs_camera_HW_DSs: %d\n", __func__, rc);
				return;
			}

			rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_camera_HW_DS), regs_camera_HW_DS);

			if (rc) {
				pr_err("%s: could not set regs_camera_HW_DS: %d\n", __func__, rc);
				return;
			}
		}
	}
	else
	{
		rc = regulator_bulk_get(NULL, ARRAY_SIZE(regs_camera), regs_camera);

		if (rc) {
			pr_err("%s: could not get regulators: %d\n", __func__, rc);
			return;
		}

		rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_camera), regs_camera);

		if (rc) {
			pr_err("%s: could not set voltages: %d\n", __func__, rc);
			return;
		}
	}
/* DTS2012050205820 zhouqiwei 20120502 end > */

#if defined(CONFIG_MSM_CAMERA_V4L2)
	msm7x27a_init_cam();
#endif
#ifndef CONFIG_MSM_CAMERA_V4L2
	if (machine_is_msm7627a_qrd1())
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices_qrd,
				ARRAY_SIZE(i2c_camera_devices_qrd));
	else
#endif
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices,
				ARRAY_SIZE(i2c_camera_devices));
}
