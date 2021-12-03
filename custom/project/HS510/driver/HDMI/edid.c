
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

/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cec.h"

#include "edid.h"

#define PRINT_LEN (128)
#define CM_2_MM(cm)                             ((cm) * 10)
#define CM_2_IN(cm)                             ((int)((cm) * 0.3937*10000))

#define HZ_2_MHZ(hz)                            ((hz) / 1000000)


void dump_section(const char *const name,
				  const BYTE *const buffer,
				  const BYTE offset,
				  const BYTE length)
{
	int i = 0;
	const BYTE *value = buffer + offset;

	if (name != NULL)
		cec_printf0("%s: ", name);

	for (i = 0 ; i < length; i++)
	{
		cec_printf0("0x%02x ", *value++);
	}

	cec_printf0("\b\n");
}


static void
dump_cea861(const uint8_t *const buffer)
{
	uint8_t i = 0;

	const struct edid_detailed_timing_descriptor *dtd = NULL;
	const struct cea861_timing_block *const ctb =
		(struct cea861_timing_block *) buffer;
	const uint8_t dof = offsetof(struct cea861_timing_block, data);

	dump_section("cea extension header",  buffer, 0x00, 0x04);

	if (ctb->dtd_offset - dof)
		dump_section("data block collection", buffer, 0x04, ctb->dtd_offset - dof);

	dtd = (struct edid_detailed_timing_descriptor *) (buffer + ctb->dtd_offset);
	for (i = 0; dtd->pixel_clock; i++, dtd++)
	{
		char *header = NULL;
		header = malloc(PRINT_LEN);

		sprintf(header, "detailed timing descriptor %u", i);
		dump_section(header, (uint8_t *) dtd, 0x00, sizeof(*dtd));
		free(header);
	}

	dump_section("padding",  buffer, (uint8_t *) dtd - buffer,
				 dof + sizeof(ctb->data) - ((uint8_t *) dtd - buffer));
	dump_section("checksum", buffer, 0x7f, 0x01);
	cec_printf0("\n");
}


int _aspect_ratio(const uint16_t hres, const uint16_t vres, char *ratio_str)
{
#define HAS_RATIO_OF(x, y)  (hres == (vres * (x) / (y)) && !((vres * (x)) % (y)))

	if (HAS_RATIO_OF(16, 10))
	{
		memcpy(ratio_str, "16:10", sizeof("16:10"));
		return 0;
	}
	if (HAS_RATIO_OF(4, 3))
	{
		memcpy(ratio_str, "4:3", sizeof("4:3"));
		return 0;
	}

	if (HAS_RATIO_OF(5, 4))
	{
		memcpy(ratio_str, "5:4", sizeof("5:4"));
		return 0;
	}
	if (HAS_RATIO_OF(16, 9))
	{
		memcpy(ratio_str, "16:9", sizeof("16:9"));
		return 0;
	}

	memcpy(ratio_str, "unknown", sizeof("unknown"));
	return -1;
}

static inline char *
_edid_timing_string(const struct edid_detailed_timing_descriptor *const dtb)
{
	char *timing = NULL;
	char ratio_str[16];
	const uint16_t hres = edid_detailed_timing_horizontal_active(dtb);
	const uint16_t vres = edid_detailed_timing_vertical_active(dtb);
	const uint32_t htotal = hres + edid_detailed_timing_horizontal_blanking(dtb);
	const uint32_t vtotal = vres + edid_detailed_timing_vertical_blanking(dtb);

	timing = malloc(PRINT_LEN);
	_aspect_ratio(hres, vres, ratio_str);

	sprintf(timing,
			"%ux%u%c at %dHz (%s)",
			hres,
			vres,
			dtb->interlaced ? 'i' : 'p',
			(int)(edid_detailed_timing_pixel_clock(dtb) / (vtotal * htotal)),
			ratio_str);

	/*
	    asprintf(&timing,
	             "%ux%u%c at %.fHz (%s)",
	             hres,
	             vres,
	             dtb->interlaced ? 'i' : 'p',
	             (double) edid_detailed_timing_pixel_clock(dtb) / (vtotal * htotal),
	             _aspect_ratio(hres, vres));
	*/

	return timing;
}

