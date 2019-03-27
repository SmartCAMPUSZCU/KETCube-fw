#!/usr/bin/python3
# -*- coding: utf-8 -*-
#

## @file LoRaWAN.py
#
# @author Jan Belohoubek
# @version 0.1
# @date    2019-03-26
# @brief   The KETCube LoRaWAN common deffinitions
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
import os

from . import common as common

## Get random bytes
#
# @param length number of random bytes
#
def _genRandomBytes(length = 16):
    
    return os.urandom(length)

## Convert byte array to HEX string
#
def _toHexString(byteArray):
    ret = ""
    for b in byteArray:
        ret = ret + str('%02X' % b)
        
    return ret

## Generate LoRaWAN Network Address
#
# @param prefix the Network Address prefix (NetID) - will be concatenated with random bytes to form 4-bytze NetAddr
#
def genNetAddr(prefix = [ 0x00 ]):
    if len(prefix) > 4:
        print("Invalid prefix!")
        common.exitError()
    
    # how many bytes generate
    gen = 4 - len(prefix)
    
    netAddr = bytearray()
    for b in prefix:
        netAddr.append(b)
    for b in _genRandomBytes(length = gen):
        netAddr.append(b)
    
    return _toHexString(netAddr)

## Generate 128-bit random AES key
#
# @note No key checking or key validation is provided. The key characteristics (RND distribution etc. depends on RND generator provided by host system)
#
def _gen128AESKey():
    byteArray = bytearray()
    for b in _genRandomBytes(length = 16):
        byteArray.append(b)
    
    return _toHexString(byteArray)

## Generate LoRaWAN 1.0 Application Key
#
def genAppKey():  
    return _gen128AESKey()

## Generate LoRaWAN 1.0 Application Session Key
#
def genAppSKey():  
    return _gen128AESKey()

## Generate LoRaWAN 1.0 Network Session Key
#
def genNwkSKey():  
    return _gen128AESKey()

## Generate LoRaWAN 1.0 Application EUI
#
def genAppEUI():  
    byteArray = bytearray()
    for b in _genRandomBytes(length = 8):
        byteArray.append(b)
    
    return _toHexString(byteArray)

## Generate random device EUI
#
def genDevEUI(prefix = None):
    byteArray = bytearray()
    
    # how many bytes generate
    if prefix == None:
        gen = 8
    else:
        gen = 8 - len(prefix)
        for b in prefix:
            byteArray.append(b)
    
    for b in _genRandomBytes(length = gen):
        byteArray.append(b)
    
    return _toHexString(byteArray)
