# sunnybotics-simulator

A virtual health monitoring system for a solar panel cleaning robot. Simulates a full field cleaning session and monitors robot health in real time. Every run is randomized — the robot may encounter a brush stall, overheating motor, depleted water tank, low battery, or pump pressure loss, or it may complete the session cleanly.

## Files
- `simulator.py` — generates robot telemetry data, models 5 operating states, and injects faults randomly
- `fault_detection.py` — 5 rule-based fault detection checks that run on the telemetry stream
- `dashboard.py` — live Streamlit dashboard that animates the session and fires alerts in real time
- `telemetry.csv` — engineered telemetry data for the simulator

## How to Run
Install dependencies:
