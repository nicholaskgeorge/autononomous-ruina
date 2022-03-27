import serial
import sys
from time import sleep
import binascii


def Radio_Encode(message):
    encode = message[:]
    lengthbit = len(message)+3
    encode = bytearray([lengthbit,0x20])+encode
    encode += bytearray([0x00,0x00])
    return encode

def Radio_decode(message):
    decode = message[11:len(message)-1]
    return decode.decode("utf-8")

def sending(message,serialhandle):
    packed = message + (119-len(message))*'2'
    send = Radio_Encode(packed.encode('utf-8'))
    # thing = str(binascii.hexlify(send))
    # print(' '.join(["0x"+ thing[i:i+2] for i in range(0,len(thing),2)]))
    # print(send)
    # print(send)
    serialhandle.write(send)
    serialhandle.reset_output_buffer()
initialize = True;
start = 0
def main():
    # while initialize:
    #     if start==0:
    #         sending("checking connection", ser)
    #     elif start == 1:
    #     n = ser.inWaiting()
    #     if n:
    #         sleep(0.1)
    #         n = ser.inWaiting()
    #         data = ser.read(n)
    # message = "/$CP:DA,0.0%"
    # message = "lap send"
    # message+= (119-len(message))*' '
    # send = Radio_Encode(message.encode('utf-8'))
    # ser.write(send)
    # sleep(0.3)
    # message = input("Enter Message:")
    # print("Sending:"+message)
    # message+= (119-len(message))*' '
    # send = Radio_Encode(message.encode('utf-8'))
    # ser.write(send)
    # sleep(0.3)
    ser = serial.Serial(port='COM8',baudrate=19200, timeout=0.5)
    confirm_computer_has_recieved = False
    send = False
    confirm_mcu_has_received = True
    decoded = ''
    message = ''
    if ser.isOpen():
        print("open")
        sending("lap send", ser)
        sleep(0.9)
        while True:
            n = ser.inWaiting()
            if n:
                sleep(0.1)
                n = ser.inWaiting()
                data = ser.read(n)
                decoded = Radio_decode(data)
                print(Radio_decode(data))
                if confirm_mcu_has_received:
                    if decoded == 'confirmed':
                        print("was confirmed")
                        confirm_mcu_has_received = False
            if confirm_computer_has_recieved:
                print("confirmed we recieved")
                sending("confirmed",ser)
                sleep(3)
                confirm_computer_has_recieved = False
                send = True
            elif decoded == 'boat send':
                decoded = ''
                confirm_computer_has_recieved = True
            elif send:
                # sending("lap send", ser)
                sleep(1)
                sending("lap send", ser)
                print("sent data")
                sleep(1)
                send = False
                confirm_mcu_has_received = True
    else:
        print(" not open")


if __name__ == '__main__':
    main()
