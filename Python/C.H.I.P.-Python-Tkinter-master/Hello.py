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

        #All Relays On Button
        self.on_button = Button(frame, text="ON", command=self.turn_on, pady=20)
        self.on_button.pack(anchor=W, fill=X)
        
        #All Relays Off Button
        self.off_button = Button(frame, text="OFF", command=self.turn_off, pady=20)
        self.off_button.pack(anchor=W, fill=X)
        
        #Quit Button
        self.button = Button(frame, text="QUIT", fg="red", command=frame.quit, pady=20)
        self.button.pack(side=BOTTOM, fill=X)
        
        #Call function to set Temperature label and register timer to update Temperature Reading every 1000 milliseconds
        self.updateTemp()
        
        #Try sending initialization command to MCP23008 chip on relay board.
        try:
            #Set all channels to outputs  
            bus.write_byte_data(0x20, 0x00, 0x00)

            time.sleep(0.5)
            #Pull all channels down.
            bus.write_byte_data(0x20, 0x06, 0x00)

            time.sleep(0.5)
        except:
            #if initialization of MCP23008 chip on relay board fails then disable on/off buttons and notify user.
            tkMessageBox.showerror("Error","Relay not Detected")
            self.on_button.config(state="disabled")
            self.off_button.config(state="disabled")
            #register function to run every 1.5 seconds
            self.root.after(1500, self.updateRelays)
        
        
        self.root.mainloop()

    def turn_on(self):
        try:
            bus.write_byte_data(0x20, 0x09, 0xFF)
        except:
            tkMessageBox.showerror("Error","Relay not Detected")
            print 'Relay Coms failed'
            self.on_button.config(state="disabled")
            self.off_button.config(state="disabled")
            print 'Buttons disabled'
            self.root.after(1500, self.updateRelays)
            pass
        
    def turn_off(self):
        try:
            bus.write_byte_data(0x20, 0x09, 0x00)
        except:
            tkMessageBox.showerror("Error","Relay not Detected")
            print 'Relay Coms failed'
            self.on_button.config(state="disabled")
            self.off_button.config(state="disabled")
            print 'Buttons disabled'
            self.root.after(1500, self.updateRelays)
            pass
    def updateTemp(self):
        try:
            bus.write_byte(0x40, 0xF3)
            time.sleep(0.5)
            data0 = bus.read_byte(0x40)
            data1 = bus.read_byte(0x40)
            cTemp = (175.72 * (data0 * 256.0 + data1) / 65536.0) - 46.85
            fTemp = cTemp * 1.8 + 32
            self.tempLabel['text'] = 'Temperature: %.2f' % fTemp
            self.root.after(2000, self.updateTemp)
        except:
            self.tempLabel['text'] = 'Temp Sensor not Detected'
            self.root.after(2000, self.updateTemp)
            pass
    def updateRelays(self):
        print 'relay recovery attempt'
        try:
            bus.write_byte_data(0x20, 0x00, 0x00)

            time.sleep(0.5)

            bus.write_byte_data(0x20, 0x06, 0x00)

            time.sleep(0.5)
            self.on_button.config(state="normal")
            self.off_button.config(state="normal")
            
        except:
            self.root.after(1500, self.updateRelays)
        


#initialize App
app = App()
