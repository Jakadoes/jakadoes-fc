from pyb import I2C
import pyb
import utime
import sensor, image, time, math

i2c = I2C(2)
i2c.init(I2C.SLAVE, addr=0x26)
#switch = pyb.Switch()
data = bytearray(1)
clock = time.clock()



while(True):
    clock.tick()

    try:
        data = i2c.recv(1)
    except OSError:
        data = None
    print (data)
    print("RIP harambe")
    utime.sleep_ms(100)


#print(clock.fps())


