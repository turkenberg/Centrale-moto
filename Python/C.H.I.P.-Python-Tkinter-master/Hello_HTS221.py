#!/usr/bin/python

from Tkinter import *
import tkMessageBox
import smbus
import sys
import time
import os

bus = smbus.SMBus(1)




class App():

    def __init__(self):
        
        while(True):
            try:
                #Since we are running this as soon as the OS boots the Display may not be ready yet.  
                #We will try to initialize the TKinter object in a loop until it succeeds due to display being ready.
                self.root = Tk()
                time.sleep(2)
                break
            except:
                time.sleep(1)
        
        

        frame = Frame()
        frame.pack()
        
        #Temperature Label
        self.tempLabel = Label(frame, text="...", padx=20, pady=20)
        self.tempLabel.pack(side=TOP)

        #Al# l Relays On Button
#         self.on_button = Button(frame, text="ON", command=self.turn_on, pady=20)
#         self.on_button.pack(anchor=W, fill=X)
#         
#         #All Relays Off Button
#         self.off_button = Button(frame, text="OFF", command=self.turn_off, pady=20)
#         self.off_button.pack(anchor=W, fill=X)
        
        #Quit Button
        self.button = Button(frame, text="QUIT", fg="red", command=frame.quit, pady=20)
        self.button.pack(side=BOTTOM, fill=X)
        
        #Call function to set Temperature label and register timer to update Temperature Reading every 1000 milliseconds
        self.updateTemp()
        
        #Try sending initialization command to MCP23008 chip on relay board.
#         try:
#             #Set all channels to outputs  
#             bus.write_byte_data(0x20, 0x00, 0x00)
# 
#             time.sleep(0.5)
#             #Pull all channels down.
#             bus.write_byte_data(0x20, 0x06, 0x00)
# 
#             time.sleep(0.5)
#         except:
#             #if initialization of MCP23008 chip on relay board fails then disable on/off buttons and notify user.
#             tkMessageBox.showerror("Error","Relay not Detected")
#             self.on_button.config(state="disabled")
#             self.off_button.config(state="disabled")
#             #register function to run every 1.5 seconds
#             self.root.after(1500, self.updateRelays)
        
        
        self.root.mainloop()

#     def turn_on(self):
#         try:
#             bus.write_byte_data(0x20, 0x09, 0xFF)
#         except:
#             tkMessageBox.showerror("Error","Relay not Detected")
#             print 'Relay Coms failed'
#             self.on_button.config(state="disabled")
#             self.off_button.config(state="disabled")
#             print 'Buttons disabled'
#             self.root.after(1500, self.updateRelays)
#             pass
#         
#     def turn_off(self):
#         try:
#             bus.write_byte_data(0x20, 0x09, 0x00)
#         except:
#             tkMessageBox.showerror("Error","Relay not Detected")
#             print 'Relay Coms failed'
#             self.on_button.config(state="disabled")
#             self.off_button.config(state="disabled")
#             print 'Buttons disabled'
#             self.root.after(1500, self.updateRelays)
#             pass
    def updateTemp(self):
        try:
			bus.write_byte_data(0x5F, 0x10, 0x1B)
			bus.write_byte_data(0x5F, 0x20, 0x85)
			time.sleep(0.5)
			val = bus.read_byte_data(0x5F, 0x30)
			H0 = val / 2
			val = bus.read_byte_data(0x5F, 0x31)
			H1 = val /2
			val0 = bus.read_byte_data(0x5F, 0x36)
			val1 = bus.read_byte_data(0x5F, 0x37)
			H2 = ((val1 & 0xFF) * 256) + (val0 & 0xFF)
			val0 = bus.read_byte_data(0x5F, 0x3A)
			val1 = bus.read_byte_data(0x5F, 0x3B)
			H3 = ((val1 & 0xFF) * 256) + (val0 & 0xFF)
			T0 = bus.read_byte_data(0x5F, 0x32)
			T0 = (T0 & 0xFF)
			T1 = bus.read_byte_data(0x5F, 0x33)
			T1 = (T1 & 0xFF)
			raw = bus.read_byte_data(0x5F, 0x35)
			raw = (raw & 0x0F)
			T0 = ((raw & 0x03) * 256) + T0
			T1 = ((raw & 0x0C) * 64) + T1
			val0 = bus.read_byte_data(0x5F, 0x3C)
			val1 = bus.read_byte_data(0x5F, 0x3D)
			T2 = ((val1 & 0xFF) * 256) + (val0 & 0xFF)
			val0 = bus.read_byte_data(0x5F, 0x3E)
			val1 = bus.read_byte_data(0x5F, 0x3F)
			T3 = ((val1 & 0xFF) * 256) + (val0 & 0xFF)
			data = bus.read_i2c_block_data(0x5F, 0x28 | 0x80, 4)
			humidity = (data[1] * 256) + data[0]
			humidity = ((1.0 * H1) - (1.0 * H0)) * (1.0 * humidity - 1.0 * H2) / (1.0 * H3 - 1.0 * H2) + (1.0 * H0)
			temp = (data[3] * 256) + data[2]
			if temp > 32767 :
				temp -= 65536
			cTemp = ((T1 - T0) / 8.0) * (temp - T2) / (T3 - T2) + (T0 / 8.0)
			fTemp = (cTemp * 1.8 ) + 32
			self.tempLabel['text'] = 'Temperature: %.2f' % fTemp
            self.root.after(2000, self.updateTemp)
        except:
            self.tempLabel['text'] = 'Temp Sensor not Detected'
            self.root.after(2000, self.updateTemp)
            pass
            
#     def updateRelays(self):
#         print 'relay recovery attempt'
#         try:
#             bus.write_byte_data(0x20, 0x00, 0x00)
# 
#             time.sleep(0.5)
# 
#             bus.write_byte_data(0x20, 0x06, 0x00)
# 
#             time.sleep(0.5)
#             self.on_button.config(state="normal")
#             self.off_button.config(state="normal")
#             
#         except:
#             self.root.after(1500, self.updateRelays)
        


#initialize App
app = App()
