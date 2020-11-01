#Telemetry Commands by Jakadoes
#Various Commands used to interact with the telemetry of the drone
import serial
###NOTE: MAKE SURE DRONE SENDS EXACT TELEMETRY SPLIT STATEMENTS (E.G Altitude:) or program may hang

def sendPacket(ser, message):
    ser.write(message)

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