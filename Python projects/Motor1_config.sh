#!/bin/bash

cd ~/Documents/Python\ projects/
./ServoConfig.py -d 1 -c SET_SERVO_ENABLE 0
./ServoConfig.py -d 1 -c SET_SERVO_CW_LIM 290
./ServoConfig.py -d 1 -c SET_SERVO_CCW_LIM 725