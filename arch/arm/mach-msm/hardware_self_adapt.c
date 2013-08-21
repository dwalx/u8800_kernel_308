/*< DTS2011062705805 yanzhijun 20110627 begin */
/*
 * Copyright (C) 2010 Huawei, Inc.
 * Copyright (c) 2008-2010, Huawei. All rights reserved.
 */
 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/errno.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>

#include <asm/mach-types.h>
#include "linux/hardware_self_adapt.h"

static unsigned int lcd_id = 0;
static unsigned int sub_board_id = 0;
#ifdef CONFIG_HUAWEI_POWER_DOWN_CHARGE
static unsigned int charge_flag = 0;
#endif

static unsigned int lcd_y_res = 480;

/* framebuffer size self adapter */
#ifdef CONFIG_FRAMEBUF_SELF_ADAPT
static __u32	frame_buffer_size = 0;
static __u32	frame_buffer_start = 0;	/* physical start address */
#endif

/*< DTS2011111407035 zhudongya 20111118 begin */
/* cust size self adapter */
static __u32	cust_buffer_size = 0;
static __u32	cust_buffer_start = 0;	/* physical start address */
/* DTS2011111407035 zhudongya 20111118 end >*/ 

static unsigned int recovery_boot_mode = 0;
 
#define ATAG_LCD_ID 0x4d534D73
int __init parse_tag_lcd_id(const struct tag *tags)
{
	 struct tag *t = (struct tag *)tags;
 
	 lcd_id = t->u.revision.rev;
	 printk(KERN_DEBUG "%s: lcd_id = 0x%x\n", __func__, lcd_id);
	 
	 return lcd_id;
}
 __tagtable(ATAG_LCD_ID, parse_tag_lcd_id);
 
#define ATAG_SUB_BOARD_ID 0x4d534D76
int __init parse_tag_sub_board_id(const struct tag *tags)
{
	 struct tag *t = (struct tag *)tags;

	 sub_board_id = t->u.revision.rev;
	 printk(KERN_DEBUG "%s: sub_board_id = 0x%x\n", __func__, sub_board_id);
	 
	 return sub_board_id;
}
 __tagtable(ATAG_SUB_BOARD_ID, parse_tag_sub_board_id);

#ifdef CONFIG_USB_AUTO_INSTALL
#define ATAG_BOOT_MODE_ID   0x4d534d77
 int __init parse_tag_boot_mode_id(const struct tag *tags)
 {
	 struct tag *t = (struct tag *)tags;
 
	 recovery_boot_mode = t->u.revision.rev;
	 printk(KERN_DEBUG "%s: usb_mode_id = 0x%x\n", __func__, recovery_boot_mode);
	 return recovery_boot_mode;
 }
 __tagtable(ATAG_BOOT_MODE_ID, parse_tag_boot_mode_id);
#endif  /* CONFIG_USB_AUTO_INSTALL */
 
#ifdef CONFIG_HUAWEI_POWER_DOWN_CHARGE
#define ATAG_CHARGE_FLAG  0x4d534D78
int __init parse_tag_charge_flag(const struct tag *tags)
{
    struct tag *t = (struct tag *)tags;

    charge_flag = t->u.revision.rev;
    printk(KERN_DEBUG "%s: charge_flag = 0x%x\n", __func__, charge_flag);

    return charge_flag;  
}
__tagtable(ATAG_CHARGE_FLAG, parse_tag_charge_flag);
#endif
 
/*get framebuffer address and size from atag, passed by bootloader*/
#ifdef CONFIG_FRAMEBUF_SELF_ADAPT
#define ATAG_FRAME_BUFFER_ID 0x4d534D79
int __init parse_tag_frame_buffer(const struct tag *tags)
{
	frame_buffer_size = tags->u.mem.size;
	frame_buffer_start = tags->u.mem.start;
	
    printk(KERN_DEBUG "%s: fb addr= 0x%x, size=0x%0x\n", __func__, frame_buffer_start, frame_buffer_size);
    return 0;
}
__tagtable(ATAG_FRAME_BUFFER_ID, parse_tag_frame_buffer);

#define ATAG_LCD_Y_RES_FLAG 0x4d534D7A
int __init parse_tag_lcd_y_res_flag(const struct tag *tags)
{
    struct tag *t = (struct tag *)tags;

    lcd_y_res= t->u.revision.rev;
    printk(KERN_DEBUG "%s: lcd_y_res = %d\n", __func__, lcd_y_res);

    return lcd_y_res;  
}
__tagtable(ATAG_LCD_Y_RES_FLAG, parse_tag_lcd_y_res_flag);

/*used in board-msm7x27a.c*/
void get_frame_buffer_mem_region(__u32 *start_addr, __u32 *size)
{
	*start_addr = frame_buffer_start;
	*size = frame_buffer_size;
}
#endif


/*< DTS2011111407035 zhudongya 20111118 begin */
/*get cust address and size from atag, passed by bootloader*/
#define ATAG_CUST_BUFFER_ID 0x4d534D7B
int __init parse_tag_cust_buffer(const struct tag * tags)
{
	cust_buffer_size = tags->u.mem.size;
	cust_buffer_start = tags->u.mem.start;
	
    printk(KERN_DEBUG "%s: cust addr= 0x%x, size=0x%0x\n", __func__, cust_buffer_start, cust_buffer_size);
    return 0;
}
__tagtable(ATAG_CUST_BUFFER_ID, parse_tag_cust_buffer);

/*used in board-msm7x27a.c*/
void get_cust_buffer_mem_region(__u32 *start_addr, __u32 *size)
{
	*start_addr = cust_buffer_start;
	*size = cust_buffer_size;
}
/* DTS2011111407035 zhudongya 20111118 end >*/ 

/* <DTS2011121403517 xiewen 20111214 begin */
/* <DTS2012032106050 niguodong 20120321 begin */
char *get_wifi_device_name(void)
{                                                                                                        
  hw_wifi_device_model wifi_device_model = WIFI_UNKNOW;  
  char *wifi_device_id = NULL;                       
                                                             
  wifi_device_model = get_hw_wifi_device_model();        
  printk("wifi_device_id = %d\n",wifi_device_model);    
  if(WIFI_BROADCOM_4329 == wifi_device_model)                 
  {                                                  
	wifi_device_id = "1.1";
  }
  
  else if(WIFI_BROADCOM_4330 == wifi_device_model)
  {                                                  
	wifi_device_id = "1.2";
  }
                                                 
  else if(WIFI_QUALCOMM_6005 == wifi_device_model)            
  { 
    wifi_device_id = "2.1"; 
  }                                                  
  else                                               
  {                                                  
    wifi_device_id = "UNKNOWN WIFI DEVICE";          
  }                                                  
  return wifi_device_id;                             
} 
/* DTS2012032106050 niguodong 20120321 end> */                                                    
/* DTS2011121403517 xiewen 20111214 end> */

