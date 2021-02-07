from pyb import I2C
import pyb
import utime
#import uio

# This example shows off single color RGB565 tracking using the OpenMV Cam.

import sensor, image, time, math

threshold_index = 0

# Flame detection in different environments (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds will be used to track flames in different settings
thresholds = [(30, 100, 15, 127, 15, 127), # insidearoom
              (30, 100, -64, -8, -32, 32), # lowexposurelighting
              (0, 30, 0, 64, -128, 0)] # brightexposurelighting

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # turned off for color tracking
sensor.set_auto_whitebal(False) # turned off for color tracking
clock = time.clock()

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" merges all overlapping blobs in the image.

class Serializer:
    def SerializeImage(self, comp_ratio):
        img1 = sensor.snapshot()
        img = img1.compressed(25)#save space
        arr = img.bytearray()
        #print("Size", img.size())
        #print("w, h", img.width(), img.height())
        #print(arr[0])
        return arr

class I2cflag(): #fire alert flag
    firealert = 0

serializer = Serializer()

alert = I2cflag()

#uio.open("test.txt", mode='rt')

while(True):
    clock.tick()
    img = sensor.snapshot() #
    # img1 = sensor.snapshot()
    # img = img1.compressed(25)
    # arr = img.bytearray()
    # print("Size", img.size())
    # print("w, h", img.width(), img.height())
    # print(arr[0])
    flag = 0
    for blob in img.find_blobs([thresholds[threshold_index]], pixels_threshold=200, area_threshold=200, merge=True):
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255,0,0))
            img.draw_line(blob.major_axis_line(), color=(0,255,0))
            img.draw_line(blob.minor_axis_line(), color=(0,0,255))
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(), blob.cy())
            img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
            img1 = img.compressed(10)
            arr = img1.bytearray()
            alert.firealert = 1
    img1 = img.compressed(10)
    arr = img1.bytearray()
    alert.firealert = 1
    #   pass
    print("arr size: ")
    print(len(arr))

    if(flag == 0):
        for i in range(len(arr)):
            print(arr[i])
        flag = 1
    #print("Size", img1.size())
    #print("w, h", img1.width(), img1.height())
    #print(arr[0])
    utime.sleep_ms(100)
    alert.firealert = 0
    #print(clock.fps())


