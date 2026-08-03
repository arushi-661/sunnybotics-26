def parse_monitor1(packet):
    pwm             = packet[0]
    enabled         = packet[1]
    motor_temp      = None if packet[2] == 0xFF else packet[2]
    controller_temp = packet[3]
    return {
        "pwm": pwm,
        "enabled": enabled,
        "motor_temp": motor_temp,
        "controller_temp": controller_temp,
    }


def parse_monitor2(packet):
    rpm = (packet[0] << 8) | packet[1]
    current_pct = packet[2]
    return {
        "rpm": rpm,
        "current_pct": current_pct,
    }


def parse_voltage(packet):
    voltage = round(packet[0] / 4.06, 2)
    return {
        "bus_voltage": voltage,
    }

def detect_faults(motor_data):
    faults = []

    voltage = motor_data["bus_voltage"]
    if voltage > 30:
        faults.append("OVER VOLTAGE")
    elif voltage < 18:
        faults.append("LOW VOLTAGE")

    controller_temp = motor_data["controller_temp"]
    if controller_temp >= 100:
        faults.append("CONTROLLER OVERTEMP SHUTDOWN")
    elif controller_temp >= 90:
        faults.append("CONTROLLER OVERTEMP WARNING")

    motor_temp = motor_data["motor_temp"]
    if motor_temp is None:
        faults.append("MOTOR TEMP SENSOR DISCONNECTED")
    elif motor_temp > 80:
        faults.append("MOTOR OVERTEMP")

    rpm = motor_data["rpm"]
    current_pct = motor_data["current_pct"]
    if rpm < 25 and current_pct > 70 and motor_data["enabled"] == 1:
        faults.append("LOCKED ROTOR")

    return faults

def read_motor(driver, channel):
    p1 = driver.send_command(0x33, channel)
    p2 = driver.send_command(0x37, channel)
    p3 = driver.send_command(0x1b, channel)

    data = {}
    data.update(parse_monitor1(p1))
    data.update(parse_monitor2(p2))
    data.update(parse_voltage(p3))
    data["channel"] = channel
    data["faults"] = detect_faults(data)

    return data

if __name__ == "__main__":
    from motor_driver import KBL2430Driver

    driver = KBL2430Driver()

    for step in range(10):
        driver.update(target_rpm_1=2000, target_rpm_2=1500)

        motor1 = read_motor(driver, 1)
        motor2 = read_motor(driver, 2)

        print(f"Step {step}")
        print(f"  Motor 1 — RPM: {motor1['rpm']}, Temp: {motor1['motor_temp']}°C, "
              f"Voltage: {motor1['bus_voltage']}V, Faults: {motor1['faults']}")
        print(f"  Motor 2 — RPM: {motor2['rpm']}, Temp: {motor2['motor_temp']}°C, "
              f"Voltage: {motor2['bus_voltage']}V, Faults: {motor2['faults']}")
        print()