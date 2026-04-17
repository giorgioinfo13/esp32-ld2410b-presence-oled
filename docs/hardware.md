# Hardware Notes

This project uses an ESP32-based controller, an LD2410B-P radar presence sensor, and an SH1106 OLED display.

## Firmware Configuration

The current code configures `HardwareSerial(2)` for the radar sensor with:

- Baud rate: `256000`
- ESP32 RX pin: `GPIO16`
- ESP32 TX pin: `GPIO17`

This means the radar module should be wired with:

- LD2410 TX -> ESP32 GPIO16
- LD2410 RX -> ESP32 GPIO17

## Project Notes Provided During Cleanup

- Sensor model: `LD2410B-P`
- VCC -> `3.3V`
- GND -> `GND`

## OLED Display

The firmware uses:

- `U8G2_SH1106_128X64_NONAME_F_HW_I2C`

This setup uses:

- SH1106 controller support
- 128x64 display layout
- ESP32 hardware I2C usage

The exact OLED wiring is not explicitly configured in code, so it likely relies on default ESP32 I2C pins.

Typical ESP32 I2C defaults are often:

- SDA: `GPIO21`
- SCL: `GPIO22`

OLED I2C modules commonly use address `0x3C`, but confirm this on the actual hardware.

## Power Notes

- The project notes indicate `3.3V` power for the radar module
- Confirm the voltage requirements of your exact LD2410B-P module revision before wiring
- Make sure the OLED module is also compatible with ESP32 logic and supply voltage

## Network and Telemetry

This firmware also uses:

- Wi-Fi station mode
- MQTT publishing to a broker on the local network

That means the device needs:

- Reliable Wi-Fi coverage
- A reachable MQTT broker
- Local credentials stored in `include/config.h`

## Troubleshooting

- If the OLED stays blank, verify power, I2C wiring, and the display controller type
- If radar data never updates, verify UART cross-wiring and the configured baud rate
- If MQTT values do not appear, check Wi-Fi credentials, broker reachability, and topic subscriptions
- If motion is reported but distance looks wrong, confirm the radar orientation and installation environment
