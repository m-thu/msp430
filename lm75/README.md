LM75 temperature sensor
=======================

## Wiring (MSP-EXP430G2 LaunchPad)
```
P1.2 (TX)  -----> RX (USB<->UART)

P1.6 (SCL) <----> SCL (LM75, pulled up to Vcc)
P1.7 (SDA) <----> SDA (LM75, pulled up to Vcc)
VCC        ------ VS  (LM75)
GND        ------ GND (LM75)
[NC]       <----- OS  (LM75)
GND/VCC    ------ A0  (LM75)
GND/VCC    ------ A1  (LM75)
GND/VCC    ------ A2  (LM75)

```

## References
[LM75 datasheet (Maxim)](https://datasheets.maximintegrated.com/en/ds/LM75.pdf)

[LM75 datasheet (TI)](http://www.ti.com/lit/ds/symlink/lm75b.pdf)

[I2C-bus specification and user manual](http://www.nxp.com/documents/user_manual/UM10204.pdf)
