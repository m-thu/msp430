MSP430
======

* [tsl2561](tsl2561/): TSL2561 digital light sensor (I2C)
* [lm75](lm75/): LM75 temperature sensor (I2C)
* [apa102](apa102/): APA102 RGB LED (SPI)

## MSP430 LaunchPad udev rules (Ubuntu 14.04):
```sh
sudo cp 90-msp430-launchpad.rules /etc/udev/rules.d/
```

## Install latest toolchain:
msp430-gcc (http://www.ti.com/tool/msp430-gcc-opensource)
```sh
chmod +x msp430-gcc-full-linux-installer-3.5.0.0.run
./msp430-gcc-full-linux-installer-3.5.0.0.run
```

mspdebug
```sh
git clone https://github.com/dlbeer/mspdebug.git
sudo apt-get install libusb-dev
cd mspdebug
make
sudo make install
```

## Install toolchain (Ubuntu 14.04):
```sh
sudo apt-get install binutils-msp430 gcc-msp430 gdb-msp430 msp430-libc msp430mcu mspdebug
```

## References
[MSP430G2553 datasheet](http://www.ti.com/lit/gpn/msp430g2553)

[MSP430x2xx Family User's Guide](http://www.ti.com/lit/pdf/slau144)

[MSP430 GCC User's Guide](http://www.ti.com/lit/pdf/slau646)