static inline char *
_edid_mode_string(const struct edid_detailed_timing_descriptor *const dtb)
{
	char *modestr = NULL;

	const uint16_t xres = edid_detailed_timing_horizontal_active(dtb);
	const uint16_t yres = edid_detailed_timing_vertical_active(dtb);
	unsigned int pixclk = edid_detailed_timing_pixel_clock(dtb);
	const uint16_t lower_margin = edid_detailed_timing_vertical_sync_offset(dtb);
	const uint16_t right_margin = edid_detailed_timing_horizontal_sync_offset(dtb);
	modestr = malloc(PRINT_LEN);



	sprintf(modestr,
			"\"%u x %u\" %u %u %u %u %u %u %u %u %u %c hsync %c vsync",
			/* resolution */
			xres, yres,

			/* dot clock frequence (MHz) */
			HZ_2_MHZ(pixclk),
			/* horizontal timings */
			xres,
			xres + right_margin,
			xres + right_margin + edid_detailed_timing_horizontal_sync_pulse_width(dtb),
			xres + edid_detailed_timing_horizontal_blanking(dtb),
			/* vertical timings */
			yres,
			yres + lower_margin,
			yres + lower_margin + edid_detailed_timing_vertical_sync_pulse_width(dtb),
			yres + edid_detailed_timing_vertical_blanking(dtb),
			/* sync direction */
			dtb->signal_pulse_polarity ? '+' : '-',
			dtb->signal_serration_polarity ? '+' : '-');

	return modestr;
}



void
edid_manufacturer(const struct edid *const edid, char manufacturer[4])
{
	manufacturer[0] = '@' + ((edid->manufacturer & 0x007c) >> 2);
	manufacturer[1] = '@' + ((((edid->manufacturer & 0x0003) >> 00) << 3) | (((edid->manufacturer & 0xe000) >> 13) << 0));
	manufacturer[2] = '@' + ((edid->manufacturer & 0x1f00) >> 8);
	manufacturer[3] = '\0';
}

int
edid_gamma(const struct edid *const edid)
{
	return (edid->display_transfer_characteristics + 100);
}

bool
edid_detailed_timing_is_monitor_descriptor(const struct edid *const edid,
		const uint8_t timing)
{
	const struct edid_monitor_descriptor *const mon =
			&edid->detailed_timings[timing].monitor;

	//assert(timing < ARRAY_SIZE(edid->detailed_timings));

	return mon->flag0 == 0x0000 && mon->flag1 == 0x00 && mon->flag2 == 0x00;
}


uint32_t
edid_standard_timing_horizontal_active(const struct edid_standard_timing_descriptor *const desc)
{
	return ((desc->horizontal_active_pixels + 31) << 3);
}

uint32_t
edid_standard_timing_vertical_active(const struct edid_standard_timing_descriptor *const desc)
{
	const uint32_t hres = edid_standard_timing_horizontal_active(desc);

	switch (desc->image_aspect_ratio)
	{
		case EDID_ASPECT_RATIO_16_10:
			return ((hres * 10) >> 4);
		case EDID_ASPECT_RATIO_4_3:
			return ((hres * 3) >> 2);
		case EDID_ASPECT_RATIO_5_4:
			return ((hres << 2) / 5);
		case EDID_ASPECT_RATIO_16_9:
			return ((hres * 9) >> 4);
	}

	return hres;
}

uint32_t
edid_standard_timing_refresh_rate(const struct edid_standard_timing_descriptor *const desc)
{
	return (desc->refresh_rate + 60);
}

struct edid_color_characteristics_data
edid_color_characteristics(const struct edid *const edid)
{
	const struct edid_color_characteristics_data characteristics =
	{
		.red = {
			.x = (edid->red_x << 2) | edid->red_x_low,
			.y = (edid->red_y << 2) | edid->red_y_low,
		},
		.green = {
			.x = (edid->green_x << 2) | edid->green_x_low,
			.y = (edid->green_y << 2) | edid->green_y_low,
		},
		.blue = {
			.x = (edid->blue_x << 2) | edid->blue_x_low,
			.y = (edid->blue_y << 2) | edid->blue_y_low,
		},
		.white = {
			.x = (edid->white_x << 2) | edid->white_x_low,
			.y = (edid->white_y << 2) | edid->white_y_low,
		},
	};

	return characteristics;
}


