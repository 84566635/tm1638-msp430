TM1638 driver for MSP430
========================
This driver uses USCIA0 or USCIB0, and some pins (see `tm.c` and `tm.h`).
`main.c` contains some demo code.

Hardware
--------
This is built for [this chinese module](http://dx.com/p/81873), which uses the TM1638 chip, has 8 7-segment digits, 8 bi-color leds, and 8 buttons.

Since this module uses a single pin for input and output data, 3 extra components are needed: 2 diodes (1N4148 or similar works fine), and 1 resistor (10K works for me). Wire like this:

```
                              ^ (VCC)
                              |
+-------------------+         R (10K)   +--------------+
| MSP430            |         |         |    TM1638    |
|     P1.2/UCA0SIMO |----|<---+---------| DIO          |
|     P1.1/UCA0SOMI |---->|---+         |              |
|      P1.4/UCA0CLK |-------------------| CLK          |
|              P1.0 |-------------------| STB0         |
+-------------------+                   +--------------+
```

Known Limitations
=================
The button-reading code is not really working now. Only button 1 is readable.
