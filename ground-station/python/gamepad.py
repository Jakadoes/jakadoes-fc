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
#naitive and pip imports 
import time 
import blinker

class GamepadHandler(object):
    app = None
    feedbackPanel = None
    serialHandler = None
    time_new = [0,0,0,0,0,0,0]#holds elapsed time slots for each axis
    time_old = [0,0,0,0,0,0,0] 
    #declare axisid's of the sticks on Xbox one controller 
    MIN_TIME_BETWEEN_EVENTS = 0.02 #in s
    LEFTSTICK_Y = 1
    LEFTSTICK_X = 0
    RIGHTSTICK_Y = 4
    LEFTTRIGGER = 2
    RIGHTTRIGGER = 5
    PITCH_SENS = 1
    ROLL_SENS = 1
    YAW_SENS = 1
    DYNAMIC_THRUST_RANGE = 0.4
    Thrust = 0
    Pitch = 0
    Yaw = 0
    Roll = 0
    rc1 = 0
    rc2 = 0
    rc3 = 0
    rc4 = 0

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
        #set max event frequency to 20 miliseconds to reduce uneeded event calls
        self.time_new[axisid] = time.clock()#measured in seconds 
        if((self.time_new[axisid] - self.time_old[axisid])< self.MIN_TIME_BETWEEN_EVENTS):
            return 0
        self.time_old[axisid] = self.time_new[axisid]
    
        print(win, stickid, axisid, value)
        #normalize input to [-100, 100]
        value_rounded = round((-100*value)/32767, 3)  
        if( axisid == self.LEFTTRIGGER or axisid == self.RIGHTTRIGGER):
            value_rounded = round(-1*value_rounded + 100, 3) #its flipped by default

        self.ProcessRCData(axisid,value_rounded)
        self.GraphicsFeedback(axisid, value_rounded)
        #send command to FC through radio
        #self.serialHandler.sendMotorCommand(axisid, value)

    def ProcessRCData(self, axisid, value_rounded):
        #NOTE: motors are defined from front left clockwise as 1,2,3,4 in this scope ONLY
        
        #thrust
        if(axisid == self.RIGHTSTICK_Y):
            self.Thrust = value_rounded
        #Roll
        elif(axisid == self.LEFTSTICK_X):
            self.Roll = value_rounded
        #pitch
        elif(axisid == self.LEFTSTICK_Y):
            self.Pitch = value_rounded
        #yaw
        elif(axisid == self.LEFTTRIGGER):
            self.YAW = -1*value_rounded 
        elif(axisid == self.RIGHTTRIGGER):
            self.YAW = value_rounded 
        #print("commands: ")
        #print("thrust:",self.Thrust,"yaw:", self.yaw,"pitch", self.Pitch,"roll", self.Roll)

        #Process RC signal per motor channel
        #output should be -100 to 100, however never passes (-80,80), this is because on joystick (round) you cant have max x and y (not square)
        #to get to 100, use math to determine circular max
        #calculate thrust, then add clipped dynamic modifier, finally adding minimum common mode thrust 
        rc1 = max( (1-self.DYNAMIC_THRUST_RANGE)*self.Thrust + (self.DYNAMIC_THRUST_RANGE/3)*(-1*self.Pitch*self.PITCH_SENS - self.Roll*self.ROLL_SENS + self.Yaw*self.YAW_SENS) , 0) + self.Thrust/10
        rc2 = max( (1-self.DYNAMIC_THRUST_RANGE)*self.Thrust + (self.DYNAMIC_THRUST_RANGE/3)*(-1*self.Pitch*self.PITCH_SENS + self.Roll*self.ROLL_SENS - self.Yaw*self.YAW_SENS) , 0) + self.Thrust/10
        rc3 = max( (1-self.DYNAMIC_THRUST_RANGE)*self.Thrust + (self.DYNAMIC_THRUST_RANGE/3)*(self.Pitch*self.PITCH_SENS + self.Roll*self.ROLL_SENS + self.Yaw*self.YAW_SENS) , 0) + self.Thrust/10
        rc4 = max( (1-self.DYNAMIC_THRUST_RANGE)*self.Thrust + (self.DYNAMIC_THRUST_RANGE/3)*(self.Pitch*self.PITCH_SENS - self.Roll*self.ROLL_SENS - self.Yaw*self.YAW_SENS) , 0) + self.Thrust/10
        
        print("RC Data processed: ")
        print("rc1:", rc1,"rc2:", rc2,"rc3:", rc3,"rc4:", rc4)
        self.rc1 = rc1
        self.rc2 = rc2
        self.rc3 = rc3
        self.rc4 = rc4
        #old - event based transmission
        #if(self.serialHandler.isConnected):
        #    self.serialHandler.mavHandler.SendRCData(rc1, rc2, rc3, rc4)

    def GraphicsFeedback(self, axisid, value_rounded):
        #show grapical feedback on GUI
        if(self.feedbackPanel.instrument1 != None and axisid == self.LEFTSTICK_Y):
                self.feedbackPanel.instrument1.reading = value_rounded
        elif(self.feedbackPanel.instrument1 != None and axisid == self.LEFTSTICK_X):
            self.feedbackPanel.instrument1.reading_sub = value_rounded
        elif(self.feedbackPanel.instrument2 != None and axisid == self.RIGHTSTICK_Y):
                self.feedbackPanel.instrument2.reading = value_rounded
        elif(self.feedbackPanel.instrument3 != None and axisid == self.LEFTTRIGGER):
                self.feedbackPanel.instrument3.reading = value_rounded
        elif(self.feedbackPanel.instrument4 != None and axisid == self.RIGHTTRIGGER):
                self.feedbackPanel.instrument4.reading = value_rounded
        else: 
            print('no instrument linked to this input ')

