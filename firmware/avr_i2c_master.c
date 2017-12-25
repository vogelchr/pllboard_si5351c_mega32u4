#include "avr_i2c_master.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

unsigned char avr_i2c_master_buf[AVR_I2C_MASTER_MAX_BUFLEN];
uint8_t avr_i2c_master_bufp;   /* current write pointer */
uint8_t avr_i2c_master_buflen; /* valid chars in buffer */
uint8_t avr_i2c_master_last_twsr;

static uint8_t avr_i2c_master_flags;

ISR(TWI_vect)
{
	unsigned char i2c_status = TWSR & 0xf8; /* exclude prescaler bits */
	unsigned char bufp = avr_i2c_master_bufp;
	unsigned char flags = avr_i2c_master_flags;
	unsigned char twcr = TWCR & ~(_BV(TWSTA)|_BV(TWSTO));

	PORTF |= _BV(1);
	avr_i2c_master_last_twsr = i2c_status;


	/* MT: master transmitter, MR: master receiver */

	switch (i2c_status) {
	case TW_START:     /* 0x08 start condition transmitted */
	case TW_REP_START: /* 0x10 repeated start condition transmitted */
		bufp=0;
		TWDR = avr_i2c_master_buf[bufp++];
		break;

	case TW_MT_SLA_ACK: /*  0x18 SLA+W transmitted, ACK received */
	case TW_MT_DATA_ACK: /* 0x28 data transmitted, ACK received */
		/* magic flag: we have written the register to read from,
		   now do a repeat-start to read the actual data */
		if (flags == AVR_I2C_MASTER_XFER_AUTORD && bufp == 2) {
			flags = AVR_I2C_MASTER_XFER_STOP;
			avr_i2c_master_buf[0] |= 0x01;  /* set bit for read op */
			twcr |= _BV(TWSTA);
			break;
		}
		if (bufp == avr_i2c_master_buflen) {
			if (flags == AVR_I2C_MASTER_XFER_STOP)
				twcr |= _BV(TWSTO);
			flags = AVR_I2C_MASTER_IDLE;
		} else {
			if (bufp >= AVR_I2C_MASTER_MAX_BUFLEN) {
				flags = AVR_I2C_MASTER_ERR_FATAL;
				twcr &= _BV(TWIE);
				goto out;
			}
			/* load data byte */
			TWDR = avr_i2c_master_buf[bufp++];
		}
		break;

	case TW_MT_SLA_NACK: /* 0x20 SLA+W transmitted, NACK received */
	case TW_MR_SLA_NACK: /* 0x48 SLA+R transmitted, NACK received */
		flags = AVR_I2C_MASTER_ERR_NAK_ADDR;
		twcr |= _BV(TWSTO); /* free bus */
		break;

	case TW_MT_DATA_NACK: /* 0x30 data transmitted, NACK received */
		flags = AVR_I2C_MASTER_ERR_NAK_DATA;
		twcr |= _BV(TWSTO); /* free bus */
		break;

	// identical case TW_MR_ARB_LOST: arbitration lost in SLA+R or NACK
	case TW_MT_ARB_LOST: /* 0x38 arbitration lost in SLA+W or data */
		flags = AVR_I2C_MASTER_ERR_ARB_LOST;
		break;

	case TW_MR_SLA_ACK:  /* 0x40 SLA+R transmitted, ACK received */
	case TW_MR_DATA_ACK: /* 0x50 data received, ACK returned */
		if (i2c_status == TW_MR_DATA_ACK) {
			if (bufp >= AVR_I2C_MASTER_MAX_BUFLEN) {
				flags = AVR_I2C_MASTER_ERR_FATAL;
				twcr &= _BV(TWIE);
				goto out;
			}
			avr_i2c_master_buf[bufp++] = TWDR;
		}

		/* <------ len = 4 ------------------------------> */
		/* [0:addr] [1:payload0] [2:payload1] [3:payload2] */
		/* after 1st data byte has been received, bufp is 2 */

		if (bufp < avr_i2c_master_buflen-1)
			twcr |= _BV(TWEA);
		else
			twcr &= ~_BV(TWEA); /* next byte will be the last one to receive */
		break;

	case TW_MR_DATA_NACK: /* 0x58 data received, NACK returned */
		if (bufp >= AVR_I2C_MASTER_MAX_BUFLEN) {
			flags = AVR_I2C_MASTER_ERR_FATAL;
			twcr &= _BV(TWIE);
			goto out;
		}
		avr_i2c_master_buf[bufp++] = TWDR;
		if (flags == AVR_I2C_MASTER_XFER_STOP)
			twcr |= _BV(TWSTO);
		flags = AVR_I2C_MASTER_IDLE;
		break;

	/* leaving out slave status codes */
	case TW_NO_INFO: /* no state information available */
	case TW_BUS_ERROR: /* illegal start or stop condition */
		flags = AVR_I2C_MASTER_ERR_OTHER;
	default:
		/* should not happen, disable interrupts */
		twcr &= ~_BV(TWIE);
		flags = AVR_I2C_MASTER_ERR_FATAL;
	}

out:
	avr_i2c_master_bufp = bufp;
	avr_i2c_master_flags = flags;
	TWCR = twcr | _BV(TWINT);   /* acknowledge interrupt */

	PORTF &= ~_BV(1);


}

void
avr_i2c_master_init()
{
	/* set i2c master clock frequency */
	/* to get 50kbps, we have to device clkIO=16MHz by 320 */
	/* baud = clkIO/(16 + 2*TWBR*4^TWPS), so TWBR=152, TWPS=0 */
	TWBR = 152;

	TWSR = 0;    /* status register, lower two RW bits are TWPS */
	TWCR = _BV(TWEN);
}

void
avr_i2c_master_trigger(uint8_t flags)
{
	cli();
	avr_i2c_master_flags = flags;
	avr_i2c_master_bufp = 0;

	if (avr_i2c_master_buflen > AVR_I2C_MASTER_MAX_BUFLEN)
		avr_i2c_master_buflen = AVR_I2C_MASTER_MAX_BUFLEN;

	TWCR = _BV(TWEN)|_BV(TWIE)|_BV(TWSTA);
	sei();
}

uint8_t
avr_i2c_master_get_flags()
{
	uint8_t ret;
	cli();
	ret = *((volatile uint8_t*)&avr_i2c_master_flags);
	sei();
	return ret;
}

