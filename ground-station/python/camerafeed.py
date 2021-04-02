from PIL import Image as PILImage
import numpy as np
from blinker.base import Signal
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import (
    NumericProperty, ReferenceListProperty, ObjectProperty
)
from kivy.uix.label import Label
from kivy.clock import Clock
from kivy.uix.image import Image as kivyImage
from kivy.uix.gridlayout import GridLayout
from kivy.core.window import Window 
from kivy.uix.textinput import Selector, TextInput
from kivy.uix.stencilview import StencilView
from kivy.uix.behaviors.focus import FocusBehavior
from numpy.lib.utils import source
#pip or naitive imports
import serial
import time 
import blinker
import enum

RED   = 0
GREEN = 1
BLUE  = 2

class CameraFeed(GridLayout):
    MODE_FIRENONE    = 0
    MODE_FIREDETECT  = 1
    MODE_FIREWAIT    = 2
    MODE_FIRELOAD    = 3
    MODE_FIRESHOW    = 4
    mode         = 0
    app          = None
    imgData      = None
    img_interp   = None
    arrData      = None
    dataProgress = 0.0#logs percentage of image downloaded
    imgWidth     = 0
    imgHeight    = 0
    LoadingWidget = None #reference to image loading screen in order to update progress

    def __init__(self, **kwargs):
        super(CameraFeed, self).__init__(**kwargs)
        subDiv   = 15
        imgHeight = 240
        imgWidth  = 320
        numrows  = int(imgHeight/subDiv) 
        numcols  = int(imgWidth/subDiv)
        self.InitImgData(numrows= numrows, numcols=numcols)#subdiv 10
        #self.InitImgData(numrows= 10, numcols=13)#subdiv 25 #THIS IS WRONG, FIX DIMENSIONS
        #should be (240/subDiv, 320/subDiv)
        #Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app or other widgets after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')

    def InitImgData(self, numrows, numcols):
        self.imgData = np.zeros( (numrows, numcols ) ).astype(np.uint8)
        self.arrData = np.zeros(numrows*numcols + numrows).astype(np.uint8)
        self.imgWidth = numcols
        self.imgHeight = numrows

    def LogData(self, startIndex, dataArr):
        self.UpdateProgress(startIndex)
        for i in range(len(dataArr)):
                row = int((startIndex+i)/self.imgWidth)
                col = int((startIndex+i)%self.imgWidth)
                if(not(  row>= self.imgHeight or col>= self.imgWidth )): #prevent overflow
                    self.imgData[row,col] = int(dataArr[i])
                    self.arrData[startIndex + i] = int(dataArr[i])
                else:
                    print("Camera feed: WARNING: overflow of data sent at index: ", startIndex+i)
        print("log data:\n", self.imgData[0:9,0:12])
    
    def UpdateProgress(self, startIndex):
        #uses most recent data logged to determine overall progress as percentage
        self.dataProgress = round(  (startIndex/float(len(self.arrData))*100) , 1)
        self.LoadingWidget.progressPercent = self.dataProgress

    def SaveData(self, fileNum):
        #save txt and png of data 
        #save raw image
        im_show = PILImage.fromarray(self.imgData, "P")#grayscale
        im_show.save("../img/img_raw_" + str(fileNum) + ".png")
        #save interpolated image
        if(not(self.img_interp is None)):
            im_show = PILImage.fromarray(self.img_interp, "RGB")#RGB
            im_show.save("../img/img_interp_" + str(fileNum) + ".png")
        #save raw image as 1D array 
        f = open("../arr/arr_raw" + str(fileNum) + ".txt", "w")
        for i in range(len(self.arrData)):
            f.write(str(self.arrData[i]) + "\n")
        f.close()
    
    def ShowFireDetect(self):
        self.clear_widgets()
        place = CameraFireDetected()
        self.mode = self.MODE_FIREDETECT
        self.add_widget(place)

    def ShowFireLoading(self):
        self.clear_widgets()
        place = CameraFireLoading()
        self.mode = self.MODE_FIRELOAD
        self.LoadingWidget = place
        self.add_widget(place)

    def ShowImage(self, fileNum):
        self.clear_widgets()
        #try to load interpolated image before raw image 
        if(not(self.img_interp is None) and False):
            img = kivyImage(source='../img/img_interp_' + str(fileNum) + '.png', allow_stretch=True)
        else:
            print("image not interpolated, showing raw data instead...")
            img = kivyImage(source='../img/img_raw_' + str(fileNum) + '.png', allow_stretch=True)
        self.add_widget(img)
    
    def ClearImage(self):
        self.clear_widgets()
        #img = kivyImage(source='../img/static.gif', allow_stretch=True)
        #label = Label(text='No Camera Feed Available...', font_size=17, bold=True, center=img.parent.center)
        #img.add_widget(label)
        place = CameraPlacehold()
        self.add_widget(place)
        #place.center = self.center
    
    def InterpolateImage(self):
        #interpolate received bayer image for color restoration 
        interpolator = EdgeDirectedInterpolator()
        img_interp = interpolator.InterpolateDirected(self.imgData)
        self.img_interp = img_interp 

    def TestInterpolate(self):
        #method used for testing
        picture = "4"
        
        im_frame = PILImage.open("../img/img_raw_" + picture + ".png")
        #im_frame = PILImage.open("../img/img_raw_4.png")
        img_in = np.array(im_frame)
        self.imgData = img_in
        self.InterpolateImage()
        self.SaveData(9)

