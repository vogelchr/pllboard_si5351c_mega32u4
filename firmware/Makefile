#
#             LUFA Library
#     Copyright (C) Dean Camera, 2013.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

###
# this firmware requires LUFA V17.04.18 unpacked
# in ../lufa-LUFA-170418. Download from github at
#   https://github.com/abcminiuser/lufa
#
# vserial-descriptors.[ch] is a copy of
#   ../lufa-LUFA-170418/Demos/Device/ClassDriver/VirtualSerial/Descriptors.[ch]
#
# 
###

# Run "make help" for target help.

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = LEONARDO
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = pllboard_fw
SRC          = $(TARGET).c vserial-descriptors.c \
	       $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = ../lufa-LUFA-170418/LUFA/
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER
LD_FLAGS     =

# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
