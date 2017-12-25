#include "cmdline.h"
#include "avr_i2c_master.h"
#include <avr/pgmspace.h>

FILE cmdline_out_stream;
FILE *pout = &cmdline_out_stream;

static unsigned char cmdline_buf[32];
static unsigned char cmdline_len;

/* as a response to read/write commands */
enum cmdline_wait_usb {
	WAIT_USB_NONE,
	WAIT_USB_READ,
	WAIT_USB_WRITE
};

uint8_t dump_si5351c_active;
uint8_t dump_si5351c_offs;

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
	uint8_t i2c_addr, i2c_reg;
	uint8_t i;

	if (cmdline_len == 0)
		return;

	if (cmdline_len == 1 && cmdline_buf[0]=='?') {
		fprintf_P(pout,PSTR(
			"?         this help\r\n"
			"rAARR     start i2c register read from addr AA register #RR\r\n"
			"R         show i2c register read result\r\n"
			"wAARRVV.. write i2c register, values VV..\r\n"
			"i         show i2c debug flags\r\n"
			"d         dump si5351c registers\r\n"
			"X         reset i2c master logic\r\n"));
		return;
	}

	if (cmdline_len == 5 && cmdline_buf[0]=='r') {
		i2c_addr = (nibble(cmdline_buf[1])<<4) | nibble(cmdline_buf[2]);
		i2c_reg  = (nibble(cmdline_buf[3])<<4) | nibble(cmdline_buf[4]);
		fprintf_P(pout,PSTR("i2c read dev=0x%02x reg=0x%02x\n"),
			i2c_addr, i2c_reg);
		avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(i2c_addr);
		avr_i2c_master_buf[1] = i2c_reg;
		avr_i2c_master_buflen = 2; /* 3 bytes in total are used */
		avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_AUTORD);
		cmdline_wait_usb = WAIT_USB_READ;
		return;
	}

	if (cmdline_len >= 7 && cmdline_buf[0]=='w') {
		i2c_addr = (nibble(cmdline_buf[1])<<4) | nibble(cmdline_buf[2]);
		i2c_reg  = (nibble(cmdline_buf[3])<<4) | nibble(cmdline_buf[4]);

		for (i=0; i<(AVR_I2C_MASTER_MAX_BUFLEN-2); i++) {
			if (6+2*i >= cmdline_len) {
				fputc('@', pout);
				break;
			}
			fputc('.', pout);
			avr_i2c_master_buf[i+2] =
				(nibble(cmdline_buf[5+2*i]) << 4) |
				 nibble(cmdline_buf[6+2*i]);
		}

		fprintf_P(pout,
			PSTR("i2c write dev=0x%02x reg=0x%02x 0x%02x byte(s) payload\r\n"),
			i2c_addr, i2c_reg, i);

		avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(i2c_addr);
		avr_i2c_master_buf[1] = i2c_reg;
		avr_i2c_master_buflen = i+2;

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

	if (cmdline_len == 1 && cmdline_buf[0] == 'd') {
		avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(0x60);
		avr_i2c_master_buf[1] = 0x00;
		avr_i2c_master_buflen = 17;
		avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_AUTORD);
		dump_si5351c_active=1;
		dump_si5351c_offs=0;
		return;
	}

	if (cmdline_len == 1 && cmdline_buf[0] == 'X') {
		fprintf_P(pout,	PSTR("i2c master init\r\n"));
		avr_i2c_master_init();
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

	flags = avr_i2c_master_get_flags();


	if (dump_si5351c_active) {
		if (flags & AVR_I2C_MASTER_RUNNING)
			return;
		if (flags == AVR_I2C_MASTER_IDLE) {
			fprintf_P(pout,PSTR("\r%02x:"), dump_si5351c_offs);
			for (i=1; i<avr_i2c_master_buflen; i++) {
				uint8_t v = avr_i2c_master_buf[i];
				fputc(to_hex(v >> 4), pout);
				fputc(to_hex(v & 0x0f), pout);
			}
			fputc('\r', pout);
			fputc('\n', pout);
			dump_si5351c_offs += 16;
			if (dump_si5351c_offs) { /* no overrun to zero! */
				avr_i2c_master_buf[0] = AVR_I2C_MASTER_ADDR_WRITE(0x60);
				avr_i2c_master_buf[1] = dump_si5351c_offs;
				avr_i2c_master_buflen = 17;
				avr_i2c_master_trigger(AVR_I2C_MASTER_XFER_AUTORD);
			} else {
				dump_si5351c_active=0;
			}
		}
		return;
	}

	if (cmdline_wait_usb != WAIT_USB_NONE) {
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
		return;

	}
}