import serial

class Radio_Control():
    def __init__(self):
        self.max_message_size = 80
        self.port = 'COM7'
        self.baudrate=19200
        self.ser = serial.Serial(port=self.port,baudrate=self.baudrate, timeout=0.5)

    def Radio_Encode(self,message):
        encode = message[:]
        lengthbit = len(message)+3
        encode = bytearray([lengthbit,0x20])+encode
        encode += bytearray([0x00,0x00])
        return encode

    def Radio_decode(self,message):
        decode = message[11:len(message)-1]
        return decode.decode("utf-8")

    def sending(self,message):
        packed = message + (self.max_message_size-len(message))*' '
        send = self.Radio_Encode(packed.encode('utf-8'))
        self.ser.write(send)
    
    def read(self):
        data = self.ser.read_until(b'\n')
        self.ser.reset_input_buffer()
        if data != b'' and data != b' ':
            return data
        return ""