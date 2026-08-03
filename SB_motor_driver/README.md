# SB_motor_driver

A virtual monitoring system for the Keya KYDBL4850-2E dual-channel brushless DC motor driver used in Sunnybotics robots. Simulates the driver reporting status over RS232, reads telemetry data, and monitors both motors live for faults.

## Files
- `driver_dashboard.py` — Streamlit dashboard for live motor monitoring
- `driver_monitor.py` — Core monitoring logic and fault detection
- `driver_simulation.py` — Simulates Keya driver responses over RS232

## How to Run
Install dependencies:
