import streamlit as st
import plotly.graph_objects as go
import time
import random
from driver_simulation import KBL2430Driver
from driver_monitor import read_motor

YELLOW  = "#F5C518"
WHITE   = "#FFFFFF"
DARK    = "#0E0E0E"
GRAY    = "#1C1C1C"
DIMGRAY = "#888888"

st.set_page_config(page_title="KBL2430-2E Motor Monitor", page_icon="⚙", layout="wide")

st.markdown(f"""
    <style>
        body, .stApp {{ background-color: {DARK}; }}
        h1, h2, h3 {{ color: {WHITE}; font-family: 'Arial', sans-serif; }}
        .stButton > button {{
            background-color: {YELLOW};
            color: black;
            font-weight: 700;
            border: none;
            padding: 10px 24px;
            border-radius: 4px;
            letter-spacing: 0.5px;
        }}
        .stButton > button:hover {{ background-color: #d4a800; }}
        div[data-testid="stToast"] {{
            background-color: rgba(245, 197, 24, 0.15);
            border: 1px solid {YELLOW};
            color: {YELLOW};
            font-weight: 700;
            letter-spacing: 1px;
        }}
    </style>
""", unsafe_allow_html=True)

st.markdown(f"<h1 style='font-size:28px; margin-bottom:0'>SUNNYBOTICS</h1>", unsafe_allow_html=True)
st.markdown(f"<p style='color:{DIMGRAY}; margin-top:0; font-size:13px; letter-spacing:2px'>KBL2430-2E DUAL MOTOR MONITOR</p>", unsafe_allow_html=True)
st.markdown("<hr style='border-color:#222; margin: 8px 0 20px 0'>", unsafe_allow_html=True)

popup_slot = st.empty()

if st.button("START MONITORING"):
    popup_slot.empty()
    st.session_state.run = True

if "run" not in st.session_state:
    st.markdown(f"<p style='color:{DIMGRAY}'>Press START MONITORING to begin.</p>", unsafe_allow_html=True)
    st.stop()

# ── Build the driver and decide on a random fault plan ──
driver = KBL2430Driver()

fault_options = ["MOTOR_OVERHEAT", "CONTROLLER_OVERHEAT", "OVER_VOLTAGE",
                 "LOW_VOLTAGE", "LOCKED_ROTOR", "SENSOR_DISCONNECT", None]

# each motor independently might get a fault, at a random step
motor1_fault = random.choice(fault_options)
motor2_fault = random.choice(fault_options)
motor1_fault_step = random.randint(20, 60)
motor2_fault_step = random.randint(20, 60)

# ── Sidebar fault log ──
st.sidebar.markdown(f"<h3 style='font-size:13px; letter-spacing:2px; color:{DIMGRAY}'>FAULT LOG</h3>", unsafe_allow_html=True)
sidebar_slot = st.sidebar.empty()

# ── Layout: two motors side by side ──
col1, col2 = st.columns(2)

with col1:
    st.markdown(f"<h3 style='font-size:13px; letter-spacing:2px; color:{DIMGRAY}'>MOTOR 1</h3>", unsafe_allow_html=True)
    chart1 = st.empty()
    status1 = st.empty()

with col2:
    st.markdown(f"<h3 style='font-size:13px; letter-spacing:2px; color:{DIMGRAY}'>MOTOR 2</h3>", unsafe_allow_html=True)
    chart2 = st.empty()
    status2 = st.empty()

# ── History for plotting ──
history = {
    1: {"rpm": [], "motor_temp": [], "controller_temp": [], "current": []},
    2: {"rpm": [], "motor_temp": [], "controller_temp": [], "current": []},
}

faults_seen = set()

def make_chart(hist):
    fig = go.Figure()
    fig.add_trace(go.Scatter(y=hist["rpm"], name="RPM", line=dict(color="#F5C518", width=1.5)))
    fig.add_trace(go.Scatter(y=hist["motor_temp"], name="Motor Temp", line=dict(color="#FFFFFF", width=1.5)))
    fig.add_trace(go.Scatter(y=hist["controller_temp"], name="Ctrl Temp", line=dict(color="#888888", width=1.5)))
    fig.add_trace(go.Scatter(y=hist["current"], name="Current %", line=dict(color="#444444", width=1.5)))
    fig.update_layout(
        height=320,
        paper_bgcolor="#0E0E0E",
        plot_bgcolor="#0E0E0E",
        font=dict(color="#888888", size=10),
        margin=dict(l=0, r=0, t=10, b=0),
        legend=dict(orientation="h", font=dict(size=9, color="#888888"), bgcolor="rgba(0,0,0,0)"),
        xaxis=dict(showgrid=False, color="#444"),
        yaxis=dict(showgrid=True, gridcolor="#1C1C1C", color="#444"),
    )
    return fig

# ── Live loop ──
for step in range(80):
    # apply faults at their scheduled step
    if motor1_fault and step == motor1_fault_step:
        driver.motor1.inject_fault(motor1_fault)
    if motor2_fault and step == motor2_fault_step:
        driver.motor2.inject_fault(motor2_fault)

    driver.update(target_rpm_1=2000, target_rpm_2=1800)

    m1 = read_motor(driver, 1)
    m2 = read_motor(driver, 2)

    for ch, m in [(1, m1), (2, m2)]:
        history[ch]["rpm"].append(m["rpm"])
        history[ch]["motor_temp"].append(m["motor_temp"] if m["motor_temp"] is not None else 0)
        history[ch]["controller_temp"].append(m["controller_temp"])
        history[ch]["current"].append(m["current_pct"])

    chart1.plotly_chart(make_chart(history[1]), use_container_width=True, key=f"c1_{step}")
    chart2.plotly_chart(make_chart(history[2]), use_container_width=True, key=f"c2_{step}")

    # fire alerts for any new faults
    new_fault = False
    for ch, m in [(1, m1), (2, m2)]:
        for f in m["faults"]:
            key = f"MOTOR {ch}: {f}"
            if key not in faults_seen:
                faults_seen.add(key)
                st.toast(key)
                new_fault = True

    # rebuild the persistent sidebar log whenever a new fault appears
    if new_fault:
        log_html = ""
        for key in sorted(faults_seen):
            log_html += (
                f"<div style='background-color:{GRAY}; border-left:3px solid {YELLOW}; "
                f"padding:10px 14px; margin-bottom:8px; border-radius:4px; color:{YELLOW}; "
                f"font-size:12px; font-weight:700; letter-spacing:1px'>{key}</div>"
            )
        sidebar_slot.markdown(log_html, unsafe_allow_html=True)

    status1.markdown(f"<p style='color:{DIMGRAY}; font-size:13px'>RPM <span style='color:{YELLOW}'>{m1['rpm']}</span> | {m1['bus_voltage']}V</p>", unsafe_allow_html=True)
    status2.markdown(f"<p style='color:{DIMGRAY}; font-size:13px'>RPM <span style='color:{YELLOW}'>{m2['rpm']}</span> | {m2['bus_voltage']}V</p>", unsafe_allow_html=True)

    time.sleep(0.1)

total_faults = len(faults_seen)
popup_slot.markdown(f"""
    <div style='
        position: fixed;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        background-color: rgba(14, 14, 14, 0.97);
        border: 1px solid {YELLOW};
        border-radius: 6px;
        padding: 48px 64px;
        text-align: center;
        z-index: 9999;
    '>
        <p style='color:{DIMGRAY}; font-size:11px; letter-spacing:3px; margin:0 0 12px 0'>MONITORING COMPLETE</p>
        <p style='color:{YELLOW}; font-size:48px; font-weight:700; margin:0; line-height:1'>{total_faults}</p>
        <p style='color:{WHITE}; font-size:14px; letter-spacing:2px; margin:8px 0 0 0'>FAULT{"S" if total_faults != 1 else ""} DETECTED</p>
    </div>
""", unsafe_allow_html=True)