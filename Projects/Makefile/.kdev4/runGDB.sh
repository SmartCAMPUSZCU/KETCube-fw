#!/bin/sh
#
# Run debug session for KDevelop
#
# Author: Jan Belohoubek
# Date: 2019-07-12
#
# This wrapper is needed because is simplifies KDevelop debug launch configuration
#

cd "$( dirname $0 )/.."

make debugMI2
