#!/usr/bin/python3
# -*- coding: utf-8 -*-
#

## @file createModule.py
#
# @author Jan Belohoubek
# @version 0.2.1
# @date    2019-07-09
# @brief   The KETCube "createModule" helper script
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

# Imports
import datetime
import sys
import os
import inspect
import string
from shutil import copyfile

# Set variable defaults
AUTHOR="KET"
VERSION="0.1"
now = datetime.datetime.now()
DATE="" + str(now.year) + "-" + str(now.month) + "-" + str(now.day)
NAME="NONE"
DESCR="-"
CATEGORIES = {"sensing" : "sensing", 
              "communication" : "communication", 
              "actuation" : "actuation" }
CATEGORY="sensing"

# Get this file PATH
#PATH = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
#PATH = os.path.dirname(sys.argv[0])
PATH = os.path.dirname(os.path.realpath(__file__))
if PATH == "":
   raise ValueError("PATH could not be detected!")

## Create copy of the file while replacing variables
#
# @param src source file (template)
# @param dst destination file
#
def copyFromTemplate(src, dst):
    global AUTHOR, VERSION, DATE, NAME, CATEGORY, NAME_LOWERCASE, NAME_UPPERCASE
    
    s = open(src, "r", encoding='utf-8', errors='ignore')
    d = open(dst, "w")
    for line in s.readlines():
        line = line.replace("[AUTHOR]"          , AUTHOR)
        line = line.replace("[VERSION]"         , VERSION)
        line = line.replace("[DATE]"            , DATE)
        line = line.replace("[NAME]"            , NAME)
        line = line.replace("[NAME_LOWERCASE]"  , NAME_LOWERCASE)
        line = line.replace("[NAME_UPPERCASE]"  , NAME_UPPERCASE)
        line = line.replace("[CATEGORY]"        , CATEGORY)
        d.write(line)
    s.close()
    d.close()
    
## Insert text at the end of file
#
# @param dst destination file
# @param text text to insert
#
def insertEnd(dst, text):
    
    d = open(dst, "a")
    d.write(text)
    d.close()
    
## Insert text to dst file before line containing pattern
#
# @param dst destination file
# @param patterns list of texts to identify line to insert text above
# @param texts texts to insert
#
def insertBefore(dst, patterns, texts):
    
    # backup file
    copyfile(dst, dst+"~")
    
    s = open(dst+"~", "r", encoding='utf-8', errors='ignore')
    d = open(dst,     "w")
    
    for line in s.readlines():
        index = 0
        for pattern in patterns:
            if (line.find(pattern) > 0):
                d.write(texts[index])
            index = index + 1
        d.write(line)
    
    s.close()
    d.close()

print("KETCube module creator helper script")
print("------------------------------------")
print()
AUTHOR=input("Set module AUTHOR full name [" + str(AUTHOR) + "]: ") or str(AUTHOR)
NAME=input("Set module NAME [" + str(NAME) + "]: ") or str(NAME)
DESCR=input("Set module DESCRIPTION [" + str(DESCR) + "]: ") or str(DESCR)
NAME_LOWERCASE=NAME.lower()
NAME_UPPERCASE=NAME.upper()
VERSION=input("Set module VERSION [" + str(VERSION) + "]: ") or str(VERSION)
CATEGORY=input("Set module CATEGORY (sensing, communication, actuation) [" + str(CATEGORY) + "]: ") or str(CATEGORY)


print()
print("Parameters: ")
print(" - NAME     : " + str(NAME))
print(" - DESCR    : " + str(DESCR))
print(" - AUTHOR   : " + str(AUTHOR))
print(" - DATE     : " + str(DATE))
print(" - VERSION  : " + str(VERSION))
print(" - CATEGORY : " + str(CATEGORY))
print()


# Copy templates
c_src=PATH + "/templates/ketCube_module.c"
c_dst=PATH + "/../Projects/src/" + CATEGORIES[CATEGORY] + "/ketCube_" + NAME_LOWERCASE + ".c"
h_src=PATH + "/templates/ketCube_module.h"
h_dst=PATH + "/../Projects/inc/" + CATEGORIES[CATEGORY] + "/ketCube_" + NAME_LOWERCASE + ".h"

if not os.path.exists(c_src):
    raise ValueError("Source file does not exist: " + (c_src))
if not os.path.exists(h_src):
    raise ValueError("Source file does not exist: " + (h_src))
if os.path.exists(c_dst):
    raise ValueError("Destination file already exist: " + (c_dst))
if os.path.exists(h_dst):
    raise ValueError("Destination file already exist: " + (h_dst))

print()
print("Generating module from template: ")
print("  copyFromTemplate(" + h_src + "," + h_dst + ")")
print("  copyFromTemplate(" + c_src + "," + c_dst + ")")
print()

copyFromTemplate(c_src, c_dst)
copyFromTemplate(h_src, h_dst)

print()
print("Insert module declarations: ")

dst=PATH + "/../Projects/inc/ketCube_compilation.h"

textInc="#define KETCUBE_CFG_INC_MOD_" + NAME_UPPERCASE + "    ///< Include " + NAME + " module; undef to disable module\n"
textID="""#ifdef KETCUBE_CFG_INC_MOD_""" + NAME_UPPERCASE + """
    KETCUBE_LISTS_MODULEID_""" + NAME_UPPERCASE + """,              /*!< Module """ + NAME + """ */
#endif

"""
print("  Inserting to: "+ dst)
insertBefore(dst, ["#define KETCUBE_CFG_INC_MOD_DUMMY", "KETCUBE_LISTS_MODULEID_LAST"], [textInc, textID])

dst=PATH + "/../Projects/src/ketCube_moduleList.c"    
textInc="#include \"ketCube_" + NAME_LOWERCASE + ".h\"\n"
textDef="""#ifdef KETCUBE_CFG_INC_MOD_""" + NAME_UPPERCASE + """
    DEF_MODULE(\"""" + NAME + """\",
               \"""" + DESCR + """\",
               KETCUBE_MODULEID_""" + NAME_UPPERCASE + """,
               &ketCube_""" + NAME_LOWERCASE + """_Init,      /* Init() */
               NULL,                        /* SleepEnter() */
               NULL,                        /* SleepExit() */
               &ketCube_""" + NAME_LOWERCASE + """_ReadData,  /* GetSensorData() */
               NULL,                        /* SendData() */
               NULL,                        /* ReceiveData() */
               NULL,                        /* ProcessData() */
               ketCube_""" + NAME_LOWERCASE + """_moduleCfg   /* Module cfg struct */
              ),
#endif

"""
print("  Inserting to: "+ dst)
insertBefore(dst, ["AUTOGEN_INSERT_INCLUDE", "AUTOGEN_INSERT_MODULE_DEF"], [textInc, textDef])

dst=PATH + "/../Projects/inc/ketCube_module_id.h"
textDef="    KETCUBE_MODULEID_" + NAME_UPPERCASE + ",\n"
print("  Inserting to: "+ dst)
insertBefore(dst, ["KETCUBE_MODULEID_AUTOGENERATED_END"], [textDef])
print()
print("NOTE: it is recommended to define module ID explicitly in " + dst + " file.")
print()


print("Modifying project-specific Makefile: ")
dst=PATH + "/../Projects/Makefile/Makefile_proj"
text="SRCS += $(COREDIR)Projects/src/sensing/ketCube_" + NAME_LOWERCASE + ".c\n"
insertEnd(dst, text)
print()


print()
print("ALL DONE!")
print()
