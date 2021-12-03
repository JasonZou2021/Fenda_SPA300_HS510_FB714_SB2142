
/*---------------------------------------------------------------------------*
Copyright c 2011 Saleem Abdulrasool <compnerd@compnerd.org>.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*---------------------------------------------------------------------------*/


// *****************************************************************************
// *    CEC process                                                           *
// *    Create by ECL                                                         *
// *    2007/10/01                                                            *
// *****************************************************************************
#ifndef EDID_H
#define EDID_H

#include "types.h"


#define CEA861_NO_DTDS_PRESENT                          (0x04)
#define ARRAY_SIZE(arr)                         (sizeof(arr) / sizeof(arr[0]))

enum cea861_data_block_type
{
	CEA861_DATA_BLOCK_TYPE_RESERVED0,
	CEA861_DATA_BLOCK_TYPE_AUDIO,
	CEA861_DATA_BLOCK_TYPE_VIDEO,
	CEA861_DATA_BLOCK_TYPE_VENDOR_SPECIFIC,
	CEA861_DATA_BLOCK_TYPE_SPEAKER_ALLOCATION,
	CEA861_DATA_BLOCK_TYPE_VESA_DTC,
	CEA861_DATA_BLOCK_TYPE_RESERVED6,
	CEA861_DATA_BLOCK_TYPE_EXTENDED,
};

enum cea861_audio_format
{
	CEA861_AUDIO_FORMAT_RESERVED,
	CEA861_AUDIO_FORMAT_LPCM,
	CEA861_AUDIO_FORMAT_AC_3,
	CEA861_AUDIO_FORMAT_MPEG_1,
	CEA861_AUDIO_FORMAT_MP3,
	CEA861_AUDIO_FORMAT_MPEG2,
	CEA861_AUDIO_FORMAT_AAC_LC,
	CEA861_AUDIO_FORMAT_DTS,
	CEA861_AUDIO_FORMAT_ATRAC,
	CEA861_AUDIO_FORMAT_DSD,
	CEA861_AUDIO_FORMAT_E_AC_3,
	CEA861_AUDIO_FORMAT_DTS_HD,
	CEA861_AUDIO_FORMAT_MLP,
	CEA861_AUDIO_FORMAT_DST,
	CEA861_AUDIO_FORMAT_WMA_PRO,
	CEA861_AUDIO_FORMAT_EXTENDED,
};

struct __attribute__ (( packed )) cea861_timing_block
{
	/* CEA Extension Header */
	uint8_t  tag;
	uint8_t  revision;
	uint8_t  dtd_offset;

	/* Global Declarations */
	unsigned native_dtds           : 4;
	unsigned yuv_422_supported     : 1;
	unsigned yuv_444_supported     : 1;
	unsigned basic_audio_supported : 1;
	unsigned underscan_supported   : 1;

	uint8_t  data[123];

	uint8_t  checksum;
};

struct __attribute__ (( packed )) cea861_data_block_header
{
	unsigned length : 5;
	unsigned tag    : 3;
};

struct __attribute__ (( packed )) cea861_short_video_descriptor
{
	unsigned video_identification_code : 7;
	unsigned native                    : 1;
};

struct __attribute__ (( packed )) cea861_video_data_block
{
	struct cea861_data_block_header      header;
	struct cea861_short_video_descriptor svd[];
};

struct __attribute__ (( packed )) cea861_short_audio_descriptor
{
	unsigned channels              : 3; /* = value + 1 */
	unsigned audio_format          : 4;
	unsigned                       : 1;

	unsigned sample_rate_32_kHz    : 1;
	unsigned sample_rate_44_1_kHz  : 1;
	unsigned sample_rate_48_kHz    : 1;
	unsigned sample_rate_88_2_kHz  : 1;
	unsigned sample_rate_96_kHz    : 1;
	unsigned sample_rate_176_4_kHz : 1;
	unsigned sample_rate_192_kHz   : 1;
	unsigned                       : 1;

	union
	{
		struct __attribute__ (( packed ))
		{
			unsigned bitrate_16_bit : 1;
			unsigned bitrate_20_bit : 1;
			unsigned bitrate_24_bit : 1;
			unsigned                : 5;
		} lpcm;

