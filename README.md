# PiLar

Giants glow stick that react to shake.

## ARTNET DMX Specs

| Universe | Channel | Description            |
| -------- | ------- | ---------------------- |
| 1        | 0       | Control / FX selection |
| 1        | 1       | Brightness             |
| 1        | 2       | Speed                  |
| 1        | 3       | Orientation            |
| 1        | 4       | Mirroring              |
| 1        | 5...9   | FX Parameters 0 to 7   |
| 2 ... 3  | 0...432 | Individual RGB         |

### Control (channel 0)

- 000: Automatic cycle
- 001: One Color (RGB = params 0-1-2)
- 002: Cycle Patterns (Timing = param 0)
- 003: Slave mode (will ignore everything else and read from MUTHUR)
- 255: Manual: Will wait for instructions for individual leds on universes 2 & 3 from channel 0 to 432.

### General Parameters (Channels 1-3)

#### Brightness (Ch1)

000 == Off
255 == Brightness

#### Speed (Ch2)

000 == slowest
255 == fastest

#### Orientation (Ch3)

000-127: Origin will be at the bottom
128-255: Origin will be at the top

#### Mirroing (Ch4)

000-127: Mirror both LED
128-255: Disable mirroring

### FX Parameters (Channels 4-11)
