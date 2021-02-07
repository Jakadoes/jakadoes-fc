from pyb import I2C
from ustruct import unpack
import pyb
import utime
import sensor, image, time, math



class I2cHandler():
    #define command enums
    CAM_COM_POLL_ALERT = 0x11
    #define alert enums
    CAM_ALERT_FIRE_DETECT = 0x55
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
        self.data1 = bytearray(4,)
        self.command = bytearray(4)

        utime.sleep_ms(500)

    def HandleI2c(self):
        #general flow of handling:
        #1. check for I2C commands
        #2. if a command is received, determine command
        #3. for any command, catch receive command shortly after and then
        #   transmit requested data
        #check for I2C commands
        try:
            print("I2C: receiving commands..")
            self.data = self.i2c.recv(1, 100)
            self.command[0] = self.data[0] #store in buffer for later procesing
            print("I2C: received")

        except OSError:
            self.data = None
        print (self.data)
      # print(self.command[0])
        #****AT THIS POINT: breakout to multiple handler functions
        if(self.command[0] == 0x11):
            print("I2C: poll requested")

            self.CatchRecieveCommand()
            if(True):
                self.Transmit(0x69)
        self.command[0] = 0
        utime.sleep_ms(10)

    def CatchRecieveCommand(self):
        #catch empty command from FC receive method
        try:
            print("I2C: stage 2..")
            self.data = self.i2c.recv(1, 500)
            self.command[0] = data[0] #store in buffer for later procesing
            print("I2C: received2")
        except OSError:#this exception is expected
            print("I2C: stage 2 no recieve")
            self.data = None
        #print (self.data)
    def Transmit(self, message):
        utime.sleep_ms(10)#need to test delay necessity
        try:
            print("I2C: sending...")
            self.i2c.send(message,0,timeout=5000)
            print("I2C: sent")
        except OSError:
            print("I2C: an OSError has occured")
            self.i2c.deinit()
            utime.sleep_ms(100)
            self.i2c.init(I2C.SLAVE, addr=self.address, dma=self.dma)

#*******#TEST CODE# *****
# comment out when this files is used as a library
clock = time.clock()

i2c = I2cHandler()
i2c.Init()


while(True):
    clock.tick()
    i2c.HandleI2c()



#print(clock.fps())
