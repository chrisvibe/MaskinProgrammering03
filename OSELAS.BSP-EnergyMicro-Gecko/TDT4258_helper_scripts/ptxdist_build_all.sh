#!/bin/bash

# Script for building kernel and drivers and everything
# Assumes ptxdist is configured

# WARNING:
# This is a sketchy script. Busybox compilation fails, so we
# just remove one line and try to build again. Seems to be working

ptxdist images
sed -i '/usleep/d' /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/platform-energymicro-efm32gg-dk3750/build-target/busybox-1.21.0/modutils/modprobe-small.c
ptxdist images