#Kivy class templates for to be called for display in camera feed 
class CameraPlacehold(GridLayout):
    pass
class CameraFireDetected(GridLayout):
    textMain   = ObjectProperty()
    textSub    = ObjectProperty()
    buttonGrid = ObjectProperty()
    buttonYes  = ObjectProperty()
    buttonNo   = ObjectProperty()
    def __init__(self, **kwargs):
        super(CameraFireDetected, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app or other widgets after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
        self.buttonYes.bind(on_press = self.RequestImage)
        self.buttonNo.bind(on_press = self.CancelImage)
        
    def RequestImage(self, trashVariable):
        self.remove_widget(self.buttonGrid)
        self.textMain.text = "Please Wait..."
        self.textSub.text = "\n\nThe image will start tranferring\n once ready"
        self.app.root.cameraFeed.mode = self.app.root.cameraFeed.MODE_FIREWAIT

        pass
    def CancelImage(self, trashVariable):
        print("Warning: CancelImage method not implemented")
        pass
class CameraFireLoading(GridLayout):
    progressPercent = ObjectProperty()
    pass
   
class EdgeDirectedInterpolator:

    def InterpolateDirected(self, img, verbose = False):
        #input, grayscale or RGB bayer pattern image 
        #output, RGB interpolated image
        if(img.ndim == 2):
            #convert grayscale to bayer RGB for processing
            img = self.GrayToRGB(img)
        img = img.astype(int)   
        img_new  = np.copy(img).astype(int)
        #print(type(img[50,50,1]))
        for row in range( len(img[:,0]) ):
            if(row%10 == 1 and verbose):
                print("row " + str(row) + " of " + str(len(img[:,0])) + " complete")
            for col in range(len(img[0,:])):
                #handle edge cases - clamp indexes to boundary 
                left1  = self.ClampIndex(col-1, len(img_new[0,:]))
                right1 = self.ClampIndex(col+1, len(img_new[0,:]))
                up1    = self.ClampIndex(row-1, len(img_new[:,0]))
                down1  = self.ClampIndex(row+1, len(img_new[:,0]))
                left2  = self.ClampIndex(col-2, len(img_new[0,:]))
                right2 = self.ClampIndex(col+2, len(img_new[0,:]))
                up2    = self.ClampIndex(row-2, len(img_new[:,0]))
                down2  = self.ClampIndex(row+2, len(img_new[:,0]))
                 #first interpolate green channels
                if(row%2 == 0 and col%2 == 0): #R
                    alpha = abs((img_new[row, left2,RED] + img_new[row, right2,RED])/2 - (img[row, col,RED]))
                    beta = abs((img_new[up2, col,RED] + img_new[down2, col,RED])/2 - (img[row, col,RED]))
                    img_new[row,col,GREEN] = self.CalculateGreenPixel(img, alpha, beta, row, col, left1, right1, up1, down1)

                elif(row%2 == 1 and col%2 == 1):#B 
                    alpha = abs((img[row, left2,BLUE] + img[row, right2,BLUE])/2 - (img[row, col,BLUE]))
                    beta = abs((img[up2, col,BLUE] + img[down2, col,BLUE])/2 - (img[row, col,BLUE]))
                    img_new[row,col,GREEN] = self.CalculateGreenPixel(img, alpha, beta, row, col, left1, right1, up1, down1)
                if(img[row,col,1] >255):
                    print("GREEN INTERP: overflow error!", row, col)
        if(verbose):
            print("Green interpolation complete")
        for row in range( len(img[:,0]) ):
            if(row%10 == 1 and verbose):
                print("row " + str(row) + " of " + str(len(img[:,0])) + " complete")
            for col in range(len(img[0,:])):
                #handle edge cases - clamp indexes to boundary 
                left1  = self.ClampIndex(col-1, len(img[0,:]))
                right1 = self.ClampIndex(col+1, len(img[0,:]))
                up1    = self.ClampIndex(row-1, len(img[:,0]))
                down1  = self.ClampIndex(row+1, len(img[:,0]))
                if(row%2 == 0 and col%2 == 0): #R
                    img_new[row,col, BLUE] = max(0, min(255,  (img_new[up1, left1,BLUE] + img_new[up1, right1,BLUE] + img_new[down1, left1,BLUE] + img_new[down1, right1,BLUE] )/4 - (img_new[up1, left1,GREEN] + img_new[up1, right1,GREEN] + img_new[down1, left1,GREEN] + img_new[down1, right1,GREEN] - 4*img_new[row,col,GREEN])/4   ))
                elif(row%2 == 1 and col%2 == 1):#B 
                    img_new[row,col, RED] =  max(0, min(255,  (img_new[up1, left1,RED] + img_new[up1, right1,RED] + img_new[down1, left1,RED] + img_new[down1, right1,RED] )/4      - (img_new[up1, left1,GREEN] + img_new[up1, right1,GREEN] + img_new[down1, left1,GREEN] + img_new[down1, right1,GREEN] - 4*img_new[row,col,GREEN])/4    ))
                elif(row%2 == 0 and col%2 == 1):#green w horizontal red
                    img_new[row,col, RED] =  max(0, min(255,  (img_new[row, left1,RED] + img_new[row, right1,RED])/2 - (img_new[row, right1,GREEN] -2*img_new[row, col,GREEN] + img_new[row, left1,GREEN])/2   ))
                    img_new[row,col, BLUE]=  max(0, min(255, (img_new[up1, col,BLUE] + img_new[down1, col,BLUE])/2 - (img_new[down1, col,GREEN] -2*img_new[row, col,GREEN] + img_new[up1, col,GREEN])/2      ))
                elif(row%2 == 1 and col%2 == 0):#green w vertical red
                    img_new[row,col, BLUE] = max(0, min(255, (img_new[row, left1,BLUE] + img_new[row, right1,BLUE])/2 - (img_new[row, right1,GREEN] -2*img_new[row, col,GREEN] + img_new[row, left1,GREEN])/2   ))
                    img_new[row,col, RED] =  max(0, min(255, (img_new[up1, col,RED] + img_new[down1, col,RED])/2 - (img_new[down1, col,GREEN] -2*img_new[row, col,GREEN] + img_new[up1, col,GREEN])/2      ))
                if(img_new[row,col,0] >255 or img_new[row,col,0] <0):
                    print("RB: overflow error! (red)", row, col)
                if(img_new[row,col,1] >255 or img_new[row,col,1] <0):
                    print("RB: overflow error! (green)", row, col)
                if(img_new[row,col,2] >255 or img_new[row,col,2] <0):
                    print("RB: overflow error! (blue)", row, col)
        if(verbose):
            print("RB interpolation complete")
            print(np.max(img_new))
        print("~~~ Interpolation  complete! ~~~")
        return img_new.astype(np.uint8)
        
    def CalculateGreenPixel(self, img, alpha, beta, row, col, left1, right1, up1, down1):
        #implement equations from paper
        img  = img.astype(int)
        if(alpha < beta):
            return ( (img[row, left1,GREEN] + img[row, right1,GREEN])/2 )
        elif(alpha > beta):
            return ( (img[up1, col,GREEN] + img[down1, col,GREEN])/2 )
        elif(alpha == beta):
            return ( (img[up1, col,GREEN] + img[down1, col,GREEN] + img[row, left1,GREEN] + img[row, right1,GREEN])/4 )
        else:
            print("ERROR IN CALCULATE GREEN PIXEL()")

    def ClampIndex(self, index, maxIndex):
        #clamps the input index within the bounds of [0,maxindex)
        #len() can be directly used as an argument for maxIndex
        #print(maxIndex)
        index_new = (index)*int( (index>=0)*(index<maxIndex) ) + (maxIndex-1)*(index>=maxIndex)
        if(index_new >=maxIndex or index_new<0):
            print("ERROR", index_new)
        return index_new 
    
    def GrayToRGB(self, img, verbose = False):
        #converts a gray scale image with values representing bayer filtered values to a 3D RGB image
        #allocate space for channels 
        r = np.zeros( ( len(img[:,0]) , len(img[0,:])  ) ).astype(np.uint8)
        #print("r shape", r.shape)
        g = np.zeros( ( len(img[:,0]) , len(img[0,:])  ) ).astype(np.uint8)
        #print("g shape", g.shape)
        b = np.zeros( ( len(img[:,0]) , len(img[0,:])  ) ).astype(np.uint8)
        #print("b shape", b.shape)
        #print("b: ", b)

        #fill in channels 
        for row in range( len(img[:,0]) ):
            for col in range(len(img[0,:])):
                if(row%2 == 0): #R/G rows
                    if(col%2 == 0):
                        r[row,col] = img[row,col]
                    else:
                        g[row,col] = img[row,col]
                elif(row%2 == 1):#G/B rows
                    if(col%2 == 1):
                        b[row,col] = img[row,col]
                    else:
                        g[row,col] = img[row,col]
        img_new = np.dstack( (r,g,b) )
        if(verbose): 
            print("gray to RGB complete")
        return img_new
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
