# octacon
midi controller tailored for bitwig with 8 encoders, 8 knobs and displays.

totally WIP.

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

The encoders send values on cc 9 to 16.
TODO: encoder buttons

## sysex

Since Octacon uses usb midi and is a hobby project, we're using the `7D` (prototyping, test, private use and experimentation). 

The following commands are implemented:
* 00: (recv) parameter names: `00 + <ix> + <len> + value`
  * id: 0...7
  * len: len of name, truncated to fit the display
  * name: ascii string data