/* < DTS2011122304248 liurennianKF62391 20111227 begin */
/* < DTS2012022702358 leiheiping 20120227 begin */
/* < DTS2012032901360 leiheiping 20120329 begin */
void get_audio_property(char *audio_property)
{
  unsigned int property = AUDIO_PROPERTY_INVALID;
  audio_property_type mic_type = MIC_NONE;
  audio_property_type fir_enable = FIR_DISABLE;
  audio_property_type fm_type = FM_BROADCOM;
  
  mic_type = get_audio_mic_type();
  fir_enable = get_audio_fir_enabled();
  fm_type =  get_audio_fm_type();

  property = fir_enable | mic_type | fm_type;

  sprintf(audio_property, "%8x", property);
}
/* DTS2012032901360 leiheiping 20120329 end > */
/* DTS2012022702358 leiheiping 20120227 end > */
/* DTS2011122304248 liurennianKF62391 20111227 end > */

unsigned int get_hw_lcd_id(void)
{
	return lcd_id;
}

hw_ver_sub_type get_hw_sub_board_id(void)
{
	return (hw_ver_sub_type)(sub_board_id&HW_VER_SUB_MASK);
}

#ifdef CONFIG_HUAWEI_POWER_DOWN_CHARGE
unsigned int get_charge_flag(void)
{
    return charge_flag;
}
#endif

lcd_type atag_get_lcd_y_res(void)
{
   return (lcd_type)lcd_y_res;
}

/* the function interface to check boot mode in kernel */
unsigned char bootimage_is_recovery(void)
{
  return recovery_boot_mode;
}
/*  FUNCTION  get_hw_lcd_interface_type
 *  DEPENDENCIES 
 *      get lcd interface type
 *      affect nfc.
 *  RETURN VALUE
 *      lcd interface type:LCD_IS_MIPI or LCD_IS_RGB
 */
hw_lcd_interface_type get_hw_lcd_interface_type(void)
{
	hw_lcd_interface_type lcd_interface_type;
	lcd_panel_type  hw_lcd_panel = LCD_NONE;
	
	hw_lcd_panel = get_lcd_panel_type();	

	if (machine_is_msm7x30_u8800())
	{
		lcd_interface_type = LCD_IS_MDDI_TYPE1;
	}	
	/* U8820 board version A is MMDI type1, so config it type1 
	 * Version B and other is MDDI type2, so config it according to LCD
	 */
	else if(machine_is_msm7x30_u8820())
	{
		if(HW_VER_SUB_VA == get_hw_sub_board_id())
		{
			lcd_interface_type = LCD_IS_MDDI_TYPE1;
		}
		else
		{
			switch(hw_lcd_panel)
			{
				case LCD_NT35582_BYD_WVGA:
				case LCD_NT35582_TRULY_WVGA:
					lcd_interface_type = LCD_IS_MDDI_TYPE1;
					break;
				case LCD_NT35510_ALPHA_SI_WVGA:
					lcd_interface_type = LCD_IS_MDDI_TYPE1;
					break;
				case LCD_NT35510_ALPHA_SI_WVGA_TYPE2:
					lcd_interface_type = LCD_IS_MDDI_TYPE2;
					break;
				default:
					lcd_interface_type = LCD_IS_MDDI_TYPE1;
					break;
			}
		}
	}
	else if (machine_is_msm7x30_u8800_51() 
		|| machine_is_msm8255_u8800_pro())
	{
		switch(hw_lcd_panel)
		{
			case LCD_NT35582_BYD_WVGA:
			case LCD_NT35582_TRULY_WVGA:
				lcd_interface_type = LCD_IS_MDDI_TYPE1;
				break;
			case LCD_NT35510_ALPHA_SI_WVGA:
				lcd_interface_type = LCD_IS_MDDI_TYPE1;
				break;
			case LCD_NT35510_ALPHA_SI_WVGA_TYPE2:
				lcd_interface_type = LCD_IS_MDDI_TYPE2;
				break;
			default:
				lcd_interface_type = LCD_IS_MDDI_TYPE1;
				break;
		}
	}
    else if (machine_is_msm8255_u8860() 
		|| machine_is_msm8255_c8860() 
		|| machine_is_msm8255_u8860lp()
        /* < DTS2012022905490 ganfan 20120301 begin */
        || machine_is_msm8255_u8860_r()
        /* DTS2012022905490 ganfan 20120301 end > */
		|| machine_is_msm8255_u8860_92()
		|| machine_is_msm8255_u8680()
		|| machine_is_msm8255_u8860_51()
		|| machine_is_msm8255_u8730())
	{
		lcd_interface_type = LCD_IS_MDDI_TYPE2;
	}

/*< DTS2012021007223 lijianzhao 20120211 begin */
	else if (machine_is_msm8255_u8667())
	{
		lcd_interface_type = LCD_IS_MDDI_TYPE1;
	}
/* DTS2012021007223 lijianzhao 20120211 end >*/
	else if(machine_is_msm7x27a_U8185()
		||machine_is_msm7x27a_M660())
	{
		lcd_interface_type = LCD_IS_RGB;
	}
	else
	{
		lcd_interface_type = LCD_IS_MIPI_CMD;
	}
	return lcd_interface_type;
}
/* < DTS2012021707865 niguodong 20120217 begin */
/* C8820VC uses PM pwm. */
hw_lcd_ctrl_bl_type get_hw_lcd_ctrl_bl_type(void)
{
    hw_lcd_ctrl_bl_type ctrl_bl_type = CTRL_BL_BY_UNKNOW;
	/*< DTS2012021602342 zhongjinrong 20120224 begin */
	/*control backlight by MSM pwm*/
	/*< DTS2012022300887 fengwei 20120224 begin */
	/* C8668D uses PM pwm. */
	if (machine_is_msm7x27a_umts() 
		|| machine_is_msm7x27a_cdma()
		|| machine_is_msm7x27a_U8815() 
		|| machine_is_msm7x27a_U8655_EMMC()
		|| machine_is_msm7x27a_U8185() 
		|| machine_is_msm7x27a_U8655()
		|| machine_is_msm7x27a_M660()
		|| machine_is_msm7x30_u8800()
		|| machine_is_msm7x30_u8820() 
		|| machine_is_msm7x30_u8800_51() 
		|| machine_is_msm8255_u8800_pro()
		|| machine_is_msm8255_u8860() 
		|| machine_is_msm8255_c8860()
		|| machine_is_msm8255_u8860_92()
		|| (machine_is_msm7x27a_C8820() && (HW_VER_SUB_VC <= get_hw_sub_board_id()))
		||machine_is_msm7x27a_U8661()
		|| machine_is_msm7x27a_C8668D() 
		)
	/* DTS2012022300887 fengwei 20120224 end >*/
		/* DTS2012021602342 zhongjinrong 20120224 end >*/
	{
		ctrl_bl_type = CTRL_BL_BY_MSM;
	}
	/*control backlight by LCD output pwm*/
/*< DTS2012021007223 lijianzhao 20120211 begin */
	else if(machine_is_msm7x27a_C8655_NAND()
		|| (machine_is_msm7x27a_C8820() && (HW_VER_SUB_VA == get_hw_sub_board_id()))
		|| machine_is_msm7x27a_C8825D()
        /* < DTS2012022905490 ganfan 20120301 begin */
        || machine_is_msm8255_u8860_r()
        /* DTS2012022905490 ganfan 20120301 end > */
		|| machine_is_msm8255_u8860lp()
		|| machine_is_msm8255_u8860_51()
		/*< DTS2012042605475 zhongjinrong 20120426 begin  */
		|| machine_is_msm8255_u8667()
     	|| machine_is_msm8255_u8680()
	 	|| machine_is_msm8255_u8730())
		/* DTS2012042605475 zhongjinrong 20120426 end >*/
/* DTS2012021007223 lijianzhao 20120211 end >*/
	{
		ctrl_bl_type = CTRL_BL_BY_LCD;
	}
	else
	{
		ctrl_bl_type = CTRL_BL_BY_LCD;
	}
    return ctrl_bl_type;
}
/* DTS2012021707865 niguodong 20120217 end > */
/*
 *brief: get lcd panel resolution
 */
