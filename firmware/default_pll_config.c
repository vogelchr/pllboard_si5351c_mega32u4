#include "default_pll_config.h"

#include <avr/pgmspace.h>

/*
 * Si5351C Rev B Configuration Register Export Header File
 *
 * This file represents a series of Silicon Labs Si5351C Rev B 
 * register writes that can be performed to load a single configuration 
 * on a device. It was created by a Silicon Labs ClockBuilder Pro
 * export tool.
 *
 * Part:		                                       Si5351C Rev B
 * Design ID:                                          
 * Includes Pre/Post Download Control Register Writes: Yes
 * Created By:                                         ClockBuilder Pro v2.20 [2017-11-21]
 * Timestamp:                                          2017-12-25 19:48:24 GMT+01:00
 *
 * A complete design report corresponding to this export is included at the end 
 * of this header file.
 *
 */

const struct si5351c_reg si5351c_default_config[] PROGMEM = {
	/* addr, val */
	{ 0x0002, 0x53 },
	{ 0x0003, 0x00 },
	{ 0x0007, 0x01 },
	{ 0x0009, 0xFF },
	{ 0x000A, 0xFF },
	{ 0x000C, 0x00 },
	{ 0x000D, 0x00 },
	{ 0x000F, 0x00 },
	{ 0x0010, 0x0F },
	{ 0x0011, 0x8C },
	{ 0x0012, 0x8C },
	{ 0x0013, 0x8C },
	{ 0x0014, 0x0F },
	{ 0x0015, 0x8C },
	{ 0x0016, 0x8C },
	{ 0x0017, 0x8C },
	{ 0x0018, 0x02 },
	{ 0x0019, 0x02 },
	{ 0x001A, 0x00 },
	{ 0x001B, 0x03 },
	{ 0x001C, 0x00 },
	{ 0x001D, 0x0E },
	{ 0x001E, 0xAA },
	{ 0x001F, 0x00 },
	{ 0x0020, 0x00 },
	{ 0x0021, 0x02 },
	{ 0x002A, 0x00 },
	{ 0x002B, 0x01 },
	{ 0x002C, 0x00 },
	{ 0x002D, 0x2B },
	{ 0x002E, 0x00 },
	{ 0x002F, 0x00 },
	{ 0x0030, 0x00 },
	{ 0x0031, 0x00 },
	{ 0x004A, 0x00 },
	{ 0x004B, 0x08 },
	{ 0x004C, 0x42 },
	{ 0x004D, 0x47 },
	{ 0x004E, 0xF0 },
	{ 0x004F, 0x00 },
	{ 0x0050, 0x00 },
	{ 0x0051, 0x00 },
	{ 0x005A, 0x00 },
	{ 0x005B, 0x00 },
	{ 0x0095, 0x00 },
	{ 0x0096, 0x00 },
	{ 0x0097, 0x00 },
	{ 0x0098, 0x00 },
	{ 0x0099, 0x00 },
	{ 0x009A, 0x00 },
	{ 0x009B, 0x00 },
	{ 0x00A2, 0x00 },
	{ 0x00A3, 0x00 },
	{ 0x00A4, 0x00 },
	{ 0x00B7, 0x92 },
};

const uint8_t si5351c_default_config_nelems =
	sizeof(si5351c_default_config)/sizeof(si5351c_default_config[0]);

