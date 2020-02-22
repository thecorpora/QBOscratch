#!/bin/bash

cd ~/Documents/Python\ projects/
./ServoConfig.py -d 1 -c SET_SERVO_ENABLE 0
./ServoConfig.py -d 1 -c SET_SERVO_ID 2
./ServoConfig.py -d 2 -c SET_SERVO_CW_LIM 420
./ServoConfig.py -d 2 -c SET_SERVO_CCW_LIM 550