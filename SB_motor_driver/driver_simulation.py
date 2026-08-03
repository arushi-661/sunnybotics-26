import random
import time


class MotorChannel:
    def __init__(self, channel_id):
        self.channel_id = channel_id

        self.bus_voltage     = 24.0
        self.motor_temp      = 25.0
        self.controller_temp = 30.0
        self.rpm             = 0
        self.phase_current   = 0.0
        self.pwm             = 0
        self.enabled         = 1
        self.fault           = None

    def inject_fault(self, fault_type):
        self.fault = fault_type

    def update(self, target_rpm):
        if self.rpm < target_rpm:
            self.rpm = min(target_rpm, self.rpm + 100)
        elif self.rpm > target_rpm:
            self.rpm = max(target_rpm, self.rpm - 100)

        self.pwm = int((self.rpm / 3000) * 100)

        self.phase_current = round((self.pwm / 100) * 30 + random.uniform(-0.5, 0.5), 2)

        if self.motor_temp is not None:
            if self.rpm > 0:
                self.motor_temp += 0.05
            else:
                self.motor_temp = max(25, self.motor_temp - 0.1)

        if self.rpm > 0:
            self.controller_temp += 0.03
        else:
            self.controller_temp = max(30, self.controller_temp - 0.08)

        self.bus_voltage = round(24.0 - (self.phase_current / 30) * 1.5 + random.uniform(-0.1, 0.1), 2)

        if self.fault == "MOTOR_OVERHEAT":
            self.motor_temp += 0.6
        elif self.fault == "CONTROLLER_OVERHEAT":
            self.controller_temp += 0.7
        elif self.fault == "OVER_VOLTAGE":
            self.bus_voltage = 32.0 + random.uniform(-0.2, 0.2)
        elif self.fault == "LOW_VOLTAGE":
            self.bus_voltage = 16.0 + random.uniform(-0.2, 0.2)
        elif self.fault == "LOCKED_ROTOR":
            self.rpm = 0
            self.phase_current = 28.0 + random.uniform(-1, 1)
        elif self.fault == "SENSOR_DISCONNECT":
            self.motor_temp = None

        self.motor_temp = round(self.motor_temp, 2) if self.motor_temp is not None else None
        self.controller_temp = round(self.controller_temp, 2)

    def get_monitor1_packet(self):
        motor_temp_byte = 0xFF if self.motor_temp is None else int(self.motor_temp)
        return bytes([
            int(self.pwm),
            int(self.enabled),
            motor_temp_byte,
            int(self.controller_temp),
            int(self.controller_temp) - 2,
            int(self.controller_temp) - 3,
        ])

    def get_monitor2_packet(self):
        rpm = int(self.rpm)
        msb = (rpm >> 8) & 0xFF
        lsb = rpm & 0xFF
        current_pct = int((self.phase_current / 30) * 100)
        return bytes([msb, lsb, current_pct])

    def get_voltage_packet(self):
        voltage_ad = int(self.bus_voltage * 4.06)
        return bytes([voltage_ad & 0xFF])


class KBL2430Driver:
    def __init__(self):
        self.motor1 = MotorChannel(1)
        self.motor2 = MotorChannel(2)

    def update(self, target_rpm_1, target_rpm_2):
        self.motor1.update(target_rpm_1)
        self.motor2.update(target_rpm_2)

    def send_command(self, command_byte, channel):
        motor = self.motor1 if channel == 1 else self.motor2

        if command_byte == 0x33:
            return motor.get_monitor1_packet()
        elif command_byte == 0x37:
            return motor.get_monitor2_packet()
        elif command_byte == 0x1b:
            return motor.get_voltage_packet()
        else:
            return bytes([0xe3])


if __name__ == "__main__":
    driver = KBL2430Driver()
    driver.motor1.inject_fault("LOCKED_ROTOR")

    for step in range(5):
        driver.update(target_rpm_1=2000, target_rpm_2=1500)
        p2 = driver.send_command(0x37, channel=1)
        rpm = (p2[0] << 8) | p2[1]
        current_pct = p2[2]
        print(f"Step {step} — Motor 1 RPM: {rpm}, Current: {current_pct}%")