Si5351C PLL + ATmega32U4 PLL / OCXO-Control / Clock Generator

A PCB to provide a Si5351C clock generator IC with power via USB
and also have a ATmega32U4 control it.

The PLL chip can be provided two clocks:

 (1) standalone XTAL for Si5351C
      -> populate XT2

 (2a) external reference input (X8)
     (top right SMA connector when looking at top layer of PCB)

     -> Populate R18 (termination for SMA X8)
     -> Populate R27 (connects X8 with J3, Pin7)
     -> Populate R21, R22 (voltage divider to match Si5351C clkin voltage)

 (2b) quartz (TCXO, OCXO) oscillator connected to J3

     -> Populate R21, R22 (voltage divider to match Si5351C clkin voltage)
     -> A MCP47x6 DAC can be populated to provide a control voltage

If the clock (2a/2b) should be disciplined, this can be achieved
by having the ATmega32U4 determine the clock phase.

 (1) have a 1PPS input on the reference input (X8)

     -> Populate R17, R18 (termination, series resistor) for the
        atmega32u4 comparator input

     Then have the Si5351C output a suitable frequency to the atmega32u4
     on clock output 3, and...

     -> pouplate R25 to feed the external timer clock input T0,
        which has a maximum frequency of clkIO/2

          or

     -> populate R26, R20 and feed this frequency to the atmega32u4
        as the system clock. in this case, the otp memory of the
        Si5351C will have to be programmed so that it starts up
        with the clk3 active, and care must be taken not to turn
        off this clock, or the atmega32u4 will stop operating

     This way the ATmega32U4 can measure the clock phase of the 1PPS.

