#!/bin/sh
g++ blacklist.cpp -o blacklist -O2 -pipe -fomit-frame-pointer -Wall -s -pie -fpie -lboost_system

#cp blacklist /usr/local/bin/
