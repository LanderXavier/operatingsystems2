import serial
import time
import threading

SERIAL_PORT = "/dev/ttyACM0"
BAUDRATE = 9600

ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)

# 🔥 Periodo RT
PERIOD = 0.05  

# Variable compartida
sensor_data = {"temp": 0, "hum": 0}
lock = threading.Lock()


# =========================
# 🧠 TASK 1: LEER SENSOR
# =========================
def task_read():
    global sensor_data

    while True:
        if ser.in_waiting:
            line = ser.readline().decode().strip()

            if "TEMP" in line:
                try:
                    parts = line.split(",")

                    temp = float(parts[0].split(":")[1])
                    hum = float(parts[1].split(":")[1])

                    with lock:
                        sensor_data["temp"] = temp
                        sensor_data["hum"] = hum

                    print(f"[READ] Temp={temp:.2f} Hum={hum:.2f}")

                except:
                    pass


# =========================
# ⚙️ TASK 2: CONTROL RT
# =========================
def task_control():
    next_time = time.time()

    while True:
        next_time += PERIOD

        with lock:
            temp = sensor_data["temp"]
            hum = sensor_data["hum"]

        # 🔥 LÓGICA DE CONTROL
        if temp > 30:
            ser.write(b"ROJO\n")
            print("[CTRL] ROJO (temp alta)")

        elif hum > 70:
            ser.write(b"AMARILLO\n")
            print("[CTRL] AMARILLO (hum alta)")

        else:
            ser.write(b"VERDE\n")
            print("[CTRL] VERDE (normal)")

        # ⏱ CONTROL DE TIEMPO REAL
        sleep_time = next_time - time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)


# =========================
# 🚀 RUN
# =========================
thread1 = threading.Thread(target=task_read)
thread2 = threading.Thread(target=task_control)

thread1.start()
thread2.start()
