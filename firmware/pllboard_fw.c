/*
Copyright 2014  Christian Vogel <vogelchr@vogel.cx>

Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

Based on the "VirtualSerial" demo, to be found in the
LUFA (Lightweight USB Framework for AVRs) library.
http://www.fourwalledcubicle.com/LUFA.php
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "vserial-descriptors.h"

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

/* the "Olimexino 32U4" is not defined as a Board in LUFA, so we'll just
 * deal with LEDs and Buttons ourselves.
 *
 *	Component
 *	Designation     Signal     ATMEGA32U4                  Pin
 *	---------------:----------:---------------------------:---
 *	LED1 (Green)    D7(LED1)   PE6/AIN0/INT6                1
 *	LED2 (Yellow)   D9(LED2)   PB5/PCINT5/OC1A/#OC4B/ADC12 29
 *	TxLED (Green)   TXLED      PD5/XCK1/CTS                22
 *	RxLed (Yellow)  D17(RXLED) PB0/SS/PCINT0                8
 *	---------------:----------:---------------------------:--- */

volatile static char led_state;

enum vserial_leds {
	LED1 = 0x01,
	LED2 = 0x02,
	TXLED = 0x04,
	RXLED = 0x08
};

/* Function Prototypes: */
static void update_leds(void);
static void init_leds(void);

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

/*
 * Standard file stream for the CDC interface when set up, so that the virtual
 * CDC COM port can be  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;

static void
print_temp_rh_report(void)
{
	unsigned char data[5];
	int16_t temp;
	uint16_t rh;
	int i;

	i = am2302_get_raw_data(data);

	/* for debugging, print raw bytes */
	fprintf(&USBSerialStream,"AS2302 raw bytes: recv cnt=%d",i);
	for (i=0; i<sizeof(data); i++)
		fprintf(&USBSerialStream," %02x",data[i]);
	fputs("\r\n",&USBSerialStream);

	/* print temperature and relative humidity */
	i = am2302_get_result(&temp, &rh);
	fprintf(&USBSerialStream,
		"status: %d, temp=%d (*0.1 dC), rh=%d (*0.1%%)\r\n",
		i, temp, rh);
}

int main(void)
{
	SetupHardware();
	int16_t i;

	/*
	 * Create a regular character stream for the interface so that it can
	 * be used with the stdio.h functions
	 */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

//	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	am2302_init();

	for (;;)
	{

		/*
		 * Must throw away unused bytes from the host, or it will
		 * lock up while waiting for the device
		 */
		i = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		if (i != -1){ /* received a byte */
			if (i == '+') {
				fputs("Triggering conversion.",&USBSerialStream);
				am2302_trigger_read();
				continue;
			}

			if (i == '?'){
				print_temp_rh_report();
				continue;
			}

			/* else just echo character back to host */
			fputc(i, &USBSerialStream);
			/* will not block, I checked :-) */
		}

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

#define SET_CLEAR_BIT(val, bit, setunset) \
	(((val) & ~(bit)) | ((setunset) ? bit : 0))

static void
update_leds()
{
	PORTE = SET_CLEAR_BIT(PORTE, _BV(6), led_state & LED1);
	PORTB = SET_CLEAR_BIT(PORTB, _BV(5), led_state & LED2);
	PORTD = SET_CLEAR_BIT(PORTD, _BV(5), led_state & TXLED);
	PORTB = SET_CLEAR_BIT(PORTB, _BV(0), led_state & RXLED);
}

static void
init_leds()
{
	DDRE |= _BV(6); /* LED1 */
	DDRB |= _BV(5); /* LED2 */
	DDRD |= _BV(5); /* TXLED */
	DDRB |= _BV(0); /* RXLED */
	led_state = 0;
	update_leds();
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

	init_leds();
	update_leds();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	led_state |= LED1;
	update_leds();
}

/* Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	led_state &= ~LED1;
	update_leds();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	if (!ConfigSuccess)
		led_state &= ~LED1;
	update_leds();
}

/* Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