lcd_type get_hw_lcd_resolution_type(void)
{
    lcd_type lcd_resolution = LCD_IS_HVGA;

/*< DTS2012020306500 lijianzhao 20120204 begin */
/* add 8x55 paltform products */
	if ( machine_is_msm7x27a_U8815() 
		|| machine_is_msm7x27a_C8820()
		|| machine_is_msm7x27a_C8825D()
		|| machine_is_msm7x30_u8800()
		|| machine_is_msm7x30_u8820() 
		|| machine_is_msm7x30_u8800_51() 
		|| machine_is_msm8255_u8800_pro()
		|| machine_is_msm8255_u8680()
		|| machine_is_msm8255_u8730())
	{
		lcd_resolution = LCD_IS_WVGA;
	}
	else if (machine_is_msm8255_u8860() 
		|| machine_is_msm8255_c8860() 
		|| machine_is_msm8255_u8860lp()
        /* < DTS2012022905490 ganfan 20120301 begin */
        || machine_is_msm8255_u8860_r()
        /* DTS2012022905490 ganfan 20120301 end > */
		|| machine_is_msm8255_u8860_92()
		|| machine_is_msm8255_u8860_51())
	{
		lcd_resolution = LCD_IS_FWVGA;
	}
/* DTS2012020306500 lijianzhao 20120204 end >*/
/*< DTS2012021602342 zhongjinrong 20120224 begin */
/*< DTS2012021007223 lijianzhao 20120211 begin */
	/*< DTS2012022300887 fengwei 20120224 begin */
	/* C8668D uses HVGA. */
	else if ( machine_is_msm7x27a_M660() 
		|| machine_is_msm7x27a_U8655()	
		|| machine_is_msm7x27a_U8655_EMMC()
		|| machine_is_msm7x27a_C8655_NAND()
		|| machine_is_msm8255_u8667()
		|| machine_is_msm7x27a_U8661()
		|| machine_is_msm7x27a_C8668D())
	/* DTS2012022300887 fengwei 20120224 end >*/
/* DTS2012021007223 lijianzhao 20120211 end >*/
/* DTS2012021602342 zhongjinrong 20120224 end >*/
	{
		lcd_resolution = LCD_IS_HVGA;
	}
	else if (machine_is_msm7x27a_U8185())
	{
		lcd_resolution = LCD_IS_QVGA;
	}
	else
	{
		lcd_resolution = LCD_IS_HVGA;
	}

    return lcd_resolution;
}

lcd_panel_type get_lcd_panel_type(void)
{
	lcd_panel_type hw_lcd_panel = LCD_NONE;

	if ( machine_is_msm7x30_u8800()
		|| machine_is_msm7x30_u8820() 
		|| machine_is_msm7x30_u8800_51() 
		|| machine_is_msm8255_u8800_pro())
	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:  
				hw_lcd_panel = LCD_NT35582_BYD_WVGA;
				break;
			case LCD_HW_ID1:  
				hw_lcd_panel = LCD_NT35582_TRULY_WVGA;
				break;
			case LCD_HW_ID2:  
				hw_lcd_panel = LCD_NT35510_ALPHA_SI_WVGA;
				break;
			case LCD_HW_ID3:  
				hw_lcd_panel = LCD_NT35510_ALPHA_SI_WVGA_TYPE2;
				break;
			default : 
				hw_lcd_panel = LCD_NT35510_ALPHA_SI_WVGA;
				break;					  
		}
	}
	else if (machine_is_msm8255_u8860() 
		|| machine_is_msm8255_c8860() 
		|| machine_is_msm8255_u8860lp()
        /* < DTS2012022905490 ganfan 20120301 begin */
        || machine_is_msm8255_u8860_r()
        /* DTS2012022905490 ganfan 20120301 end > */
		|| machine_is_msm8255_u8860_92()
		|| machine_is_msm8255_u8860_51())
	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = LCD_NT35560_TOSHIBA_FWVGA;
				break;
			default : 
				hw_lcd_panel = LCD_NT35560_TOSHIBA_FWVGA;
				break;					  
		}
	}
	else if( machine_is_msm8255_u8680()
		|| machine_is_msm8255_u8730())

	{
/*< DTS2012021007223 lijianzhao 20120211 begin */
/*< DTS2012021602342 zhongjinrong 20120224 begin */
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = MDDI_RSP61408_CHIMEI_WVGA;
				break;
			case LCD_HW_ID1:
				hw_lcd_panel = MDDI_HX8369A_TIANMA_WVGA;
				break;
			case LCD_HW_ID2:
				hw_lcd_panel = MDDI_RSP61408_BYD_WVGA;
				break;
			/*< DTS2012042605475 zhongjinrong 20120426 begin  */
			/* <DTS2012030102766 sunkai 20120301 begin */
            case LCD_HW_ID3:
                hw_lcd_panel = MDDI_RSP61408_TRULY_WVGA;
                break;
			/* DTS2012030102766 sunkai 20120301 end> */
			/* DTS2012042605475 zhongjinrong 20120426 end >*/
			default : 
				hw_lcd_panel = MDDI_RSP61408_CHIMEI_WVGA;
				break;
		}