		uint8_t maximum_bit_rate;       /* formats 2-8; = value * 8 kHz */

		uint8_t format_dependent;       /* formats 9-13; */

		struct __attribute__ (( packed ))
		{
			unsigned profile : 3;
			unsigned         : 5;
		} wma_pro;

		struct __attribute__ (( packed ))
		{
			unsigned      : 3;
			unsigned code : 5;
		} extension;
	} flags;
};

struct __attribute__ (( packed )) cea861_audio_data_block
{
	struct cea861_data_block_header      header;
	struct cea861_short_audio_descriptor sad[];
};

struct __attribute__ (( packed )) cea861_speaker_allocation
{
	unsigned front_left_right        : 1;
	unsigned front_lfe               : 1;   /* low frequency effects */
	unsigned front_center            : 1;
	unsigned rear_left_right         : 1;
	unsigned rear_center             : 1;
	unsigned front_left_right_center : 1;
	unsigned rear_left_right_center  : 1;
	unsigned front_left_right_wide   : 1;

	unsigned front_left_right_high   : 1;
	unsigned top_center              : 1;
	unsigned front_center_high       : 1;
	unsigned                         : 5;

	unsigned                         : 8;
};

struct __attribute__ (( packed )) cea861_speaker_allocation_data_block
{
	struct cea861_data_block_header  header;
	struct cea861_speaker_allocation payload;
};

struct __attribute__ (( packed )) cea861_vendor_specific_data_block
{
	struct cea861_data_block_header  header;
	uint8_t                          ieee_registration[3];
	uint8_t                          data[30];
};

