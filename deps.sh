#!/bin/bash
# Install X and companions
sudo apt install xserver-xorg
sudo apt install xinit
sudo apt install x11-xserver-utils

# Install build tools
sudo apt install cmake pkg-config build-essentials

# Install sound machine
sudo apt install libjack libjack-dev

# Install libraries
sudo apt install libglfw libglfw-dev &&
sudo apt install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev &&
sudo apt install libglew-dev libglm-dev &&
sudo apt install libao-dev libmpg-dev libxinerama-dev libxcursor-dev libxi-dev