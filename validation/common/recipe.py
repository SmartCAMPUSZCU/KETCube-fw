#!/usr/bin/python3
# -*- coding: utf-8 -*-
#

## @file recipe.py
#
# @author Jan Belohoubek
# @version 0.1
# @date    2019-03-04
# @brief   The KETCube recipe deffinitions
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

from . import common as common
from . import terminal as term


recipe = None

## Open text file containing recipe (list of KETCube commands)
#
def _openRecipe(recipePath):
    global recipe
    
    try:  
        recipe = open(recipePath)
    except:
        print("Error when openning recipe \'" + recipePath + "\'!")
        common.exitError()

## Close text file containing recipe (list of KETCube commands)
#
def _closeRecipe():
    global recipe
    
    if (recipe == None):
        return
    
    recipe.close()

## Process simple recipe
#
# @param simpleRecipe file containing KETCube commands - one per line
#
def processSimpleRecipe(simpleRecipe):
    
    _openRecipe(simpleRecipe)
    
    if (recipe == None):
        return
    
    for line in recipe:
        line = line.rstrip() # remove newline
        
        term.sendCommand(line)
        term.getCmdResp()

    _closeRecipe()


## Process data recipe - get/set values from KETCube
#
# @param setShow - set or show string to decide if set or show will be executed; set expects 4 records in the recipe
# @param simpleRecipe file containing KETCube storage, command sub-tree and data type delimited by ";" - one record per line
#
# @retval recipe - returns SET-type recipe: for setShow == "set", the recipe is equal to input recipe; for "show", it contains current values read from device
#
def processSetShowRecipe(setShow, dataRecipe):
    
    # set recipe generated from input recipe
    newSetRecipe = ""
    
    _openRecipe(dataRecipe)
    
    if (recipe == None):
        return
    
    if (setShow != "show") and (setShow != "set"):
        return
    
    for line in recipe:
        line = line.rstrip() # remove newline
        records = (line.split(';'))
        
        if ((len(records) < 3) and (setShow == "show")) or ((len(records) < 4) and (setShow == "set")):
            print("Record \'" + line + "\'in source recipe malformed!")
            print("Recipe record format: {RAM|EEPROM}; {RETVAL DATA TYPE}; {CMD}; {CMD PARAM};")
            _closeRecipe()
            common.exitError()
        
        records[0] = records[0].strip()
        if   (records[0] == "EEPROM"):
            rootCmd = setShow
        elif (records[0] == "RAM"):
            rootCmd = setShow + "r"
        else:
            print("Unknown storageType \'" + records[0] + "\'")
            _closeRecipe()
            common.exitError()
        
        records[1] = records[1].strip()
        dataType = common.strToType(records[1])
        if (dataType  == None):
            print("Unknown dataType \'" + records[1] + "\'")
            _closeRecipe()
            common.exitError()
        
        cmd = records[2].strip()
        
        if (setShow == "set"):
            cmd = cmd + " " + records[3].strip()
        
        term.sendCommand(rootCmd + " " + cmd)
        resp = term.getCmdResp(dataType)
        if resp == "ERROR":
            _closeRecipe()
            common.exitError()
        
        newSetRecipe = newSetRecipe + records[0] + "; " + records[1] + "; " + records[2] + "; " + resp + ";\n"

    _closeRecipe()
    
    return newSetRecipe
