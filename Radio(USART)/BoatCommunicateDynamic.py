import threading
import queue
import serial
from time import sleep
import keyboard

#all messages will be padded with spaces so that they are this length. Note that this
#is only the actul data length. The full message including the WIZE protocol which is
#actually sent by the radio is 7 bytes larger. Through testing, it has been noted that
#this should not be set to be greater than about 119. NOTE: if this is changed, the
#buffer length on the MCU for incomming messages MUST be made 7 bytes larger
max_message_size = 80

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
    ser = serial.Serial(port='COM6',baudrate=19200, timeout=0.5)
    dataQueue = queue.Queue()
    dataThread = threading.Thread(target=read_data, args=(dataQueue,ser,), daemon=True)
    dataThread.start()
    confirm_computer_has_recieved = False
    send = False
    message = ''
    decoded = ''
    confirm_mcu_has_received = False
    # sending("lap send", ser)
    # sleep(1)
    while True:
        try:
            if keyboard.is_pressed('shift'):
                print("\n")
                input_str = input()
                print("sending "+input_str)
                sending(input_str,ser)
                confirm_mcu_has_received = True
                print(confirm_mcu_has_received)
                print(dataQueue.qusize())
        except:
            pass
        #print(dataQueue.qsize())
        if (dataQueue.qsize() > 0):
            decoded = dataQueue.get()
            print(dataQueue.qsize())
            print(decoded)
            if confirm_mcu_has_received:
                if decoded == 'confirmed':
                    print("was confirmed")
                    confirm_mcu_has_received = False
            else:
                confirm_computer_has_recieved = True
        
        if confirm_computer_has_recieved:
            print("confirmed we recieved")
            sending("confirmed",ser)
            sleep(3)
            confirm_computer_has_recieved = False
            send = True
        
        '''
        elif send:
            sending("lap send", ser)
            sleep(1)
            sending("lap send", ser)
            print("sent data")
            sleep(1)
            send = False
            confirm_mcu_has_received = True
            '''
        sleep(1)

if (__name__ == '__main__'):
    main()
