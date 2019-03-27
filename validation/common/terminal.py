#!/usr/bin/python3
# -*- coding: utf-8 -*-
#

## @file terminal.py
#
# @author Jan Belohoubek
# @version 0.1
# @date    2019-03-02
# @brief   The KETCube Terminal common validation module
#
# @note Requirements:
#    Standard Python3 installation (Tested Fedora ...)
#
# @attention
# 
#  <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
#  All rights reserved.</center></h2>
# 
#  Developed by:
#  The SmartCampus Team
#  Department of Technologies and Measurement
#  www.smartcampus.cz | www.zcu.cz
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy 
#  of this software and associated documentation files (the “Software”), 
#  to deal with the Software without restriction, including without limitation 
#  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
#  and/or sell copies of the Software, and to permit persons to whom the Software 
#  is furnished to do so, subject to the following conditions:
# 
#     - Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimers.
#     
#     - Redistributions in binary form must reproduce the above copyright notice, 
#       this list of conditions and the following disclaimers in the documentation 
#       and/or other materials provided with the distribution.
#     
#     - Neither the names of The SmartCampus Team, Department of Technologies and Measurement
#       and Faculty of Electrical Engineering University of West Bohemia in Pilsen, 
#       nor the names of its contributors may be used to endorse or promote products 
#       derived from this Software without specific prior written permission. 
# 
#  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
#  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
#  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS 
#  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
#  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
#  OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.

### Imports

import sys
import time
import serial
import serial.tools.list_ports
import re
from enum import IntEnum

from . import common as common

### Defines

class ParamReturnTypes(IntEnum):
    PARAMS_NONE       =  0
    PARAMS_BOOLEAN    =  1
    PARAMS_STRING     =  2
    PARAMS_BYTE       =  3
    PARAMS_INT32      =  4
    PARAMS_UINT32     =  5
    PARAMS_INT32_PAIR =  6
    PARAMS_BYTE_ARRAY =  7

### Connection options
COM="COM8"
BAUD_SPEED=9600 
TIMEOUT=0.1     # [s]

### KETCube options
ENDL=b'\x0a\x0d'
PROMPT=b'>> '

ser = None

# remove control characters from KETCube command line output
#
# @param byteArray
# @retval string
#

def removeCtrlChars(line):
    global ENDL, PROMPT
    
    line2 = line.decode("utf-8")
    line2 = line2.rstrip(ENDL.decode("utf-8"))
    line2 = line2.lstrip(ENDL.decode("utf-8"))
    line2 = line2.replace(PROMPT.decode("utf-8"), "")
    
    return line2

# confirm command execution
def sendConfirm():
    global ser, ENDL
    
    cnt = 0
    
    while True:
        if cnt > 3:
            return False
        
        ser.write(ENDL)
        ser.flush()
        echo = ser.read(1)
        if (echo[0] != 0x0A):
            cnt = cnt + 1
            continue
        else:
            return True

def flushRxBuffer():

    while ser.inWaiting():
        ser.read()

def selectComPort():
    global COM
    
    COM = ""
    print("Available COM ports: ")
    for p in serial.tools.list_ports.comports():
        print(str(p.device) + ": " + str(p.description))
        if 'TTL232R-3V3' in p.description:
            COM=p.device
        if 'USB Serial Port' in p.description:
            COM=p.device
        if 'KETCube FTDI Board (FT231X)' in p.description:
            COM=p.device
        if 'FT231X USB UART' in p.description:
            COM=p.device
    
    if COM == "":
        print("KETCube port not found! Automatic port selection FAILED!")
    
    COM = input("Select COM port [" + COM + "]:") or str(COM)


def initCOM():
    global COM, BAUD_SPEED, TIMEOUT, ser
    
    print("Connecting serial: "  + str(COM) + "; " + str(BAUD_SPEED) + "; "+ str(TIMEOUT) + "!")

    ser = serial.Serial(COM, BAUD_SPEED, timeout=TIMEOUT)


def unInitCOM():
    global ser
    
    print("Closing serial: "  + str(COM) + "; " + str(BAUD_SPEED) + "; "+ str(TIMEOUT) + "!")

    ser.close()

def sendChar(c):
    global ser
    
    byteArr = [ c ]
    
    flushRxBuffer()
    
    cnt = 0
    
    while True:
        if cnt > 3:
            return False
        
        length = ser.write(byteArr)
        ser.flush()
        echo = ser.read(1)
        if (echo[0] != byteArr[0]):
            cnt = cnt + 1
            continue
        else:
            return True

def sendCommand(cmd):
    global ser, ENDL
    
    sendConfirm()
    time.sleep(0.1)
    flushRxBuffer()
    
    if ser == None:
        return
    
    print("Executing cmd: " + cmd)
    try:
        data = cmd.encode()
        for b in data:
            if (sendChar(b) == False):
                common.exitError()    
        
        if (sendConfirm() == False):
            common.exitError()
            
    except:
        common.exitError()

# Parse CMDline parameters
def parseParams(paramType = None, line = None):
    # check input parameters
    if (paramType == None) or (line == None):
        return
    
    index = line.find(" returned: ")
    if (index < 0):
        return
    
    index = index + len(" returned: ")
    line = line[index:]
    print("Parameter found: " + line)
    
    if (ParamReturnTypes.PARAMS_BYTE_ARRAY):
        # remove byte delimiters
        line = line.replace('-', '')
        print("ByteArray parsed: " + line)
        return line
    
def getCmdResp(paramType = ParamReturnTypes.PARAMS_NONE):
    global ser, ENDL
    
    if ser == None:
        return
    
    while True:
        try:
            # wait or not?
            if (ser.in_waiting == 0):
                pass
            
            line = ser.readline()
        except:
            common.exitError()
            
        line = removeCtrlChars(line)
        print("Recv: " + line)
        
        if (line.find("Command not found!") >= 0):
            print("RETURNED ERROR")
            return
        
        if (line.find("Executing command: ") >= 0):
            print("Detected command execution start!")
            continue
    
        if (line.find("Command execution OK") >= 0):
            if (paramType != ParamReturnTypes.PARAMS_NONE):
                try:
                    line = ser.readline()
                except:
                    print("Param procesing failed!")
                    return
                line = removeCtrlChars(line)
                print("Recv: " + line)
                return parseParams(paramType, line)
            return
            
        if (line.find("Command execution ERROR") >= 0):
            print("RETURNED ERROR")
            return
    
        if (line.find("Help for command: ") >= 0):
            print("RETURNED ERROR - inclomplete command!")
            return
                
