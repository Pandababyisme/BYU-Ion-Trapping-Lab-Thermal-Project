import serial
import time
from datetime import datetime
from pathlib import Path

PORT = "COM4"
BAUD = 9600

SAMPLE_INTERVAL_SECONDS = 30 * 60   # 30 minutes

filename = Path(__file__).with_name("pt100_data.csv")

print("Starting PT100 data logger...")
print("Port:", PORT)
print("Baud:", BAUD)
print("Output file:", filename)
print("Sampling once every 30 minutes.")
print("Opening serial port...")

try:
    ser = serial.Serial(PORT, BAUD, timeout=2)
except Exception as e:
    print("ERROR: Could not open serial port.")
    print(e)
    print()
    print("Make sure Arduino Serial Monitor and Serial Plotter are closed.")
    raise SystemExit

time.sleep(2)

print("Serial port opened.")
print("It will keep recording until you press Ctrl+C.")
print()

try:
    with open(filename, "a", newline="") as file:
        while True:
            print("Taking one sample now...")

            # Read until we get a real data line
            line = ""
            while not line:
                line = ser.readline().decode("utf-8", errors="ignore").strip()

            real_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            new_line = real_time + "," + line

            print(new_line)
            file.write(new_line + "\n")
            file.flush()

            print("One sample saved. Waiting 30 minutes...")
            print()

            time.sleep(SAMPLE_INTERVAL_SECONDS)

except KeyboardInterrupt:
    print()
    print("Recording stopped by user.")

finally:
    ser.close()
    print("Serial port closed.")
    print("Data saved to:", filename)