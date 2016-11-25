#!/bin/bash
service network-manager stop
iwconfig wlan4 mode Ad-Hoc
iwconfig wlan4 essid redeadhoc
iwconfig wlan4 key s:senha12345678
sleep 1
ifconfig wlan4 up

#ifconfig wlan3 del fe80::ea94:f6ff:fe09:aa12/64
#ifconfig wlan3 0.0.0.0 
