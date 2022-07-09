import threading
import queue
from turtle import Turtle
import serial
from time import sleep
import keyboard

#all messages will be padded with spaces so that they are this length. Note that this
#is only the actul data length. The full message including the WIZE protocol which is
#actually sent by the radio is 7 bytes larger. Through testing, it has been noted that
#this should not be set to be greater than about 119. NOTE: if this is changed, the
#buffer length on the MCU for incomming messages MUST be made 7 bytes larger
max_message_size = 80
resend_sleep_time = 1
manual_send_sleep_time = 1


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
    packed = message + (max_message_size-len(message))*' '
    send = Radio_Encode(packed.encode('utf-8'))
    '''
    thing = str(binascii.hexlify(send))
    print(' '.join(["0x"+ thing[i:i+2] for i in range(0,len(thing),2)]))
    print(send)
    print(send)
    '''
    serialhandle.write(send)

def read_data(dataQueue,ser):
    while True:
        data = ser.read_until(b'\n')
        ser.reset_input_buffer()
        if data != b'' and data != b' ':
            print("Incoming data: "+Radio_decode(data))
            dataQueue.put(Radio_decode(data))
        sleep(0.1)

def main():
    ser = serial.Serial(port='COM5',baudrate=19200, timeout=0.5)
    dataQueue = queue.Queue(maxsize=5)
    dataThread = threading.Thread(target=read_data, args=(dataQueue,ser,), daemon=True)
    dataThread.start()
    confirm_computer_has_recieved = False
    confirm_print = True
    message = ''
    decoded = ''
    confirm_mcu_has_received = False

    resend_index = 1
    resend_limit = 3
    # sending("lap send", ser)
    # sleep(1)
    while True:
        try:
            if keyboard.is_pressed('`'):
                print("you can type now:")
                print("\n")
                input_str = input()
                print("sending " + input_str)
                sending(input_str,ser)
                confirm_mcu_has_received = True
                confirm_print = False
                sleep(manual_send_sleep_time)
        except:
            pass

        if (dataQueue.qsize()>0):   
            decoded = dataQueue.get()

            if confirm_mcu_has_received:
                if decoded == 'confirmed':
                    print('was confirmed')
                    confirm_mcu_has_received = False
                    confirm_print = True
            else:
                print("happened")
                confirm_computer_has_recieved = True

        if (confirm_computer_has_recieved):
            print("confirmed we received")
            sending("confirmed",ser)
            sleep(1.2)
            confirm_computer_has_recieved = False
            confirm_print = True

        if (confirm_print == False) & (dataQueue.qsize()==0):
            print('\n')
            print('resending')
            print('sending '+ input_str)
            print('\n')
            sending(input_str,ser)
            sleep(resend_sleep_time)
            if resend_index==resend_limit:
                confirm_print = True
                break            
            resend_index+=1
            
        sleep(0.5)

            
 

if (__name__ == '__main__'):
    main()