/* DTS2012021602342 zhongjinrong 20120224 end >*/
	}
	else if( machine_is_msm8255_u8667())
	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = MDDI_HX8357C_CHIMEI_HVGA;
				break;
			/*< DTS2012042605475 zhongjinrong 20120426 begin  */
			/*< DTS2012022401352 qitongliang 20120224 begin */
			case LCD_HW_ID1:
				hw_lcd_panel = MDDI_HX8357C_TIANMA_IPS_HVGA;
				break;
			/* DTS2012022401352 qitongliang 20120224 end >*/
			/* DTS2012042605475 zhongjinrong 20120426 end >*/

			case LCD_HW_ID2:
				hw_lcd_panel = MDDI_HX8357C_CHIMEI_IPS_HVGA;
				break;
			case LCD_HW_ID3:
				hw_lcd_panel = MDDI_HX8357C_TIANMA_HVGA;
				break;
			default:
				hw_lcd_panel = MDDI_HX8357C_CHIMEI_HVGA;
				break;		
		}
	}
/* DTS2012021007223 lijianzhao 20120211 end >*/
	else if( machine_is_msm7x27a_umts()
		|| machine_is_msm7x27a_cdma())
	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = MIPI_NT35560_TOSHIBA_FWVGA;
				break;
			case LCD_HW_ID1:
				hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
				break;
			default:
				/*no mipi LCD lead to block, so default lcd RGB */
				hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
				break;
		}

	}
	else if( machine_is_msm7x27a_U8815() 
		|| machine_is_msm7x27a_C8820()
		|| machine_is_msm7x27a_C8825D() )

	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = MIPI_RSP61408_CHIMEI_WVGA;
				break;
			case LCD_HW_ID1:
				hw_lcd_panel = MIPI_HX8369A_TIANMA_WVGA;
				break;
			case LCD_HW_ID2:
				hw_lcd_panel = MIPI_RSP61408_BYD_WVGA;
				break;
			/* <DTS2012022501992 liguosheng 20120229 begin */
			case LCD_HW_ID3:
				hw_lcd_panel = MIPI_RSP61408_TRULY_WVGA;
				break;
			/* DTS2012022501992 liguosheng 20120229 end> */
			default:
				/*no mipi LCD lead to block, so default lcd RGB */
				hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
				break;
		}
	}

	/*< DTS2012022300887 fengwei 20120224 begin */
	else if( machine_is_msm7x27a_U8655() 
		|| machine_is_msm7x27a_U8655_EMMC() 
		|| machine_is_msm7x27a_C8655_NAND()
		|| machine_is_msm7x27a_U8661()
		|| machine_is_msm7x27a_C8668D())
	/* DTS2012022300887 fengwei 20120224 end >*/
	{
		switch (lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = MIPI_HX8357C_CHIMEI_HVGA;
				break;
			/* <DTS2012022501992 liguosheng 20120229 begin */
			case LCD_HW_ID1:
			    hw_lcd_panel = MIPI_HX8357C_TIANMA_IPS_HVGA;
				break;
			/* DTS2012022501992 liguosheng 20120229 end> */
			case LCD_HW_ID2:
				hw_lcd_panel = MIPI_HX8357C_CHIMEI_IPS_HVGA;
				break;
			case LCD_HW_ID3:
				hw_lcd_panel = MIPI_HX8357C_TIANMA_HVGA;
				break;
			default: 
				/*no mipi LCD lead to block, so default lcd RGB */
				hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
				break;
		}
	}
	else if( machine_is_msm7x27a_U8185())
	{
		switch(lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = LCD_HX8347D_TRULY_QVGA;
				break;
			case LCD_HW_ID2:
				hw_lcd_panel = LCD_HX8347G_TIANMA_QVGA;
				break;
			case LCD_HW_ID3:
				hw_lcd_panel = LCD_HX8347D_CHIMEI_QVGA;
				break;
			default:
				hw_lcd_panel = LCD_HX8347G_TIANMA_QVGA;
				break;
		}
	}
	else if(machine_is_msm7x27a_M660())
	{
		switch(lcd_id)
		{
			case LCD_HW_ID0:
				hw_lcd_panel = LCD_HX8357C_TIANMA_HVGA;
				break;
			case LCD_HW_ID1:
				hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
				break;
			default:
				hw_lcd_panel = LCD_HX8357C_TIANMA_HVGA;
				break;
		}
	}
	else
	{
		hw_lcd_panel = LCD_HX8357B_TIANMA_HVGA;
	}
	return hw_lcd_panel;
}
/* < DTS2012013004920 zhangmin 20120130 begin */
/*===========================================================================


FUNCTION     get_compass_gs_position

DESCRIPTION
  This function probe which G-sensor & Campass status type should be used

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
compass_gs_position_type  get_compass_gs_position(void)
{
	compass_gs_position_type compass_gs_position=COMPASS_TOP_GS_TOP;
	/* modify compass and gs position by board id */
    //move C8820\25D define from TOP to BOTTOM
	if (machine_is_msm7x27a_surf() 
	 || machine_is_msm7x27a_ffa() 
	 || machine_is_msm7x27a_umts() 
	 || machine_is_msm7x27a_cdma()
	 || machine_is_msm7x27a_U8815()) 
	{
		compass_gs_position=COMPASS_TOP_GS_TOP;
	}
	/*< DTS2012022006500 yangbo 20120220 begin */
	/*version A and version B has compass, since version C don't have compass*/
	else if(machine_is_msm7x27a_C8820() && (HW_VER_SUB_VC <= get_hw_sub_board_id()))
	{
		compass_gs_position=COMPASS_NONE_GS_BOTTOM;
	}
	/* DTS2012022006500 yangbo 20120220 end > */
	/* add U8655_EMMC, use the u8655 configuration */
	else if (machine_is_msm7x27a_U8655() 
		  || machine_is_msm7x27a_U8655_EMMC()  
		  || machine_is_msm7x27a_C8655_NAND()  
		  || machine_is_msm7x27a_M660()  
		  || machine_is_msm7x27a_U8661()    
		  || machine_is_msm7x27a_C8820()   
		  || machine_is_msm7x27a_C8825D()
		  || machine_is_msm7x30_u8800()
		  || machine_is_msm7x30_u8820() 
		  || machine_is_msm7x30_u8800_51()
		  || machine_is_msm8255_u8800_pro()
		  || machine_is_msm8255_u8860() 
		  || machine_is_msm8255_c8860() 
		  || machine_is_msm8255_u8860lp()
          /* < DTS2012022905490 ganfan 20120301 begin */
          || machine_is_msm8255_u8860_r()
          /* DTS2012022905490 ganfan 20120301 end > */
		  || machine_is_msm8255_u8860_92()            
		  || machine_is_msm8255_u8860_51())
	{
		compass_gs_position=COMPASS_BOTTOM_GS_BOTTOM;
	}
	else if (machine_is_msm7x27a_U8185())	
	{
		compass_gs_position=COMPASS_NONE_GS_TOP;
	}
	else if(machine_is_msm8255_u8680() 
	     || machine_is_msm8255_u8730())
	{
	    compass_gs_position=COMPASS_TOP_GS_TOP;
	}
	/*< DTS2012022300887 fengwei 20120224 begin */
    else if (machine_is_msm7x27a_C8668D() )	
	{
		compass_gs_position = COMPASS_BOTTOM_GS_TOP;
	}
	/* DTS2012022300887 fengwei 20120224 end >*/
	return compass_gs_position;
}
/* DTS2012013004920 zhangmin 20120130 end > */