static const struct cea861_timing
{
	const uint16_t hactive;
	const uint16_t vactive;
	const enum
	{
		INTERLACED,
		PROGRESSIVE,
	} mode;

	const uint16_t htotal;
	const uint16_t hblank;
	const uint16_t vtotal;
	const double   vblank;
	const double   hfreq;
	const double   vfreq;
	const double   pixclk;
} cea861_timings[] =
{
	[1]  = {  640,  480, PROGRESSIVE,  800,  160,  525, 45.0,  31.469,  59.940,  25.175 },
	[2]  = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0,  31.469,  59.940,  27.000 },
	[3]  = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0,  31.469,  59.940,  27.000 },
	[4]  = { 1280,  720, PROGRESSIVE, 1650,  370,  750, 30.0,  45.000,  60.000,  74.250 },
	[5]  = { 1920, 1080,  INTERLACED, 2200,  280, 1125, 22.5,  33.750,  60.000,  72.250 },
	[6]  = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  15.734,  59.940,  27.000 },
	[7]  = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  15.734,  59.940,  27.000 },
	[8]  = { 1440,  240, PROGRESSIVE, 1716,  276,  262, 22.0,  15.734,  60.054,  27.000 },  /* 9 */
	[9]  = { 1440,  240, PROGRESSIVE, 1716,  276,  262, 22.0,  15.734,  59.826,  27.000 },  /* 8 */
	[10] = { 2880,  480,  INTERLACED, 3432,  552,  525, 22.5,  15.734,  59.940,  54.000 },
	[11] = { 2880,  480,  INTERLACED, 3432,  552,  525, 22.5,  15.734,  59.940,  54.000 },
	[12] = { 2880,  240, PROGRESSIVE, 3432,  552,  262, 22.0,  15.734,  60.054,  54.000 },  /* 13 */
	[13] = { 2880,  240, PROGRESSIVE, 3432,  552,  262, 22.0,  15.734,  59.826,  54.000 },  /* 12 */
	[14] = { 1440,  480, PROGRESSIVE, 1716,  276,  525, 45.0,  31.469,  59.940,  54.000 },
	[15] = { 1440,  480, PROGRESSIVE, 1716,  276,  525, 45.0,  31.469,  59.940,  54.000 },
	[16] = { 1920, 1080, PROGRESSIVE, 2200,  280, 1125, 45.0,  67.500,  60.000, 148.500 },
	[17] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0,  31.250,  50.000,  27.000 },
	[18] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0,  31.250,  50.000,  27.000 },
	[19] = { 1280,  720, PROGRESSIVE, 1980,  700,  750, 30.0,  37.500,  50.000,  74.250 },
	[20] = { 1920, 1080,  INTERLACED, 2640,  720, 1125, 22.5,  28.125,  50.000,  74.250 },
	[21] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  15.625,  50.000,  27.000 },
	[22] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  15.625,  50.000,  27.000 },
	[23] = { 1440,  288, PROGRESSIVE, 1728,  288,  312, 24.0,  15.625,  50.080,  27.000 },  /* 24 */
	[24] = { 1440,  288, PROGRESSIVE, 1728,  288,  313, 25.0,  15.625,  49.920,  27.000 },  /* 23 */
	// [24] = { 1440,  288, PROGRESSIVE, 1728,  288,  314, 26.0,  15.625,  49.761,  27.000 },
	[25] = { 2880,  576,  INTERLACED, 3456,  576,  625, 24.5,  15.625,  50.000,  54.000 },
	[26] = { 2880,  576,  INTERLACED, 3456,  576,  625, 24.5,  15.625,  50.000,  54.000 },
	[27] = { 2880,  288, PROGRESSIVE, 3456,  576,  312, 24.0,  15.625,  50.080,  54.000 },  /* 28 */
	[28] = { 2880,  288, PROGRESSIVE, 3456,  576,  313, 25.0,  15.625,  49.920,  54.000 },  /* 27 */
	// [28] = { 2880,  288, PROGRESSIVE, 3456,  576,  314, 26.0,  15.625,  49.761,  54.000 },
	[29] = { 1440,  576, PROGRESSIVE, 1728,  288,  625, 49.0,  31.250,  50.000,  54.000 },
	[30] = { 1440,  576, PROGRESSIVE, 1728,  288,  625, 49.0,  31.250,  50.000,  54.000 },
	[31] = { 1920, 1080, PROGRESSIVE, 2640,  720, 1125, 45.0,  56.250,  50.000, 148.500 },
	[32] = { 1920, 1080, PROGRESSIVE, 2750,  830, 1125, 45.0,  27.000,  24.000,  74.250 },
	[33] = { 1920, 1080, PROGRESSIVE, 2640,  720, 1125, 45.0,  28.125,  25.000,  74.250 },
	[34] = { 1920, 1080, PROGRESSIVE, 2200,  280, 1125, 45.0,  33.750,  30.000,  74.250 },
	[35] = { 2880,  480, PROGRESSIVE, 3432,  552,  525, 45.0,  31.469,  59.940, 108.500 },
	[36] = { 2880,  480, PROGRESSIVE, 3432,  552,  525, 45.0,  31.469,  59.940, 108.500 },
	[37] = { 2880,  576, PROGRESSIVE, 3456,  576,  625, 49.0,  31.250,  50.000, 108.000 },
	[38] = { 2880,  576, PROGRESSIVE, 3456,  576,  625, 49.0,  31.250,  50.000, 108.000 },
	[39] = { 1920, 1080,  INTERLACED, 2304,  384, 1250, 85.0,  31.250,  50.000,  72.000 },
	[40] = { 1920, 1080,  INTERLACED, 2640,  720, 1125, 22.5,  56.250, 100.000, 148.500 },
	[41] = { 1280,  720, PROGRESSIVE, 1980,  700,  750, 30.0,  75.000, 100.000, 148.500 },
	[42] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0,  62.500, 100.000,  54.000 },
	[43] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0,  62.500, 100.000,  54.000 },
	[44] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  31.250, 100.000,  54.000 },
	[45] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  31.250, 100.000,  54.000 },
	[46] = { 1920, 1080,  INTERLACED, 2200,  280, 1125, 22.5,  67.500, 120.000, 148.500 },
	[47] = { 1280,  720, PROGRESSIVE, 1650,  370,  750, 30.0,  90.000, 120.000, 148.500 },
	[48] = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0,  62.937, 119.880,  54.000 },
	[49] = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0,  62.937, 119.880,  54.000 },
	[50] = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  31.469, 119.880,  54.000 },
	[51] = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  31.469, 119.880,  54.000 },
	[52] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0, 125.000, 200.000, 108.000 },
	[53] = {  720,  576, PROGRESSIVE,  864,  144,  625, 49.0, 125.000, 200.000, 108.000 },
	[54] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  62.500, 200.000, 108.000 },
	[55] = { 1440,  576,  INTERLACED, 1728,  288,  625, 24.5,  62.500, 200.000, 108.000 },
	[56] = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0, 125.874, 239.760, 108.000 },
	[57] = {  720,  480, PROGRESSIVE,  858,  138,  525, 45.0, 125.874, 239.760, 108.000 },
	[58] = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  62.937, 239.760, 108.000 },
	[59] = { 1440,  480,  INTERLACED, 1716,  276,  525, 22.5,  62.937, 239.760, 108.000 },
	[60] = { 1280,  720, PROGRESSIVE, 3300, 2020,  750, 30.0,  18.000,  24.000,  59.400 },
	[61] = { 1280,  720, PROGRESSIVE, 3960, 2680,  750, 30.0,  18.750,  25.000,  74.250 },
	[62] = { 1280,  720, PROGRESSIVE, 3300, 2020,  750, 30.0,  22.500,  30.000,  74.250 },
	[63] = { 1920, 1080, PROGRESSIVE, 2200,  280, 1125, 45.0, 135.000, 120.000, 297.000 },
	[64] = { 1920, 1080, PROGRESSIVE, 2640,  720, 1125, 45.0, 112.500, 100.000, 297.000 },
};