int
edid_decode_fixed_point(uint16_t value)
{
	double result = 0.0;
	uint8_t i = 0;

	//assert((~value & 0xfc00) == 0xfc00);    /* edid fraction is 10 bits */

	for (i = 0; value && (i < 10); i++, value >>= 1)
		result = result + ((value & 0x1) * (1.0 / (1 << (10 - i))));

	return (result * 1000);
}

static void
disp_edid1(const struct edid *const edid)
{
	const struct edid_monitor_range_limits *monitor_range_limits = NULL;
	edid_monitor_descriptor_string monitor_serial_number = {0};
	edid_monitor_descriptor_string monitor_model_name = {0};
	bool has_ascii_string = false;
	char manufacturer[4] = {0};

	struct edid_color_characteristics_data characteristics;
	const uint8_t vlen = edid->maximum_vertical_image_size;
	const uint8_t hlen = edid->maximum_horizontal_image_size;
	uint8_t i;
	char *ptr;

	static const char *const display_type[] =
	{
		[EDID_DISPLAY_TYPE_MONOCHROME] = "Monochrome or greyscale",
		[EDID_DISPLAY_TYPE_RGB]        = "sRGB colour",
		[EDID_DISPLAY_TYPE_NON_RGB]    = "Non-sRGB colour",
		[EDID_DISPLAY_TYPE_UNDEFINED]  = "Undefined",
	};

	edid_manufacturer(edid, manufacturer);
	characteristics = edid_color_characteristics(edid);


	for (i = 0; i < ARRAY_SIZE(edid->detailed_timings); i++)
	{
		const struct edid_monitor_descriptor *const mon =
				&edid->detailed_timings[i].monitor;

		if (!edid_detailed_timing_is_monitor_descriptor(edid, i))
			continue;

		switch (mon->tag)
		{
			case EDID_MONTIOR_DESCRIPTOR_MANUFACTURER_DEFINED:
				/* This is arbitrary data, just silently ignore it. */
				break;
			case EDID_MONITOR_DESCRIPTOR_ASCII_STRING:
				has_ascii_string = true;
				break;
			case EDID_MONITOR_DESCRIPTOR_MONITOR_NAME:

				strncpy(monitor_model_name, (char *) mon->data,
						sizeof(monitor_model_name) - 1);
				//*strchrnul(monitor_model_name, '\n') = '\0';
				ptr = strchr(monitor_model_name, '\n');
				if (ptr != NULL)
					*ptr = '\0';
				break;
			case EDID_MONITOR_DESCRIPTOR_MONITOR_RANGE_LIMITS:
				monitor_range_limits = (struct edid_monitor_range_limits *) &mon->data;
				break;
			case EDID_MONITOR_DESCRIPTOR_MONITOR_SERIAL_NUMBER:
				strncpy(monitor_serial_number, (char *) mon->data,
						sizeof(monitor_serial_number) - 1);
				//*strchrnul(monitor_serial_number, '\n') = '\0';
				ptr = strchr(monitor_serial_number, '\n');
				if (ptr != NULL)
					*ptr = '\0';
				break;
			default:
				//fprintf(stderr, "unknown monitor descriptor type 0x%02x\n",
				//        mon->tag);
				cec_printf0("unknown monitor descriptor type\n");
				break;
		}
	}

	cec_printf0("Monitor\n");

	cec_printf0("  Model name............... %s\n",
				*monitor_model_name ? monitor_model_name : "n/a");

	cec_printf0("  Manufacturer............. %s\n",
				manufacturer);

	cec_printf0("  Product code............. %u\n",
				*(uint16_t *) edid->product);

	if (*(uint32_t *) edid->serial_number)
	{
		cec_printf0("  Module serial number..... %x, %x, %x, %x\n",
					edid->serial_number[0],
					edid->serial_number[1],
					edid->serial_number[2],
					edid->serial_number[3]
				   );
	}

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  Plug and Play ID......... %s\n", NULL);
#endif

	cec_printf0("  Serial number............ %s\n",
				*monitor_serial_number ? monitor_serial_number : "n/a");

	cec_printf0("  Manufacture date......... %u", edid->manufacture_year + 1990);
	if (edid->manufacture_week <= 52)
		cec_printf0(", ISO week %u", edid->manufacture_week);
	cec_printf0("\n");

	cec_printf0("  EDID revision............ %u.%u\n",
				edid->version, edid->revision);

	cec_printf0("  Input signal type........ %s\n",
				edid->video_input_definition.digital.digital ? "Digital" : "Analog");

	if (edid->video_input_definition.digital.digital)
	{
		cec_printf0("  VESA DFP 1.x supported... %s\n",
					edid->video_input_definition.digital.dfp_1x ? "Yes" : "No");
	}
	else
	{
		/* TODO print analog flags */
	}

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  Color bit depth.......... %s\n", NULL);
#endif

	cec_printf0("  Display type............. %s\n",
				display_type[edid->feature_support.display_type]);

	cec_printf0("  Screen size.............. %u mm x %u mm (%d/10000  in)\n",
				CM_2_MM(hlen), CM_2_MM(vlen),
				CM_2_IN(sqrt(hlen * hlen + vlen * vlen)));

	cec_printf0("  Power management......... %s%s%s%s\n",
				edid->feature_support.active_off ? "Active off, " : "",
				edid->feature_support.suspend ? "Suspend, " : "",
				edid->feature_support.standby ? "Standby, " : "",

				(edid->feature_support.active_off ||
				 edid->feature_support.suspend    ||
				 edid->feature_support.standby) ? "\b\b  " : "n/a");

	cec_printf0("  Extension blocks......... %u\n",
				edid->extensions);

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  DDC/CI................... %s\n", NULL);
#endif

	cec_printf0("\n");

	if (has_ascii_string)
	{
		edid_monitor_descriptor_string string = {0};

		cec_printf0("General purpose ASCII string\n");

		for (i = 0; i < ARRAY_SIZE(edid->detailed_timings); i++)
		{
			const struct edid_monitor_descriptor *const mon =
					&edid->detailed_timings[i].monitor;

			if (!edid_detailed_timing_is_monitor_descriptor(edid, i))
				continue;

			if (mon->tag == EDID_MONITOR_DESCRIPTOR_ASCII_STRING)
			{
				char *ptr;
				strncpy(string, (char *) mon->data, sizeof(string) - 1);
				//*strchrnul(string, '\n') = '\0';
				ptr = strchr(string, '\n');
				if (ptr != 0)
					*ptr = '\0';

				cec_printf0("  ASCII string............. %s\n", string);
			}
		}

		cec_printf0("\n");
	}

	cec_printf0("Color characteristics\n");

	cec_printf0("  Default color space...... %ssRGB\n",
				edid->feature_support.standard_default_color_space ? "" : "Non-");

	cec_printf0("  Display gamma............ %d/100\n",
				edid_gamma(edid));

	cec_printf0("  Red chromaticity......... Rx %d/1000 - Ry %d/1000\n",
				edid_decode_fixed_point(characteristics.red.x),
				edid_decode_fixed_point(characteristics.red.y));

	cec_printf0("  Green chromaticity....... Gx %d/1000 - Gy %d/1000\n",
				edid_decode_fixed_point(characteristics.green.x),
				edid_decode_fixed_point(characteristics.green.y));

	cec_printf0("  Blue chromaticity........ Bx %d/1000 - By %d/1000\n",
				edid_decode_fixed_point(characteristics.blue.x),
				edid_decode_fixed_point(characteristics.blue.y));

	cec_printf0("  White point (default).... Wx %d/1000 - Wy %d/1000\n",
				edid_decode_fixed_point(characteristics.white.x),
				edid_decode_fixed_point(characteristics.white.y));

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  Additional descriptors... %s\n", NULL);
#endif

	cec_printf0("\n");

	cec_printf0("Timing characteristics\n");

	if (monitor_range_limits)
	{
		cec_printf0("  Horizontal scan range.... %u - %u kHz\n",
					monitor_range_limits->minimum_horizontal_rate,
					monitor_range_limits->maximum_horizontal_rate);

		cec_printf0("  Vertical scan range...... %u - %u Hz\n",
					monitor_range_limits->minimum_vertical_rate,
					monitor_range_limits->maximum_vertical_rate);

		cec_printf0("  Video bandwidth.......... %u MHz\n",
					monitor_range_limits->maximum_supported_pixel_clock * 10);
	}

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  CVT standard............. %s\n", NULL);
#endif

	cec_printf0("  GTF standard............. %sSupported\n",
				edid->feature_support.default_gtf ? "" : "Not ");

#if defined(DISPLAY_UNKNOWN)
	cec_printf0("  Additional descriptors... %s\n", NULL);
#endif

	cec_printf0("  Preferred timing......... %s\n",
				edid->feature_support.preferred_timing_mode ? "Yes" : "No");

	if (edid->feature_support.preferred_timing_mode)
	{
		char *string = NULL;

		string = _edid_timing_string(&edid->detailed_timings[0].timing);
		cec_printf0("  Native/preferred timing.. %s\n", string);
		free(string);

		string = _edid_mode_string(&edid->detailed_timings[0].timing);
		cec_printf0("    Modeline............... %s\n", string);
		free(string);
	}
	else
	{
		cec_printf0("  Native/preferred timing.. n/a\n");
	}

	cec_printf0("\n");

	cec_printf0("Standard timings supported\n");
	if (edid->established_timings.timing_720x400_70)
		cec_printf0("   720 x  400p @ 70Hz - IBM VGA\n");
	if (edid->established_timings.timing_720x400_88)
		cec_printf0("   720 x  400p @ 88Hz - IBM XGA2\n");
	if (edid->established_timings.timing_640x480_60)
		cec_printf0("   640 x  480p @ 60Hz - IBM VGA\n");
	if (edid->established_timings.timing_640x480_67)
		cec_printf0("   640 x  480p @ 67Hz - Apple Mac II\n");
	if (edid->established_timings.timing_640x480_72)
		cec_printf0("   640 x  480p @ 72Hz - VESA\n");
	if (edid->established_timings.timing_640x480_75)
		cec_printf0("   640 x  480p @ 75Hz - VESA\n");
	if (edid->established_timings.timing_800x600_56)
		cec_printf0("   800 x  600p @ 56Hz - VESA\n");
	if (edid->established_timings.timing_800x600_60)
		cec_printf0("   800 x  600p @ 60Hz - VESA\n");

	if (edid->established_timings.timing_800x600_72)
		cec_printf0("   800 x  600p @ 72Hz - VESA\n");
	if (edid->established_timings.timing_800x600_75)
		cec_printf0("   800 x  600p @ 75Hz - VESA\n");
	if (edid->established_timings.timing_832x624_75)
		cec_printf0("   832 x  624p @ 75Hz - Apple Mac II\n");
	if (edid->established_timings.timing_1024x768_87)
		cec_printf0("  1024 x  768i @ 87Hz - VESA\n");
	if (edid->established_timings.timing_1024x768_60)
		cec_printf0("  1024 x  768p @ 60Hz - VESA\n");
	if (edid->established_timings.timing_1024x768_70)
		cec_printf0("  1024 x  768p @ 70Hz - VESA\n");
	if (edid->established_timings.timing_1024x768_75)
		cec_printf0("  1024 x  768p @ 75Hz - VESA\n");
	if (edid->established_timings.timing_1280x1024_75)
		cec_printf0("  1280 x 1024p @ 75Hz - VESA\n");

	for (i = 0; i < ARRAY_SIZE(edid->standard_timing_id); i++)
	{
		const struct edid_standard_timing_descriptor *const desc =
				&edid->standard_timing_id[i];

		if (!memcmp(desc, EDID_STANDARD_TIMING_DESCRIPTOR_INVALID, sizeof(*desc)))
			continue;

		cec_printf0("  %u x %u%c @ %uHz - VESA STD\n",
					(unsigned int)edid_standard_timing_horizontal_active(desc),
					(unsigned int)edid_standard_timing_vertical_active(desc),
					'p',
					(unsigned int)edid_standard_timing_refresh_rate(desc));
	}

	cec_printf0("\n");
}


