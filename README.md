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