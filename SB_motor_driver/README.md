KBL2430-2E Motor Driver Monitor
A virtual monitoring system for the Keya KBL2430-2E dual-channel motor driver used in Sunnybotics robots. It simulates the driver reporting status over RS232, reads that data, and monitors both motors live for faults.

How To Run                      
Install dependencies:               
pip3 install streamlit plotly  

Launch the dashboard:                
python3 -m streamlit run driver_dashboard.py                
Then press START MONITORING. Each run is randomized, so motors may run clean or hit faults like a locked rotor, overheating, or voltage problems. Run it a few times to see different outcomes.

Files                       
driver_simulation.py simulates the driver and outputs RS232-style byte packets
driver_monitor.py parses those packets and detects faults
driver_dashboard.py live dashboard showing both motors with a fault log
