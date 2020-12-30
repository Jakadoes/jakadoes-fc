#kivy imports 
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
import enum
import blinker

class TerminalModes(enum.Enum):
    #modes determine how to terminal will behave in terms of displaying data
    SERIALMONITOR = 1
    CLI = 2

class Terminal(FocusBehavior, GridLayout):
    focus_next = ObjectProperty(None)
    terminalInput = ObjectProperty(None)
    textInput = ObjectProperty(None)
    textLog = ObjectProperty(None)
    app = None
    mode = TerminalModes.SERIALMONITOR

    def __init__(self, **kwargs):
        super(Terminal, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
        blinker.signal('newData').connect(self.HandleTerminal)

    def HandleTerminal(self, eventhandlerdata):
        #method to be called during new data event 
        #eventhandlerdata is only there to stop errors 
        if(self.mode == TerminalModes.SERIALMONITOR):
            if(self.app.root.serialHandler.isConnected):
                self.LogMessage( "Drone: "+str(self.app.root.serialHandler.rx_buffer) )
            else:
                self.OverwriteLog("not connected to COM port, please connect to monitor")

    def RefocusInput(self, event):
        self.get_focus_next().focus = True
    #collect input data from input once entered
    
    def LogMessage(self, message):
        #add message to log 
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
    def OverwriteLog(self, message):
        #clear log and replace with message
        self.textLog.text = message
    def on_enter(self, instance):
        #process enter event from input to terminal 
        #print(instance.text)
        self.LogMessage(instance.text)
        #print(self.app)
        if(self.mode == TerminalModes.SERIALMONITOR):
            self.app.root.serialHandler.sendRawPacket(instance.text)
        instance.text = '>'#reset after enter
        Clock.schedule_once(self.RefocusInput) #keep input selected after pressing enter



class TerminalLog(GridLayout, StencilView): #create as stencil view to mask label widget going out of box 
    pass

class TerminalInput(GridLayout):
    pass
