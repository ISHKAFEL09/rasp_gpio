#!/bin/bash
make
sshpass -p "wangyin09" scp *.init *.ko *.sh pi@169.254.60.218:/home/pi/ldd/rasp_gpio
