COD_V3 — Sunny Clean Bot Firmware

Platform: ESP32 | Protocol: Keya ASCII RS232 115200 8N1

Overview

Firmware for a solar panel cleaning robot. Controls four brushless DC motors (two traction, two brush) through two Keya KYDBL4850-2E dual-channel controllers. RC input is handled by a Radiolink AT9S receiver.

Task layout
Core	Task	Role
0	ModbusTransmission	2ms control loop — RC input, kinematics, motor commands
1	indicaciones	WS2812B status LED
1	keyaTelemetryTask	RS232 read-back from both drivers, 50ms/tick
LED status
Color	Meaning
Solid green	Normal
Flashing yellow	Warning — RC link lost or voltage out of range
Solid red	Critical fault — motors stopped

To identify a fault in the field, query ?FF\r over RS232 and convert the response to binary. Full bit table in docs/analysis.md.

Known limitations
Serial (UART0) is shared between the brush driver and Arduino debug output — active Serial.print() calls will corrupt brush telemetry reads.
Telemetry is RAM-only and lost on power cycle.
