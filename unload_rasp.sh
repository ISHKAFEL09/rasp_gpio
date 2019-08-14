#!/bin/sh
module="rasp_gpio"
device="rasp_gpio"
mode="664"

/sbin/rmmod ./$module $* || exit 1

rm -f /dev/$device