//edid{

#define EDID_I2C_DDC_DATA_ADDRESS               (0x50)

#define EDID_BLOCK_SIZE                         (0x80)
#define EDID_MAX_EXTENSIONS                     (0xfe)


static const uint8_t EDID_HEADER[] = { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };
static const uint8_t EDID_STANDARD_TIMING_DESCRIPTOR_INVALID[] = { 0x01, 0x01 };


enum edid_extension_type
{
	EDID_EXTENSION_TIMING           = 0x01, // Timing Extension
	EDID_EXTENSION_CEA              = 0x02, // Additional Timing Block Data (CEA EDID Timing Extension)
	EDID_EXTENSION_VTB              = 0x10, // Video Timing Block Extension (VTB-EXT)
	EDID_EXTENSION_EDID_2_0         = 0x20, // EDID 2.0 Extension
	EDID_EXTENSION_DI               = 0x40, // Display Information Extension (DI-EXT)
	EDID_EXTENSION_LS               = 0x50, // Localised String Extension (LS-EXT)
	EDID_EXTENSION_MI               = 0x60, // Microdisplay Interface Extension (MI-EXT)
	EDID_EXTENSION_DTCDB_1          = 0xa7, // Display Transfer Characteristics Data Block (DTCDB)
	EDID_EXTENSION_DTCDB_2          = 0xaf,
	EDID_EXTENSION_DTCDB_3          = 0xbf,
	EDID_EXTENSION_BLOCK_MAP        = 0xf0, // Block Map
	EDID_EXTENSION_DDDB             = 0xff, // Display Device Data Block (DDDB)
};

enum edid_display_type
{
	EDID_DISPLAY_TYPE_MONOCHROME,
	EDID_DISPLAY_TYPE_RGB,
	EDID_DISPLAY_TYPE_NON_RGB,
	EDID_DISPLAY_TYPE_UNDEFINED,
};

enum edid_aspect_ratio
{
	EDID_ASPECT_RATIO_16_10,
	EDID_ASPECT_RATIO_4_3,
	EDID_ASPECT_RATIO_5_4,
	EDID_ASPECT_RATIO_16_9,
};

enum edid_signal_sync
{
	EDID_SIGNAL_SYNC_ANALOG_COMPOSITE,
	EDID_SIGNAL_SYNC_BIPOLAR_ANALOG_COMPOSITE,
	EDID_SIGNAL_SYNC_DIGITAL_COMPOSITE,
	EDID_SIGNAL_SYNC_DIGITAL_SEPARATE,
};