/* CEA861 routines */
/*! \todo move to cea861.c */

static inline void
disp_cea861_audio_data(const struct cea861_audio_data_block *const adb)
{
	uint8_t i = 0;

	const uint8_t descriptors = adb->header.length / sizeof(*adb->sad);

	cec_printf0("CE audio data (formats supported)\n");
	for (i = 0; i < descriptors; i++)
	{
		const struct cea861_short_audio_descriptor *const sad =
			(struct cea861_short_audio_descriptor *) &adb->sad[i];

		switch (sad->audio_format)
		{
			case CEA861_AUDIO_FORMAT_LPCM:
				cec_printf0("  LPCM    %u-channel, %s%s%s\b%s",
							sad->channels + 1,
							sad->flags.lpcm.bitrate_16_bit ? "16/" : "",
							sad->flags.lpcm.bitrate_20_bit ? "20/" : "",
							sad->flags.lpcm.bitrate_24_bit ? "24/" : "",

							((sad->flags.lpcm.bitrate_16_bit +
							  sad->flags.lpcm.bitrate_20_bit +
							  sad->flags.lpcm.bitrate_24_bit) > 1) ? " bit depths" : "-bit");
				break;
			case CEA861_AUDIO_FORMAT_AC_3:
				cec_printf0("  AC-3    %u-channel, %uk max. bit rate",
							sad->channels + 1,
							(sad->flags.maximum_bit_rate << 3));
				break;
			default:
				//fprintf(stderr, "unknown audio format 0x%02x\n",
				//        sad->audio_format);
				cec_printf_E( "unknown audio format 0x%02x\n", sad->audio_format);
				continue;
		}

		cec_printf0(" at %s%s%s%s%s%s%s\b kHz\n",
					sad->sample_rate_32_kHz ? "32/" : "",
					sad->sample_rate_44_1_kHz ? "44.1/" : "",
					sad->sample_rate_48_kHz ? "48/" : "",
					sad->sample_rate_88_2_kHz ? "88.2/" : "",
					sad->sample_rate_96_kHz ? "96/" : "",
					sad->sample_rate_176_4_kHz ? "176.4/" : "",
					sad->sample_rate_192_kHz ? "192/" : "");
	}

	cec_printf0("\n");
}

