from PIL import Image

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