enum edid_stereo_mode
{
	EDID_STEREO_MODE_NONE,
	EDID_STEREO_MODE_RESERVED,
	EDID_STEREO_MODE_FIELD_SEQUENTIAL_RIGHT,
	EDID_STEREO_MODE_2_WAY_INTERLEAVED_RIGHT,
	EDID_STEREO_MODE_FIELD_SEQUENTIAL_LEFT,
	EDID_STEREO_MODE_2_WAY_INTERLEAVED_LEFT,
	EDID_STEREO_MODE_4_WAY_INTERLEAVED,
	EDID_STEREO_MODE_SIDE_BY_SIDE_INTERLEAVED,
};

enum edid_monitor_descriptor_type
{
	EDID_MONTIOR_DESCRIPTOR_MANUFACTURER_DEFINED        = 0x0f,
	EDID_MONITOR_DESCRIPTOR_STANDARD_TIMING_IDENTIFIERS = 0xfa,
	EDID_MONITOR_DESCRIPTOR_COLOR_POINT                 = 0xfb,
	EDID_MONITOR_DESCRIPTOR_MONITOR_NAME                = 0xfc,
	EDID_MONITOR_DESCRIPTOR_MONITOR_RANGE_LIMITS        = 0xfd,
	EDID_MONITOR_DESCRIPTOR_ASCII_STRING                = 0xfe,
	EDID_MONITOR_DESCRIPTOR_MONITOR_SERIAL_NUMBER       = 0xff,
};

enum edid_secondary_timing_support
{
	EDID_SECONDARY_TIMING_NOT_SUPPORTED,
	EDID_SECONDARY_TIMING_GFT           = 0x02,
};


struct __attribute__ (( packed )) edid_detailed_timing_descriptor
{
	uint16_t pixel_clock;                               /* = value * 10000 */

	uint8_t  horizontal_active_lo;
	uint8_t  horizontal_blanking_lo;

	unsigned horizontal_blanking_hi         : 4;
	unsigned horizontal_active_hi           : 4;

	uint8_t  vertical_active_lo;
	uint8_t  vertical_blanking_lo;

	unsigned vertical_blanking_hi           : 4;
	unsigned vertical_active_hi             : 4;

	uint8_t  horizontal_sync_offset_lo;
	uint8_t  horizontal_sync_pulse_width_lo;

	unsigned vertical_sync_pulse_width_lo   : 4;
	unsigned vertical_sync_offset_lo        : 4;

	unsigned vertical_sync_pulse_width_hi   : 2;
	unsigned vertical_sync_offset_hi        : 2;
	unsigned horizontal_sync_pulse_width_hi : 2;
	unsigned horizontal_sync_offset_hi      : 2;

	uint8_t  horizontal_image_size_lo;
	uint8_t  vertical_image_size_lo;

	unsigned vertical_image_size_hi         : 4;
	unsigned horizontal_image_size_hi       : 4;

	uint8_t  horizontal_border;
	uint8_t  vertical_border;

	unsigned stereo_mode_lo                 : 1;
	unsigned signal_pulse_polarity          : 1; /* pulse on sync, composite/horizontal polarity */
	unsigned signal_serration_polarity      : 1; /* serrate on sync, vertical polarity */
	unsigned signal_sync                    : 2;
	unsigned stereo_mode_hi                 : 2;
	unsigned interlaced                     : 1;
};

static inline uint32_t
edid_detailed_timing_pixel_clock(const struct edid_detailed_timing_descriptor *const dtb)
{
	return dtb->pixel_clock * 10000;
}

static inline uint16_t
edid_detailed_timing_horizontal_blanking(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->horizontal_blanking_hi << 8) | dtb->horizontal_blanking_lo;
}

static inline uint16_t
edid_detailed_timing_horizontal_active(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->horizontal_active_hi << 8) | dtb->horizontal_active_lo;
}

static inline uint16_t
edid_detailed_timing_vertical_blanking(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->vertical_blanking_hi << 8) | dtb->vertical_blanking_lo;
}

static inline uint16_t
edid_detailed_timing_vertical_active(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->vertical_active_hi << 8) | dtb->vertical_active_lo;
}

static inline uint8_t
edid_detailed_timing_vertical_sync_offset(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->vertical_sync_offset_hi << 4) | dtb->vertical_sync_offset_lo;
}

