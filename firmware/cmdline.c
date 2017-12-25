#include "cmdline.h"
#include "avr_i2c_master.h"
#include <avr/pgmspace.h>

FILE cmdline_out_stream;
FILE *pout = &cmdline_out_stream;

static unsigned char cmdline_buf[32];
static unsigned char cmdline_len;

enum cmdline_wait_usb {
	WAIT_USB_NONE,
	WAIT_USB_READ,
	WAIT_USB_WRITE
};

enum cmdline_wait_usb cmdline_wait_usb;

static uint8_t nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - ('a' + 10);
	if (c >= 'A' && c <= 'F')
		return c - ('A' + 10);
	return 0;
}

static char to_hex(uint8_t c) {
	if (c < 10)
		return '0' + c;
	return 'a' + (c-10);
}

void
cmdline_parse(void)
{
	uint32_t i2c_addr, i2c_reg, i2c_val;
	int i;

	if (cmdline_len == 0)
		return;

	if (cmdline_len == 1 && cmdline_buf[0]=='?') {
		fprintf_P(pout,PSTR(
			"?: this help\r\n"
			"rAARR   start i2c register read from addr AA register #RR\r\n"
			"R       show i2c register read result\r\n"
			"wAARRVV write i2c register, value VV\r\n"
			"i       show i2c debug flags\r\n"));
		return;
	}

	if (cmdline_len == 5 && cmdline_buf[0]=='r') {
		i2c_addr = (nibble(cmdline_buf[1])<<4) | nibble(cmdline_buf[2]);
		i2c_reg  = (nibble(cmdline_buf[3])<<4) | nibble(cmdline_buf[4]);
		fprintf_P(pout,PSTR("read 0x%02x 0x%02x\n"),
			i2c_addr, i2c_reg);
		avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(i2c_addr);
		avr_i2c_master_buf[1] = i2c_reg;
		avr_i2c_master_buflen = 2; /* 3 bytes in total are used */
		avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_AUTORD);
		cmdline_wait_usb = WAIT_USB_READ;
		return;
	}

	if (cmdline_len == 7 && cmdline_buf[0]=='w') {
		i2c_addr = (nibble(cmdline_buf[1])<<4) | nibble(cmdline_buf[2]);
		i2c_reg  = (nibble(cmdline_buf[3])<<4) | nibble(cmdline_buf[4]);
		i2c_val  = (nibble(cmdline_buf[5])<<4) | nibble(cmdline_buf[6]);

		fprintf_P(pout,PSTR("write 0x%02x 0x%02x 0x%02x\r\n"),
			i2c_addr, i2c_reg, i2c_val);

		avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(i2c_addr);
		avr_i2c_master_buf[1] = i2c_reg;
		avr_i2c_master_buf[2] = i2c_val;
		avr_i2c_master_buflen = 3;
		avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_STOP);
		cmdline_wait_usb = WAIT_USB_WRITE;
		return;
	}

	if (cmdline_len == 1 && cmdline_buf[0] == 'i') {
		fprintf_P(pout,
			PSTR("i2c status 0x%02x flags 0x%02x buflen 0x%02x\r\n"),
			avr_i2c_master_last_twsr,
			avr_i2c_master_get_flags(),
			avr_i2c_master_buflen);
		for (i=0; i<avr_i2c_master_buflen; i++) {
			uint8_t v = avr_i2c_master_buf[i];
			fputc(to_hex(v >> 4), pout);
			fputc(to_hex(v & 0x0f), pout);
		}
		fputc('\r', pout);
		fputc('\n', pout);
		return;
	}

	fprintf_P(pout,	PSTR("unknown command \""));
	fwrite(cmdline_buf, cmdline_len, 1, pout);
	fprintf_P(pout, PSTR("\", try ? for help\r\n"));
}

void
cmdline_eat_char(unsigned char c)
{
	if (c == 0x7f || c == 0x08) {
		if (cmdline_len > 0) {
			cmdline_len--;
			/* left, space, left */
			fprintf_P(pout,PSTR("\033[D \033[D"));
		}
	}
	if (c>=32 && c<0x7f) {
		if (cmdline_len < sizeof(cmdline_buf)) {
			cmdline_buf[cmdline_len++] = c;
			fputc(c,pout);
		}
	}
	if (c == '\r' || c == '\f') {
		fputc('\r', pout);
		fputc('\n', pout);
		if (c == '\r')
			cmdline_parse();
		cmdline_len = 0;
		fprintf_P(pout,PSTR("\r\n>"));
	}
}

void
cmdline_tick()
{
	uint32_t flags;
	int i;

	if (cmdline_wait_usb != WAIT_USB_NONE) {
		flags = avr_i2c_master_get_flags();
		if (!(flags & AVR_I2C_MASTER_RUNNING)) {
			fputc('\r', pout);
			fputc('\n', pout);

			if (flags & AVR_I2C_MASTER_ERR)
				fprintf_P(pout,
					PSTR("USB ERROR flags=%02x "), flags);
			else
				fprintf_P(pout,PSTR("USB:"));

			for (i=1; i<avr_i2c_master_buflen; i++) {
				uint8_t v = avr_i2c_master_buf[i];
				fputc(to_hex(v >> 4), pout);
				fputc(to_hex(v & 0x0f), pout);
			}

			fputc('\r', pout);
			fputc('\n', pout);
			fputc('>', pout);
			fwrite(cmdline_buf, cmdline_len, 1, pout);

			cmdline_wait_usb = WAIT_USB_NONE;
		}

	}
}