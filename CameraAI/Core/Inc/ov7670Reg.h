/*
 * ov7670Reg.h
 *
 *  Author bnigg2 and bmalo4
 */

#ifndef OV7670_OV7670REG_H_
#define OV7670_OV7670REG_H_

#define QVGA_MODE 0
#define QCIF_MODE 1
#define OLD 0
#define REG_END 0xFF

const uint8_t OV7670_reg[][2] = {
		//href looks right for vga
		//30us for href low
		//60hz vsync
#if QVGA_MODE
		/* 30 FPS QVGA RGB565 Mode (320 x 240) */
		{0x11, 0x04},   // CLKRC, 30 fps frame rate for QVGA. 0x04 for 30hz, 12Mhz pclk
//		{0x11, 0x01},   // CLKRC, 60 fps frame rate for QVGA. 24MHz pclk
		{0x12, 0x12},   // COM7, QVGA Preset, RGB mode.
		{0x6B, 0x7a},	// DBLV, PLL Multiplier by 4x (Using PLL gives better than our supplied PCLK).
		{0x40, 0xD0},	// COM15, Output Range [00] to [FF], RGB 565 Output.
//		{0x15, 0x20},	// COM10, PCLK Gated by HREF (Data not transfered when on horizontal blank).
//		{0x14, 0x6A},
//		{0x4F, 0xB3},
//		{0x50, 0xB3},
//		{0x51, 0x00},
//		{0x52, 0x3D},
//		{0x53, 0xA7},
//		{0x54, 0xE4},
//		{0x3D, 0x99 | 0x40},
#endif
#if QCIF_MODE
		/* 30 FPS QCIF RGB Mode (176 x 144) */
		{0x11, 0x04},    // CLKRC, 60 fps frame rate for QCIF 24MHz.
		{0x12, 0x06},    // COM7, QCIF Preset, RGB mode.
		{0x6B, 0x7a},	 // DBLV, PLL Multiplier by 4x
		{0x40, 0xD0},	 // COM15, Output Range [00] to [FF], RGB 555 Output
		//{0x15, 0x20},	 // COM10, PCLK Gated by HREF (Data not transfered when on horizontal blank).
		{0x14, 0x6A},	 // COM9, 128x AGC
		{0x4F, 0xB3},	 // Matrix Coefficient 1
		{0x50, 0xB3},    // Matrix Coefficient 2
		{0x51, 0x00},    // Matrix Coefficient 3
		{0x52, 0x3D},    // Matrix Coefficient 4
		{0x53, 0xA7},    // Matrix Coefficient 5
		{0x54, 0xE4},    // Matrix Coefficient 6
		//{0x3D, 0x99 | 0x40}, // COM13,
#endif

#if OLD

  /* Color mode related */
  {0x12, 0x14},   			// QVGA, RGB
  {0x8C, 0x00},   			// RGB444 Disable
  {0x40, 0x10 + 0xc0},   	// RGB565, 00 - FF
  {0x3A, 0x04 + 8},   		// UYVY (why?)
  {0x3D, 0x80 + 0x00},   	// gamma enable, UV auto adjust, UYVY
  {0xB0, 0x84}, 			// important, reserved register

  //brightness related
  {0x55, 0x40}, // brightness, bit7: 0 positive, 1 negative bits[6:0] brightness val
  {0x56, 0x40}, // contrast, 0x40 is no contrast adjustment

  /* clock related */
  {0x0C, 0x04},  // DCW enable
  {0x3E, 0x19},  // manual scaling, pclk/=2
  {0x70, 0x3A},  // scaling_xsc
  {0x71, 0x35},  // scaling_ysc
  {0x72, 0x11}, // down sample by 2
  {0x73, 0xf1}, // DSP clock /= 2

  /* windowing (empirically decided...) */
  {0x17, 0x16},   // HSTART
  {0x18, 0x04},   // HSTOP
  {0x32, 0x80},   // HREF
  {0x19, 0x03},   // VSTART =  14 ( = 3 * 4 + 2)
  {0x1a, 0x7b},   // VSTOP  = 494 ( = 123 * 4 + 2)
  {0x03, 0x0a},   // VREF (VSTART_LOW = 2, VSTOP_LOW = 2)

  /* color matrix coefficient */
#if 0
  {0x4f, 0xb3},
  {0x50, 0xb3},
  {0x51, 0x00},
  {0x52, 0x3d},
  {0x53, 0xa7},
  {0x54, 0xe4},
  {0x58, 0x9e},
#else
  {0x4f, 0x80},
  {0x50, 0x80},
  {0x51, 0x00},
  {0x52, 0x22},
  {0x53, 0x5e},
  {0x54, 0x80},
  {0x58, 0x9e},
#endif

  /* 3a */
//  {0x13, 0x84},
//  {0x14, 0x0a},   // AGC Ceiling = 2x
//  {0x5F, 0x2f},   // AWB B Gain Range (empirically decided)
//                  // without this bright scene becomes yellow (purple). might be because of color matrix
//  {0x60, 0x98},   // AWB R Gain Range (empirically decided)
//  {0x61, 0x70},   // AWB G Gain Range (empirically decided)
  {0x41, 0x38},   // edge enhancement, de-noise, AWG gain enabled


  /* gamma curve */
#if 1
  {0x7b, 16},
  {0x7c, 30},
  {0x7d, 53},
  {0x7e, 90},
  {0x7f, 105},
  {0x80, 118},
  {0x81, 130},
  {0x82, 140},
  {0x83, 150},
  {0x84, 160},
  {0x85, 180},
  {0x86, 195},
  {0x87, 215},
  {0x88, 230},
  {0x89, 244},
  {0x7a, 16},
#else
  /* gamma = 1 */
  {0x7b, 4},
  {0x7c, 8},
  {0x7d, 16},
  {0x7e, 32},
  {0x7f, 40},
  {0x80, 48},
  {0x81, 56},
  {0x82, 64},
  {0x83, 72},
  {0x84, 80},
  {0x85, 96},
  {0x86, 112},
  {0x87, 144},
  {0x88, 176},
  {0x89, 208},
  {0x7a, 64},
#endif

  /* fps */
  {0x6B, 0x7a}, //PLL  x4
  {0x11, 0x01}, // pre-scalar = 1/1

  /* others */
  {0x1E, 0x31}, //mirror flip
//  {0x42, 0x08}, // color bar
#endif
  {REG_END, REG_END}
};


#endif /* OV7670_OV7670REG_H_ */