/*===========================================================================


FUNCTION     get_lcd_align_type

DESCRIPTION
  This function probe which LCD align type should be used

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
lcd_align_type get_lcd_align_type (void)
{
    lcd_panel_type  hw_lcd_panel = LCD_NONE;
    lcd_align_type  lcd_align    = LCD_PANEL_ALIGN_LSB;
     
	hw_lcd_panel = get_lcd_panel_type();

	if ((hw_lcd_panel == LCD_ILI9481DS_TIANMA_HVGA) 
		||(hw_lcd_panel == LCD_ILI9481D_INNOLUX_HVGA))
	{
		lcd_align = LCD_PANEL_ALIGN_MSB;
	}
	else
	{
		lcd_align = LCD_PANEL_ALIGN_LSB;
	}

    return lcd_align;
}
char *get_lcd_panel_name(void)
{
	lcd_panel_type hw_lcd_panel = LCD_NONE;
	char *pname = NULL;	 

	hw_lcd_panel = get_lcd_panel_type();

	switch (hw_lcd_panel)
	{
		case LCD_S6D74A0_SAMSUNG_HVGA:
			pname = "SAMSUNG S6D74A0";
			break;

		case LCD_ILI9325_INNOLUX_QVGA:
			pname = "INNOLUX ILI9325";
			break;

		case LCD_ILI9325_BYD_QVGA:
			pname = "BYD ILI9325";
			break;

		case LCD_ILI9325_WINTEK_QVGA:
			pname = "WINTEK ILI9325";
			break;

		case LCD_SPFD5408B_KGM_QVGA:
			pname = "KGM SPFD5408B";
			break;

		case LCD_HX8357A_BYD_QVGA:
			pname = "BYD HX8357A";
			break;

		case LCD_HX8368A_SEIKO_QVGA:
			pname = "SEIKO HX8368A";
			break;

		case LCD_HX8347D_TRULY_QVGA:
			pname = "TRULY HX8347D";
			break;

		case LCD_ILI9325C_WINTEK_QVGA:
			pname = "WINTEK ILI9325C";
			break;

		case LCD_NT35582_BYD_WVGA:
			pname = "BYD NT35582";
			break;

		case LCD_NT35582_TRULY_WVGA:
			pname = "TRULY NT35582";
			break;

		case LCD_NT35560_TOSHIBA_FWVGA:
			pname = "TOSHIBA NT35560";
			break;

		case LCD_NT35510_ALPHA_SI_WVGA:
			pname = "TRULY NT35510";
			break;

		case LCD_NT35510_ALPHA_SI_WVGA_TYPE2:
			pname = "SUCCESS NT35510";
			break;
		/*< DTS2012021602342 zhongjinrong 20120224 begin */
		case MDDI_RSP61408_CHIMEI_WVGA:
			pname = "CHIMEI RSP61408";
			break;
		/*< DTS2012021007223 lijianzhao 20120211 begin */
		case MDDI_RSP61408_BYD_WVGA:
			pname = "BYD RSP61408";
			break;
		case MDDI_HX8369A_TIANMA_WVGA:
			pname = "TIANMA HX8369A";
			break;
		case MDDI_HX8357C_TIANMA_HVGA:
			pname = "TIANMA HX8357C";
			break;
			
		case MDDI_HX8357C_CHIMEI_HVGA:
			pname = "CHIMEI HX8357C";
			break;
		case MDDI_HX8357C_CHIMEI_IPS_HVGA:
			pname = "CHIMEI IPS HX8357C";
			break;
		/* DTS2012021007223 lijianzhao 20120211 end >*/
		/* DTS2012021602342 zhongjinrong 20120224 end >*/

		case LCD_HX8368A_TRULY_QVGA:
			pname = "TRULY HX8368A";
			break;

		case LCD_HX8347D_CHIMEI_QVGA:
			pname = "CHIMEI HX8347D";
			break;

		case LCD_HX8347G_TIANMA_QVGA:
			pname = "TIANMA HX8347G";
			break;

		case LCD_HX8357B_TIANMA_HVGA:
			pname = "TIANMA HX8357B";
			break;  

		case LCD_HX8357C_TIANMA_HVGA:
			pname = "TIANMA HX8357C";
			break;

		case LCD_R61529_TRULY_HVGA:
			pname = "TRULY R61529";
			break; 
			
		case LCD_ILI9481DS_TIANMA_HVGA:
			pname = "TIANMA ILI9481";
			break;

		case LCD_ILI9481D_INNOLUX_HVGA:
			pname = "INNOLUX ILI9481";
			break;

		case LCD_NT35410_CHIMEI_HVGA:
			pname = "CHIMEI NT35410";
			break;
			
		case MIPI_RSP61408_CHIMEI_WVGA:
			pname = "CHIMEI RSP61408";
			break;
			
		case MIPI_RSP61408_BYD_WVGA:
			pname = "BYD RSP61408";
			break;

		/* <DTS2012022501992 liguosheng 20120229 begin */
		case MIPI_RSP61408_TRULY_WVGA: 
			pname = "TRULY RSP61408";
			break;

		case MIPI_HX8357C_TIANMA_IPS_HVGA:
		    pname = "TIANMA IPS HX8357C";
		    break;
		/* DTS2012022501992 liguosheng 20120229 end> */
		
		case MIPI_HX8357C_CHIMEI_HVGA:
			pname = "CHIMEI HX8357C";
			break;
			
		case MIPI_HX8357C_TIANMA_HVGA:
			pname = "TIANMA HX8357C";
			break;
			
		case MIPI_HX8369A_TIANMA_WVGA:
			pname = "TIANMA HX8369A";
			break;
			
		case MIPI_HX8357C_CHIMEI_IPS_HVGA:
			pname = "CHIMEI IPS HX8357C";
			break;
		/*< DTS2012042605475 zhongjinrong 20120426 begin  */
		/*< DTS2012022401352 qitongliang 20120224 begin */
		case MDDI_HX8357C_TIANMA_IPS_HVGA:
			pname = "TIANMA IPS HX8357C";
			break;
		/* DTS2012022401352 qitongliang 20120224 end >*/
		/* <DTS2012030102766 sunkai 20120301 begin */
        case MDDI_RSP61408_TRULY_WVGA:
            pname = "TRULY RSP61408";
            break;
		/* DTS2012030102766 sunkai 20120301 end> */
		/* DTS2012042605475 zhongjinrong 20120426 end >*/

		default:
			pname = "UNKNOWN LCD";
			break;
	}

	return pname;
}

