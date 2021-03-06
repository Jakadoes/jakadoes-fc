#Telemetry Commands by Jakadoes
#Various Commands used to interact with the telemetry of the drone
import serial
import pymavlink
from kivy.clock import Clock
import blinker
from pymavlink import mavutil
###NOTE: MAKE SURE DRONE SENDS EXACT TELEMETRY SPLIT STATEMENTS (E.G Altitude:) or program may hang

def sendPacket(ser, message):
    ser.write(message)

def RawRead(ser, numBytes):
    telem_in = ser.read(numBytes)
    return telem_in

def ReadAltitude(ser):
    print("waiting for altitude telemetry...")
    telem_in = ser.read_until("ALTBREAK")#waits until altitude data given
    print(telem_in)
    if("ALTBREAK" in telem_in):
        print("Altitude: " + telem_in.split("Altitude: ")[1].split(" ")[0])
    else:
        print("request timed out")
    return 

def ReadBattery(ser):
    print("waiting for battery telemetry...")
    telem_in = ser.read_until("BATTBREAK")#waits until altitude data given
    print(telem_in)
    if("BATTBREAK" in telem_in):
        print("Battery: " + telem_in.split("Battery: ")[1].split(" ")[0])
    else:
        print("request timed out")
    return 

class MAVHandler():
    #you can either use a MAVLink class for lower level access or
    #mavutil for a higher level api that handles the connection
    mavUtil = mavutil
    theConnection = None
    
    def __init__(self, **kwargs):
        super(MAVHandler, self).__init__(**kwargs)
        self.mavUtil.MAVLINK20 = 1 #enable mavlink v2
        blinker.signal('gamepadControlInput').connect(self.SendRCData)

    def Connect(self,connectionSettings, baudRate):
        self.theConnection = self.mavUtil.mavlink_connection(connectionSettings, baud=baudRate)
      
    def SendRCData(self, rcData1, rcData2, rcData3, rcData4):
        #INPUT: rc data scaled between 0 and 100.0 (float)
        #OUTPUT: rc data scaled between  and 1000 (int)
        UNUSED = 32767 #passing this value indicates unused channel in protocol
        rcData1_scaled = int(rcData1*10)
        rcData2_scaled = int(rcData2*10)
        rcData3_scaled = int(rcData3*10)
        rcData4_scaled = int(rcData4*10)

        print("rc data sent: ", rcData1_scaled, rcData2_scaled, rcData3_scaled, rcData4_scaled)
        self.theConnection.mav.rc_channels_scaled_send(time_boot_ms= 0, port=0,rssi=0, chan1_scaled =  rcData1_scaled, chan2_scaled = rcData2_scaled, chan3_scaled = rcData3_scaled, chan4_scaled = rcData4_scaled, chan5_scaled = 32767, chan6_scaled = 32767, chan7_scaled = 32767, chan8_scaled = 32767)

    #def SendRCDAta(self, rcData1, rcData2, rcData3, rcData4):
        