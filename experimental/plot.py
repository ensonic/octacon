#!/usr/bin/python3

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Serial port configuration (adjust as needed)
serial_port = '/dev/ttyUSB0'
baud_rate = 115200
try:
    ser = serial.Serial(serial_port, baud_rate)
    ser.flushInput()
    # TODO: figure how to properly snyc, right now we need to 
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit()

# Initialize plot
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
ys = []

def animate(i, xs, ys):
    try:
        data = ser.readline().decode().strip() # Read data, decode, and remove extra whitespace
        if not data: 
            return
        try:
            if not data.startswith('plt:'):
                return
            data=data.removeprefix('plt:')
            values=[((int(x)/2048.0)-1.0) for x in data.split(',',2)]
            xs.append(values[0])
            ys.append(values[1])

            # Limit x and y lists to a reasonable size
            xs = xs[-50:]
            ys = ys[-50:]

            # Draw x and y lists
            ax.clear()
            ax.set_xlim(-1.0, 1.0)
            ax.set_ylim(-1.0, 1.0)
            ax.plot(xs, ys)

        except ValueError:
            print(f"Invalid data received: {data}") 

    except serial.SerialException as e:
        print(f"Serial port error during read: {e}")
        ser.close() # close the serial port
        exit() # exit the program

# Set up animation, interval is delay between frames in ms
# If we make this too small it lags ...
ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys), interval=50, save_count=0)
plt.show()

try:
    plt.show() # start the plot
except KeyboardInterrupt:
    print("Plot closed by user.")
finally:
    ser.close() # Ensure the serial port is closed when the program exits.