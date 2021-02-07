from pyb import I2C
from struct import unpack
import pyb
import utime
import sys
import sensor, image, time, math
import os
#import uasyncio

# Single Color RGB565 Blob Tracking Example
#
# This example shows off single color RGB565 tracking using the OpenMV Cam.

threshold_index = 0 # 0 for red, 1 for green, 2 for blue

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green/blue things. You may wish to tune them...
thresholds = [(30, 100, 15, 127, 15, 127), # generic_red_thresholds
              (30, 100, -64, -8, -32, 32), # generic_green_thresholds
              (0, 30, 0, 64, -128, 0)] # generic_blue_thresholds

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
clock = time.clock()

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" merges all overlapping blobs in the image.
#Define enums:
CAM_MODE_SCANNING = 0x11 #takes pics, searches for fires
CAM_MODE_DETECTED = 0x22 #a fire has been detected, image being stored
CAM_MODE_STORED   = 0x33 #image stored, ready for download
RED   = 0x00
GREEN = 0x01
BLUE  = 0x02
#classes:
class FireData:#holds information on fire detection
    mode = CAM_MODE_SCANNING
    photoData = None
    subDiv = 0
    row = 0
    col = 0
    def Init(self, img1):
        self.InitPhoto(img1, subDiv = 25)
    def InitPhoto(self,img1,subDiv):
        row_sub = int( img1.height()/subDiv )
        col_sub = int( img1.width() /subDiv )
        arr_r = bytearray((row_sub+1)*(col_sub+1) )
        arr_g = bytearray((row_sub+1)*(col_sub+1) )
        arr_b = bytearray((row_sub+1)*(col_sub+1))
        print("arr size",(row_sub)*(col_sub))
        print(row_sub)
        print(col_sub)
        self.photoData = (arr_r, arr_g, arr_b)
        self.subDiv = subDiv
    def StoreImage(self, img1):
        #stores image in byte array for later transmission
        row_sub = int(img1.height()/self.subDiv)
        col_sub = int(img1.width() /self.subDiv)
        arrInd = col_sub*int(self.row/self.subDiv) + int(self.col/self.subDiv)
        #store next pixel
        self.photoData[RED][arrInd] = img1.get_pixel(self.col,self.row)[0]
        self.photoData[GREEN][arrInd] = img1.get_pixel(self.col,self.row)[1]
        self.photoData[BLUE][arrInd] = img1.get_pixel(self.col,self.row)[2]
        #print(self.photoData[GREEN][arrInd])
        #index and check for end of image
        self.col = self.col+1*self.subDiv
        if(self.col >= img1.width()):
            self.col = 0
            self.row = self.row +1*self.subDiv
        if(self.row >= img1.height()):
            #finished storing image
            self.row = 0
            self.col = 0
            self.mode = CAM_MODE_STORED
            print ("cam status: " + "IMG STORED" )
            print("image saved, waiting for collection")
            return CAM_MODE_STORED
        return CAM_MODE_DETECTED#keep storing

