import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('/dev/rfcomm0', 9600, timeout = 0)
ser.flushInput()

serBuffer = ""
isOpen = 1

plot_window = 20
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    c = ser.read() # attempt to read a character from Serial

    #was anything read?
    if len(c) == 0:         # nothing read
        break
    
    if c == '\n':           # received new line ?
        isOpen = 1         # re-open buffer
        break

    if isOpen == 1:
        if c == '\t':           # PRINT instruction ; open resets at NL
            print(serBuffer + '.') 
            y_var = np.append(y_var, float(serBuffer))
            y_var = y_var[1:plot_window+1]
            line.set_ydata(y_var)
            ax.relim()
            ax.autoscale_view()
            fig.canvas.draw()
            fig.canvas.flush_events()
            serBuffer = '' #empty buffer (to be filled until newline)
            isOpen = 0     # close buffer until newline
        else:
            serBuffer += c  # add char to buffer if open