static inline uint8_t
edid_detailed_timing_vertical_sync_pulse_width(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->vertical_sync_pulse_width_hi << 4) | dtb->vertical_sync_pulse_width_lo;
}

static inline uint8_t
edid_detailed_timing_horizontal_sync_offset(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->horizontal_sync_offset_hi << 4) | dtb->horizontal_sync_offset_lo;
}

static inline uint8_t
edid_detailed_timing_horizontal_sync_pulse_width(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->horizontal_sync_pulse_width_hi << 4) | dtb->horizontal_sync_pulse_width_lo;
}

static inline uint16_t
edid_detailed_timing_horizontal_image_size(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->horizontal_image_size_hi << 8) | dtb->horizontal_image_size_lo;
}

static inline uint16_t
edid_detailed_timing_vertical_image_size(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->vertical_image_size_hi << 8) | dtb->vertical_image_size_lo;
}

static inline uint8_t
edid_detailed_timing_stereo_mode(const struct edid_detailed_timing_descriptor *const dtb)
{
	return (dtb->stereo_mode_hi << 2 | dtb->stereo_mode_lo);
}


struct __attribute__ (( packed )) edid_monitor_descriptor
{
	uint16_t flag0;
	uint8_t  flag1;
	uint8_t  tag;
	uint8_t  flag2;
	uint8_t  data[13];
};

typedef char edid_monitor_descriptor_string[sizeof(((struct edid_monitor_descriptor *)0)->data) + 1];


struct __attribute__ (( packed )) edid_monitor_range_limits
{
	uint8_t  minimum_vertical_rate;             /* Hz */
	uint8_t  maximum_vertical_rate;             /* Hz */
	uint8_t  minimum_horizontal_rate;           /* kHz */
	uint8_t  maximum_horizontal_rate;           /* kHz */
	uint8_t  maximum_supported_pixel_clock;     /* = (value * 10) Mhz (round to 10 MHz) */

	/* secondary timing formula */
	uint8_t  secondary_timing_support;
	uint8_t  reserved;
	uint8_t  secondary_curve_start_frequency;   /* horizontal frequency / 2 kHz */
	uint8_t  c;                                 /* = (value >> 1) */
	uint16_t m;
	uint8_t  k;
	uint8_t  j;                                 /* = (value >> 1) */
};


struct __attribute__ (( packed )) edid_standard_timing_descriptor
{
	uint8_t  horizontal_active_pixels;         /* = (value + 31) * 8 */

	unsigned refresh_rate       : 6;           /* = value + 60 */
	unsigned image_aspect_ratio : 2;
};


struct __attribute__ (( packed )) edid
{
	/* header information */
	uint8_t  header[8];

	/* vendor/product identification */
	uint16_t manufacturer;

	uint8_t  product[2];
	uint8_t  serial_number[4];
	uint8_t  manufacture_week;
	uint8_t  manufacture_year;                  /* = value + 1990 */

	/* EDID version */
	uint8_t  version;
	uint8_t  revision;

	/* basic display parameters and features */
	union
	{
		struct __attribute__ (( packed ))
		{
			unsigned dfp_1x                 : 1;    /* VESA DFP 1.x */
			unsigned                        : 6;
			unsigned digital                : 1;
		} digital;
		struct __attribute__ (( packed ))
		{
			unsigned vsync_serration        : 1;
			unsigned green_video_sync       : 1;
			unsigned composite_sync         : 1;
			unsigned separate_sync          : 1;
			unsigned blank_to_black_setup   : 1;
			unsigned signal_level_standard  : 2;
			unsigned digital                : 1;
		} analog;
	} video_input_definition;

	uint8_t  maximum_horizontal_image_size;     /* cm */
	uint8_t  maximum_vertical_image_size;       /* cm */

	uint8_t  display_transfer_characteristics;  /* gamma = (value + 100) / 100 */

	struct __attribute__ (( packed ))
	{
		unsigned default_gtf                    : 1; /* generalised timing formula */
		unsigned preferred_timing_mode          : 1;
		unsigned standard_default_color_space   : 1;
		unsigned display_type                   : 2;
		unsigned active_off                     : 1;
		unsigned suspend                        : 1;
		unsigned standby                        : 1;
	} feature_support;

