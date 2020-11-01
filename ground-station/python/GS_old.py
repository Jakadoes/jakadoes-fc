import serial 
import msvcrt
import telemetry 

#FIX THISSS 
READTIMEOUT = 3 #sets read timeout in seconds, set 0 to non-block
ser = serial.Serial('COM5', timeout=READTIMEOUT)  # open serial port
ser.baudrate = 57600; 
print(ser.name)         # check which port was really used

while True:
    while(1):
        command_in = raw_input("Enter a Telemetry Command: ")
        command_in = command_in.split(" ")#split command into list of arguments
        #print("DEBUG: " + command_in)
        if command_in[0] == "drone-ctls":
            break
        #****READ IN COMMANDS
        elif command_in[0] == "read": #if command first says read
            if(len(command_in) < 2):
                print("please select a value to read (e.g 'read alt')")
            else:
                command_2 = command_in[1]
                #print("DEBUG: command_2 is " + command_2)
                if command_2 == "alt":
                    telemetry.ReadAltitude(ser)
                elif command_2 == "batt":
                    telemetry.ReadBattery(ser)
                else:
                  print("could not understand read argument, please try again")

        else:
            print("command not understood")

    ranAlready = False
    while True:  # making a loop
        if not(ranAlready):
            print("Taking flight controls of Drone D-1A...")
            print("Press the tilde (`) key to escape")
            ranAlready = True
        keyStroke = msvcrt.getch()
        if keyStroke == '`':
            break
        else:
            ser.write(keyStroke)  

   

ser.close()             # close portss