import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('/dev/rfcomm0')
ser.flushInput()

plot_window = 20
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    try:
        c = ser.readline() # attempt to read a character from Serial
        
        #was anything read?
        if len(c) == 0:
            break  

        try:
            decoded_bytes = float(c[0:4].decode("utf-8")) # on prend les 4 premiers caractères de chaque ligne
            print(decoded_bytes)
        except:
            continue
        y_var = np.append(y_var,decoded_bytes)
        y_var = y_var[1:plot_window+1]
        line.set_ydata(y_var)
        ax.relim()
        ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()
    except:
        print("Keyboard Interrupt")
        break