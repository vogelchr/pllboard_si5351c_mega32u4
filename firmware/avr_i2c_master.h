#ifndef AVR_I2C_MASTER_H
#define AVR_I2C_MASTER_H

#include <stdint.h>

#define AVR_I2C_MASTER_ADDR_WRITE(a) (((a) << 1))       
#define AVR_I2C_MASTER_ADDR_READ(a)  (((a) << 1) | 0x01)

/* values for flags */

#define AVR_I2C_MASTER_IDLE         0x00  /* transaction if finished */
#define AVR_I2C_MASTER_ERR          0x40  /* transaction has error */
#define AVR_I2C_MASTER_ERR_NAK_ADDR 0x41  /* transaction ended, NAK during addr */
#define AVR_I2C_MASTER_ERR_NAK_DATA 0x42  /* transaction ended, NAK during data */
#define AVR_I2C_MASTER_ERR_ARB_LOST 0x43  /* transaction ended, arbitration lost */
#define AVR_I2C_MASTER_ERR_OTHER    0x44
#define AVR_I2C_MASTER_ERR_FATAL    0x45  /* we had to turn off the interrupts */

#define AVR_I2C_MASTER_RUNNING      0x80  /* transaction ongoing */
#define AVR_I2C_MASTER_XFER         0x81  /* transaction ongoing, should not free bus */
#define AVR_I2C_MASTER_XFER_STOP    0x82  /* transaction ongoing, should end with stop */
#define AVR_I2C_MASTER_XFER_AUTORD  0x83  /* after writing addr, read data... */

/* max. number of chars to hold during xfer, 1st byte always is (address << 1) */
#define AVR_I2C_MASTER_MAX_BUFLEN 18

extern unsigned char avr_i2c_master_buf[AVR_I2C_MASTER_MAX_BUFLEN];
extern uint8_t avr_i2c_master_bufp;
extern uint8_t avr_i2c_master_buflen;
extern uint8_t avr_i2c_master_last_twsr;

extern void avr_i2c_master_init(void);

extern void avr_i2c_master_trigger(uint8_t flags);
extern uint8_t avr_i2c_master_get_flags(void);


#endif