static inline void
disp_cea861_video_data(const struct cea861_video_data_block *const vdb)
{
	uint8_t i = 0;

	cec_printf0("CE video identifiers (VICs) - timing/formats supported\n");
	for (i = 0; i < vdb->header.length; i++)
	{
		const struct cea861_timing *const timing =
				&cea861_timings[vdb->svd[i].video_identification_code];

		cec_printf0(" %s CEA Mode %u: %4u x %4u%c @ %.fHz\n",
					vdb->svd[i].native ? "*" : " ",
					vdb->svd[i].video_identification_code,
					timing->hactive, timing->vactive,
					(timing->mode == INTERLACED) ? 'i' : 'p',
					timing->vfreq);
	}

	cec_printf0("\n");
}

static inline void
disp_cea861_vendor_data(const struct cea861_vendor_specific_data_block *vsdb)
{
	uint8_t i = 0;

	const uint8_t oui[] = { vsdb->ieee_registration[2],
							vsdb->ieee_registration[1],
							vsdb->ieee_registration[0]
						  };

	cec_printf0("CEA vendor specific data (VSDB)\n");
	cec_printf0("  IEEE registration number. 0x");
	for (i = 0; i < ARRAY_SIZE(oui); i++)
		cec_printf0("%02X", oui[i]);
	cec_printf0("\n");

	if (!memcmp(oui, HDMI_OUI, sizeof(oui)))
	{
		const struct hdmi_vendor_specific_data_block *const hdmi =
			(struct hdmi_vendor_specific_data_block *) vsdb;

		cec_printf0("  CEC physical address..... %u.%u.%u.%u\n",
					hdmi->port_configuration_a,
					hdmi->port_configuration_b,
					hdmi->port_configuration_c,
					hdmi->port_configuration_d);

		if (hdmi->header.length >= HDMI_VSDB_EXTENSION_FLAGS_OFFSET)
		{
			cec_printf0("  Supports AI (ACP, ISRC).. %s\n",
						hdmi->audio_info_frame ? "Yes" : "No");
			cec_printf0("  Supports 48bpp........... %s\n",
						hdmi->colour_depth_48_bit ? "Yes" : "No");
			cec_printf0("  Supports 36bpp........... %s\n",
						hdmi->colour_depth_36_bit ? "Yes" : "No");
			cec_printf0("  Supports 30bpp........... %s\n",
						hdmi->colour_depth_30_bit ? "Yes" : "No");
			cec_printf0("  Supports YCbCr 4:4:4..... %s\n",
						hdmi->yuv_444_supported ? "Yes" : "No");
			cec_printf0("  Supports dual-link DVI... %s\n",
						hdmi->dvi_dual_link ? "Yes" : "No");
		}

		if (hdmi->header.length >= HDMI_VSDB_MAX_TMDS_OFFSET)
		{
			if (hdmi->max_tmds_clock)
				cec_printf0("  Maximum TMDS clock....... %uMHz\n",
							hdmi->max_tmds_clock * 5);
			else
				cec_printf0("  Maximum TMDS clock....... n/a\n");
		}

		if (hdmi->header.length >= HDMI_VSDB_LATENCY_FIELDS_OFFSET)
		{
			if (hdmi->latency_fields)
			{
				cec_printf0("  Video latency %s........ %ums\n",
							hdmi->interlaced_latency_fields ? "(p)" : "...",
							(hdmi->video_latency - 1) << 1);
				cec_printf0("  Audio latency %s........ %ums\n",
							hdmi->interlaced_latency_fields ? "(p)" : "...",
							(hdmi->audio_latency - 1) << 1);
			}

			if (hdmi->interlaced_latency_fields)
			{
				cec_printf0("  Video latency (i)........ %ums\n",
							hdmi->interlaced_video_latency);
				cec_printf0("  Audio latency (i)........ %ums\n",
							hdmi->interlaced_audio_latency);
			}
		}
	}

	cec_printf0("\n");
}

