from PIL import Image
import numpy as np
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

class CameraFeed(GridLayout):
    def getFeedBack(self):
        pass

'''
img = np.zeros( (int(240/5), int(320/5))).astype(np.uint8)
f = open("testData_subDiv5_r.txt", "r")
#print(f.readline())
for row in range(int(240/5)):
            for col in range(int(320/5)):
                print(row, col)
                img[row,col] = f.readline()

print(img)
im_show = Image.fromarray(img, "P")
#im_show = Image.fromarray(img)
im_show.show()
im_show.save("testImage2.png")

'''
