#only works with python 3.5 to 3.7
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
import telemetry as telem
import serial
import time 


class GamepadHandler(object):
    app = None
    feedbackPanel = None
    serialHandler = None
    time_new = [0,0,0,0,0,0,0]#holds elapsed time slots for each axis
    time_old = [0,0,0,0,0,0,0] 

    def __init__(self, **kwargs):
        super(GamepadHandler, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
        self.feedbackPanel = self.app.root.motorPanel
        self.serialHandler = self.app.root.serialHandler

    #Event function to bind controller events to window
    def on_joy_axis(self, win, stickid, axisid, value):
        #declare axisid's of the sticks on Xbox one controller 
        LEFTSTICK_Y = 1
        LEFTSTICK_X = 0
        RIGHTSTICK_Y = 4
        LEFTTRIGGER = 2
        RIGHTTRIGGER = 5

        #set max event frequency to 20 miliseconds to reduce uneeded event calls
        self.time_new[axisid] = time.clock()#measured in seconds 
        if((self.time_new[axisid] - self.time_old[axisid])< 0.020):
            return 0
        self.time_old[axisid] = self.time_new[axisid]
        

        print(win, stickid, axisid, value)
        #normalize input to [-100, 100]
        value_rounded = round((-100*value)/32767, 3)  
        if( axisid == LEFTTRIGGER or axisid == RIGHTTRIGGER):
            value_rounded = -1*value_rounded

        #send command to FC through radio
        self.serialHandler.sendMotorCommand(axisid, value)

        #show grapical feedback
        if(self.feedbackPanel.instrument1 != None and axisid == LEFTSTICK_Y):
                self.feedbackPanel.instrument1.reading = value_rounded
        elif(self.feedbackPanel.instrument1 != None and axisid == LEFTSTICK_X):
            self.feedbackPanel.instrument1.reading_sub = value_rounded
        elif(self.feedbackPanel.instrument2 != None and axisid == RIGHTSTICK_Y):
                self.feedbackPanel.instrument2.reading = value_rounded
        elif(self.feedbackPanel.instrument3 != None and axisid == LEFTTRIGGER):
                self.feedbackPanel.instrument3.reading = value_rounded
        elif(self.feedbackPanel.instrument4 != None and axisid == RIGHTTRIGGER):
                self.feedbackPanel.instrument4.reading = value_rounded
        else: 
            print('WARNING: MOTOR1 NOT DEFINED ')

class SerialHandler():
    READTIMEOUT = 3 #sets read timeout in seconds, set 0 to non-block
    ser = serial.Serial('COM6', timeout=READTIMEOUT)  # open serial port
    ser.baudrate = 57600 
    print(ser.name)         # check which port was really used

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
    def getFeedBack(self):
        pass

class CameraFeed(GridLayout):
    def getFeedBack(self):
        pass

class Terminal(FocusBehavior, GridLayout):
    focus_next = ObjectProperty(None)
    terminalInput = ObjectProperty(None)
    textInput = ObjectProperty(None)
    textLog = ObjectProperty(None)
    app = None

    def __init__(self, **kwargs):
        super(Terminal, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')

    def RefocusInput(self, event):
        self.get_focus_next().focus = True
    #collect input data from input once entered
    
    def LogMessage(self, message):
        print(message)
        self.textLog.text = self.textLog.text + '\n' + message

        #delete old text if it doesnt fit on panel
        if(self.textLog.texture_size[1]+20 > self.textLog.size[1]):
            for i in range(2): #max timeout: 100
                self.textLog.text = self.textLog.text.split('\n',1)[1]
                if(self.textLog.texture_size[1] < self.textLog.size[1]): #pretty sure this part does nothing 
                    break
        #to do: add handling capabilities for multi-line messages based on log width
    #process text input into terminal 
    def on_enter(self, instance):
        print(instance.text)

        self.LogMessage(instance.text)
        print(self.app)
        self.app.root.serialHandler.sendRawPacket(instance.text)
        instance.text = '>'#reset after enter
        Clock.schedule_once(self.RefocusInput) #keep input selected after pressing enter
    

class TerminalLog(GridLayout, StencilView): #create as stencil view to mask label widget going out of box 
    pass

class TerminalInput(GridLayout):
    pass

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
    motorPanel = ObjectProperty(None)
    terminal = ObjectProperty(None)
    serialHandler = SerialHandler()
    gamepadHandler = GamepadHandler()

    def start(self, vel=(4, 0)):
        pass

    def update(self, dt):
        pass
    


class CommandAndControlApp(App):
    def build(self):
        Command = CommandWidget()
        Command.start()
        #bind events
        Window.bind(on_joy_axis=Command.gamepadHandler.on_joy_axis)
        Command.terminal.textInput.bind(on_text_validate=Command.terminal.on_enter) #bind events to terminal log input 
        Clock.schedule_interval(Command.update, 1.0 / 60.0)
        return Command


if __name__ == '__main__':
    CommandAndControlApp().run()