static inline void
disp_cea861_speaker_allocation_data(const struct cea861_speaker_allocation_data_block *const sadb)
{
	const struct cea861_speaker_allocation *const sa = &sadb->payload;
	const uint8_t *const channel_configuration = (uint8_t *) sa;

	cec_printf0("CEA speaker allocation data\n");
	cec_printf0("  Channel configuration.... %u.%u\n",
				((channel_configuration[0] & 0xe9) << 1) +
				((channel_configuration[0] & 0x14) << 0) +
				((channel_configuration[1] & 0x01) << 1) +
				((channel_configuration[1] & 0x06) << 0),
				(channel_configuration[0] & 0x02));
	cec_printf0("  Front left/right......... %s\n",
				sa->front_left_right ? "Yes" : "No");
	cec_printf0("  Front LFE................ %s\n",
				sa->front_lfe ? "Yes" : "No");
	cec_printf0("  Front center............. %s\n",
				sa->front_center ? "Yes" : "No");
	cec_printf0("  Rear left/right.......... %s\n",
				sa->rear_left_right ? "Yes" : "No");
	cec_printf0("  Rear center.............. %s\n",
				sa->rear_center ? "Yes" : "No");
	cec_printf0("  Front left/right center.. %s\n",
				sa->front_left_right_center ? "Yes" : "No");
	cec_printf0("  Rear left/right center... %s\n",
				sa->rear_left_right_center ? "Yes" : "No");
	cec_printf0("  Front left/right wide.... %s\n",
				sa->front_left_right_wide ? "Yes" : "No");
	cec_printf0("  Front left/right high.... %s\n",
				sa->front_left_right_high ? "Yes" : "No");
	cec_printf0("  Top center............... %s\n",
				sa->top_center ? "Yes" : "No");
	cec_printf0("  Front center high........ %s\n",
				sa->front_center_high ? "Yes" : "No");

	cec_printf0("\n");
}

