#ifndef __USB_H__
#define __USB_H__


typedef enum
{
	USB_EYE_HIGH_SPEED,
	USB_EYE_FULL_SPEED,
	USB_EYE_SWITCH_EHCI,
} eUSBEyeTest;


void usb_cfg_handle(eUSBEyeTest cfg_value);


void usb_mode_switch(bool host);

#endif	// __USB_H__

