/*
Copyright 2014  Christian Vogel <vogelchr@vogel.cx>

Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

Based on the "VirtualSerial" demo, to be found in the
LUFA (Lightweight USB Framework for AVRs) library.
http://www.fourwalledcubicle.com/LUFA.php
*/

#include "avr_i2c_master.h"
#include "cmdline.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>

#include "vserial-descriptors.h"

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

/*
 * LUFA CDC Class driver interface configuration and state information. This
 * structure is  passed to all CDC Class driver functions, so that multiple
 * instances of the same class  within a device can be differentiated from one
 * another.
 */

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface = {
	.Config = {
		.ControlInterfaceNumber   = 0,
		.DataINEndpoint           = {
			.Address          = CDC_TX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.DataOUTEndpoint = {
			.Address          = CDC_RX_EPADDR,
			.Size             = CDC_TXRX_EPSIZE,
			.Banks            = 1,
		},
		.NotificationEndpoint = {
			.Address          = CDC_NOTIFICATION_EPADDR,
			.Size             = CDC_NOTIFICATION_EPSIZE,
			.Banks            = 1,
		},
	},
};

int main(void)
{
	SetupHardware();
	int16_t i;

	/*
	 * Create a regular character stream for the interface so that it can
	 * be used with the stdio.h functions
	 */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &cmdline_out_stream);

	GlobalInterruptEnable();


	DDRF |= _BV(0) | _BV(1);

	for (;;)
	{
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		/*
		 * Must throw away unused bytes from the host, or it will
		 * lock up while waiting for the device
		 */
		i = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if (i == -1) {
			cmdline_tick();
			continue;
		}
		cmdline_eat_char(i);

#if 0
		if (i != -1){ /* received a byte */
			if (i == 'r') {
				fprintf(&USBSerialStream, "trigger i2c read...\r\n");
				avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(0x60);
				avr_i2c_master_buf[1] = 0x01;
				avr_i2c_master_buflen = 3;
				avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_AUTORD);
				continue;
			}

			if (i == 'w') {
				fprintf(&USBSerialStream, "trigger i2c write...\r\n");
				avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(0x60);
				avr_i2c_master_buf[1] = 0x02;
				avr_i2c_master_buf[2] = 0x04;
				avr_i2c_master_buflen = 3;
				avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_STOP);
				continue;
			}


			if (i == '?') {
				uint8_t s = avr_i2c_master_get_flags();
				fprintf_P(&USBSerialStream,
					PSTR("flags %02x len %d bufp %d\r\n"
						"TWCR=%02x TWSR=0x%02x (last=%02x)\r\n"),
					s,
					avr_i2c_master_buflen,
					avr_i2c_master_bufp,
					TWCR, TWSR, avr_i2c_master_last_twsr);
				continue;
			}

			/* else just echo character back to host */
			fputc(i, &USBSerialStream);
			/* will not block, I checked :-) */
		}
#endif

	}
}

/*
 * Configures the board hardware and chip peripherals
 * for the demo's functionality.
 */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();

	avr_i2c_master_init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	/* ... */
}

/* Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	/* ... */
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/* Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

