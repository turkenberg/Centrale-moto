#!/bin/sh
sleep 30
export DISPLAY=":0.0"
su chip -c "python /home/chip/pyscripts/Hello.py"