int board_surport_fingers(bool * is_surport_fingers)
{
	 int result = 0;

	 if (is_surport_fingers == NULL)
	 {
		  return -ENOMEM;
	 }
	 
	 *is_surport_fingers = false;

	 return result;
}

int board_use_tssc_touch(bool * use_touch_key)
{
	 int result = 0;

	 *use_touch_key = false;
	 return result;
}

int board_support_ofn(bool * ofn_support)
{
	 int ret = 0;

	 if(NULL == ofn_support)
	 {
		 return -EPERM;
	 }

	 *ofn_support = false;
	 return ret;
}

static bool camera_i2c_state = false;
bool camera_is_supported(void)
{
	 return camera_i2c_state;
}

void set_camera_support(bool status)
{
	 camera_i2c_state = status;
}

/*< DTS2011111602756 yuguangcai 20111206 begin */
bool board_support_flash(void)
{
	 /*only U8815 has light flash for now*/
	 if(machine_is_msm7x27a_U8815())
	 {
		 return true;
	 }
	 return false;
}
/* DTS2011111602756 yuguangcai 20111206 end > */
static bool st303_gs_state = false;
bool st303_gs_is_supported(void)
{
	 return st303_gs_state;
}

void set_st303_gs_support(bool status)
{
	 st303_gs_state = status;
}

/*
*  return: 0 ----not support RGB LED driver
* 		 1 ----support RGB LED driver
*/
bool rgb_led_is_supported(void)
{
	bool ret = false;

	return ret;
}

bool qwerty_is_supported(void)
{
	bool ret = false;
/*< DTS2011082902761 zhongjinrong 20110829 begin */
/* < DTS2011121507916 liuyuntao 20111215 begin */
	ret=( machine_is_msm7x27a_umts()||machine_is_msm7x27a_cdma() || machine_is_msm7x27a_M660());
/* DTS2011121507916 liuyuntao 20111215 end > */
/*DTS2011082902761 zhongjinrong 20110829 end> */
	return ret;
}
/* < DTS2011122701816  liujinggang 20111227 begin */
/* get sensors list by product */
/* < DTS2012052505397 zhangmin 20120531 begin */
extern int hasGyro ;
/*hasGyro is a flag to note if has gyro,c8860e has gyro but M886 doesn't*/
static int get_sensors_list(void)
{
	/* < DTS2012020902104 zhangmin 20120209 begin */
	/*add 7x30's list*/
	int sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR;
	int hasgyro = 0;
//	hasgyro = hasGyro;
	/*< DTS2012022006500 yangbo 20120220 begin */
	if(machine_is_msm8255_u8860()
	|| machine_is_msm8255_u8860_92()
	|| machine_is_msm8255_u8860_51()
	/* < DTS2012022905490 ganfan 20120301 begin */
	|| machine_is_msm8255_u8860_r()
	/* DTS2012022905490 ganfan 20120301 end > */
	|| machine_is_msm8255_u8860lp()
	||(machine_is_msm8255_c8860() && hasgyro ))
	{
		sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR + GY_SENSOR;
		printk("####This device has gyro\n");
	}
	
	/*version A and version B has compass, since version C don't have compass*/
	else if( (  machine_is_msm7x27a_C8820() && (HW_VER_SUB_VC <= get_hw_sub_board_id()) )
		||  machine_is_msm7x27a_U8661())
	{
		sensors_list = G_SENSOR + L_SENSOR + P_SENSOR;
	}
	/*< DTS2012022300887 fengwei 20120224 begin */
	else if (machine_is_msm7x27a_U8655()
	||  machine_is_msm7x27a_U8655_EMMC()
	||  machine_is_msm7x27a_C8655_NAND()
	||  machine_is_msm7x27a_M660()
	|| 	machine_is_msm7x27a_U8815()	
	|| 	machine_is_msm7x27a_C8820()	
	|| 	machine_is_msm7x27a_C8825D()
	||  machine_is_msm7x27a_C8668D()
	|| 	machine_is_msm7x30_u8800()	
	|| 	machine_is_msm7x30_u8820()	
	|| 	machine_is_msm7x30_u8800_51()
	|| 	machine_is_msm8255_u8800_pro()
	||	machine_is_msm8255_c8860()
	||	machine_is_msm8255_u8667()
	||	machine_is_msm8255_u8680()
	||	machine_is_msm8255_u8730())
	/* DTS2012022300887 fengwei 20120224 end >*/
	{
		sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR;
		printk("####This device doesn't own gyro\n");
	}
	/* move this else if to above for avoid C8820 without compass can not run in */
	/* DTS2012022006500 yangbo 20120220 end > */
	else if(machine_is_msm7x27a_U8185())
	{
		sensors_list = G_SENSOR;
	}
	else
	{
		sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR;
	}
	/* DTS2012020902104 zhangmin 20120209 end > */
	return sensors_list;
}
/* DTS2012052505397 zhangmin 20120531 end > */
char *get_sensors_list_name(void)
{
	int sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR;
	char *list_name=NULL;

	sensors_list = get_sensors_list();

	switch(sensors_list)
	{
		case G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR + GY_SENSOR:
			 list_name = "G_L_P_M_GY_SENSORS";
			 break;
			 
		case G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR:
			 list_name = "G_L_P_M_SENSORS";
			 break;
			 
		case G_SENSOR + L_SENSOR + P_SENSOR:
			 list_name = "G_L_P_SENSORS";
			 break;
			 
		case G_SENSOR:
			 list_name = "G_SENSORS";
			 break;
			 
		case G_SENSOR + M_SENSOR + GY_SENSOR:
			 list_name = "G_M_GY_SENSORS";
			 break;
			 
		case G_SENSOR + M_SENSOR:
			 list_name = "G_M_SENSORS";
			 break;
			 
		case NONE_SENSOR:
			 list_name = "NONE_SENSORS";
			 break;
			 
		default:
			 list_name = "G_L_P_M_SENSORS";
			 break;
	}

	return list_name;
	
}

