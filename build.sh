#!/bin/sh
g++ -O2 -pipe -fomit-frame-pointer -Wall -s -pie -fpie -lboost_system -o blacklist blacklist.cpp
sudo cp blacklist /usr/local/bin/