/*
 * Design Report
 *
 * Overview
 * ========
 * Part:         Si5351C Rev B
 * Created By:   ClockBuilder Pro v2.20 [2017-11-21]
 * Timestamp:    2017-12-25 19:48:24 GMT+01:00
 * 
 * Design Rule Check
 * =================
 * Errors:
 * - No errors
 * 
 * Warnings:
 * - No warnings
 * 
 * Design
 * ======
 * Inputs:
 *     IN0: 27 MHz
 *     IN1: Unused
 * 
 * Outputs:
 *    OUT0: 10 MHz
 *          Enabled LVCMOS 8 mA
 *          Offset 0.000 s 
 *    OUT1: Unused
 *    OUT2: Unused
 *    OUT3: Unused
 *    OUT4: 48 kHz
 *          Enabled LVCMOS 8 mA
 *          Offset 0.000 s 
 *    OUT5: Unused
 *    OUT6: Unused
 *    OUT7: Unused
 * 
 * Frequency Plan
 * ==============
 * PLL_A:
 *    Enabled Features = None
 *    Fvco             = 900 MHz
 *    M                = 33.3333333333333333... [ 33 + 1/3 ]
 *    Input0:
 *       Source           = Crystal
 *       Source Frequency = 27 MHz
 *       Fpfd             = 27 MHz
 *       Load Capacitance = Load_08pF
 *    Output0:
 *       Features       = None
 *       Disabled State = HiZ
 *       R              = 1  (2^0)
 *       Fout           = 10 MHz
 *       N              = 90
 *    Output4:
 *       Features       = None
 *       Disabled State = HiZ
 *       R              = 16  (2^4)
 *       Fout           = 48 kHz
 *       N              = 1171.875 [ 1171 + 7/8 ]
 * 
 * Settings
 * ========
 * 
 * Location      Setting Name       Decimal Value      Hex Value        
 * ------------  -----------------  -----------------  -----------------
 * 0x0002[3]     XO_LOS_MASK        0                  0x0              
 * 0x0002[4]     CLK_LOS_MASK       1                  0x1              
 * 0x0002[5]     LOL_A_MASK         0                  0x0              
 * 0x0002[6]     LOL_B_MASK         1                  0x1              
 * 0x0002[7]     SYS_INIT_MASK      0                  0x0              
 * 0x0003[7:0]   CLK_OEB            0                  0x00             
 * 0x0007[7:4]   I2C_ADDR_CTRL      0                  0x0              
 * 0x0009[7:0]   OEB_MASK0          255                0xFF             
 * 0x000A[7:0]   OEB_MASK1          255                0xFF             
 * 0x000C[7:0]   SLAB_FRQSEL0_CTRL  0                  0x00             
 * 0x000C[11:4]  SLAB_FRQSEL1_CTRL  0                  0x00             
 * 0x000F[2]     PLLA_SRC           0                  0x0              
 * 0x000F[3]     PLLB_SRC           0                  0x0              
 * 0x000F[4]     PLLA_INSELB        0                  0x0              
 * 0x000F[5]     PLLB_INSELB        0                  0x0              
 * 0x000F[7:6]   CLKIN_DIV          0                  0x0              
 * 0x0010[1:0]   CLK0_IDRV          3                  0x3              
 * 0x0010[3:2]   CLK0_SRC           3                  0x3              
 * 0x0010[4]     CLK0_INV           0                  0x0              
 * 0x0010[5]     MS0_SRC            0                  0x0              
 * 0x0010[6]     MS0_INT            0                  0x0              
 * 0x0010[7]     CLK0_PDN           0                  0x0              
 * 0x0011[1:0]   CLK1_IDRV          0                  0x0              
 * 0x0011[3:2]   CLK1_SRC           3                  0x3              
 * 0x0011[4]     CLK1_INV           0                  0x0              
 * 0x0011[5]     MS1_SRC            0                  0x0              
 * 0x0011[6]     MS1_INT            0                  0x0              
 * 0x0011[7]     CLK1_PDN           1                  0x1              
 * 0x0012[1:0]   CLK2_IDRV          0                  0x0              
 * 0x0012[3:2]   CLK2_SRC           3                  0x3              
 * 0x0012[4]     CLK2_INV           0                  0x0              
 * 0x0012[5]     MS2_SRC            0                  0x0              
 * 0x0012[6]     MS2_INT            0                  0x0              
 * 0x0012[7]     CLK2_PDN           1                  0x1              
 * 0x0013[1:0]   CLK3_IDRV          0                  0x0              
 * 0x0013[3:2]   CLK3_SRC           3                  0x3              
 * 0x0013[4]     CLK3_INV           0                  0x0              
 * 0x0013[5]     MS3_SRC            0                  0x0              
 * 0x0013[6]     MS3_INT            0                  0x0              
 * 0x0013[7]     CLK3_PDN           1                  0x1              
 * 0x0014[1:0]   CLK4_IDRV          3                  0x3              
 * 0x0014[3:2]   CLK4_SRC           3                  0x3              
 * 0x0014[4]     CLK4_INV           0                  0x0              
 * 0x0014[5]     MS4_SRC            0                  0x0              
 * 0x0014[6]     MS4_INT            0                  0x0              
 * 0x0014[7]     CLK4_PDN           0                  0x0              
 * 0x0015[1:0]   CLK5_IDRV          0                  0x0              
 * 0x0015[3:2]   CLK5_SRC           3                  0x3              
 * 0x0015[4]     CLK5_INV           0                  0x0              
 * 0x0015[5]     MS5_SRC            0                  0x0              
 * 0x0015[6]     MS5_INT            0                  0x0              
 * 0x0015[7]     CLK5_PDN           1                  0x1              
 * 0x0016[1:0]   CLK6_IDRV          0                  0x0              
 * 0x0016[3:2]   CLK6_SRC           3                  0x3              
 * 0x0016[4]     CLK6_INV           0                  0x0              
 * 0x0016[5]     MS6_SRC            0                  0x0              
 * 0x0016[6]     FBA_INT            0                  0x0              
 * 0x0016[7]     CLK6_PDN           1                  0x1              
 * 0x0017[1:0]   CLK7_IDRV          0                  0x0              
 * 0x0017[3:2]   CLK7_SRC           3                  0x3              
 * 0x0017[4]     CLK7_INV           0                  0x0              
 * 0x0017[5]     MS7_SRC            0                  0x0              
 * 0x0017[6]     FBB_INT            0                  0x0              
 * 0x0017[7]     CLK7_PDN           1                  0x1              
 * 0x0018[1:0]   CLK0_DIS_STATE     2                  0x2              
 * 0x0019[1:0]   CLK4_DIS_STATE     2                  0x2              
 * 0x001C[17:0]  MSNA_P1            3754               0x00EAA          
 * 0x001F[19:0]  MSNA_P2            2                  0x00002          
 * 0x001F[23:4]  MSNA_P3            3                  0x00003          
 * 0x002C[17:0]  MS0_P1             11008              0x02B00          
 * 0x002F[19:0]  MS0_P2             0                  0x00000          
 * 0x002F[23:4]  MS0_P3             1                  0x00001          
 * 0x004C[17:0]  MS4_P1             149488             0x247F0          
 * 0x004C[6:4]   R4_DIV             4                  0x4              
 * 0x004F[19:0]  MS4_P2             0                  0x00000          
 * 0x004F[23:4]  MS4_P3             8                  0x00008          
 * 0x005A[7:0]   MS6_P1             0                  0x00             
 * 0x005B[7:0]   MS7_P1             0                  0x00             
 * 0x0095[14:0]  SSDN_P2            0                  0x0000           
 * 0x0095[7]     SSC_EN             0                  0x0              
 * 0x0097[14:0]  SSDN_P3            0                  0x0000           
 * 0x0097[7]     SSC_MODE           0                  0x0              
 * 0x0099[11:0]  SSDN_P1            0                  0x000            
 * 0x009A[15:4]  SSUDP              0                  0x000            
 * 0x00A2[21:0]  VCXO_PARAM         0                  0x000000         
 * 0x00B7[2:0]   SLAB_PLLA_CL       2                  0x2              
 * 0x00B7[5:3]   SLAB_PLLB_CL       2                  0x2              
 * 0x00B7[7:6]   XTAL_CL            2                  0x2
 * 
 *
 */