/* DTS2011122701816  liujinggang 20111227 end > */
/* < DTS2011082001050  liujinggang 20110822 begin */
/*return the string by compass position*/
char *get_compass_gs_position_name(void)
{
	compass_gs_position_type compass_gs_position=COMPASS_TOP_GS_TOP;
	char *position_name=NULL;

	compass_gs_position = get_compass_gs_position();

	switch(compass_gs_position)
	{
		case COMPASS_TOP_GS_TOP:
			 position_name = "COMPASS_TOP_GS_TOP";
			 break;
			 
		case COMPASS_TOP_GS_BOTTOM:
			 position_name = "COMPASS_TOP_GS_BOTTOM";
			 break;

		case COMPASS_BOTTOM_GS_TOP:
			 position_name = "COMPASS_BOTTOM_GS_TOP";
			 break;

		case COMPASS_BOTTOM_GS_BOTTOM:
			 position_name = "COMPASS_BOTTOM_GS_BOTTOM";
			 break;
			 
		case COMPASS_NONE_GS_BOTTOM:
			 position_name = "COMPASS_NONE_GS_BOTTOM";
			 break;
		/* < DTS2011120202356  liujinggang 20111202 begin */
		/*add gs position of COMPASS_NONE_GS_TOP*/			 
		case COMPASS_NONE_GS_TOP:
			 position_name = "COMPASS_NONE_GS_TOP";
			 break;
		/* DTS2011120202356  liujinggang 20111202 end > */

		default:
			 position_name = "COMPASS_TOP_GS_TOP";
			 break;
	}

	return position_name;
	
}

/*  FUNCTION  get_hw_wifi_device_type
 *  DEPENDENCIES 
 *      get wifi device type.
 *      affect wifi and camer.
 *  RETURN VALUE
 *      wifi device type:WIFI_QUALCOMM or WIFI_BROADCOM
 */
hw_wifi_device_type get_hw_wifi_device_type(void)
{
  /* < DTS2011122105081  niguodong 20111222 begin */
  /*< DTS2012022300887 fengwei 20120224 begin */
  if (machine_is_msm7x27a_U8185() || machine_is_msm7x30_u8800()
	|| machine_is_msm7x27a_U8661() 
	|| machine_is_msm7x27a_C8668D())
  /* DTS2012022300887 fengwei 20120224 end >*/
  /* DTS2011122105081  niguodong 20111222 end > */
  {
      return WIFI_QUALCOMM;
  }
  else
  {
      return WIFI_BROADCOM;
  }
}
/* <DTS2012032106050 niguodong 20120321 begin */
/*  FUNCTION  get_hw_wifi_device_model
 *  DEPENDENCIES 
 *      get wifi device model.
 *      affect app_info.
 *  RETURN VALUE
 *      wifi device model:WIFI_QUALCOMM_6005 or WIFI_BROADCOM_4329 ro WIFI_BROADCOM_4330
 */
hw_wifi_device_model get_hw_wifi_device_model(void)
{
  /*< DTS2012042703826 lixin 20120427 begin */
  if(machine_is_msm7x27a_U8185()|| machine_is_msm7x27a_U8661()|| machine_is_msm7x27a_C8668D()
     || (machine_is_msm7x27a_U8815() && (HW_VER_SUB_VC == get_hw_sub_board_id())))
  {
      return WIFI_QUALCOMM_6005;
  }
  /* DTS2012042703826 lixin 20120427 end >*/
  else if(machine_is_msm8255_u8860() 
		|| machine_is_msm8255_c8860() 
		|| machine_is_msm8255_u8860lp()
        || machine_is_msm8255_u8860_r()
		|| machine_is_msm8255_u8860_92()
/* < DTS2012050801712 xiedayong 20120508 begin */
/* add msm8255_u8800_pro proudct,it uses BCM4329 chip */
        || machine_is_msm8255_u8860_51()
        || machine_is_msm8255_u8800_pro())
/* DTS2012050801712 xiedayong 20120508 end > */
  {
      return WIFI_BROADCOM_4329;
  }
  else
  {
      return WIFI_BROADCOM_4330;
  }
}
/* DTS2012032106050 niguodong 20120321 end> */

/* < DTS2011121402399 yangyang 20111214 begin */
/*  FUNCTION  get_hw_ds_type
 *  DEPENDENCIES 
 *      get single sim card or double sim card,
 *      affect led.
 *  RETURN VALUE
 *      single sim card:sim card type HW_NODS 
 *      double sim card:sim card type HW_DS
 */
