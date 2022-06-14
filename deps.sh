#!/bin/bash
# Optional: Install X and companions
sudo apt install xserver-xorg xinit x11-xserver-utils

# Install build tools
sudo apt install git cmake pkg-config build-essential

# Install sound machine and graphics libraries
sudo apt install jackd2 libjack-jackd2-0 libjack-jackd2-dev libglfw3 libglfw3-dev libncurses5-dev mesa-utils libglu1-mesa-dev mesa-common-dev