	/* color characteristics block */
	unsigned green_y_low    : 2;
	unsigned green_x_low    : 2;
	unsigned red_y_low      : 2;
	unsigned red_x_low      : 2;

	unsigned white_y_low    : 2;
	unsigned white_x_low    : 2;
	unsigned blue_y_low     : 2;
	unsigned blue_x_low     : 2;

	uint8_t  red_x;
	uint8_t  red_y;
	uint8_t  green_x;
	uint8_t  green_y;
	uint8_t  blue_x;
	uint8_t  blue_y;
	uint8_t  white_x;
	uint8_t  white_y;

	/* established timings */
	struct __attribute__ (( packed ))
	{
		unsigned timing_800x600_60   : 1;
		unsigned timing_800x600_56   : 1;
		unsigned timing_640x480_75   : 1;
		unsigned timing_640x480_72   : 1;
		unsigned timing_640x480_67   : 1;
		unsigned timing_640x480_60   : 1;
		unsigned timing_720x400_88   : 1;
		unsigned timing_720x400_70   : 1;

		unsigned timing_1280x1024_75 : 1;
		unsigned timing_1024x768_75  : 1;
		unsigned timing_1024x768_70  : 1;
		unsigned timing_1024x768_60  : 1;
		unsigned timing_1024x768_87  : 1;
		unsigned timing_832x624_75   : 1;
		unsigned timing_800x600_75   : 1;
		unsigned timing_800x600_72   : 1;
	} established_timings;

	struct __attribute__ (( packed ))
	{
		unsigned reserved            : 7;
		unsigned timing_1152x870_75  : 1;
	} manufacturer_timings;

	/* standard timing id */
	struct  edid_standard_timing_descriptor standard_timing_id[8];

	/* detailed timing */
	union
	{
		struct edid_monitor_descriptor         monitor;
		struct edid_detailed_timing_descriptor timing;
	} detailed_timings[4];

	uint8_t  extensions;
	uint8_t  checksum;
};

struct __attribute__ (( packed )) edid_color_characteristics_data
{
	struct
	{
		uint16_t x;
		uint16_t y;
	} red, green, blue, white;
};

struct __attribute__ (( packed )) edid_block_map
{
	uint8_t tag;
	uint8_t extension_tag[126];
	uint8_t checksum;
};


struct __attribute__ (( packed )) edid_extension
{
	uint8_t tag;
	uint8_t revision;
	uint8_t extension_data[125];
	uint8_t checksum;
};

//edid}

//Hdmi

/*! \todo figure out a better way to determine the offsets */
#define HDMI_VSDB_EXTENSION_FLAGS_OFFSET        (0x06)
#define HDMI_VSDB_MAX_TMDS_OFFSET               (0x07)
#define HDMI_VSDB_LATENCY_FIELDS_OFFSET         (0x08)

static const uint8_t HDMI_OUI[]                 = { 0x00, 0x0C, 0x03 };

struct __attribute__ (( packed )) hdmi_vendor_specific_data_block
{
	struct cea861_data_block_header header;

	uint8_t  ieee_registration_id[3];           /* LSB */

	unsigned port_configuration_b      : 4;
	unsigned port_configuration_a      : 4;
	unsigned port_configuration_d      : 4;
	unsigned port_configuration_c      : 4;

	/* extension fields */
	unsigned dvi_dual_link             : 1;
	unsigned                           : 2;
	unsigned yuv_444_supported         : 1;
	unsigned colour_depth_30_bit       : 1;
	unsigned colour_depth_36_bit       : 1;
	unsigned colour_depth_48_bit       : 1;
	unsigned audio_info_frame          : 1;

	uint8_t  max_tmds_clock;                    /* = value * 5 */

	unsigned                           : 6;
	unsigned interlaced_latency_fields : 1;
	unsigned latency_fields            : 1;

	uint8_t  video_latency;                     /* = (value - 1) * 2 */
	uint8_t  audio_latency;                     /* = (value - 1) * 2 */
	uint8_t  interlaced_video_latency;
	uint8_t  interlaced_audio_latency;

	uint8_t  reserved[];
};

void parse_edid_info(const uint8_t *const data);

#endif

