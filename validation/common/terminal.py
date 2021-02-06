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

### Connection options
COM="COM8"
BAUD_SPEED=9600 
TIMEOUT=0.1     # [s]

### KETCube options
ENDL=b'\x0a'
PROMPT=b'>> '

ser = None

## Remove control characters from KETCube command line output
#
# @param line byte Array
# @retval string
#
def _removeCtrlChars(line):
    global ENDL, PROMPT
    
    line2 = line.decode("utf-8")
    line2 = line2.rstrip(ENDL.decode("utf-8"))
    line2 = line2.lstrip(ENDL.decode("utf-8"))
    line2 = line2.replace(PROMPT.decode("utf-8"), "")
    line2 = line2.replace('\b', '')
    
    return line2

## Confirm command execution
#
# Send ENDL to confirm command execution
#
def _sendConfirm():
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

## Flush Rx Buffer
#
def flushRxBuffer():

    while ser.inWaiting():
        ser.read()

## Select COM port
#
# User-interactive COM port selection
#
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


## Initialize COM port
#
# initialize selected COM port
#
# @note COM must be selected
#
def initCOM():
    global COM, BAUD_SPEED, TIMEOUT, ser
    
    print("Connecting serial: "  + str(COM) + "; " + str(BAUD_SPEED) + "; "+ str(TIMEOUT) + "!")

    ser = serial.Serial(COM, BAUD_SPEED, timeout=TIMEOUT)

## Deinitialize COM port
#
#
def unInitCOM():
    global ser
    
    print("Closing serial: "  + str(COM) + "; " + str(BAUD_SPEED) + "; "+ str(TIMEOUT) + "!")

    ser.close()

## Send a single character
#
# Use echo to verify if character was processed correctly
#
def _sendChar(c):
    global ser
    
    byteArr = [ c ]
    byteRestore = [ 8 ]
    
    flushRxBuffer()
    
    cnt = 0
    
    try:
        while True:
            if cnt > 10:
                return False
            
            length = ser.write(byteArr)
            ser.flush()
            echo = ser.read(1)
            if (len(echo) < 1):
                cnt = cnt + 1
                continue
            elif (len(echo) > 1):
                # probably unrelated cmdline output
                time.sleep(0.1)
            
            if ((echo[0] != byteArr[0]) or (len(echo) > 1)):
                cnt = cnt + 1
                time.sleep(0.01)
                ser.write(byteRestore) # \b
                # read \b and any unrelated junk
                echo = ser.read()
                continue
            else:
                return True
    except Exception as e:
        print(str(e))
        return False

## Send a single command
#
# @param cmd string containing complete KETCube comand
#
def sendCommand(cmd):
    global ser, ENDL
    
    #_sendConfirm()
    _sendChar(ENDL[0])
    time.sleep(0.1)
    flushRxBuffer()
    
    if ser == None:
        return
    
    print("Executing cmd: " + cmd)
    try:
        data = cmd.encode()
        for b in data:
            if (_sendChar(b) == False):
                common.exitError()    
        
        if (_sendChar(ENDL[0]) == False):
            common.exitError()
            
    except:
        common.exitError()


## Parse CMDline parameters
#
# @param paramType type of parameter to parse
# @line string obtained from KETCube containing command response
#
# @retval parsed parameter
#
def _parseParams(paramType = None, line = None):
    # check input parameters
    if (paramType == None) or (line == None):
        return
    
    index = line.find(" returned: ")
    if (index < 0):
        return
    
    index = index + len(" returned: ")
    line = line[index:]
    print("Parameter found: " + line)
    
    if (paramType == common.Types.PARAMS_BYTE_ARRAY):
        # remove byte delimiters
        line = line.replace('-', '')
        line = line.replace(' ', '')
        print("ByteArray parsed: " + line)
        return line
    elif (paramType == common.Types.PARAMS_BOOLEAN):
        if (line.find("TRUE") >= 0):
            return "1"
        else:
            return "0"
    else:
        return line
    
    return ""

## Get command response
#
# @param paramType type of parameter to parse, None if nothing expected
#
# @retval parsed parameter (if expected)
#
def getCmdResp(paramType = None):
    global ser, ENDL
    
    if ser == None:
        return "ERROR"
    
    while True:
        try:
            # wait or not?
            if (ser.in_waiting == 0):
                pass
            
            line = ser.readline()
        except:
            common.exitError()
            
        line = _removeCtrlChars(line)
        print("Recv: " + line)
        
        if (line.find("Command not found!") >= 0):
            print("RETURNED ERROR")
            return "ERROR"
        
        if (line.find("Executing command: ") >= 0):
            print("Detected command execution start!")
            continue
    
        if (line.find("Command execution OK") >= 0):
            if (paramType != None):
                try:
                    line = ser.readline()
                except:
                    print("Param procesing failed!")
                    return "ERROR"
                line = _removeCtrlChars(line)
                print("Recv: " + line)
                return _parseParams(paramType, line)
            return "OK"
            
        if (line.find("Command execution ERROR") >= 0):
            print("RETURNED ERROR")
            return "ERROR"
    
        if (line.find("Help for command: ") >= 0):
            print("RETURNED ERROR - inclomplete command!")
            return "ERROR"
                
