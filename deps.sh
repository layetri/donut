#!/bin/bash
# Install X and companions
sudo apt install xserver-xorg xinit x11-xserver-utils

# Install build tools
sudo apt install git cmake pkg-config build-essential

# Install sound machine
sudo apt install jackd2 libjack-jackd2-0 libjack-jackd2-dev

# Install libraries
sudo apt install libglfw3 libglfw3-dev &&
sudo apt install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev &&
sudo apt install libglew-dev libglm-dev &&
sudo apt install libao-dev libmpg-dev libxinerama-dev libxcursor-dev libxi-dev

# FRESH INSTALL:
