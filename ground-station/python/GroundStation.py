#Created by Damien Sabljak
#only works with python 3.5 to 3.7
#kivy imports 
from blinker.base import Signal
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.vector import Vector
from kivy.clock import Clock
from kivy.uix.gridlayout import GridLayout
from kivy.core.window import Window 
from kivy.uix.textinput import TextInput
from kivy.uix.stencilview import StencilView
from kivy.uix.behaviors.focus import FocusBehavior
#pip or naitive imports
import serial
import time 
import blinker
import enum
#user defined imports 
from terminal import Terminal
import telemetry as telem
from gamepad import GamepadHandler
from camerafeed import CameraFeed

class SignalTypes(enum.Enum):
    #currently only for refference, may not work in code 
    newData = blinker.signal("newData")
    gamepadControlInput = blinker.signal("gamepadControlInput")
    
class SerialHandler():
    app = None
    #serial parameters
    READTIMEOUT = 3 #sets read timeout in seconds, set 0 to non-block
    baudRate = 55555
    isConnected = False #check if com port is connected before giving commands 
    ser = None
    gamepadHandler = None
    mavHandler = None
    useMAV = True

    #message buffers
    rx_buffer = None
    fireAlert = False

    def __init__(self, **kwargs):
        super(SerialHandler, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 
    
    def after_init(self, dt): #provide refference to app after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
        self.gamepadHandler = self.app.root.gamepadHandler

    def ConnectToRadio(self, comNum, useMAV = True):
        if(useMAV):
            #Mavlink connection
            self.mavHandler = telem.MAVHandler()
            self.mavHandler.Connect('COM' + str(comNum), self.baudRate)
            self.useMAV = True
        else:
            #py serial connection
            self.ser = serial.Serial('COM' + str(comNum), timeout=self.READTIMEOUT,bytesize=8, parity=serial.PARITY_NONE)  # open serial port
            self.ser.baudrate = 57600 
            print(self.ser.name)         # check which port was really used
            self.useMAV = False
        #set connection flag 
        self.isConnected = True
    
    def HandleSerial(self):
        if(self.isConnected):
            if(self.useMAV == True):
                if(True):
                    #--send RC commands--
                    self.mavHandler.SendRCData(self.gamepadHandler.rc1,self.gamepadHandler.rc2, self.gamepadHandler.rc3, self.gamepadHandler.rc4)

                    #--receive telemetry data--
                    self.mavHandler.HandleMessage()
            else:
                if(self.ser.inWaiting() >= 4):#read in message whenever its larger then designated packet size 
                    self.RawRead(4)
                    sig = blinker.signal('newData') #create an event for other pieces of code to listen to
                    sig.send()
                    #print("signal shoud be being sent..")

    def RawRead(self, numBytes):
        #returns bytes of serial data 
        telem_in = telem.RawRead(self.ser, numBytes)
        self.rx_buffer = telem_in

    def sendRawPacket(self, message):
        telem.sendPacket(self.ser, message.encode('utf-8'))

    def sendMotorCommand(self, axisid, value):
        telem.sendPacket(self.ser, (str(axisid) +" "+ str(value)).encode('utf-8'))
    
class MyTextInput(TextInput):
    def on_parent(self, widget, parent):
        self.focus = True

class PanelHeader(GridLayout):
    def doNothing(self): #dummy method
        pass

class ButtonPanel(GridLayout):
    app = None #stores refferences to app, to access other members 
    radio_connect_button = ObjectProperty(None)
    radio_port_input = ObjectProperty(None)

    def __init__(self, **kwargs):
        super(ButtonPanel, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 
        
    def after_init(self, dt):
        self.radio_connect_button.bind(on_press = self.ConnectToRadio)
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
    
    def ConnectToRadio(self, trashVariable):#button event passes two arguments, but none needed
        print("connect to radio button panel method called..")
        self.app.root.serialHandler.ConnectToRadio(int(self.radio_port_input.text))

class Instrument(Widget):
    reading = NumericProperty(0)

    def GetReading(self):
        pass

class InstrumentPanel(GridLayout):
    instrument1 = ObjectProperty(None)
    instrument2 = ObjectProperty(None)
    instrument3 = ObjectProperty(None)
    instrument4 = ObjectProperty(None)
    velocity_x = NumericProperty(0)
    velocity_y = NumericProperty(0)
    velocity = ReferenceListProperty(velocity_x, velocity_y)

    def move(self):
        self.pos = Vector(*self.velocity) + self.pos

    def GetReadings(self):
        self.instrument1.GetReading()
        
class CommandWidget(GridLayout):
    motorPanel     = ObjectProperty(None)
    terminal       = ObjectProperty(None)
    cameraFeed     = ObjectProperty(None)
    serialHandler  = SerialHandler()
    gamepadHandler = GamepadHandler()
    #cameraFeed     = CameraFeed()
    

    def start(self, vel=(4, 0)):
       pass

    def update(self, dt):
        pass
        #print(dt) #dt is time between update() calls (in seconds)
        #self.serialHandler.HandleSerial() # should be scheduled in start now 
        #self.terminal.HandleTerminal() #should no longer be called here, event driven 
    def HandleSerial(self, dt):
        self.serialHandler.HandleSerial()
    
    
class CommandAndControlApp(App):
    def build(self):
        Command = CommandWidget()
        Command.start()
        #bind events
        Window.bind(on_joy_axis=Command.gamepadHandler.on_joy_axis)
        Command.terminal.textInput.bind(on_text_validate=Command.terminal.on_enter) #bind events to terminal log input 
        Clock.schedule_interval(Command.update, 1.0 / 60.0) #set update interval (in seconds i think)
        Clock.schedule_interval(Command.HandleSerial, 1.0/ 80.0)#throttle serial commands to not cause UART backup, baud rate / (8*max MAV packet size)
        return Command


if __name__ == '__main__':
    CommandAndControlApp().run()