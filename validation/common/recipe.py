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


### TestBench imports

from . import common as common
from . import terminal as term


recipe = None


def openRecipe(recipePath):
    global recipe
    
    try:  
        recipe = open(recipePath)
    except:
         common.exitError()
        
def closeRecipe():
    global recipe
    
    if (recipe == None):
        return
    
    recipe.close()

def processSimpleRecipe(simpleRecipe):
    
    openRecipe(simpleRecipe)
    
    if (recipe == None):
        return
    
    for line in recipe:
        line = line.rstrip() # remove newline
        
        term.sendCommand(line)
        term.getCmdResp()

    closeRecipe()
