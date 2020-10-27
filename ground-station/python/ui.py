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

    def __init__(self, **kwargs):
        super(Terminal, self).__init__(**kwargs)

    def RefocusInput(self, event):
        self.get_focus_next().focus = True
    #collect input data from input once entered
    
    def on_enter(self, instance):
        print(instance.text)
        self.textLog.text = self.textLog.text + '\n' + instance.text

        #delete old text if it doesnt fit on panel
        if(self.textLog.texture_size[1]+20 > self.textLog.size[1]):
            for i in range(2): #max timeout: 100
                self.textLog.text = self.textLog.text.split('\n',1)[1]
                if(self.textLog.texture_size[1] < self.textLog.size[1]): #pretty sure this part does nothing 
                    break

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
    
    #Event function to bind controller events to window
    def on_joy_axis(self, win, stickid, axisid, value):
        #declare axisid's of the sticks on Xbox one controller 
        LEFTSTICK_Y = 1
        LEFTSTICK_X = 0
        RIGHTSTICK_Y = 4
        LEFTTRIGGER = 2
        RIGHTTRIGGER = 5
        print(win, stickid, axisid, value)
        print('TEST')
        if(self.instrument1 != None and axisid == LEFTSTICK_Y):
                self.instrument1.reading = round(((value)/(32767))*-100, 2)  #normalize input to [-100, 100]
        elif(self.instrument1 != None and axisid == LEFTSTICK_X):
            self.instrument1.reading_sub = round(((value)/(32767))*-100, 2) #normalize input to [-100, 100]
        elif(self.instrument2 != None and axisid == RIGHTSTICK_Y):
                self.instrument2.reading = round(((value)/(32767))*-100, 2)  #normalize input to [-100, 100]
        elif(self.instrument3 != None and axisid == LEFTTRIGGER):
                self.instrument3.reading = round(((value)/(32767))*100, 2)  #normalize input to [-100, 100]
        elif(self.instrument4 != None and axisid == RIGHTTRIGGER):
                self.instrument4.reading = round(((value)/(32767))*100, 2)  #normalize input to [-100, 100]
        else: 
            print('WARNING: MOTOR1 NOT DEFINED ')
        

class CommandWidget(GridLayout):
    motorPanel = ObjectProperty(None)
    terminal = ObjectProperty(None)

    def start(self, vel=(4, 0)):
        pass

    def update(self, dt):
        pass
    


class CommandAndControlApp(App):
    def build(self):
        Command = CommandWidget()
        Command.start()
        #bind events
        Window.bind(on_joy_axis=Command.motorPanel.on_joy_axis)
        Command.terminal.textInput.bind(on_text_validate=Command.terminal.on_enter) #bind events to terminal log input 
        Clock.schedule_interval(Command.update, 1.0 / 60.0)
        return Command


if __name__ == '__main__':
    CommandAndControlApp().run()