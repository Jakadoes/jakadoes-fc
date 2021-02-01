from pyb import I2C
from ustruct import unpack
import pyb
import utime
import sensor, image, time, math

#define command enums
CAM_COM_POLL_ALERT = 0x11
#define alert enums
CAM_ALERT_FIRE_DETECT = 0x55

#initialize I2C peripheral
i2c = I2C(2)
i2c.init(I2C.SLAVE, addr=0x26, dma= False)
#switch = pyb.Switch()
data1 = bytearray(4,)
command = bytearray(4)
a = bytearray(b'\xc7\x14')
clock = time.clock()


utime.sleep_ms(500)
while(True):
    clock.tick()

    #check for I2C commands
    try:
        print("receiving commands..")
        data = i2c.recv(1, 100)
        command[0] = data[0] #store in buffer for later procesing
        print("received")
        pass
    except OSError:
        data = None
    print (data)
   # print(command[0])

    if(command[0] == 0x11):
        print("poll requested")
        try:
            print("stage 2..")
            data = i2c.recv(1, 500)
            command[0] = data[0] #store in buffer for later procesing
            print("received2")
            pass
        except OSError:
            print("stage 2 no recieve")
            data = None
        print (data)
        if(True):
            utime.sleep_ms(10)
            try:
             print("sending...")
             i2c.send(0x69,0,timeout=5000)
             print("sent")
            except OSError:
                print("an OSError has occured")
                i2c.deinit()
                utime.sleep_ms(100)
                i2c.init(I2C.SLAVE, addr=0x26, dma=False)
    command[0] = 0
    utime.sleep_ms(10)


#print(clock.fps())

class I2cHandler():
    def Init():
        #initialize I2C peripheral
        i2c = I2C(2)
        i2c.init(I2C.SLAVE, addr=0x26, dma= False)
        #switch = pyb.Switch()
        data1 = bytearray(4,)
        command = bytearray(4)
        clock = time.clock()
