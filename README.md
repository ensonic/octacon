# Octacon
A midi controller tailored for bitwig with 8 encoders, 8 knobs and 2 displays.

![wip image of the day](/docs/img/20250614.v3_build.jpg)

The lastest [video tour](https://www.youtube.com/watch?v=5QIr6vOy6ok) can be watched on youtube.

# Getting started

## Flashing a release

* download and install [pictool](https://github.com/raspberrypi/picotool)
* download an octacon [firmware release](https://github.com/ensonic/octacon/releases) or [build it yourself](#code)
* connect octacon to usb
* flash the device using this command under linux:
  ```shell
  stty 1200 /dev/ttyACM0 && sleep 2 && picotool load -f ~/Downloads/firmware.uf2 && picotool reboot
  ```

## Installing the extension

Download an octacon [daw extension](https://github.com/ensonic/octacon/releases)
or [build it yourself](#code) and copy it into the `Extensions` folder of your
Bitwig Studio installation:
* Linux: ~/Bitwig Studio/Extensions/
* Mac: ~/Documents/Bitwig Studio/Extensions/
* Windows: %USERPROFILE%\Documents\Bitwig Studio\Extensions\

For Bitwig Studio > 3.4, you can also drag the file from your downloads into the Bitwig Studio window.

In Bitwig Studio, go to Settings > Controller and add a new one. Pick `ensonic`
as the vendor and select `Octacon` as the hardware. Ensure both midi ports have
`Octacon` or `Octacon MIDI 1` selected.

## Usage

TODO:
* active/passive mode
* control and navigation mode

# Hardware & Firmware

* raspberry pico 2
* 8 endless dual wiper potentiometers with buttons (RV142FPF-40B1-15F-0B20K-004)
* 3 analog multiplexers (e.g. CD74HC4051)
* 8 NeoPixel LEDs (WS2812x)
* 2 128x64 OLED displays connected via SPI
* 1 capacitive touch control

The `pcb` subfolder has the the schematics and the pcb files for [easyeda](https://easyeda.com/).
You should be able to import them into the Std Edition.

## Enclosure

The `3d` subfolder has openScad files and rendered STL for 3d printing.

The front_test.scad is mostly for prototyping:
Front view | Side View
:---------:|:---------:
![front](/docs/img/20250325.enc_housing_front.jpg) | ![front](/docs/img/20250325.enc_housing_side.jpg)

## MIDI implementation

Under Linux you can watch the communication using `asqedump`:

```shell
aseqdump -l | grep Octacon
 32:0    Octacon                          Octacon MIDI 1
aseqdump -p32:0
Source  Event                  Ch  Data
 32:0   Control change          0, controller 9, value 51
 32:0   Control change          0, controller 9, value 52
```

### Control changes

The encoders send 7bit values on cc 9 to 16. In 14-bit mode, the lsb is send on
cc 41 to 49.
The encoder buttons send values {0=off, 64=on} on cc 17 to 25.

### Sysex

Since Octacon uses usb midi and is a hobby project, we're using the `7D`
(prototyping, test, private use and experimentation). 

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
* 03: (recv) led pattern: `03 + <pattern>`
* 04: (recv) info string: `04 + <len> + <text>`
* 05: (recv) param ticks: `05 + <ix> <value>`

# Code

## Firmware

The code uses [platformIO](https://platformio.org/). See [platformio.ini](platformio.ini) for the used libraries.

```shell
alias pio=$HOME/.platformio/penv/bin/pio
pio test
pio run -t upload
```

With regards to flashing from command-line, see:
  * https://github.com/maxgerhardt/platform-raspberrypi/issues/65
  * https://github.com/raspberrypi/picotool/issues/88

## DAW Extensions

The daw extension for Bitwig is written in java.

```shell
cd daw/bitwig
mvn install
```

# Debugging

Since we're using usb-midi, we can't use it for serial logging at the same time. I am simply using a nodemcu esp8266 as serial2. On the nodemcu connect EN to GND to disable the ESP. Then connect the nodemcu to the pico2 as follow:
nodemcu esp8266 | raspberry pico2 
:--------------:|:---------------:
GND  | GND
3.3V | 3.3V (optional)
TX   | UART1 TX, pin 6 
RX   | UART1 RX, pin 7

Now plug the nodemcu to usb and open /dev/ttyUSB0 to see the serial log.

# Links

I've got inspiration from many other places. Let me mention a few similar
projects:
* [Ottopot](https://gerotakke.de/ottopot/)
