import serial
import sys
from time import sleep
ser = serial.Serial(port='COM7',baudrate=19200, timeout=0.5)

def Radio_Encode(message):
    encode = message[:]
    lengthbit = len(message)+3
    encode = bytearray([lengthbit,0x20])+encode
    encode += bytearray([0x00,0x00])
    return encode

def Radio_decode(message):
    decode = message[11:len(message)-1]
    return decode.decode("utf-8")

def main():
    # message = "/$CP:DA,0.0%"
    while True:
        message = input("Enter Message:")
        print("Sending:"+message)
        message+= (119-len(message))*' '
        send = Radio_Encode(message.encode('utf-8'))
        ser.write(send)
        sleep(0.3)
    # print(send)
    # if ser.isOpen():
    #     print("open")
    #     while True :
    #         n = ser.inWaiting()
    #         if n :
    #             sleep(0.09)
    #             n = ser.inWaiting()
    #             data = ser.read(n)
    #             print(Radio_decode(data))
    # else:
    #     print(" not open")


if __name__ == '__main__':
    main()