hw_ds_type get_hw_ds_type(void)
{
    hw_ds_type ret = HW_NONES;
    /* < DTS2011122105081  niguodong 20111222 begin */
	/*< DTS2012022300887 fengwei 20120224 begin */
	/*add C8668D type*/
    if( machine_is_msm7x27a_C8820()
 	  || machine_is_msm7x27a_C8825D() 
	  || machine_is_msm7x27a_U8661()
	  || machine_is_msm7x27a_C8668D())
	/* DTS2012022300887 fengwei 20120224 end >*/
    /* DTS2011122105081  niguodong 20111222 end > */
    {
	    ret = HW_DS;
    }
    else
    {
      ret = HW_NODS;
    }
  return ret;
}
/* DTS2011121402399 yangyang 20111214 end >*/
/*  FUNCTION  get_hw_sd_trigger_type
 *  DEPENDENCIES 
 *      get sd interrupt trigger type
 *      affect sd detect.
 *  RETURN VALUE
 *      raise edge trigger : return RAISE_TRIGGER
 *      fall edge trigger : return FALL_TRIGGER
 */
hw_sd_trigger_type get_hw_sd_trigger_type(void)
{
  if(machine_is_msm7x27a_U8185())
  {
      return RAISE_TRIGGER;
  }
  else
  {
      return FALL_TRIGGER;
  }
}
/* DTS2011120607065 xiewen 20111207 end> */

/* < DTS2012022206848 yangyang 20120220 begin */ 
/*  FUNCTION  get_hw_sd_trigger_type
 *  DESCRIPTION 
 *      get the bt wakeup gpio type
 *
 *  RETURN VALUE
 *       the gpio number
 */
hw_bt_wakeup_gpio_type get_hw_bt_wakeup_gpio_type(void)
{
    hw_bt_wakeup_gpio_type bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_NONES;
    hw_ver_sub_type ver_sub_type = HW_VER_SUB_MAX;
    ver_sub_type = get_hw_sub_board_id();
	
    if (machine_is_msm7x27a_U8815())
    {
        bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
    }
    else if (machine_is_msm7x27a_U8655())
    {
        bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
    }
    else if (machine_is_msm7x27a_U8655_EMMC())
    {
        if ((ver_sub_type > HW_VER_SUB_VB && ver_sub_type < HW_VER_SUB_VE) || ver_sub_type > HW_VER_SUB_VE)
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_27;
		}
		else
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
		}
    }
    else if (machine_is_msm7x27a_C8655_NAND())
    {
        if (ver_sub_type > HW_VER_SUB_VB)
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_27;
		}
		else
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
		}
    }
    else if (machine_is_msm7x27a_M660())
    {
        if (ver_sub_type > HW_VER_SUB_VA)
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_27;
		}
		else
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
		}
    }
    else if (machine_is_msm7x27a_C8820())
    {
        if (ver_sub_type > HW_VER_SUB_VB)
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_27;
		}
		else
		{
            bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
		}
    }
    else if (machine_is_msm7x27a_C8825D())
    {
        bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
    }
	/*< DTS2012022300887 fengwei 20120224 begin */
    else if (machine_is_msm7x27a_C8668D())
    {
        bt_wakeup_gpio_type = HW_BT_WAKEUP_GPIO_IS_83;
    }
	/* DTS2012022300887 fengwei 20120224 end >*/
    	
    printk(KERN_INFO "the bt_wakeup_gpio_type is %d\n", bt_wakeup_gpio_type);
    return bt_wakeup_gpio_type;
}
/* DTS2012022206848 yangyang 20120220 end >*/

/* < DTS2011122304248 liurennianKF62391 20111227 begin */
/* < DTS2012021707865 niguodong 20120217 begin */
/* add C8820VC for SINGLE_MIC */
/*< DTS2012042703826 lixin 20120427 begin */
audio_property_type get_audio_mic_type(void)
{
  if(machine_is_msm7x27a_U8185()
     || (machine_is_msm7x27a_C8820() && (HW_VER_SUB_VC <= get_hw_sub_board_id()))
     || (machine_is_msm7x27a_U8815() && (HW_VER_SUB_VC == get_hw_sub_board_id()))
	 )
  {
      return SINGLE_MIC;
  }
  else
  {
      return DUAL_MIC;
  }  
}
/* DTS2012042703826 lixin 20120427 end >*/
/* DTS2012021707865 niguodong 20120217 end > */
/* DTS2011122304248 liurennianKF62391 20111227 end > */

/* < DTS2012022702358 leiheiping 20120227 begin */
/* if you want to enable fir function, please return FIR_ENABLE for adapted project */
audio_property_type get_audio_fir_enabled(void)
{
    /* < DTS2012040701964 taohanwen 20120407 begin */
    if (machine_is_msm7x27a_C8820())
    {
        return FIR_ENABLE;
    }
    else
    {
        return FIR_DISABLE;
    }
    /* DTS2012040701964 taohanwen 20120407 end > */
}
/* DTS2012022702358 leiheiping 20120227 end > */
/* < DTS2012032901360 leiheiping 20120329 begin */
audio_property_type get_audio_fm_type(void)
{
   if (machine_is_msm7x27a_U8185()
	|| machine_is_msm7x27a_U8661() 
	|| machine_is_msm7x27a_C8668D())
   {
       return FM_QUALCOMM;
   }
   else
   {
       return FM_BROADCOM;
   }
}
/* DTS2012032901360 leiheiping 20120329 end > */
/*< DTS2012020400396 zhangyu 20120206 begin */
/*< DTS2012013103283   songxiaoming 20120201 begin */
hw_camera_type get_hw_camera_mirror_type(void)
{
    hw_camera_type ret = HW_CAMERA_NONES;
    if( machine_is_msm7x27a_C8820() || machine_is_msm7x27a_C8825D() || machine_is_msm7x27a_U8661())
    {
        ret = HW_MIRROR_AND_FLIP;
    }
    else
    {
      ret = HW_NOT_MIRROR_OR_FLIP;
    }
  return ret;
}
/* DTS2012013103283  songxiaoming 20120201 end > */
/* DTS2012020400396 zhangyu 20120206 end > */
/* < DTS2012051004932 huzheng 20120509 begin */
/* get touch info */
char *get_touch_info(void)
{
	char *touch_info = NULL;

	touch_info = get_synaptics_touch_info();
	if (touch_info != NULL)
		return touch_info;

/*	touch_info = get_melfas_touch_info();
	if (touch_info != NULL)
		return touch_info;
*/

	/* < DTS2012070604482 fengzhiqiang 20120712 begin */
    touch_info = get_atmel_touch_info();
	if (touch_info != NULL)
		return touch_info;
    /* DTS2012070604482  fengzhiqiang 20120712 end > */

	return NULL;
}
/* DTS2012051004932 huzheng 20120509 end > */
