from kivy.clock import Clock
from kivy.app import App
#pip or naitive imports
import serial
import blinker
import enum
#user defined imports 
import telemetry as telem

class ParamSetMsg():
    target_component = 0
    param_id = 0
    param_value = 0

class SerialHandler():
    app = None
    #serial parameters
    READTIMEOUT = 3 #sets read timeout in seconds, set 0 to non-block
    baudRate = 55555
    isConnected = False #check if com port is connected before giving commands 
    ser = None
    gamepadHandler = None
    mavHandler = None
    useMAV = True
    msgQueue = []
    msgqueueIter = 0#used to alternate between non critical messages and RC messages 

    #message buffers
    rx_buffer = None
    fireAlert = False

    def __init__(self, **kwargs):
        super(SerialHandler, self).__init__(**kwargs)
        Clock.schedule_once(self.after_init)#provide ref to app after inits 
    
    def after_init(self, dt): #provide refference to app after initializations
        self.app= App.get_running_app()
        if(self.app == None):
            print('WARNING: refference to app was not made, null pointers may occur')
        self.gamepadHandler = self.app.root.gamepadHandler

    def ConnectToRadio(self, comNum, useMAV = True):
        try:
            if(useMAV):
                #Mavlink connection
                self.mavHandler = telem.MAVHandler()
                self.mavHandler.Connect('COM' + str(comNum), self.baudRate)
                self.useMAV = True
            else:
                #py serial connection
                self.ser = serial.Serial('COM' + str(comNum), timeout=self.READTIMEOUT,bytesize=8, parity=serial.PARITY_NONE)  # open serial port
                self.ser.baudrate = 57600 
                print(self.ser.name)         # check which port was really used
                self.useMAV = False
            #set connection flag 
            self.isConnected = True
        except:
            self.app.root.terminal.LogMessage(">Connect error: " + str(comNum) + " is not a proper com port, check connections")
    
    def HandleSerial(self):
        if(self.isConnected):
            if(self.useMAV):
                if(True):
                    if(not(self.msgQueue == None) and self.msgqueueIter and len(self.msgQueue) > 0):
                        if(isinstance(self.msgQueue[0], ParamSetMsg)):
                            msg = self.msgQueue[0]
                            #print("sending msg from msg queue")
                            #print(self.msgQueue[0])
                            self.mavHandler.SendParamSet(msg.target_component, msg.param_id, msg.param_value)
                            self.msgqueueIter = 0
                            self.msgQueue.remove(self.msgQueue[0])
                        else:
                            print("WARNING: msg type in msgQueue not known")
                    else:
                        #--send RC commands--
                        #self.mavHandler.SendRCData(self.gamepadHandler.rc1,self.gamepadHandler.rc2, self.gamepadHandler.rc3, self.gamepadHandler.rc4)
                        self.msgqueueIter = 1

                    #--receive telemetry data--
                    self.mavHandler.HandleMessage()
            else:
                if(self.ser.inWaiting() >= 4):#read in message whenever its larger then designated packet size 
                    self.RawRead(4)
                    sig = blinker.signal('newData') #create an event for other pieces of code to listen to
                    sig.send()
                    #print("signal shoud be being sent..")

    def RawRead(self, numBytes):
        #returns bytes of serial data 
        telem_in = telem.RawRead(self.ser, numBytes)
        self.rx_buffer = telem_in

    def sendRawPacket(self, message):
        telem.sendPacket(self.ser, message.encode('utf-8'))

    def sendMotorCommand(self, axisid, value):
        #OLD - DOESNT USE MAVLINK 
        telem.sendPacket(self.ser, (str(axisid) +" "+ str(value)).encode('utf-8'))
    def QueueParamSetMsg(self, targetComponent, paramId, paramValue):
        paramSetMsg = ParamSetMsg()
        paramSetMsg.target_component = targetComponent
        paramSetMsg.param_id = paramId
        paramSetMsg.param_value = paramValue
        if(self.msgQueue == None):
            self.msgQueue = [paramSetMsg]
        else:
            self.msgQueue.append(paramSetMsg)


 