static void
disp_cea861(const struct edid_extension *const ext)
{
	const struct edid_detailed_timing_descriptor *dtd = NULL;
	const struct cea861_timing_block *const ctb =
		(struct cea861_timing_block *) ext;
	const uint8_t offset = offsetof(struct cea861_timing_block, data);
	uint8_t index = 0, i;

	/*! \todo handle invalid revision */

	cec_printf0("CEA-861 Information\n");
	cec_printf0("  Revision number.......... %u\n",
				ctb->revision);

	if (ctb->revision >= 2)
	{
		cec_printf0("  IT underscan............. %supported\n",
					ctb->underscan_supported ? "S" : "Not s");
		cec_printf0("  Basic audio.............. %supported\n",
					ctb->basic_audio_supported ? "S" : "Not s");
		cec_printf0("  YCbCr 4:4:4.............. %supported\n",
					ctb->yuv_444_supported ? "S" : "Not s");
		cec_printf0("  YCbCr 4:2:2.............. %supported\n",
					ctb->yuv_422_supported ? "S" : "Not s");
		cec_printf0("  Native formats........... %u\n",
					ctb->native_dtds);
	}

	dtd = (struct edid_detailed_timing_descriptor *) ((uint8_t *) ctb + ctb->dtd_offset);
	for (i = 0; dtd->pixel_clock; i++, dtd++)
	{
		char *string;

		/*! \todo ensure that we are not overstepping bounds */

		string = _edid_timing_string(dtd);
		cec_printf0("  Detailed timing #%u....... %s\n", i + 1, string);
		free(string);

		string = _edid_mode_string(dtd);
		cec_printf0("    Modeline............... %s\n", string);
		free(string);
	}

	cec_printf0("\n");

	if (ctb->revision >= 3)
	{
		do
		{
			const struct cea861_data_block_header *const header =
				(struct cea861_data_block_header *) &ctb->data[index];

			switch (header->tag)
			{
				case CEA861_DATA_BLOCK_TYPE_AUDIO:
				{
					const struct cea861_audio_data_block *const db =
						(struct cea861_audio_data_block *) header;

					disp_cea861_audio_data(db);
				}
				break;
				case CEA861_DATA_BLOCK_TYPE_VIDEO:
				{
					const struct cea861_video_data_block *const db =
						(struct cea861_video_data_block *) header;

					disp_cea861_video_data(db);
				}
				break;
				case CEA861_DATA_BLOCK_TYPE_VENDOR_SPECIFIC:
				{
					const struct cea861_vendor_specific_data_block *const db =
						(struct cea861_vendor_specific_data_block *) header;

					disp_cea861_vendor_data(db);
				}
				break;
				case CEA861_DATA_BLOCK_TYPE_SPEAKER_ALLOCATION:
				{
					const struct cea861_speaker_allocation_data_block *const db =
						(struct cea861_speaker_allocation_data_block *) header;

					disp_cea861_speaker_allocation_data(db);
				}
				break;
				default:
					//fprintf(stderr, "unknown CEA-861 data block type 0x%02x\n",
					//        header->tag);
					cec_printf_E("unknown CEA-861 data block type 0x%02x\n", header->tag);
					break;
			}

			index = index + header->length + sizeof(*header);
		}
		while (index < ctb->dtd_offset - offset);
	}

	cec_printf0("\n");
}


/* parse edid routines */

static const struct edid_extension_handler
{
	void (* const hex_dump)(const uint8_t *const);
	void (* const inf_disp)(const struct edid_extension *const);
} edid_extension_handlers[] =
{
	[EDID_EXTENSION_CEA] = { dump_cea861, disp_cea861 },
};

void parse_edid_info(const uint8_t *const data)
{
	//uint8_t i = 0;

	const struct edid *const edid = (struct edid *) data;
	//const struct edid_extension * const extensions =
	//    (struct edid_extension *) (data + sizeof(*edid));

	//dump_edid1((uint8_t *) edid);
	disp_edid1(edid);


#if 0 // print bug
	for (i = 0; i < edid->extensions; i++)
	{
		const struct edid_extension *const extension = &extensions[i];
		const struct edid_extension_handler *const handler =
				&edid_extension_handlers[extension->tag];

		if (!handler)
		{
			cec_printf0(
				"WARNING: block %u contains unknown extension (%x)\n",
				i, extensions[i].tag);
			continue;
		}

		if (handler->hex_dump)
			(*handler->hex_dump)((uint8_t *) extension);

		if (handler->inf_disp)
			(*handler->inf_disp)(extension);
	}
#endif
	cec_printf0("EDID parsing end\n");

}


