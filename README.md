# autononomous-ruina
Fall 2020 collaboration between autonomous plane and boat with Ruina group

@author [Daisy Zhang](mailto:dz298@cornell.edu?subject=[GitHub]%20Source%20Han%20Sans)

# Overview
This repo contains C scripts for the autonomous plane and boat projects while 
the microcontroller in both projects is **SAM E70 Xplained Evaluation Kit**.

Code is made to be modular so that it can be applied to similar projects with minor edition. 

Different modules include:
- SD Card data logging 
- Anemometer
- GPS
- Satellite communication (not finished yet)

# Usage
Each folder represents a component module. The name of the folder shows the component and its communication peripherial port and protocol with our MCU. 

Inside the folder, there're:
- a screenshot of the component's configuration with MPLAB X Harmony v3
- the header file of the module
- the source file of the module
- a main script showing how to call the module


## Usage Example
We take 'Anemometer' the module as an example:

'Anemometer' folder includes scripts for parsing and processing the message sent by the component and a graph showing the configuration of it with MPLAB X Harmony v3.  

![alt text](https://github.com/cec272/autononomous-ruina/blob/master/Anemometer%20(USART)/USART1%20config%20for%20Anemometer.png)

The anemometer being used in our research is TriSonica Mini Wind & Weather Anemometer. Its RX and TX pins are connected to USART1_TX and USART1_RX pins on the MCU. Therefore the graph shows the configuration of USART1. If any other USART ports is used for the anemoemeter, the anemometer's RX/TX pins connection, the peripheral port in the configuration, and functions of corresponding USART_TX/RX pins have to be changed accordingly. 

The most important feature on the graph is the baud rate of this port. It has to be the same as that of our anemometer, which is 115200. 

After the configuration is finished, the user can generate code based on the configuration, which will turn the graphic user interface to scripts to configure the desired functinoality of the MCU. 

Then users are free to use code in 'main.c' to use functions in 'Anemometer'.

# SAM E70 Xplained Evaluation Kit Pin Out