class I2cHandler():
    #define command enums
    CAM_COM_POLL_ALERT = 0x11
    CAM_COM_POLL_IMAGE = 0x13
    #define alert enums
    #CAM_ALERT_FIRE_DETECT = 0x55
    #define object attributes
    address = 0x26
    dma = False
    i2c = None
    data1 = None
    command = None
    def Init(self):
        #initialize I2C peripheral
        self.i2c = I2C(2)
        self.i2c.init(I2C.SLAVE, addr=self.address, dma= self.dma)
        #switch = pyb.Switch()
        self.data1 = bytearray(6,)
        self.command = bytearray(4)
        utime.sleep_ms(500)
    def HandleI2c(self, fireData):
        #***general flow of handling***:
        #1. check for I2C commands
        #2. if a command is received, determine command
        #3. for any command, catch receive command shortly after and then
        #   transmit requested data
        #check for I2C commands
        try:#step 1
            print("I2C: receiving commands..")
            self.data = self.i2c.recv(1, 50)
            self.command[0] = self.data[0] #store in buffer for later procesing
            print("I2C: received")
        except OSError:
            print("I2C: OSE error recv")
            self.data = None
        print (self.data)
      # print(self.command[0])
        #****AT THIS POINT: breakout to multiple handler functions
        #step 2
        if(self.command[0] == self.CAM_COM_POLL_ALERT):
            print("I2C: fire poll requested")
            #self.CatchRecieveCommand()
            self.Transmit(fireData.mode)
        elif(self.command[0] == self.CAM_COM_POLL_IMAGE):
            print("I2C: image data requested")
            try:
                print("I2C: recv command 2..")
                self.data1 = self.i2c.recv(5, 100)#LSB at [0], numBytes
                print("data 2:")
                print(self.data1[0])
                print(self.data1[1])
                print(self.data1[2])
                print(self.data1[3])
                print(self.data1[4])
            except OSError:
                self.data1 = None
            if(not(self.data1 == None)):
                numBytes = self.data1[0]
                img_buffer = bytearray(numBytes)
                #img_buffer[0] = 0x01
                #img_buffer[1] = 5
                for i in range(numBytes):
                    img_buffer[i] = i+1 #sudo photo data
                print(img_buffer)
                #
                self.Transmit(img_buffer)#must transmit as fast as possible
                #self.Transmit(img_buffer)
            #self.command[0] = self.data1[0] #store in buffer for later procesing
            print("I2C: received")
        #reset and wait for next command
        self.command[0] = 0
        utime.sleep_ms(10)
    def CatchRecieveCommand(self):
        #catch empty command from FC receive method
        try:
            print("I2C: catch recieve..")
            self.data = self.i2c.recv(1, 100)
            #self.command[0] = data[0] #store in buffer for later procesing
            print("I2C: recieve caught")
        except OSError:#this exception is expected
            print("I2C: catch - no recieve")
            self.data = None
        #print (self.data)
    def Transmit(self, message):
        utime.sleep_ms(10)#need to test delay necessity
        try:
            print("I2C: sending...")
            self.i2c.send(message,0,timeout=5000)
            print("I2C: sent")
        except OSError:
            print("I2C Transmit(): an OSError has occured")
            self.i2c.deinit()
            utime.sleep_ms(100)
            self.i2c.init(I2C.SLAVE, addr=self.address, dma=self.dma)

#***starting code***
i2cEnable  = pyb.Pin("P9", pyb.Pin.IN, pyb.Pin.PULL_DOWN)
firewatch = FireData()
i2cHandler = I2cHandler()
img = sensor.snapshot()#needed for init
firewatch.Init(img)
i2cHandler.Init()
enableValue = 0
#print(os.listdir())
#f = open('data.txt', 'w')
#f.write('poop')
#f.close()
#***end start code***
while(True):
    clock.tick()
    if(not(i2cEnable.value() == enableValue)):
        print( "I2C enable value: " + str(i2cEnable.value()) )

    if( i2cEnable.value() ):
        enableValue = 1
        i2cHandler.HandleI2c(firewatch)
    elif(firewatch.mode == CAM_MODE_SCANNING):
        enableValue = 0
        img = sensor.snapshot()
        for blob in img.find_blobs([thresholds[threshold_index]], pixels_threshold=200, area_threshold=200, merge=True):
            # These values depend on the blob not being circular - otherwise they will be shaky.
            if blob.elongation() > 0.5:
                img.draw_edges(blob.min_corners(), color=(255,0,0))
                img.draw_line(blob.major_axis_line(), color=(0,255,0))
                img.draw_line(blob.minor_axis_line(), color=(0,0,255))
            # These values are stable all the time.
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(), blob.cy())
            # Note - the blob rotation is unique to 0-180 only.
            img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
            firewatch.mode = CAM_MODE_DETECTED
            print("fire detected, saving image...")
        #print ("cam status: " + "Scanning..." )
    elif(firewatch.mode == CAM_MODE_DETECTED):
        print("storing pixel: ", (firewatch.row, firewatch.col))
        firewatch.mode = firewatch.StoreImage(img)
        #print ("cam status: " + "Detected" )
    elif(firewatch.mode == CAM_MODE_STORED):

        flag = 0
        if(flag ==0):
            #f.write(firewatch.photoData[0])
            #print(unpack('<H', firewatch.photoData[0]))
            #print(firewatch.photoData[0])
            #f.close()
            flag = 1
        pass#wait for image to be collected
    #print( img.get_pixel(100, 220) )

    utime.sleep_ms(100)


#print(clock.fps())


