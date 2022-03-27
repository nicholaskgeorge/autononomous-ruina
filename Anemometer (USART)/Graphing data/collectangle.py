
import serial
import sys
from time import sleep
import csv
import math



def main():
    ser = serial.Serial(port='COM16',baudrate=115200, timeout=0.5)
    start = False
    count = 0
    index = 0
    uav = 0
    vav = 0
    avgnum = 1
    with open('data.csv', mode='w') as log:
        log_data = csv.writer(log, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        log_data.writerow(["time","angle"])
        if ser.isOpen():
            print("open")
            while True :
                n = ser.inWaiting()
                if n :
                    sleep(0.09)
                    n = ser.inWaiting()
                    data = str(ser.read(n))
                    # a positive v comes from back to fron
                    #a positive u comes from left to right
                    v = float(data[data.find('V')+1:data.find('W')])*-1
                    u = float(data[data.find('U')+1:data.find('V')])*-1
                    vav += v
                    uav += u
                    index += 1
                    if index == avgnum:
                        vav /= avgnum
                        uav /= avgnum
                        if vav>0 and uav>0:
                            direction = int(math.atan(abs(uav/vav)) * 180/3.14)*-1
                        elif vav<0 and uav>0:
                            direction = (180-int(math.atan(abs(uav/vav)) * 180/3.14))*-1
                        elif vav>0 and uav<0:
                            direction = int(math.atan(abs(uav/vav)) * 180/3.14)
                        else:
                            direction = 180-int(math.atan(abs(uav/vav)) * 180/3.14)
                        count += 1
                        row = [count,str(direction)]
                        log_data.writerow(row)
                        index = 0
                        average = 0
        else:
            print(" not open")

if __name__ == '__main__':
    main()
