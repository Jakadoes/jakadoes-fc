#Telemetry Commands by Jakadoes
#Various Commands used to interact with the telemetry of the drone
import serial
import pymavlink
from kivy.app import App
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
    #class parameters
    app = None
    mavUtil = mavutil #you can either use a MAVLink class for lower level access or mavutil for a higher level api that handles the connection
    theConnection = None
    #message enums
    ALERT_FIRE_SCANNING = 0x11
    ALERT_FIRE_ALERT    = 0x22
    ALERT_FIRE_STORED   = 0x33
    ALERT_FIRE_NONE = 0x10
    
    def __init__(self, **kwargs):
        super(MAVHandler, self).__init__(**kwargs)
        self.mavUtil.MAVLINK20 = 1 #enable mavlink v2
        blinker.signal('gamepadControlInput').connect(self.SendRCData)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 

    def after_init(self, dt): #provide refference to app or other widgets after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')

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

        #print("rc data sent: ", rcData1_scaled, rcData2_scaled, rcData3_scaled, rcData4_scaled)
        self.theConnection.mav.rc_channels_scaled_send(time_boot_ms= 0, port=0,rssi=0, chan1_scaled =  rcData1_scaled, chan2_scaled = rcData2_scaled, chan3_scaled = rcData3_scaled, chan4_scaled = rcData4_scaled, chan5_scaled = 32767, chan6_scaled = 32767, chan7_scaled = 32767, chan8_scaled = 32767)

    def HandleMessage(self):
        #handler for incoming mavlink messages
        msg = self.theConnection.recv_match()
        #print(msg)
        #create switch for all defined message types 
        if not msg:
            return
        elif (msg.get_type() == "NAMED_VALUE_INT"):
            self.HandleNamedValueInt(msg)
        elif (msg.get_type() == "RAW_IMU"):
            self.HandleRawIMU(msg)
        elif (msg.get_type() == "FILE_TRANSFER_PROTOCOL"):
            self.HandleFileTransferProtocol(msg)
        
    def HandleNamedValueInt(self, msg):
        print("Named int message recieved: ")
        print(msg.name)
        print(msg.value) 

        if(msg.name == "FireAlert"):
            if(msg.value == self.ALERT_FIRE_SCANNING):
                self.app.root.serialHandler.fireAlert = False
            elif(msg.value == self.ALERT_FIRE_ALERT or msg.value == self.ALERT_FIRE_STORED):
                self.app.root.serialHandler.fireAlert = True
                if(msg.value == self.ALERT_FIRE_ALERT):
                    self.app.root.cameraFeed.ShowFireDetect()
                #***invoke a HandleFireAlert method based on this flag***
            else:
                print("warning: FireAlert content was not an expected enum, data may be corrupted")
        elif(msg.name == "CAM_DONE"):
            self.app.root.cameraFeed.SaveData(4)#save data to file space 1 
    def HandleRawIMU(self, msg):
        #print("accel data",msg.xacc,msg.yacc,msg.zacc)
        print("gyro data",msg.xgyro,msg.ygyro,msg.zgyro)
        self.app.root.instPanel.instrument1.reading     = msg.xacc/-100.0
        self.app.root.instPanel.instrument1.reading_sub = msg.yacc/-100.0
        self.app.root.instPanel.instrument2.reading     = msg.ygyro/100.0 #NOTEE THIS IS FLIPPED FROM ACC 
        self.app.root.instPanel.instrument2.reading_sub = msg.xgyro/-100.0

    def HandleFileTransferProtocol(self, msg):
        print("FTP message received")
        print(msg.payload)
        #decode payload:
        #first byte is number of bytes used in transmission
        #next 3 bytes are for index with payload[1] being LSB
        #remaining bytes are photo data 
        index = msg.payload[1] + msg.payload[2]*(2**8) + msg.payload[3]*(2**8)#decode index 
        self.app.root.cameraFeed.LogData(index, msg.payload[4:msg.payload[0]+1])

        