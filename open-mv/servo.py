# Untitled - By: pi - Wed Feb 17 2021

#import sensor, image, time
import pyb, time
#sensor.reset()
#sensor.set_pixformat(sensor.RGB565)
#sensor.set_framesize(sensor.QVGA)
#sensor.skip_frames(time = 2000)

clock = time.clock()
servo = pyb.Servo(1)
#servo.angle(110,1000)
servo.angle(20)
servo.speed(0)
angle = 0
while(True):
    clock.tick()
    servo.pulse_width(900)
    pyb.delay(1000)
    #servo.pulse_width(2000)
    servo.pulse_width(0)
    pyb.delay(1000)
    #angle = angle + 50