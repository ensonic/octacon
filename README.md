# octacon
A midi controller tailored for bitwig with 8 encoders, 8 knobs and displays. Still pretty much WIP.

![wip image of the day](/docs/img/20250325.proto_build.jpg)

## hardware

* raspberry pico 2
* 8 digital encoders with buttons EC11 (need to find a better choice)
* 3 CD74HC4051 analog muxers
* 8 NeoPixel Diffused 5mm Through-Hole LEDs
* 2 128x64 OLED displays connected via SPI

## enclosure

The 3d subfolder has openScad files and rendered STL for 3d printing.

The front_test.scad is mostly for prototyping:
Front view | Side View
:---------:|:---------:
![front](/docs/img/20250325.enc_housing_front.jpg) | ![front](/docs/img/20250325.enc_housing_side.jpg)

## midi implementation

```
aseqdump -l | grep Octacon
 32:0    Octacon                          Octacon MIDI 1
aseqdump -p32:0
Source  Event                  Ch  Data
 32:0   Control change          0, controller 9, value 51
 32:0   Control change          0, controller 9, value 52
```

## control changes

The encoders send 7bit values on cc 9 to 16. In 14-bit mode, the lsb is send on
cc 41 to 49.
The encoder buttons send values {0=off, 64=on} on cc 17 to 25.

## sysex

Since Octacon uses usb midi and is a hobby project, we're using the `7D` (prototyping, test, private use and experimentation). 

The following commands are implemented:
* 00: (recv) parameter names: `00 + <ix> + <len> + <name>`
  * id: 0...7
  * len: len of name, truncated to fit the display
  * name: ascii string data
* 01: (recv) pretty parameter value: `01 + <ix> + <len> + <name>`
  * id: 0...7
  * len: len of name, truncated to fit the display
  * name: ascii string data
* 02: (recv) daw connected? : `02 + <connected>`
  * connected: 0 = off, anything else = on
  * when off, parameter names and pretty values are generated

# Code

The code uses [platformIO](https://platformio.org/). See [platformio.ini](platformio.ini) for the used libraries.

```
alias pio=$HOME/.platformio/penv/bin/pio
pio test
```

# Debugging

Since we're using usb-midi, we can't use it for serial logging. I am simply using a nodemcu esp8266 as serial2. On the nodemcu connect EN to GND to disaled the ESP. Then  connenct the nodemcu to the pico2 as follow:
nodemcu esp8266 | raspberry pico2 
:--------------:|:---------------:
GND  | GND
3.3V | 3.3V
TX   | UART1 TX, pin 6 
RX   | UART1 RX, pin 7

Now plug the nodemcu to usb and open /dev/ttyUSB0 to see the serial log.

# Links

I've got inspiration from many other places. Let me mention a few similar
projects:
* [Ottopot](https://gerotakke.de/ottopot/)
