#!/bin/sh

avrdude -B 2MHz -p atmega32u4 -P usb -c dragon_isp \
	-U flash:w:pllboard_fw.hex

#	-U efuse:w:0xfb:m \
#	-U hfuse:w:0x99:m \
#	-U lfuse:w:0xae:m \
#

# avrdude: safemode: Fuses OK (E:FB, H:99, L:AE)

# lfuse 0xee : standard
# lfuse 0xae : output clkIO (16MHz) on TP for Port C7, "CLKO" (bottom of # PCB)
