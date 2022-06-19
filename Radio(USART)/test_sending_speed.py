from sympy import N
from radio_function_class import Radio_Control as rad
import threading
import queue
from time import sleep
from time import time
import numpy as np
import matplotlib.pyplot as plt

num_messages_to_check = 10
max_message_per_second = 10
max_message_size = 80

#Function which returns the percent of messages which were sucessfully received at some
# the specified speed and number of messages (all 140 bytes) 

def send_thread(num_messages,speed,sender):
    for i in num_messages:
        sender.sending(str(i))
        sleep(1/speed)
#this function will wait the amount of time needed to read all the messages
#based on the speed calculte what percent of the messages were received
def read_thread(num_messages,speed,results,receiver):
    start_time = time()
    #total amount of time which should be needed to process the messages0
    total = num_messages/speed
    sucessfully_received = [0]*num_messages
    while (time()-start_time) < total:
        data = receiver.read()
        if data != "":
            sucessfully_received[int(data.strip())] = 1
    results[0] = sum(sucessfully_received)/num_messages


def test_at_speed(num_messages,speed):
    result=[0]
    rec = rad(port = 'COM7', max_message_size=max_message_size)
    send = rad(port = 'COM6', max_message_size=max_message_size)
    readthread = threading.Thread(target=read_thread, args=(num_messages,speed,result,rec,), daemon=True)
    sendthread = threading.Thread(target=send_thread, args=(num_messages,speed,send,), daemon=True)

    #start threads
    readthread.start()
    sendthread.start()
    
    #wait till both threads are done
    readthread.join()
    sendthread.join()

    return result[0]

def graph_reliability(num_messages,max_speed):
    speeds = np.arange(1,max_speed,2)
    reliability = [0]*len(speeds)
    for i in range(len(reliability)):
        reliability[i] = test_at_speed(num_messages,speeds[i])
    
    xlabel = "Speed (Messages/s with " + str(max_message_size)+" byte messages)"
    plt.xlabel(xlabel)
    plt.ylabel("Percent Reliability (percentge of messages which went through)")
    plt.plot(speeds,reliability)
    plt.show()

graph_reliability(num_messages_to_check,max_message_per_second)
