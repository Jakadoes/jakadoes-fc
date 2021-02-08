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
    imgData = None
    arrData = None
    imgWidth = 0
    imgHeight = 0

    def __init__(self, **kwargs):
        super(CameraFeed, self).__init__(**kwargs)
        self.InitImgData(numrows= 24, numcols=32)#subdiv 10
        #self.InitImgData(numrows= 10, numcols=13)#subdiv 25 #THIS IS WRONG, FIX DIMENSIONS
        #should be (240/subDiv, 320/10)
        #Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def InitImgData(self, numrows, numcols):
        self.imgData = np.zeros( (numrows, numcols ) ).astype(np.uint8)
        self.arrData = np.zeros(numrows*numcols + numrows).astype(np.uint8)
        self.imgWidth = numcols
        self.imgHeight = numrows

    def LogData(self, startIndex, dataArr):
        for i in range(len(dataArr)):
                row = int((startIndex+i)/self.imgWidth)
                col = int((startIndex+i)%self.imgWidth)
                if(not(  row>= self.imgHeight or col>= self.imgWidth )): #prevent overflow
                    self.imgData[row,col] = int(dataArr[i])
                    self.arrData[startIndex + i] = int(dataArr[i])
                else:
                    print("Camera feed: WARNING: overflow of data sent at index: ", startIndex+i)
        print("log data:\n", self.imgData[0:9,0:12])
    
    def SaveData(self, fileNum):
        #save txt and png of data 
        im_show = Image.fromarray(self.imgData, "P")#grayscale
        im_show.save("capturedImage" + str(fileNum) + ".png")
        f = open("capturedData" + str(fileNum) + ".txt", "w")
        for i in range(len(self.arrData)):
            f.write(str(self.arrData[i]) + "\n")
        f.close()

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
