import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('/dev/rfcomm0')
ser.flushInput()

global serBuffer
isOpen = 1

plot_window = 20
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    try:
        c = ser.read() # attempt to read a character from Serial

        #was anything read?
        if len(c) == 0:         # nothing read
            break  
        
        if c == '\n':           # received new line ?
            isOpen == 1         # re-open buffer

        if isOpen == 1:
            serBuffer += c  # add char to buffer if open

        if len(serBuffer) == 4: # buffer is full --> PRINT
            isOpen == 0     # close buffer until newl ine
            y_var = np.append(y_var,decoded_bytes)
            y_var = y_var[1:plot_window+1]
            line.set_ydata(y_var)
            ax.relim()
            ax.autoscale_view()
            fig.canvas.draw()
            fig.canvas.flush_events()
            serBuffer = "" #empty buffer (to be filled until newline)
        try:
            if len(serBuffer) == 4:
            decoded_bytes = float(serBuffer.decode("utf-8")) # 4 premiers caractères de chaque ligne
            print(decoded_bytes)
        except:
            continue
    except:
        print("Keyboard Interrupt")
        break